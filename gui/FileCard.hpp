#include <string>
#include "../libs/chesto/src/Element.hpp"

#ifdef _3DS_MOCK
#define FILE_CARD_WIDTH 80
#define LISTING_SCREEN_WIDTH (SCREEN_WIDTH - 80)
#else
#define FILE_CARD_WIDTH 220
#define LISTING_SCREEN_WIDTH SCREEN_WIDTH
#endif

class FileCard : public Element
{
public:
	FileCard(bool isFolder, const char* name);
	~FileCard();
	void update(bool isFolder, const char* name);
	void openMyFile();

	bool isFolder = false;
	std::string* path = NULL;
};