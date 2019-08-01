#include "FileCard.hpp"
#include "../libs/chesto/src/ImageElement.hpp"
#include "../libs/chesto/src/TextElement.hpp"
#include "MainDisplay.hpp"

FileCard::FileCard(Element* parent)
{
	this->width = 220;
	this->height = 200;
	this->parent = parent;
	touchable = true;
}

void FileCard::update(bool folder, const char* name)
{
	ImageElement* icon = new ImageElement(folder ? ROMFS "res/folder.png" : ROMFS "res/file.png");
	icon->resize(110, 110);
	icon->position(this->x + this->width / 2 - icon->width / 2, this->y + 10);
	this->elements.push_back(icon);

	SDL_Color color = { 0xFF, 0xFF, 0xFF, 0xFF };
	TextElement* label = new TextElement(name, 20, &color, NORMAL, 200);
	label->position(this->x + this->width / 2 - label->width / 2, this->y + 130);
	this->elements.push_back(label);

	this->folder = folder;
}

void FileCard::openMyFile()
{
	((MainDisplay*)RootDisplay::mainDisplay)->openFile(folder, path);
}

void FileCard::render(Element* parent)
{
	return super::render(parent);
}

bool FileCard::process(InputEvents* event)
{
	this->xOff = this->parent->x;
	this->yOff = this->parent->y;

	return super::process(event);
}

// TODO: destructor, cleanup path
