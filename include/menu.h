#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>
#include <map>

#include <ncurses.h>

#include "parser.h"

/**
 * @struct info
 * @brief contiene le informazioni riguardanti il canale di entrata e il bus di ogni membro della band. 
 */
struct info
{
  std::map<std::string, int> instr2bus;   // indica per ogni 'strumento' presente, il 'bus' che utilizza per gli ascolti. 
  std::map<std::string, int> instr2ch;    // indica per ogni 'strumento' presente, il 'canale' con cui entra nel mixer. 
};

/**
 * @class menu
 * @brief si occupa della visualizzazione delle informazioni a terminale.
 */
class menu
{
private:
  bool running;                           // 'true' se il programma è in esecuzione, 'false' altrimenti
  int current_option;                     // opzione selezionata sul menu
  std::vector<std::string> options;       // titoli delle opzioni del menu
  std::vector<std::string> title;         // ASCII art 'Practice Mode'
  std::vector<std::string> content;       // informazioni da stampare nella finestra dei contenuti

  info routing;                           // informazioni di routing della scena
  parser *file_parser = nullptr;          // inizializza e modifica 'routing'

  std::string XR18_IP = "192.168.1.xxx";  // indirizzo IP del mixer
  int XR18_PORT = 10024;                  // numero di porta del mixer

  WINDOW *menu_window;                    // finestra generale del menu
  WINDOW *content_window;                 // finestra in cui vengono stampati i vari contenuti


  /**
   * @brief manda a schermo le modifiche apportate alla finestre.
   */
  void display();

  /**
   * @brief stampa il contenuto nella 'content_window'
   */
  void print_content();

  /**
   * @brief disegna la linea superiore delle opzioni
   * @param h altezza a cui disegnare la linea
   */
  void draw_option_line(int h);

  /**
   * @brief stampa le opzioni del menu (quella corrente è evidenziata)
   */
  void draw_options();

  /**
   * @brief stampa nella finestra dei contenuti la variabile 'content'
   */
  void draw_content_window();
public:
  /**
   * @brief costruttore per la classe 'menu'
   * @param title vettore ASCII art (riga per riga)
   * @param options vettore contenente le opzioni da stampare
   * 
   * inizializza il menu con il titolo e le opzioni date, non lo manda ancora a schermo.
   */
  menu(std::vector<std::string> &title, std::vector<std::string> &options, std::string &filename);

  /**
   * @brief distruttore del menu
   */
  ~menu();

  /**
   * @brief fa partire il programma e gestisce gli input dell'utente per la navigazione nel menu.
   */
  void run();
};

#endif