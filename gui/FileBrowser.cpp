#include "FileBrowser.hpp"
#include "../libs/chesto/src/Button.hpp"
#include "../libs/chesto/src/TextElement.hpp"
#include "FileCard.hpp"
#include "MainDisplay.hpp"
#include <dirent.h>
#include <string.h>
#include <unistd.h>

FileBrowser::FileBrowser(const char* pwd)
{
	update_path(pwd);

	x = 40;
	y = 0;

	listfiles();
}

bool FileBrowser::process(InputEvents* events)
{
	if (((MainDisplay*)RootDisplay::mainDisplay)->editorView != NULL)
		return false;

	if (events->isTouchDown())
	{
		touchMode = true;
		selected = -1;
	}

	else if (events->pressed(UP_BUTTON) || events->pressed(DOWN_BUTTON) || events->pressed(LEFT_BUTTON) || events->pressed(RIGHT_BUTTON))
	{
		touchMode = false;

		if (selected == -1)
		{
			selected = 0;
		}

		selected -= events->pressed(UP_BUTTON) * 5;
		selected += events->pressed(DOWN_BUTTON) * 5;
		selected += events->pressed(RIGHT_BUTTON);
		selected -= events->pressed(LEFT_BUTTON);

		if (selected < 0) selected = 0;
		if (selected >= elements.size() - 1) selected = elements.size() - 2; // account for path

		if ((selected / 5) > 3)
		{
			this->y = -200 * (selected / 5);
			return true;
		}
		else
		{
			this->y = 0;
			return true;
		}
	}

	if (events->released(A_BUTTON))
	{
		if (selected < 0) return false;

		// activate this file's editor
		((FileCard*)elements[1 + selected])->openMyFile();
		return true;
	}

	return ListElement::process(events);
}

void FileBrowser::render(Element* parent)
{
	if (((MainDisplay*)RootDisplay::mainDisplay)->editorView != NULL)
		return;

	if (selected >= 0)
	{
		// draw the cursor for this file
		CST_Rect dimens4 = { this->x + (selected % 5) * 220 + 60, this->y + (selected / 5) * 200 + 100, 210, 210 };
		CST_SetDrawColor(parent->renderer, { 0xaa, 0xaa, 0xaa, 0xff });
		CST_DrawRect(parent->renderer, &dimens4);
	}

	return ListElement::render(parent);
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
#if defined(PC)
	// PC can use realpath, other platforms might not have it
	realpath(pwd, path);
	this->pwd = new std::string(path);
#else
	this->pwd = new std::string(pwd);
#endif
}

void FileBrowser::listfiles()
{
	// go through all files in current directory and create FileCard children out of them

	this->elements.clear();

	// current path at the top
	CST_Color white = { 0xFF, 0xFF, 0xFF, 0xFF };
	TextElement* path = new TextElement(pwd->c_str(), 25, &white, MONOSPACED);
	path->position(10, 30);
	this->elements.push_back(path);

	// new folder and file buttons
	// Button* newFolder = new Button("New Folder", X_BUTTON, true);
	// newFolder->position(820, 20);
	// this->elements.push_back(newFolder);

	// Button* newFile = new Button("New File", Y_BUTTON, true);
	// newFile->position(1020, 20);
	// this->elements.push_back(newFile);

	DIR* dirp;
	struct dirent* entry;

	dirp = opendir(pwd->c_str());

	int count = 0;

	// create a hardcoded "up" link to the parent directory
	if (*pwd != std::string("/"))
	{
		FileCard* card = new FileCard(true, ".. (parent)");
		card->position(this->x + (count % 5) * card->width, this->y + 115 + (count / 5) * card->height);
		std::string cwd = dir_name(*pwd);
		card->path = new std::string(cwd == "" ? "/" : cwd);
		card->action = std::bind(&FileCard::openMyFile, card);
		this->elements.push_back(card);
		count++;
	}

	if (dirp)
	{
		while ((entry = readdir(dirp)) != NULL)
		{
			// skip the '.' and '..' entries if they exist in the filesystem
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				continue;

			FileCard* card = new FileCard(entry->d_type == DT_DIR, entry->d_name);
			card->position(this->x + (count % 5) * card->width, this->y + 115 + (count / 5) * card->height);
			card->path = new std::string(*pwd + (*pwd != "/" ? std::string("/") : "") + entry->d_name);
			card->action = std::bind(&FileCard::openMyFile, card);
			child(card);
			count++;
		}

		closedir(dirp);
	}
}