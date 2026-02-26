#include <stdio.h>
#include <vector>
#include <iostream>

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

bool parse_arguments(int argc, char *argv[], std::string &filename)
{
  // se ci sono più di due argomenti OPPURE ce ne sono due e uno di questi è '--help' visualizza istruzioni
  if ((argc == 2 && std::string(argv[1]) == "--help") || argc > 2)
  {
    std::cout << "Utilizzo: " << argv[0] << " [path_al_file]\n" << std::endl;
    std::cout << "  --help     Mostra questo messaggio\n\n" << std::endl;
    return true;
  }

  // se ci sono esattamente due argomenti, il secondo è il nome del file da parsare
  if (argc == 2)
    filename = std::string(argv[1]);
  
  return false;
  
}

//* PROGRAM HEAD
int main(int argc, char *argv[])
{
  std::string filename = "";

  if (parse_arguments(argc, argv, filename))
    return 0;

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
  std::vector<std::string> options = {"Mixer", "Bus", "Channels", "Timer", "Exit", "START PRACTICE"};

  menu start_menu(title, options);

  start_menu.run();

  endwin();
  return 0;
}