#pragma once

#include <memory>
#include <vector>
#include <set>
#include <string>

#include <i3ipc++/ipc.hpp>

#include "NameFinder.h"

class WorkspaceManager
{
public:
  WorkspaceManager(NameFinder name_finder, std::shared_ptr<i3ipc::connection> i3_conn);

  void setWorkspaceName(std::string new_name, unsigned ws_number);

  void addWindow(uint64_t id, std::string wm_class, std::string title, unsigned ws_number);
  void removeWindow(uint64_t id, unsigned ws_number);

private:
  struct Window
  {
    uint64_t id;
    std::string wm_class;
    std::string title;

    NameFinder::NameIndex name_index;
  };
  struct Workspace
  {
    std::string current_name;
    std::set<std::shared_ptr<Window> > window_ids;
    std::set<std::shared_ptr<Window> > window_indices;
  };

  NameFinder m_name_finder;
  std::shared_ptr<i3ipc::connection> m_i3_conn;

  std::vector<Workspace> m_workspaces;
};
