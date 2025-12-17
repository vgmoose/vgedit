#include "../libs/chesto/src/ListElement.hpp"
#include <string>

using namespace Chesto;

// the root of the filesystem, that we can't go above
#if defined(PC)
#define START_PATH "."
#elif defined(__WIIU__)
#define START_PATH "/vol/external01/"
#else
#define START_PATH "/"
#endif

class FileCard;

char* my_realpath(const char* path, char resolved_path[]);

class FileBrowser : public ListElement
{
public:
	FileBrowser(const char* pwd);
	bool process(InputEvents* event);
	void render(Element* parent);
	void listfiles();
	void update_path(const char* path);

	FileCard* upCard = NULL;

	std::string* pwd = NULL;
	bool touchMode = true;
	int cardsPerRow = 5;
	bool needsReload = false;
};