#include "MainDisplay.hpp"
#include "EditorView.hpp"
#include "FileBrowser.hpp"

#if defined(SWITCH)
#include <switch.h>
#elif defined(__WIIU__)
#include <romfs-wiiu.h>
#endif

SDL_Renderer* MainDisplay::mainRenderer = NULL;
Element* MainDisplay::subscreen = NULL;
MainDisplay* MainDisplay::mainDisplay = NULL;

MainDisplay::MainDisplay()
{
	// populate image cache with any local version info if it exists
	this->imageCache = new ImageCache("tmp");

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0)
	{
		//        printf("SDL init failed: %s\n", SDL_GetError());
		return;
	}

	if (TTF_Init() < 0)
	{
		//        printf("SDL ttf init failed: %s\n", SDL_GetError());
		return;
	}

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		//        printf("SDL image init failed: %s\n", SDL_GetError());
		return;
	}

	// initialize teh romfs for switch/wiiu
#if defined(SWITCH) || defined(__WIIU__)
	romfsInit();
#endif

	int height = 720;
	int width = 1280;

	this->window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 0);
	this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);

	//Detach the texture
	SDL_SetRenderTarget(this->renderer, NULL);

	MainDisplay::mainRenderer = this->renderer;
	MainDisplay::mainDisplay = this;

	for (int i = 0; i < SDL_NumJoysticks(); i++)
	{
		if (SDL_JoystickOpen(i) == NULL)
		{
			SDL_Quit();
			return;
		}
	}
}

void MainDisplay::openFile(bool folder, std::string* path)
{
	// don't allow a file to be opened if we're already showing an editor
	if (editorView != NULL)
		return;

	if (folder)
	{
		browser->update_path(path->c_str());
		browser->y = 0;
		browser->listfiles();
	}
	else
	{
		Editor* editor = new Editor(path->c_str());
		editorView = new EditorView(editor);
		this->elements.push_back(editorView);
	}
}

bool MainDisplay::process(InputEvents* event)
{
	if (MainDisplay::subscreen)
		return MainDisplay::subscreen->process(event);

	// keep processing child elements
	return super::process(event) || showingSplash;
}

void MainDisplay::render(Element* parent)
{
	// set the background color
	MainDisplay::background(0x42, 0x45, 0x48);

	// if (MainDisplay::subscreen)
	// {
	// 	MainDisplay::subscreen->render(this);
	// 	this->update();
	// 	return;
	// }

	// render the rest of the subelements
	super::render(this);

	// commit everything to the screen
	this->update();
}

void MainDisplay::closeEditor()
{
	elements.erase(elements.begin() + 1); // second element should be the editor (TODO: something smarter)
	delete editorView;
	editorView = NULL;
}

void MainDisplay::background(int r, int g, int b)
{
	SDL_SetRenderDrawColor(this->renderer, r, g, b, 0xFF);
	SDL_RenderFillRect(this->renderer, NULL);
}

void MainDisplay::update()
{
	// never exceed 60fps because there's no point

	//    int now = SDL_GetTicks();
	//    int diff = now - this->lastFrameTime;
	//
	//    if (diff < 16)
	//        return;

	SDL_RenderPresent(this->renderer);
	//    this->lastFrameTime = now;
}

void my_quit()
{
	IMG_Quit();
	TTF_Quit();

	SDL_Delay(10);
	SDL_DestroyWindow(MainDisplay::mainDisplay->window);

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_Quit();

#if defined(__WIIU__)
	romfsExit();
#endif

	exit(0);
}
