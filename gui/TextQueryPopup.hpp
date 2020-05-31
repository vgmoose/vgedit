#include <string>
#include "../libs/chesto/src/TextElement.hpp"

class TextQueryPopup : public Element
{
public:
	TextQueryPopup(const char* prompt, const char* ctaText, std::function<void(const char*)> onConfirm);
	~TextQueryPopup();

	std::string query = "";
	TextElement* queryText = NULL;

	void render(Element* parent);
};