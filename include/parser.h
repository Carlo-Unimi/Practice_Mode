#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <map>

/**
 * @class parser
 * @brief parsa i file che descrivono il routing degli strumenti e bus nella scena.
 */
class parser
{
private:
  std::string filename;                     // nome del file da cui caricare la configurazione del routing
  std::map<std::string, int> instr2bus;     // mappa: instrument -> bus
  std::map<std::string, int> instr2ch;      // mappa: instrument -> input channel

  /**
   * @brief restituisce 'key' e 'value' da una riga del file di configurazione.
   * @param map mappa in cui inserire l'associazione
   * @param row riga del file di config
   */
  std::pair<std::string, int> parse(std::string &row);

public:
  /**
   * @brief costruttore di 'parser': inizializza un routing standard [instN, -1].
   */
  parser();

  /**
   * @brief costruttore di 'parser': prende informazioni da un file di configurazione.
   */
  parser(std::string &filename);

  /**
   * @brief distruttore del parser (default)
   */
  ~parser() = default;

  /**
   * @brief getter per il routing dei bus
   * @return instr2bus
   */
  std::map<std::string, int> get_instr2bus();

  /**
   * @brief getter per il routing dei canali input
   * @return instr2ch
   */
  std::map<std::string, int> get_instr2ch();

  /**
   * @brief inserisce in una mappa la coppia [string, int]; se la chiave 'string' è già presente, modifica il valore 'int' associato.
   * @param map mappa su cui operare
   * @param key chiave da inserire / modificare
   * @param value valore da inserire / modificare
   */
  void update_map(std::map<std::string, int> &map, std::string &key, int &value);

  /**
   * @brief elimina in una mappa la coppia [key, value]; se la 'key' passata come parametro non è all'interno della mappa non restituisce errori.
   * @param map mappa su cui operare
   * @param key chiave da eliminare
   */
  void delet_assoc(std::map<std::string, int> &map, std::string &key);
};

#endif