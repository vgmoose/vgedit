#include "MainDisplay.hpp"

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
	// romfsInit();
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
			//                printf("SDL_JoystickOpen: %s\n", SDL_GetError());
			SDL_Quit();
			return;
		}
	}

	// set up the SDL needsRender event
	this->needsRender.type = SDL_USEREVENT;


	this->error = 0;

	// create the first two elements (icon and app title)
	ImageElement* icon = new ImageElement(ROMFS "res/icon.png");
	icon->position(330 + this->error * 140, 255 - this->error * 230);
	icon->resize(70 - this->error * 35, 70 - this->error * 35);
	this->elements.push_back(icon);

	TextElement* title = new TextElement("Homebrew App Store", 50 - this->error * 25);
	title->position(415 + this->error * 100, 255 - this->error * 230);
	this->elements.push_back(title);

	if (this->error)
	{
	
	}
}

bool MainDisplay::process(InputEvents* event)
{
  if (MainDisplay::subscreen)
    return MainDisplay::subscreen->process(event);
  // keep processing child elements
  return super::process(event);
}

void MainDisplay::render(Element* parent)
{
	// set the background color
	MainDisplay::background(0x42, 0x45, 0x48);

	if (MainDisplay::subscreen)
	{
		MainDisplay::subscreen->render(this);
		this->update();
		return;
	}

	// render the rest of the subelements
	super::render(this);

	// commit everything to the screen
	this->update();
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

void quit()
{
	IMG_Quit();
	TTF_Quit();

	SDL_Delay(10);
	SDL_DestroyWindow(MainDisplay::mainDisplay->window);

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_Quit();

#if defined(__WIIU__)
	// romfsExit();
#endif

	exit(0);
}
