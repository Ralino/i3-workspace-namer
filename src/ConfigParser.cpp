#include "ConfigParser.h"

NameFinder config_parser::readConfig(std::istream* config)
{
  Json::Value root;
  *config >> root;

  const Json::Value names = root["names"];
  if (names.isNull())
  {
    throw std::runtime_error("Failed to find field 'names' in config");
  }

  NameFinder name_finder;

  for (unsigned i = 0; i < names.size(); i++)
  {
    const std::string name = names[i].get("name", "").asString();
    const std::string wm_class = names[i].get("class", "").asString();
    const std::string window_name = names[i].get("window_name", "").asString();

    name_finder.addName(name, wm_class, window_name);
  }

  return name_finder;
}
