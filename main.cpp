#include "gui/EditorView.hpp"
#include "gui/FileBrowser.hpp"
#include "gui/MainDisplay.hpp"
#include "libs/hb-appstore/gui/Element.hpp"
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

	bool running = true;
	while (running)
	{
		bool atLeastOneNewEvent = false;
		bool viewChanged = false;

		int frameStart = SDL_GetTicks();

		// get any new input events
		while (events->update())
		{
			// process the inputs of the supplied event
			viewChanged |= display->process(events);
			atLeastOneNewEvent = true;

			// quit on enter/start
			if (events->released(SELECT_BUTTON))
			{
				if (display->editorView == NULL)
					running = false;
				else
				{
					display->closeEditor();
					viewChanged = true;
				}
			}
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
			int delayTime = (SDL_GetTicks() - frameStart);
			if (delayTime < 0)
				delayTime = 0;
			if (delayTime < 16)
				SDL_Delay(16 - delayTime);
		}

		MainDisplay::mainDisplay->showingSplash = false;
	}

	my_quit();

	return 0;
}
