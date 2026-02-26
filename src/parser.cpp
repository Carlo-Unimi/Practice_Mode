#include <sstream>
#include <fstream>

#include "../include/parser.h"

void parser::no_args()
{
  // inizializza la mappa: strumento -> sub
  for (int i = 0; i < 4; i++)
    this->instr2bus.insert(std::pair<std::string, std::string>("instr" + std::to_string(i + 1), "-1"));

  // inizializza la mappa: strumento -> channel
  for (int i = 0; i < 4; i++)
    this->instr2ch.insert(std::pair<std::string, std::string>("instr" + std::to_string(i + 1), "-1"));
}

std::pair<std::string, std::string> parser::parse(std::string &row)
{
  std::string key, value_str;
  std::istringstream iss(row);

  iss >> key >> value_str;

  return std::pair<std::string, std::string>(key, value_str);
}

parser::parser(std::string &filename)
{
  std::ifstream file(filename);
  bool aux = true;
  if (file.is_open())
  {
    std::string line;
    while (std::getline(file, line))
    {
      if (line.empty())
      {
        aux = false;
        continue;
      }

      if (line[0] == '#') // ignora le linee con cancelletto
        continue;

      auto [key, value] = this->parse(line);

      if (aux)
        update_map(this->instr2bus, key, value);
      else
        update_map(this->instr2ch, key, value);
    }
    file.close();
  }
  else
    no_args();
}

std::map<std::string, std::string> parser::get_instr2bus()
{
  return this->instr2bus;
}

std::map<std::string, std::string> parser::get_instr2ch()
{
  return this->instr2ch;
}

void parser::update_map(std::map<std::string, std::string> &map, std::string &key, std::string &value)
{
  auto it = map.find(key);
  if (it != map.end())
    it->second = value;
  else
    map.insert(std::pair<std::string, std::string>(key, value));
}

void parser::delet_assoc(std::map<std::string, std::string> &map, std::string &key)
{
  auto it = map.find(key);
  if (it != map.end())
    map.erase(it);
}