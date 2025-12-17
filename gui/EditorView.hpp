#ifndef EDITORVIEW_H
#define EDITORVIEW_H

#include "../edit/Editor.hpp"
#include "../libs/chesto/src/Element.hpp"
#include "TextInputElement.hpp"
#include "Toolbar.hpp"

using namespace Chesto;

class Toolbar;

class EditorView : public Element
{
public:
	EditorView(Editor* editor);

	Editor* editor = NULL;
	EKeyboard* keyboard = NULL;
	Toolbar* toolbar = NULL;
	std::string* copiedText = NULL;

	std::string* text = NULL;

	// set to false when scrolling away with touch controls
	bool keepCursorOnscreen = true;

	// the big, main text field for the editor
	TextInputElement* mainTextField;
	bool process(InputEvents* e);
	void render(Element* parent);

	void syncText();
	bool copySelection();
	bool pasteSelection();

	void moveCursor(InputEvents* e);
	void reset_bounds();
};
#endif