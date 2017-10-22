#include "WorkspaceManager.h"

WorkspaceManager::WorkspaceManager(NameFinder name_finder,
                                   std::shared_ptr<i3ipc::connection> i3_conn)
  : m_name_finder(name_finder)
  , m_i3_conn(i3_conn)
  , m_workspaces()
{
  //TODO get current workspaces and windows
}

void WorkspaceManager::setWorkspaceName(std::string new_name, unsigned ws_number)
{
  //TODO
}

void WorkspaceManager::addWindow(
    uint64_t id, std::string wm_class, std::string title, unsigned ws_number)
{
  //TODO
}

void WorkspaceManager::removeWindow(uint64_t id, unsigned ws_number)
{
  //TODO
}
