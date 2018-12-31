#include "FileBrowser.hpp"
#include "FileCard.hpp"
#include "../libs/hb-appstore/gui/TextElement.hpp"
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

bool FileBrowser::process(InputEvents* event)
{
  return ListElement::process(event);
}

void FileBrowser::render(Element* parent)
{
  renderer = parent->renderer;
  return super::render(this);
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
  SDL_Color white = { 0xFF, 0xFF, 0xFF, 0xFF };
  TextElement* path = new TextElement(pwd->c_str(), 25, &white, MONOSPACED);
  path->position(5, 5);
  this->elements.push_back(path);

  DIR* dirp;
  struct dirent* entry;

  dirp = opendir(pwd->c_str());

  int count = 0;

  // create a hardcoded "up" link to the parent directory
  if (*pwd != std::string("/"))
  {
    FileCard* card = new FileCard(this);
    card->position(this->x + (count % 5) * card->width, this->y + 100 + (count / 5) * card->width );
    card->update(true, ".. (parent)");
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
        
        FileCard* card = new FileCard(this);
        card->position(this->x + (count % 5) * card->width, this->y + 100 + (count / 5) * card->width );
        card->update(entry->d_type == DT_DIR, entry->d_name);
        card->path = new std::string(*pwd + (*pwd != "/" ? std::string("/") : "") + entry->d_name);
        card->action = std::bind(&FileCard::openMyFile, card);
        this->elements.push_back(card);
        count++;
      }

      closedir(dirp);
  }
}