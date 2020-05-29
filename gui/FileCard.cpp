#include "FileCard.hpp"
#include "../libs/chesto/src/ImageElement.hpp"
#include "../libs/chesto/src/TextElement.hpp"
#include "../libs/chesto/src/Container.hpp"
#include "MainDisplay.hpp"

FileCard::FileCard(bool isFolder, const char* name)
{
	this->width = 220;
	this->height = 200;
	touchable = true;
	update(isFolder, name);
}

void FileCard::update(bool isFolder, const char* name)
{
	// new vertical container
	Container* con = new Container(COL_LAYOUT, 5);

	// folder or file icon
	auto thumbPath = isFolder ? RAMFS "res/folder.png" : RAMFS "res/file.png";
	auto thumbIcon = con->add((new ImageElement(thumbPath))->setSize(110, 100));

	// text label (centered)
	CST_Color color = { 0xFF, 0xFF, 0xFF, 0xFF };
	con->add((new TextElement(name, 20, &color, NORMAL, this->width)))->centerHorizontallyIn(con);
	thumbIcon->centerHorizontallyIn(con);

	// add and center container
	child(con->centerIn(this));

	this->isFolder = isFolder;
}

void FileCard::openMyFile()
{
	((MainDisplay*)RootDisplay::mainDisplay)->openFile(isFolder, path);
}

FileCard::~FileCard()
{
	this->wipeAll();
}