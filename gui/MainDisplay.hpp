#include "../libs/hb-appstore/gui/Element.hpp"
#include "../libs/hb-appstore/gui/ImageCache.hpp"
#include "../libs/hb-appstore/gui/ImageElement.hpp"
#include "../libs/hb-appstore/gui/TextElement.hpp"
#include <unordered_map>

class MainDisplay : public Element
{
public:
	MainDisplay();
	bool process(InputEvents* event);
	void render(Element* parent);
	void background(int r, int g, int b);
	void update();

	TextElement* notice = NULL;

	static SDL_Renderer* mainRenderer;
	static Element* subscreen;
	static MainDisplay* mainDisplay;

	ImageCache* imageCache = NULL;
	bool error = false;

	int lastFrameTime = 99;

	bool showingSplash = true;
	int count = 0;
};

void quit();
