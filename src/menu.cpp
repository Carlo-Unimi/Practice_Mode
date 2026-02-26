#include "../include/menu.h"

void menu::print_content()
{
  for (size_t i = 0; i < this->content.size(); i++)
    mvwprintw(this->content_window, 1 + i, 2, this->content[i].c_str());
}

void menu::draw_option_line(int h)
{
  int total_length = 1;
  for (const auto &opt : this->options)
    total_length += opt.length() + 4;

  std::string line(total_length, '-');
  mvwprintw(this->menu_window, this->title.size() + h, 2, line.c_str());
}

void menu::draw_options()
{
  int aux = 2;
  this->draw_option_line(2);
  for (size_t i = 0; i < this->options.size(); i++)
  {
    mvwprintw(this->menu_window, this->title.size() + 3, aux, "|");
    if (i == this->current_option)
      wattron(this->menu_window, A_REVERSE);
    mvwprintw(this->menu_window, this->title.size() + 3, aux + 2, this->options[i].c_str());
    if (i == this->current_option)
      wattroff(this->menu_window, A_REVERSE);
    aux += this->options[i].length() + 4;
  }
  mvwprintw(this->menu_window, this->title.size() + 3, aux, "|");
  this->draw_option_line(4);
}

void menu::display()
{
  wrefresh(this->menu_window);
  wrefresh(this->content_window);
}

menu::menu(std::vector<std::string> &title, std::vector<std::string> &options, std::string &filename) : title(title), options(options)
{
  int max_y = getmaxy(stdscr);
  int max_x = getmaxx(stdscr);
  this->running = true;
  this->current_option = 0;

  // parsa il file di config passato da riga di comando e popola 'routing'
  this->file_parser = new parser(filename);
  this->routing.instr2bus = this->file_parser->get_instr2bus();
  this->routing.instr2ch = this->file_parser->get_instr2ch();

  // crea la finestra del menu
  this->menu_window = newwin(max_y, max_x, 0, 0);
  keypad(this->menu_window, TRUE);
  box(this->menu_window, 0, 0);

  // crea la finestra dei contenuti
  this->content_window = derwin(this->menu_window, max_y - 12, max_x - 4, 11, 2);
  box(this->content_window, 0, 0);

  // stampa il titolo centrato
  for (size_t i = 0; i < this->title.size(); i++)
    mvwprintw(this->menu_window, i + 1, (max_x - this->title[i].length()) / 2, this->title[i].c_str());

  menu::draw_options();
}

menu::~menu()
{
  if (this->file_parser != nullptr)
    delete this->file_parser;

  delwin(this->content_window);
  delwin(this->menu_window);
}

void menu::draw_content_window()
{
  wclear(this->content_window);
  box(this->content_window, 0, 0);

  switch (this->current_option)
  {
  //* Mixer config
  case 0:
    break;

  //* Bus config
  case 1:
    this->content.clear();
    this->content.push_back("Strumento -> Bus");
    for (const auto &pair : this->routing.instr2bus)
      this->content.push_back(std::string(pair.first) + " -> " + std::string(pair.second));
    break;

  //* Channels config
  case 2:
    this->content.clear();
    this->content.push_back("Strumento -> Canale");
    for (const auto &pair : this->routing.instr2ch)
      this->content.push_back(std::string(pair.first) + " -> " + std::string(pair.second));
    break;

  //* Timer config
  case 3:
    break;

  //* Exit
  case 4:
    this->content = {"Premi INVIO per uscire dal programma."};
    break;

  //* START-STOP PRACTICE
  case 5:
    this->content = {"Premi INVIO per iniziare la Practice Mode."};
    break;
  }

  print_content();
}

void menu::run()
{
  while (running)
  {
    draw_options();
    draw_content_window();
    display();

    int ch = wgetch(this->menu_window);

    switch (ch)
    {
    //* OPZIONE PRECEDENTE
    case KEY_LEFT:
    case 'a':
      if (this->current_option > 0)
				this->current_option--;
			else
				this->current_option = this->options.size() - 1;
      break;

    //* OPZIONE SUCCESSIVA
    case KEY_RIGHT:
    case 'd':
      if (this->current_option < (int)this->options.size() - 1)
				this->current_option++;
			else
				this->current_option = 0;
      break;

    //* ENTER
    case 10:

      if (current_option == 4)
        running = false;
      break;
    }
  }
}