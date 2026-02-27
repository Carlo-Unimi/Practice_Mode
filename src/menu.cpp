#include "../include/menu.h"

void menu::set_IP()
{
  echo();
  curs_set(1);
  char buf[100];
  mvwprintw(this->content_window, getmaxy(this->content_window) - 2, 2, "Nuovo IP: ");
  wgetnstr(this->content_window, buf, 99);
  noecho();
  curs_set(0);

  if (buf[0] != '\0')
    this->XR18_IP = std::string(buf);
}

void menu::set_port()
{
  echo();
  curs_set(1);
  char buf[100];
  mvwprintw(this->content_window, getmaxy(this->content_window) - 2, 2, "Nuova porta: ");
  wgetnstr(this->content_window, buf, 99);
  noecho();
  curs_set(0);

  if (buf[0] != '\0')
    try {
      this->XR18_PORT = std::stoi(buf);
    } catch (const std::exception &) {
      mvwprintw(this->content_window, getmaxy(this->content_window) - 2, 25, "[inserire un numero valido]");
      wrefresh(this->content_window);
      napms(2000);
    }
}

void menu::print_content()
{
  for (size_t i = 0; i < this->content.size(); i++)
  {
    if ((this->current_option == 1 || this->current_option == 2) && (int)i == this->selected_item + 2)
      wattron(this->content_window, A_REVERSE);
    mvwprintw(this->content_window, 1 + i, 2, this->content[i].c_str());
    if ((this->current_option == 1 || this->current_option == 2) && (int)i == this->selected_item + 2)
      wattroff(this->content_window, A_REVERSE);
  }
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
  this->selected_item = 0;

  // parsa il file di config passato da riga di comando e popola 'routing'
  this->file_parser = new parser(filename);
  this->routing.instr2bus = this->file_parser->get_instr2bus();
  this->routing.instr2ch = this->file_parser->get_instr2ch();

  // inizializza il mixer_controller con l'indirizzo IP e la porta del mixer
  this->mixer_ctrl = new mixer_controller(this->XR18_IP, this->XR18_PORT);
  mixer_ctrl->connect();

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
}

menu::~menu()
{
  if (this->file_parser != nullptr)
    delete this->file_parser;

  delwin(this->content_window);
  delwin(this->menu_window);
  delete this->mixer_ctrl;
}

void menu::draw_content_window()
{
  wclear(this->content_window);
  box(this->content_window, 0, 0);

  switch (this->current_option)
  {
  //* Mixer config
  case 0:
    this->content.clear();
    this->content.push_back("Mixer IP: " + this->XR18_IP);
    if (this->mixer_ctrl->isConnected)
      this->content.back() += "       [Stato: connesso]";
    else
      this->content.back() += "       [Stato: non connesso]";
    this->content.push_back("Mixer Port: " + std::to_string(this->XR18_PORT));

    this->content.push_back("");
    this->content.push_back("[i]: modifica l'indirizzo IP.");
    this->content.push_back("[p]: modifica la porta di connessione.");
    this->content.push_back("[r]: riprova a connetterti al mixer.");
    break;

  //* Bus config
  case 1:
    this->content.clear();
    this->content = {" [Strumento]   -> [Bus]         [S: str] [B: bus]", ""};
    for (const auto &pair : this->routing.instr2bus)
    {
      int len = 12 - pair.first.length();
      this->content.push_back("  " + std::string(pair.first));
      this->content.back() += std::string(len, ' ') + " ->  " + std::string(pair.second);
    }
    break;

  //* Channels config
  case 2:
    this->content.clear();
    this->content = {" [Strumento]   -> [Canale]      [S: str] [C: ch]", ""};
    for (const auto &pair : this->routing.instr2ch)
    {
      int len = 12 - pair.first.length();
      this->content.push_back("  " + std::string(pair.first));
      this->content.back() += std::string(len, ' ') + " ->  " + std::string(pair.second);
    }
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
      this->selected_item = 0;
      if (this->current_option > 0)
        this->current_option--;
      else
        this->current_option = this->options.size() - 1;
      break;

    //* OPZIONE SUCCESSIVA
    case KEY_RIGHT:
    case 'd':
      this->selected_item = 0;
      if (this->current_option < (int)this->options.size() - 1)
        this->current_option++;
      else
        this->current_option = 0;
      break;

    //* ITEM PRECEDENTE
    case KEY_UP:
    case 'w':
      if ((this->current_option == 1 || this->current_option == 2) && this->selected_item > 0)
        this->selected_item--;
      break;

    //* ITEM SUCCESSIVO
    case KEY_DOWN:
    case 'z':
      if (this->current_option == 1)
      {
        if (this->selected_item < (int)this->routing.instr2bus.size() - 1)
          this->selected_item++;
      }
      else if (this->current_option == 2)
      {
        if (this->selected_item < (int)this->routing.instr2ch.size() - 1)
          this->selected_item++;
      }
      break;

    //* MODIFICA STRUMENTO
    case 's':
    case 'S':
      if (this->current_option == 1 || this->current_option == 2)
      {
        auto it = this->routing.instr2bus.begin();
        if (this->current_option == 1 && this->selected_item < (int)this->routing.instr2bus.size())
          std::advance(it, this->selected_item);
        else if (this->current_option == 2 && this->selected_item < (int)this->routing.instr2ch.size())
        {
          auto it2 = this->routing.instr2ch.begin();
          std::advance(it2, this->selected_item);
          it = this->routing.instr2bus.find(it2->first);
        }
        else
          break;

        if (it == this->routing.instr2bus.end())
          break;

        std::string old_key = it->first;
        std::string bus_val = it->second;
        std::string ch_val = this->routing.instr2ch[old_key];

        echo();
        curs_set(1);
        char buf[15];
        mvwprintw(this->content_window, getmaxy(this->content_window) - 2, 2, "Nuovo nome: ");
        wgetnstr(this->content_window, buf, 99);
        noecho();
        curs_set(0);
        std::string new_key = buf;

        if (!new_key.empty() && new_key != old_key)
        {
          // rimuove la vecchia associazione
          this->file_parser->delete_assoc(this->routing.instr2bus, old_key);
          this->file_parser->delete_assoc(this->routing.instr2ch, old_key);

          // inserisce la nuova associazione
          this->file_parser->update_map(this->routing.instr2bus, new_key, bus_val);
          this->file_parser->update_map(this->routing.instr2ch, new_key, ch_val);
        }
      }
      break;

    //* MODIFICA BUS
    case 'b':
    case 'B':
      if (this->current_option == 1 && this->selected_item < (int)this->routing.instr2bus.size())
      {
        auto it = this->routing.instr2bus.begin();
        std::advance(it, this->selected_item);

        echo();
        curs_set(1);
        char buf[5];
        mvwprintw(this->content_window, getmaxy(this->content_window) - 2, 2, "Nuovo bus: ");
        wgetnstr(this->content_window, buf, 99);
        noecho();
        curs_set(0);

        if (buf[0] != '\0')
        {
          std::string key = it->first;
          std::string val = std::string(buf);
          this->file_parser->update_map(this->routing.instr2bus, key, val);
        }
      }
      break;

    //* MODIFICA CANALE
    case 'c':
    case 'C':
      if (this->current_option == 2 && this->selected_item < (int)this->routing.instr2ch.size())
      {
        auto it = this->routing.instr2ch.begin();
        std::advance(it, this->selected_item);

        echo();
        curs_set(1);
        char buf[5];
        mvwprintw(this->content_window, getmaxy(this->content_window) - 2, 2, "Nuovo canale: ");
        wgetnstr(this->content_window, buf, 99);
        noecho();
        curs_set(0);

        if (buf[0] != '\0')
        {
          std::string key = it->first;
          std::string val = std::string(buf);
          this->file_parser->update_map(this->routing.instr2ch, key, val);
        }
      }
      break;
    
    //* RICONNETTI AL MIXER
    case 'r':
    case 'R':
      this->mixer_ctrl->connect();
      mvwprintw(this->content_window, getmaxy(this->content_window) - 2, 2, "Riconnessione al mixer in corso...");
      wrefresh(this->content_window);
      napms(2000);
      break;
    
    //* MODIFICA IP
    case 'i':
    case 'I':
      this->set_IP();
      break;
    
    //* MODIFICA PORTA
    case 'p':
    case 'P':
      this->set_port();
      break;

    //* ENTER
    case 10:
      if (current_option == 4)
        running = false;
      break;
    }
  }
}