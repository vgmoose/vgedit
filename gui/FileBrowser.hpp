#include "../libs/hb-appstore/gui/ListElement.hpp"
#include <string>

class FileBrowser : public ListElement
{
  public:
  FileBrowser(const char* pwd);
  bool process(InputEvents* event);
  void render(Element* parent);
  void listfiles();

  std::string* pwd = NULL;
};