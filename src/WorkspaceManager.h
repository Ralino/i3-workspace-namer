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
   * Resets all workspace names
   **/
  void updateAllWorkspaces();
  /**
   * Sets a new name for the workspace
   **/
  void updateWorkspaceName(std::string new_name, unsigned ws_number);
  /**
   * Remove workspace
   **/
  void removeWorkspace(unsigned ws_num);
  /**
   * register new workspace
   **/
  void initWorkspace(unsigned ws_num, std::string name);
  /**
   * Changes workspace name (when the workspace is changed from a different process)
   **/
  void changeWorkspaceName(unsigned ws_num, std::string new_name);
  /**
   * Add a new window to the workspace ws_number. If ws_number is 0 (default),
   * this will search for the window in the tree and add it to the appropriate workspace
   * Updates the workspace name if necessary
   **/
  void addWindow(uint64_t id, std::string wm_class, std::string title, unsigned ws_number = 0);
  /**
   * Change title to {@param new_title} of window {@param id}.
   * Updates the workspace name if necessary
   **/
  void changeWindowTitle(uint64_t id, std::string new_title);
  /**
   * Removes the window {@param id}
   * Updates the workspace name if necessary
   * @return workspace number the window was removed from
   **/
  unsigned removeWindow(uint64_t id);
  /**
   * Move existing window
   **/
  void moveWindow(uint64_t id);

private:
  struct Window
  {
    uint64_t id;
    unsigned ws_number;
    std::string wm_class;
    std::string title;

    NameFinder::NameIndex name_index;
  };
  struct Workspace
  {
    unsigned ws_number;
    std::string current_name;
    std::multimap<NameFinder::NameIndex, std::shared_ptr<Window>> window_indices;
  };

  NameFinder m_name_finder;
  std::shared_ptr<i3ipc::connection> m_i3_conn;

  std::map<uint64_t, std::shared_ptr<Window>> m_window_ids;

  std::map<unsigned, std::shared_ptr<Workspace>> m_workspaces;

  //helper funcs
  unsigned findWindow(uint64_t win_id);
};
