#include <string>
#include "../libs/chesto/src/Element.hpp"

class TextQueryPopup : public Element
{
public:
	TextQueryPopup(const char* prompt, const char* ctaText, std::function<void(const char*)> onConfirm);
	~TextQueryPopup();

	std::string query = "";

	void render(Element* parent);
};