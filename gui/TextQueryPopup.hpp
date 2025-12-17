#include "../libs/chesto/src/Screen.hpp"
#include "../libs/chesto/src/TextElement.hpp"
#include <string>

using namespace Chesto;

class TextQueryPopup : public Screen
{
public:
	TextQueryPopup(const char* prompt, const char* ctaText, std::function<void(const char*)> onConfirm);
	~TextQueryPopup();

	std::string query = "";
	TextElement* queryText = NULL;

	void render(Element* parent);
	void rebuildUI() override { }
};