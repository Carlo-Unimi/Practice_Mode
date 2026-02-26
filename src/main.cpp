#include <stdio.h>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <ncurses.h>
#include "../oscpkt.hh"

#include "menu.h"

#define XR18_IP "192.168.1.xxx"
#define XR18_PORT 10024

//* PROGRAM HEAD
int main(int argc, char **argv[])
{
  initscr();
  noecho();
  cbreak();
  curs_set(0);
  refresh();

  std::vector<std::string> title = {
      "",
      "",
      "",
      "",
      ""};
  std::vector<std::string> options = {};

  menu start_menu(title, options);

  start_menu.run();

  endwin();
  return 0;
}