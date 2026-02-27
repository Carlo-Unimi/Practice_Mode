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
    try
    {
      this->XR18_PORT = std::stoi(buf);
    }
    catch (const std::exception &)
    {
      mvwprintw(this->content_window, getmaxy(this->content_window) - 2, 25, "[inserire un numero valido]");
      wrefresh(this->content_window);
      napms(2000);
    }
}

void menu::set_timer()
{
  echo();
  curs_set(1);
  char buf[10];
  mvwprintw(this->content_window, getmaxy(this->content_window) - 2, 2, "Practice Mode (minuti): ");
  wgetnstr(this->content_window, buf, 9);
  noecho();
  curs_set(0);

  if (buf[0] != '\0')
    try
    {
      int minutes = std::stoi(buf);
      if (minutes <= 0)
      {
        mvwprintw(this->content_window, getmaxy(this->content_window) - 2, 2, "[inserire un numero maggiore di 0]");
        wrefresh(this->content_window);
        napms(2000);
      }
      else
      {
        this->practice_minutes = minutes;
      }
    }
    catch (const std::exception &)
    {
      mvwprintw(this->content_window, getmaxy(this->content_window) - 2, 2, "[inserire un numero valido]");
      wrefresh(this->content_window);
      napms(2000);
    }
}

void menu::reset_timer()
{
  this->practice_start_time = std::chrono::steady_clock::now();
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
  this->starter_scene = this->file_parser->get_starter_scene();

  // inizializza il mixer_controller con l'indirizzo IP e la porta del mixer
  this->mixer_ctrl = new mixer_controller(this->XR18_IP, this->XR18_PORT);
  mixer_ctrl->connect();

  // salva la scena nello snapshot indicato nel file di configurazione
  // this->mixer_ctrl->save_scene(this->starter_scene, "Broken_Scene");

  // crea la finestra del menu
  this->menu_window = newwin(max_y, max_x, 0, 0);
  keypad(this->menu_window, TRUE);
  wtimeout(this->menu_window, 1000); // timeout di 1 secondo per aggiornare il timer
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
    this->content.push_back("Starter Scene: " + std::to_string(this->starter_scene));

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
    this->content = {"Durata Practice Mode: " + std::to_string(this->practice_minutes) + " minuti", "", "[ENTER]: modifica la durata."};
    break;

  //* Exit
  case 4:
    this->content = {"Premi INVIO per uscire dal programma."};
    break;

  //* START-STOP PRACTICE
  case 5:
    if (this->practice_mode)
    {
      auto now = std::chrono::steady_clock::now();
      auto elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(now - this->practice_start_time).count();
      int total_seconds = this->practice_minutes * 60;
      int remaining = total_seconds - (int)elapsed_seconds;
      if (remaining < 0)
        remaining = 0;
      int mins = remaining / 60;
      int secs = remaining % 60;
      std::string timer_str = std::to_string(mins) + ":" + (secs < 10 ? "0" : "") + std::to_string(secs);
      this->content = {"Practice Mode attiva.", "", "Tempo rimanente: " + timer_str, "", "Premi INVIO per terminare."};
    }
    else
    {
      this->content = {"Premi INVIO per iniziare la Practice Mode."};
    }
    break;
  }

  print_content();
}

void menu::start_practice_mode()
{
  // salva la scena nello snapshot indicato nel file di configurazione, in modo da poterla caricare alla fine della Practice Mode
  this->mixer_ctrl->save_scene(this->starter_scene, "Broken_Scene");
  this->mixer_ctrl->save_scene(1, "backup");

  // azzero i volumi dei canali nei bus di ascolto, tranne il canale del proprio strumento nel proprio bus
  for (const auto &pair : this->routing.instr2bus)
  {
    std::string instr = pair.first;
    std::string bus = pair.second;
    std::string ch = this->routing.instr2ch[instr];
    this->mixer_ctrl->zero_bus(bus, ch);
  }
}

void menu::stop_practice_mode(std::string str)
{
  // prova a caricare la scena salvata all'inizio della Practice Mode, se fallisce carica lo snapshot di backup
  if (!this->mixer_ctrl->load_scene(this->starter_scene))
    this->mixer_ctrl->load_scene(1);

  // mostra un messaggio di conferma del termine della Practice Mode
  mvwprintw(this->content_window, getmaxy(this->content_window) - 2, 2, str.c_str());
  wrefresh(this->content_window);
  napms(2000);
}

void menu::run()
{
  while (running)
  {
    draw_options();
    draw_content_window();
    display();

    int ch = wgetch(this->menu_window);

    // controlla se il timer è scaduto
    if (this->practice_mode)
    {
      auto now = std::chrono::steady_clock::now();
      auto elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(now - this->practice_start_time).count();
      if (elapsed_seconds >= this->practice_minutes * 60)
      {
        this->options[5] = "START PRACTICE";
        this->practice_mode = false;
        stop_practice_mode("Tempo scaduto! Practice Mode terminata. Ripristino scena...");
      }
    }

    if (ch == ERR)
      continue; // nessun input, aggiorna solo il display

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
        char buf_instr[100];
        mvwprintw(this->content_window, getmaxy(this->content_window) - 2, 2, "Nuovo nome: ");
        wgetnstr(this->content_window, buf_instr, 49);
        noecho();
        curs_set(0);
        std::string new_key = buf_instr;

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
        char buf_bus[20];
        mvwprintw(this->content_window, getmaxy(this->content_window) - 2, 2, "Nuovo bus: ");
        wgetnstr(this->content_window, buf_bus, 19);
        noecho();
        curs_set(0);

        if (buf_bus[0] != '\0')
        {
          std::string key = it->first;
          std::string val = std::string(buf_bus);
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
        char buf[100];
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
      switch (this->current_option)
      {
      case 3: // modifica timer
        this->set_timer();
        break;
      case 4: // termina programma
        stop_practice_mode("Uscita dal programma. Ripristino scena in corso...");

        this->running = false;
        break;
      case 5: // avvia Practice Mode
        if (this->practice_mode)
        {
          this->options[5] = "START PRACTICE";
          this->practice_mode = false;

          stop_practice_mode("Practice Mode terminata. Scena ripristinata.");
        }
        else
        {
          this->options[5] = "STOP  PRACTICE";
          this->practice_mode = true;
          this->practice_start_time = std::chrono::steady_clock::now();

          start_practice_mode();
        }
        break;
      }
      break;
    }
  }
}