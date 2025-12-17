#include "FileCard.hpp"
#include "../libs/chesto/src/Constraint.hpp"
#include "../libs/chesto/src/Container.hpp"
#include "../libs/chesto/src/ImageElement.hpp"
#include "../libs/chesto/src/TextElement.hpp"
#include "MainDisplay.hpp"

using namespace Chesto;

FileCard::FileCard(bool isFolder, const char* name)
{
	this->width = FILE_CARD_WIDTH;
	this->height = FILE_CARD_WIDTH - (this->width > 100 ? 20 : 0);
	touchable = true;
	update(isFolder, name);
}

void FileCard::update(bool isFolder, const char* name)
{
	// new vertical container
	auto con = createNode<Container>(COL_LAYOUT, 5);

	// folder or file icon
	auto thumbPath = isFolder ? RAMFS "res/folder.png" : RAMFS "res/file.png";
	auto thumbIcon = con->createNode<ImageElement>(thumbPath);
	thumbIcon->setSize(this->width / 2, this->height / 2);
	thumbIcon->constrain(ALIGN_CENTER_HORIZONTAL);

	// text label (centered)
	CST_Color color = { 0xFF, 0xFF, 0xFF, 0xFF };
	auto textElem = con->createNode<TextElement>(name, this->width / 11, &color, NORMAL, this->width);
	textElem->constrain(ALIGN_CENTER_HORIZONTAL);

	// center container
	con->constrain(ALIGN_CENTER_BOTH);

	this->isFolder = isFolder;
}

void FileCard::openMyFile()
{
	((MainDisplay*)RootDisplay::mainDisplay)->openFile(isFolder, path.get());
}

FileCard::~FileCard()
{
}