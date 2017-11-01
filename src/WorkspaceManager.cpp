#include <sstream>
#include <iostream>

#include "WorkspaceManager.h"

WorkspaceManager::WorkspaceManager(NameFinder name_finder,
                                   std::shared_ptr<i3ipc::connection> i3_conn)
  : m_name_finder(name_finder)
  , m_i3_conn(i3_conn)
  , m_workspaces()
{
  updateAllWorkspaces();
}

void WorkspaceManager::updateAllWorkspaces()
{
  m_workspaces.clear();
  for (unsigned index = 0; index < 10; index++)
  {
    m_workspaces.emplace_back(index);
  }
  auto root = m_i3_conn->get_tree();
  unsigned ws_number = 10;
  bool set_name = true;
  std::function<void(std::shared_ptr<i3ipc::container_t>)> depth_first = [&] (std::shared_ptr<i3ipc::container_t> root) -> void
  {
    std::cout << root->name << ", " << root->ws_num << ", " << (root->xwindow_id? root->window_properties.wm_class : "")  << std::endl;
    if (root->type == "workspace" && root->name != "__i3_scratch")
    {
      if (root->ws_num < 1 || root->ws_num > 10)
      {
        std::cerr << "Unexpected ws number" << std::endl;
        return;
      }
      if (!set_name) //Check if last workspace's name was set
      {
        setWorkspaceName("", ws_number);
      }
      set_name = false;
      ws_number = root->ws_num - 1;
      m_workspaces[ws_number].current_name = root->name;
    }
    if (root->xwindow_id && ws_number < 10)
    {
      addWindow(root->xwindow_id, root->window_properties.wm_class, root->window_properties.title, ws_number);
      set_name = true;
    }
    for (auto child : root->nodes)
    {
      depth_first(child);
    }
    for (auto floating_child : root->floating_nodes)
    {
      depth_first(floating_child);
    }
  };
  depth_first(root);
}

void WorkspaceManager::setWorkspaceName(std::string new_name, unsigned ws_number)
{
  if (new_name.empty())
  {
    new_name = "Empty";
  }
  std::stringstream ss;
  ss << ws_number + 1 << ": " << new_name;

  //if (std::string(ss.str()) != m_workspaces[ws_number].current_name)
  //{
  m_workspaces[ws_number].current_name = ss.str();
  updateWorkspace(ws_number);
  //}
}

void WorkspaceManager::updateWorkspace(unsigned ws_number)
{
  //TODO set actual name with i3_conn
  std::cout << "Set a new name: \"" << m_workspaces[ws_number].current_name << "\"" << std::endl;
}

void WorkspaceManager::addWindow(
    uint64_t id, std::string wm_class, std::string title, unsigned ws_number)
{
  std::shared_ptr<Window> new_window = std::make_shared<Window>();
  new_window->id = id;
  new_window->wm_class = wm_class;
  new_window->title = title;
  new_window->name_index = m_name_finder.getNameIndex(wm_class, title);

  m_workspaces[ws_number].window_ids.insert(new_window);

  auto it = m_workspaces[ws_number].window_indices.insert(new_window);
  if (it == m_workspaces[ws_number].window_indices.cbegin())
  {
    std::string new_ws_name = m_name_finder.getName(new_window->name_index);
    if (new_ws_name == "CLASS")
    {
      new_ws_name = new_window->wm_class;
    }
    setWorkspaceName(new_ws_name, ws_number);
  }
}

void WorkspaceManager::removeWindow(uint64_t id, unsigned ws_number)
{
  std::shared_ptr<Window> comp_win = std::make_shared<Window>();
  comp_win->id = id;

  auto id_it = m_workspaces[ws_number].window_ids.find(comp_win);
  if (id_it == m_workspaces[ws_number].window_ids.cend())
  {
    std::cerr << "Could not found window " << id << " on workspace " << ws_number << std::endl;
    return;
  }
  else
  {
    auto it = m_workspaces[ws_number].window_indices.find(*id_it);
    if (it == m_workspaces[ws_number].window_ids.cbegin())
    {
      auto next = it;
      next++;
      if (next == m_workspaces[ws_number].window_ids.cend())
      {
        //Removing last window
        setWorkspaceName("", ws_number);
      }
      else
      {
        std::string new_ws_name = m_name_finder.getName((*next)->name_index);
        if (new_ws_name == "CLASS")
        {
          new_ws_name = (*next)->wm_class;
        }
        setWorkspaceName(new_ws_name, ws_number);
      }

      m_workspaces[ws_number].window_ids.erase(id_it);
      m_workspaces[ws_number].window_indices.erase(it);
    }
  }
}

WorkspaceManager::Workspace::Workspace(unsigned index)
  : current_name(std::to_string(index + 1))
  , window_ids([](std::shared_ptr<Window> lhs, std::shared_ptr<Window> rhs) -> bool
                {
                  return lhs->id < rhs->id;
                })
  , window_indices([](std::shared_ptr<Window> lhs, std::shared_ptr<Window> rhs) -> bool
                    {
                      return lhs->name_index < rhs->name_index;
                    })
{
}
