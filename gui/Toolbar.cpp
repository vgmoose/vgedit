#include "Toolbar.hpp"
#include "../libs/chesto/src/Container.hpp"
#include "../libs/chesto/src/Button.hpp"

Toolbar::Toolbar(const char* path, EditorView* editorView)
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

	Container* con = new Container(ROW_LAYOUT, 10);
	elements.push_back(con);

	con->add((new Button("Exit", SELECT_BUTTON))->setAction([](){
		printf("TODO: Implement exit here\n");
	}));

	con->add((new Button("Save", START_BUTTON))->setAction([](){
		printf("TODO: Implement exit here\n");
	})),

	con->add((new Button("Copy", X_BUTTON))->setAction([](){
		printf("TODO: paste\n");
	}));

	con->add((new Button("Paste", Y_BUTTON))->setAction([](){
		printf("TODO: paste\n");
	}));

	con->add((new Button("", L_BUTTON))->setAction([](){
		printf("TODO: left dir\n");
	}));

	con->add((new Button("Select", R_BUTTON))->setAction([](){
		printf("TODO: right dir\n");
	}));

	con->add((new Button("Delete", B_BUTTON))->setAction([](){
		printf("TODO: Type\n");
	}));

	con->add((new Button("Type", A_BUTTON))->setAction([](){
		printf("TODO: Type\n");
	}));

	con->position(SCREEN_WIDTH - 10 - con->width, 40);
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
	// SDL_SetRenderDrawColor(parent->renderer, 0x0, 0x0, 0x0, 0xFF);
	// SDL_RenderFillRect(parent->renderer, &background);

	super::render(this);

	// if (keyboardShowing)
	// 	keyActions->render(this);
	// else
	// 	actions->render(this);
}