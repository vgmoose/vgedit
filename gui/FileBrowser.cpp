#include "FileBrowser.hpp"
#include "FileCard.hpp"
#include "../libs/hb-appstore/gui/TextElement.hpp"
#include <dirent.h>

FileBrowser::FileBrowser(const char* pwd)
{
  this->pwd = new std::string(pwd);

  x = 40;
  y = 0;

  listfiles();
}

bool FileBrowser::process(InputEvents* event)
{
  return ListElement::process(event);
}

void FileBrowser::render(Element* parent)
{
  renderer = parent->renderer;
  return super::render(this);
}

void FileBrowser::listfiles()
{
  // go through all files in current directory and create FileCard children out of them

  this->elements.clear();

  // current path at the top
  SDL_Color white = { 0xFF, 0xFF, 0xFF, 0xFF };
  TextElement* path = new TextElement(pwd->c_str(), 25, &white, MONOSPACED);
  path->position(5, 5);
  this->elements.push_back(path);

  DIR* dirp;
  struct dirent* entry;

  dirp = opendir(this->pwd->c_str());

  int count = 0;

  if (dirp)
  {
      while ((entry = readdir(dirp)) != NULL)
      {
        FileCard* card = new FileCard(this);
        card->position(this->x + (count % 5) * card->width, this->y + 100 + (count / 5) * card->width );
        card->update(entry->d_type == DT_DIR, entry->d_name);
        card->path = new std::string(*pwd + std::string("/") + entry->d_name);
        card->action = std::bind(&FileCard::openMyFile, card);
        this->elements.push_back(card);
        count++;
      }

      closedir(dirp);
  }
}
