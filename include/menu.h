#include <string>
#include <vector>

#include <ncurses.h>

class menu
{
private:
public:
  menu::menu(std::vector<std::string> &title, std::vector<std::string> &options);

  void menu::run();
};