#include "../gui/EditorView.hpp"
#include "../gui/FileBrowser.hpp"
#include "../gui/MainDisplay.hpp"
#include "../libs/chesto/src/Element.hpp"
#include <algorithm>

#if defined(PC)
#define START_PATH "."
#elif defined(SWITCH)
#define START_PATH "/"
#else
#define START_PATH "/vol/external01/"
#endif

int main(int argc, char* argv[])
{
	// initialize main title screen
	MainDisplay* display = new MainDisplay();

	// the main input handler
	auto events = display->events;
	events->rapidFireRate = 6; // 2x faster directional key repeat

	FileBrowser* fileBrowser = new FileBrowser(START_PATH);
	display->browser = fileBrowser;
	display->elements.push_back(fileBrowser);

	display->mainLoop();

	return 0;
}
