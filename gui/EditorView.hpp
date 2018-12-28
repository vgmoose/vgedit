#include "../libs/hb-appstore/gui/Element.hpp"
#include "../edit/Editor.hpp"
#include "TextInputElement.hpp"

class Keyboard;

class EditorView : public Element
{
  public:
  EditorView(Editor* editor);

  Editor* editor = NULL;
  Keyboard* keyboard = NULL;
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