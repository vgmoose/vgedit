#include "../edit/Editor.hpp"
#include "../libs/chesto/src/Element.hpp"
#include "TextInputElement.hpp"
#include "Toolbar.hpp"

class EKeyboard;

class EditorView : public Element
{
public:
	EditorView(Editor* editor);

	Editor* editor = NULL;
	EKeyboard* keyboard = NULL;
	Toolbar* toolbar = NULL;
	std::string* copiedText = NULL;

	// the big, main text field for the editor
	TextInputElement* mainTextField;
	bool process(InputEvents* e);
	void render(Element* parent);

	void syncText();
	bool copySelection();
	bool pasteSelection();

	void reset_bounds();
};