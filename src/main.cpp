#include <iostream>
#include <fstream>
#include <memory>

#include <i3ipc++/ipc.hpp>

#include "ConfigParser.h"
#include "NameFinder.h"
#include "SignalHandler.h"
#include "WorkspaceManager.h"

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
  std::shared_ptr<i3ipc::connection> conn = std::make_shared<i3ipc::connection>();

  std::shared_ptr<WorkspaceManager> ws_manager = std::make_shared<WorkspaceManager>(
      config_parser::readConfig(&config_file), conn);
  SignalHandler sh(conn, ws_manager);

  while (true)
  {
    conn->handle_event();
  }

  return 0;
}
