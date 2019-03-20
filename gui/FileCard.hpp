#include "../libs/hb-appstore/gui/Element.hpp"

class FileCard : public Element
{
public:
	FileCard(Element* parent);
	void update(bool folder, const char* name);
	void render(Element* parent);
	bool process(InputEvents* event);
	void openMyFile();

	bool folder = false;
	std::string* path = NULL;
};