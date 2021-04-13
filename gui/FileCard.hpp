#include <string>
#include "../libs/chesto/src/Element.hpp"

#define FILE_CARD_WIDTH 220

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