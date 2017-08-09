#include "ConfigParser.h"

void config_parser::readConfig(std::istream* config)
{

  Json::Value root;
  *config >> root;

}
