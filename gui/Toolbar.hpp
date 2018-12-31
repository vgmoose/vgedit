#include "../libs/hb-appstore/gui/TextElement.hpp"


class Toolbar : public Element
{
  public:
  Toolbar(const char* path);
  void setModified(bool modified);
  void render(Element* parent);

  TextElement* pathE = NULL;
  TextElement* actions = NULL;
  TextElement* keyActions = NULL;

  const char* path = NULL;
  bool modified = true;
  bool keyboardShowing = false;
};