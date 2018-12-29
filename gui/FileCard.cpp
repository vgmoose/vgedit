#include "../libs/hb-appstore/gui/ImageElement.hpp"
#include "../libs/hb-appstore/gui/TextElement.hpp"
#include "FileCard.hpp"

FileCard::FileCard(Element* parent)
{
  this->width = 200;
  this->height = 200;
  this->parent = parent;
  touchable = true;
}

void FileCard::update(bool folder, const char* name)
{
  ImageElement* icon = new ImageElement(folder ? ROMFS "res/folder.png" : ROMFS "res/file.png");
  icon->resize(100, 100);
  icon->position(this->x + 10, this->y + 10);
  this->elements.push_back(icon);

  SDL_Color color =  {0xFF, 0xFF, 0xFF, 0xFF };
  TextElement* label = new TextElement(name, 20, &color, MONOSPACED, 90);
  label->position(this->x + 5, this->y + 120);
  this->elements.push_back(label);
}

void FileCard::openMyFile()
{
  printf(path->c_str());
  printf("\n");
  // MainDisplay::mainDisplay->openFile(pwd + "/" + name);
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