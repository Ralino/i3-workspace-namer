#include <iostream>
#include <fstream>

#include "ConfigParser.h"
#include "NameFinder.h"
#include "SignalHandler.h"

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
  SignalHandler sh(config_parser::readConfig(&config_file));

  sh.spin();

  return 0;
}
