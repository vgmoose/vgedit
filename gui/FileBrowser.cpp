#include "FileBrowser.hpp"
#include "../libs/chesto/src/Button.hpp"
#include "../libs/chesto/src/Constraint.hpp"
#include "../libs/chesto/src/Container.hpp"
#include "../libs/chesto/src/TextElement.hpp"
#include "FileCard.hpp"
#include "MainDisplay.hpp"
#include "TextQueryPopup.hpp"
#include <algorithm>
#include <dirent.h>
#include <fstream>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace Chesto;

#if defined(WIN32)
#include <sys/types.h>
#endif

#ifdef __WIIU__
#include <sysapp/launch.h>
#endif

FileBrowser::FileBrowser(const char* pwd)
{
	update_path(pwd);

	cardsPerRow = LISTING_SCREEN_WIDTH / FILE_CARD_WIDTH;

	x = ((SCREEN_WIDTH - (cardsPerRow * FILE_CARD_WIDTH)) / 4);
	width = -1 * x; // the other margin
	y = 0;

	listfiles();
}

bool FileBrowser::process(InputEvents* events)
{
	bool updateUI = false;
	if (((MainDisplay*)RootDisplay::mainDisplay)->editorView != NULL)
		return false;

	if (events->isTouchDown())
	{
		// remove a highlight if it exists (TODO: same as an above if statement)
		if (this->highlighted >= 0 && this->highlighted < this->elements.size() && this->elements[this->highlighted])
			this->elements[this->highlighted]->elasticCounter = NO_HIGHLIGHT;

		touchMode = true;
	}

	else if (events->pressed(UP_BUTTON) || events->pressed(DOWN_BUTTON) || events->pressed(LEFT_BUTTON) || events->pressed(RIGHT_BUTTON))
	{
		if (touchMode) highlighted = 0;
		touchMode = false;

		// look up whatever is currently chosen as the highlighted position
		// and remove its highlight
		// (copypasta'd from AppList in HBAS, TODO: consolidate logic in a GridView in chesto)
		if (this->highlighted >= 0 && this->elements[this->highlighted])
			this->elements[this->highlighted]->elasticCounter = NO_HIGHLIGHT;

		highlighted -= events->pressed(UP_BUTTON) * cardsPerRow;
		highlighted += events->pressed(DOWN_BUTTON) * cardsPerRow;
		highlighted += events->pressed(RIGHT_BUTTON);
		highlighted -= events->pressed(LEFT_BUTTON);

		updateUI |= true;
	}

	if (highlighted < 0) highlighted = 0;
	if (highlighted >= elements.size() - 2) highlighted = elements.size() - 3;

	updateUI |= ListElement::process(events);

	// excluding path and buttons at the end, this should be a FileCard
	auto currentCard = (FileCard*)elements[highlighted].get();

	int cardY = currentCard->yAbs;

	// Below code is lifted from HBAS's AppList.cpp
	// TODO: consolidate into Chesto's ListElement
	if (!touchMode && this->elements.size() > this->highlighted && this->highlighted >= 0 && this->elements[this->highlighted])
	{
		// if our highlighted position is large enough, force scroll the screen so that our cursor stays on screen
		Element* curTile = this->elements[this->highlighted].get();

		// the y-position of the currently highlighted tile, precisely on them screen (accounting for scroll)
		// this means that if it's < 0 or > SCREEN_HEIGHT then it's not visible
		int normalizedY = curTile->y + this->y;

		// if we're FAR out of range upwards, speed up the scroll wheel (additive) to get back in range quicker
		if (normalizedY < -200)
			events->wheelScroll += 0.3;

		// far out of range, for bottom of screen
		else if (normalizedY > SCREEN_HEIGHT - curTile->height + 200)
			events->wheelScroll -= 0.3;

		// if we're slightly out of range above, recenter at the top row slowly
		else if (normalizedY < -100)
			events->wheelScroll = 1;

		// special case, scroll when we're on the bottom row of the top of the not-yet-scrolled screen
		else if (this->y == 0 && normalizedY > SCREEN_HEIGHT / 2)
			events->wheelScroll -= 0.5;

		// if we're out of range below, recenter at bottom row
		else if (normalizedY > SCREEN_HEIGHT - curTile->height + 100)
			events->wheelScroll = -1;

		// if the card is this close to the top, just set it the list offset to 0 to scroll up to the top
		else if (this->y != 0 && this->highlighted < cardsPerRow)
			events->wheelScroll = 1;

		if (this->elements[this->highlighted] && this->elements[this->highlighted]->elasticCounter == NO_HIGHLIGHT)
		{
			this->elements[this->highlighted]->elasticCounter = THICK_HIGHLIGHT;
			updateUI |= true;
		}
	}

	if (events->released(A_BUTTON))
	{
		// activate this file's editor
		currentCard->openMyFile();
		updateUI = true;
	}

	if (events->released(B_BUTTON))
	{
		// activate an upCard if it exists, to go back
		if (upCard != NULL)
			upCard->openMyFile();
		updateUI = true;
	}

	// do we need reo reload the file list?
	if (needsReload)
	{
		needsReload = false;
		RootDisplay::deferAction([this]()
			{ listfiles(); });
		updateUI = true;
	}

	return updateUI;
}

void FileBrowser::render(Element* parent)
{
	if (((MainDisplay*)RootDisplay::mainDisplay)->editorView != NULL)
		return;

	ListElement::render(parent);
}

// copy-pasta'd from Utils.cpp in hb-appstore
// TODO: consolidate
const std::string dir_name(std::string file_path)
{
	// turns "/hi/man/thing.txt to /hi/man"
	int pos = file_path.find_last_of("/");

	// no "/" in string, return empty string
	if (pos == std::string::npos)
		return "";

	return file_path.substr(0, pos);
}

void FileBrowser::update_path(const char* pwd)
{
	if (this->pwd)
		delete this->pwd;

	char path[2048];
#if defined(PC) && !defined(WIN32)
	// PC can use realpath, other platforms might not have it
	realpath(pwd, path);
	this->pwd = new std::string(path);
#else
	this->pwd = new std::string(pwd);
#endif
}

bool is_dir(const char* pwd, struct dirent* entry)
{
#ifndef WIN32
	return entry->d_type & DT_DIR;
#else
	// windows check, using stat
	struct stat s;
	// get full path using dir and entry
	std::string full_path = std::string(pwd) + "/" + std::string(entry->d_name);
	stat(full_path.c_str(), &s);
	return s.st_mode & S_IFDIR;
#endif
}

void FileBrowser::listfiles()
{
	// go through all files in current directory and create FileCard children out of them

	upCard = NULL;
	this->removeAll();

	DIR* dirp;
	struct dirent* entry;

	dirp = opendir(pwd->c_str());

	int count = 0;
	int topOfList = 115;

#if defined(_3DS) || defined(_3DS_MOCK)
	topOfList = SCREEN_HEIGHT / 2 + 40;
#endif

	// create a hardcoded "up" link to the parent directory
	if (*pwd != std::string("/") && *pwd != std::string(START_PATH))
	{
		auto card = std::make_unique<FileCard>(true, ".. (parent)");
		card->position(this->x + (count % cardsPerRow) * card->width, this->y + topOfList + (count / cardsPerRow) * card->height);
		std::string cwd = dir_name(*pwd);
		card->path = std::make_unique<std::string>(cwd == "" ? "/" : cwd);
		this->addNode(std::move(card));

		upCard = (FileCard*)elements.back().get();
		auto pathCopy = std::make_shared<std::string>(*upCard->path);
		upCard->action = [pathCopy]()
		{
			((MainDisplay*)RootDisplay::mainDisplay)->openFile(true, pathCopy.get());
		};
		count++;
	}

	if (dirp)
	{
		struct FileInfo
		{
			bool isFolder;
			std::string name;
			std::string path;
		};
		std::vector<FileInfo> fileInfos;

		while ((entry = readdir(dirp)) != NULL)
		{
			// skip the '.' and '..' entries if they exist in the filesystem
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				continue;

			// full path to the file (C++ string)
			FileInfo info;
			info.isFolder = is_dir(pwd->c_str(), entry);
			info.name = entry->d_name;
			info.path = *pwd + (*pwd != "/" ? std::string("/") : "") + entry->d_name;
			fileInfos.push_back(info);
		}

		// sort alphabetically
		std::sort(fileInfos.begin(), fileInfos.end(), [](const FileInfo& a, const FileInfo& b)
			{ return strncasecmp(a.path.c_str(), b.path.c_str(), std::min(a.path.length(), b.path.length())) < 0; });

		// make children of the browser
		for (auto& info : fileInfos)
		{
			auto card = std::make_unique<FileCard>(info.isFolder, info.name.c_str());
			card->path = std::make_unique<std::string>(info.path);
			card->position(this->x + (count % cardsPerRow) * card->width, this->y + topOfList + (count / cardsPerRow) * card->height);
			addNode(std::move(card));

			auto cardPtr = (FileCard*)elements.back().get();
			auto isFolder = cardPtr->isFolder;
			auto pathCopy = std::make_shared<std::string>(*cardPtr->path);
			cardPtr->action = [isFolder, pathCopy]()
			{
				((MainDisplay*)RootDisplay::mainDisplay)->openFile(isFolder, pathCopy.get());
			};
			count++;
		}

		closedir(dirp);
	}

	// current path at the top
	CST_Color white = { 0xFF, 0xFF, 0xFF, 0xFF };

	// limit it to 45 chars
	std::string clippedPath(*pwd);
	if (clippedPath.length() > 45)
	{
		clippedPath.erase(clippedPath.begin(), clippedPath.end() - 45);
		clippedPath.insert(0, "…");
	}

	auto pathPtr = std::make_unique<TextElement>(clippedPath.c_str(), 25, &white, MONOSPACED);
	pathPtr->position(10, topOfList - 85);
	auto path = pathPtr.get();
	this->addNode(std::move(pathPtr));

	auto mainDisplay = RootDisplay::mainDisplay;

	// new folder, file, and exit buttons
	auto conPtr = std::make_unique<Container>(ROW_LAYOUT, 10);
	auto con = conPtr.get();

	std::function<void()> quitWrapper = [mainDisplay]()
	{
		mainDisplay->requestQuit();
	};

	con->createNode<Button>("Exit", SELECT_BUTTON, true)->setAction(quitWrapper);

	con->createNode<Button>("New Folder", X_BUTTON, true)->setAction([this, mainDisplay]()
		{
		std::function<void(const char*)> createFunc = [this](const char* name){
			printf("Creating folder [%s]\n", name);
#ifndef WIN32
			::mkdir((*this->pwd + "/" + name).c_str(), 0775);
#else
			mkdir((*this->pwd + "/" + name).c_str());
#endif
			this->needsReload = true;
		};
		auto popup = std::make_unique<TextQueryPopup>("Enter new folder name", "Create", createFunc);
		RootDisplay::pushScreen(std::move(popup)); });

	con->createNode<Button>("New File", Y_BUTTON, true)->setAction([this, mainDisplay]()
		{
		std::function<void(const char*)> createFunc = [this](const char* name){
			printf("Creating file [%s]\n", name);
			std::ofstream output((*this->pwd + "/" + name).c_str());
			this->needsReload = true;
		};
		auto popup = std::make_unique<TextQueryPopup>("Enter new file name", "Create", createFunc);
		RootDisplay::pushScreen(std::move(popup)); });

	con->position(SCREEN_WIDTH - con->width - this->x * 2, topOfList - 85);
#if defined(_3DS) || defined(_3DS_MOCK)
	con->position(SCREEN_WIDTH / 2 - con->width / 2 - this->x, topOfList - 30);
	path->position(10, topOfList - 60);

	// for 3ds, the top screen will show the title initially
	auto titleConPtr = std::make_unique<Container>(ROW_LAYOUT, 10);
	auto titleCon = titleConPtr.get();
	titleCon->createNode<ImageElement>(RAMFS "res/icon_nobg.png")->setSize(50, 50);
	titleCon->createNode<TextElement>("VGEdit", 30, &white);
	titleCon->constrain(ALIGN_CENTER_HORIZONTAL);
	this->addNode(std::move(titleConPtr));

#endif
	this->addNode(std::move(conPtr));
}