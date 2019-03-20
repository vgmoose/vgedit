#include "../libs/hb-appstore/gui/Element.hpp"
#include "../libs/hb-appstore/gui/ImageCache.hpp"
#include "../libs/hb-appstore/gui/ImageElement.hpp"
#include "../libs/hb-appstore/gui/TextElement.hpp"
#include <unordered_map>

// TODO: don't forward declare
class FileBrowser;
class EditorView;

class MainDisplay : public Element
{
public:
	MainDisplay();
	bool process(InputEvents* event);
	void render(Element* parent);
	void background(int r, int g, int b);
	void update();
	void closeEditor();
	void openFile(bool folder, std::string* path);

	TextElement* notice = NULL;
	FileBrowser* browser = NULL;
	EditorView* editorView = NULL;

	static SDL_Renderer* mainRenderer;
	static Element* subscreen;
	static MainDisplay* mainDisplay;

	char path_string[2048];

	ImageCache* imageCache = NULL;
	bool error = false;

	int lastFrameTime = 99;

	bool showingSplash = true;
	int count = 0;
};

void my_quit();
