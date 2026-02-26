#include "../include/parser.h"

parser::parser()
{
  // inizializzo la mappa a 4 strumenti con numero di bus=-1 e nomi standard: [inst1, -1]
}

parser::parser(std::string &filename)
{
  // inizializza le mappe come fa file passato. se non trova il file chiama il costruttore standard.
}

std::map<std::string, int> parser::get_instr2bus()
{
  return this->instr2bus;
}

std::map<std::string, int> parser::get_instr2ch()
{
  return this->instr2ch;
}

void parser::update_map(std::map<std::string, int> &map, std::string &key, int &value)
{
  auto it = map.find(key);
  if (it != map.end())
    it->second = value;
  else
    map.insert(std::pair<std::string, int>(key, value));
}

void parser::delet_assoc(std::map<std::string, int> &map, std::string &key)
{
  auto it = map.find(key);
  if (it != map.end())
    map.erase(it);
}