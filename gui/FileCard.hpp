#include <string>
#include "../libs/chesto/src/Element.hpp"

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