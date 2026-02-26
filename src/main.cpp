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

//* PROGRAM HEAD
int main(int argc, char **argv[])
{
  initscr();
  noecho();
  cbreak();
  curs_set(0);
  refresh();

  std::vector<std::string> title = {
      " _____             _   _            _____       _     ",
      "|  _  |___ ___ ___| |_|_|___ ___   |     |___ _| |___ ",
      "|   __|  _| .'|  _|  _| |  _| -_|  | | | | . | . | -_|",
      "|__|  |_| |__,|___|_| |_|___|___|  |_|_|_|___|___|___|",
      "                                                      "};
  std::vector<std::string> options = {"Mixer", "Bus", "Channels", "Timer", "START PRACTICE"};

  menu start_menu(title, options);

  start_menu.run();

  endwin();
  return 0;
}