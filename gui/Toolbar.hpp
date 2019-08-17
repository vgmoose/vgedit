#include "../libs/chesto/src/TextElement.hpp"

class Toolbar : public Element
{
public:
	const static int MAX_PATH_LENGTH = 48;

	Toolbar(const char* path);
	void setModified(bool modified);
	void render(Element* parent);

	TextElement* pathE = NULL;
	TextElement* actions = NULL;
	TextElement* keyActions = NULL;

	char path[MAX_PATH_LENGTH + 1];
	bool modified = true;
	bool keyboardShowing = false;
};