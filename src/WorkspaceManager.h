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

  /**
   * Reset the cached window tree and rebuild it from i3ipc.get_tree
   * Re-sets all workspace names
   **/
  void updateAllWorkspaces();
  /**
   * Sets a new name for the workspace
   **/
  void setWorkspaceName(std::string new_name, unsigned ws_number);
  /**
   * Tell i3 about the workspace name for workspace {@param ws_number}
   **/
  void updateWorkspace(unsigned ws_number);

  /**
   * Add a new window to the workspace {@param ws_number}.
   * Updates the workspace name if necessary
   **/
  void addWindow(uint64_t id, std::string wm_class, std::string title, unsigned ws_number);
  /**
   * Removes the window {@param id} from the workspace {@param ws_number}.
   * Updates the workspace name if necessary
   **/
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
    Workspace(unsigned index);

    std::string current_name;
    std::set<std::shared_ptr<Window>, std::function<bool(std::shared_ptr<Window>, std::shared_ptr<Window>)> > window_ids;
    std::multiset<std::shared_ptr<Window>, std::function<bool(std::shared_ptr<Window>, std::shared_ptr<Window>)> > window_indices;
  };

  NameFinder m_name_finder;
  std::shared_ptr<i3ipc::connection> m_i3_conn;

  std::vector<Workspace> m_workspaces;
};
