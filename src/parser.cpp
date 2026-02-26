#include <sstream>

#include "../include/parser.h"

parser::parser()
{
  // inizializza la mappa: strumento -> sub
  for (int i=0; i<4; i++)
    this->instr2bus.insert(std::pair<std::string, int>("instr" + std::to_string(i+1), -1));

  // inizializza la mappa: strumento -> channel
  for (int i=0; i<4; i++)
    this->instr2ch.insert(std::pair<std::string, int>("instr" + std::to_string(i+1), -1));
}

std::pair<std::string, int> parser::parse(std::string &row)
{
  std::string key, value_str;
  int value;
  std::istringstream iss(row);

  iss >> key >> value_str;
  value = std::stoi(value_str);

  return std::pair<std::string, int>(key, value);
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