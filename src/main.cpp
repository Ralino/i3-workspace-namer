#include <iostream>
#include <fstream>
#include <i3ipc++/ipc.hpp>
#include <json/value.h>

#include "ConfigParser.h"

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    std::cout << "Missing argument: config file" << std::endl;
    return 1;
  }

  std::ifstream config_file(argv[1]);
  if (!(config_file.is_open() && config_file.good()))
  {
    std::cout << "Failed to open config file " << argv[1] << std::endl;
    return 2;
  }
  config_parser::readConfig(&config_file);

  i3ipc::connection conn;
  //Listen to i3ipc

  return 0;
}
