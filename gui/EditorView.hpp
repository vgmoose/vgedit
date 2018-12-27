#include "../libs/hb-appstore/gui/Element.hpp"
#include "../edit/Editor.hpp"
#include "TextInputElement.hpp"

class EditorView : public Element
{
  public:
  EditorView(Editor* editor);

  Editor* editor = NULL;

  // the big, main text field for the editor
  TextInputElement* mainTextField;
  bool process(InputEvents* e);
  void render(Element* parent);

  void reset_bounds();
};