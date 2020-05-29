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
	InputEvents* events = new InputEvents();

	FileBrowser* fileBrowser = new FileBrowser(START_PATH);
	display->browser = fileBrowser;
	display->elements.push_back(fileBrowser);

	while (!display->exitRequested)
	{
		bool atLeastOneNewEvent = false;
		bool viewChanged = false;

		int frameStart = CST_GetTicks();

		// get any new input events
		while (events->update())
		{
			// process the inputs of the supplied event
			viewChanged |= display->process(events);
			atLeastOneNewEvent = true;
		}

		// one more event update if nothing changed or there were no previous events seen
		// needed to non-input related processing that might update the screen to take place
		if ((!atLeastOneNewEvent && !viewChanged) || display->showingSplash)
		{
			events->update();
			viewChanged |= display->process(events);
		}

		// draw the display if we processed an event or the view
		if (viewChanged)
			display->render(NULL);
		else
		{
			// delay for the remainder of the frame to keep up to 60fps
			// (we only do this if we didn't draw to not waste energy
			// if we did draw, then proceed immediately without waiting for smoother progress bars / scrolling)
			int delayTime = (CST_GetTicks() - frameStart);
			if (delayTime < 0)
				delayTime = 0;
			if (delayTime < 16)
				CST_Delay(16 - delayTime);
		}

		// free up any elements that are in the trash
		display->recycle();

		display->showingSplash = false;
	}

  // 👋
  delete display;

	return 0;
}
