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

void FileBrowser::update_path(const char* pwd)
{
  if (this->pwd)
    delete this->pwd;

  char path[2048];
  my_realpath(pwd, path);
  this->pwd = new std::string(path);
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

  if (dirp)
  {
      while ((entry = readdir(dirp)) != NULL)
      {
        // skip the '.' entry if it exists in the filesystem
        if (strcmp(entry->d_name, ".") == 0)
          continue;
        
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

#define MAX_READLINKS 32
#define PATH_MAX 1024

// from http://www.scs.stanford.edu/histar/src/pkg/uclibc/libc/stdlib/realpath.c
char* my_realpath(const char *path, char resolved_path[])
{
	char copy_path[PATH_MAX];
	char link_path[PATH_MAX];
	char got_path[PATH_MAX];
	char *new_path = got_path;
	char *max_path;
	int readlinks = 0;
	int n;

	if (path == NULL) {
		return NULL;
	}
	if (*path == '\0') {
		return NULL;
	}
	/* Make a copy of the source path since we may need to modify it. */
	if (strlen(path) >= PATH_MAX - 2) {
		return NULL;
	}
	strcpy(copy_path, path);
	path = copy_path;
	max_path = copy_path + PATH_MAX - 2;
	/* If it's a relative pathname use getcwd for starters. */
	if (*path != '/') {
		/* Ohoo... */
		getcwd(new_path, PATH_MAX - 1);
		new_path += strlen(new_path);
		if (new_path[-1] != '/')
			*new_path++ = '/';
	} else {
		*new_path++ = '/';
		path++;
	}
	/* Expand each slash-separated pathname component. */
	while (*path != '\0') {
		/* Ignore stray "/". */
		if (*path == '/') {
			path++;
			continue;
		}
		if (*path == '.') {
			/* Ignore ".". */
			if (path[1] == '\0' || path[1] == '/') {
				path++;
				continue;
			}
			if (path[1] == '.') {
				if (path[2] == '\0' || path[2] == '/') {
					path += 2;
					/* Ignore ".." at root. */
					if (new_path == got_path + 1)
						continue;
					/* Handle ".." by backing up. */
					while ((--new_path)[-1] != '/');
					continue;
				}
			}
		}
		/* Safely copy the next pathname component. */
		while (*path != '\0' && *path != '/') {
			if (path > max_path) {
				return NULL;
			}
			*new_path++ = *path++;
		}
		*new_path++ = '/';
	}
	/* Delete trailing slash but don't whomp a lone slash. */
	if (new_path != got_path + 1 && new_path[-1] == '/')
		new_path--;
	/* Make sure it's null terminated. */
	*new_path = '\0';
	strcpy(resolved_path, got_path);
	return resolved_path;
}
