#include "../libs/chesto/src/Element.hpp"
#include <memory>
#include <string>

using namespace Chesto;

#if defined(_3DS) || defined(_3DS_MOCK)
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
	std::unique_ptr<std::string> path;
};