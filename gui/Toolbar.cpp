#include "Toolbar.hpp"

Toolbar::Toolbar(const char* path)
{
	if (strlen(path) > MAX_PATH_LENGTH) {
		strcpy(this->path, "...");
		strcpy(this->path + 3, path + strlen(path) - (MAX_PATH_LENGTH - 3));
	} else {
		strcpy(this->path, path);
	}

	pathE = new TextElement(this->path, 20, 0, MONOSPACED);
	pathE->position(10, 10);
	elements.push_back(pathE);

	SDL_Color white = { 0xFF, 0xFF, 0xFF, 0xFF };

	// toolbar actions
	// TODO: make real touchable buttons, move handler logic here
	actions = new TextElement("(-) Exit    (+) Save    (x) Copy    (y) Paste    (l/r) Select    (b) Delete    (a) Type", 18, &white);
	actions->position(620, 10);

	keyActions = new TextElement("(-) Exit    (+) Save    (x) Caps    (y) Stow Keyboard    (b) Backspace    (a) Type", 18, &white);
	keyActions->position(610, 10);
}

void Toolbar::setModified(bool modified)
{
	this->modified = modified;

	std::string text;
	text += path;
	text += (modified ? "*" : "");

  pathE->setText(text);
  pathE->update();
}

void Toolbar::render(Element* parent)
{
	SDL_Rect background = { 0, 0, 1280, 50 };

	// draw top bar on top of other things
	SDL_SetRenderDrawColor(parent->renderer, 0x0, 0x0, 0x0, 0xFF);
	SDL_RenderFillRect(parent->renderer, &background);

	super::render(this);

	if (keyboardShowing)
		keyActions->render(this);
	else
		actions->render(this);
}