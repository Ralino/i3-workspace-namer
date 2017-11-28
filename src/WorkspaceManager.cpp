#include <sstream>
#include <iostream>

#include "WorkspaceManager.h"

WorkspaceManager::WorkspaceManager(NameFinder name_finder,
                                   std::shared_ptr<i3ipc::connection> i3_conn)
  : m_name_finder(name_finder)
  , m_i3_conn(i3_conn)
{
  updateAllWorkspaces();
}

void WorkspaceManager::updateAllWorkspaces()
{
  m_workspaces.clear();
  m_window_ids.clear();

  auto root = m_i3_conn->get_tree();
  unsigned last_ws_number = 0;
  bool set_name = true;
  std::function<void(std::shared_ptr<i3ipc::container_t>)> depth_first = [&] (std::shared_ptr<i3ipc::container_t> root) -> void
  {
    std::cout << root->name << ", " << root->ws_num << ", " << (root->xwindow_id? root->window_properties.wm_class : "")  << std::endl;
    if (root->type == "workspace" && root->name != "__i3_scratch")
    {
      auto new_workspace = std::make_shared<Workspace>();
      new_workspace->ws_number = root->ws_num;
      new_workspace->current_name = root->name;
      m_workspaces.emplace(new_workspace->ws_number, new_workspace);

      if (!set_name) //Check if last workspace's name was set
      {
        updateWorkspaceName("", last_ws_number);
      }
      set_name = false;
      last_ws_number = new_workspace->ws_number;
    }
    if (root->xwindow_id && last_ws_number != 0)
    {
      addWindow(root->id, root->window_properties.wm_class, root->window_properties.title, last_ws_number);
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

void WorkspaceManager::updateWorkspaceName(std::string new_name, unsigned ws_number)
{
  if (new_name.empty())
  {
    new_name = "Empty";
  }
  std::stringstream ss;
  ss << ws_number << ": " << new_name;

  //if (std::string(ss.str()) != m_workspaces[ws_number].current_name)
  //{
  //TODO set actual name with i3_conn
  std::cout << "Set a new name: \"" << ss.str() << "\"" << std::endl;
  //}
}

void WorkspaceManager::initWorkspace(unsigned ws_num, std::string name)
{
  if (m_workspaces.find(ws_num) != m_workspaces.cend())
  {
    std::cerr << "Trying to initialize a workspace which already exists" << std::endl;
    return;
  }
  auto workspace = std::make_shared<Workspace>();
  workspace->ws_number = ws_num;
  workspace->current_name = name;
  m_workspaces.emplace(ws_num, workspace);

  this->updateWorkspaceName("", ws_num);
}

void WorkspaceManager::removeWorkspace(unsigned ws_num)
{
  std::shared_ptr<Workspace> workspace;
  try
  {
    workspace = m_workspaces.at(ws_num);
  }
  catch (std::out_of_range e)
  {
    std::cerr << "Removing non existing workspace" << std::endl;
    return;
  }

  if (!workspace->window_indices.empty())
  {
    std::cerr << "Removing workspace which still contains windows" << std::endl;
    //Continuing anyway

    for (auto pair : workspace->window_indices)
    {
      this->removeWindow(pair.second->id);
    }
  }

  m_workspaces.erase(ws_num);
}

void WorkspaceManager::changeWorkspaceName(unsigned ws_num, std::string new_name)
{
  std::shared_ptr<Workspace> workspace;
  try
  {
    workspace = m_workspaces.at(ws_num);
  }
  catch (std::out_of_range e)
  {
    std::cerr << "Changing name of non existing workspace" << std::endl;
    return;
  }

  workspace->current_name = new_name;
}

void WorkspaceManager::addWindow(
    uint64_t id, std::string wm_class, std::string title, unsigned ws_number)
{
  if (ws_number == 0)
  {
    ws_number = findWindow(id);
  }

  if (m_workspaces.find(ws_number) == m_workspaces.cend())
  {
    std::cerr << "Adding window to workspace which does not exist" << std::endl;
    return;
  }

  std::shared_ptr<Window> new_window = std::make_shared<Window>();
  new_window->id = id;
  new_window->ws_number = ws_number;
  new_window->wm_class = wm_class;
  new_window->title = title;
  new_window->name_index = m_name_finder.getNameIndex(wm_class, title);

  m_window_ids.emplace(new_window->id, new_window);

  auto it = m_workspaces.at(ws_number)->window_indices.emplace(new_window->name_index, new_window);
  if (it == m_workspaces.at(ws_number)->window_indices.cbegin())
  {
    std::string new_ws_name = m_name_finder.getName(new_window->name_index);
    if (new_ws_name == "CLASS")
    {
      new_ws_name = new_window->wm_class;
    }
    updateWorkspaceName(new_ws_name, ws_number);
  }
}

void WorkspaceManager::moveWindow(uint64_t id)
{
  std::shared_ptr<Window> win;
  try
  {
    win = m_window_ids.at(id);
  }
  catch (std::out_of_range e)
  {
    std::cerr << "Moving non existing window" << std::endl;
    return;
  }

  std::shared_ptr<Workspace> new_ws;
  try
  {
    new_ws = m_workspaces.at(findWindow(win->id));
  }
  catch (std::out_of_range e)
  {
    std::cerr << "Moving a window to a non existing workspace" << std::endl;
    return;
  }

  if (new_ws->ws_number != win->ws_number)
  {
    this->removeWindow(win->id);
    this->addWindow(win->id, win->wm_class, win->title, new_ws->ws_number);
  }
}

void WorkspaceManager::changeWindowTitle(uint64_t id, std::string new_title)
{
  auto pair = m_window_ids.find(id);
  if (pair == m_window_ids.cend())
  {
    std::cerr << "Tried to change window title of non existing window" << std::endl;
    return;
  }
  auto win = pair->second;
  win->title = new_title;
  auto new_index = m_name_finder.getNameIndex(win->wm_class, win->title);
  if (new_index != (win)->name_index)
  {
    (win)->name_index = new_index;
    std::shared_ptr<Workspace> workspace;
    try
    {
      workspace = m_workspaces.at(win->ws_number);
    }
    catch (std::out_of_range e)
    {
      std::cerr << "Changing window title on non existing workspace" << std::endl;
      return;
    }
    auto it = workspace->window_indices.emplace(win->name_index, win);
    if (it == workspace->window_indices.cbegin())
    {
      std::string new_ws_name = m_name_finder.getName((win)->name_index);
      if (new_ws_name == "CLASS")
      {
        new_ws_name = (win)->wm_class;
      }
      updateWorkspaceName(new_ws_name, (win)->ws_number);
    }
  }
}

unsigned WorkspaceManager::removeWindow(uint64_t id)
{
  std::shared_ptr<Window> win;
  try
  {
    win = m_window_ids.at(id);
  }
  catch (std::out_of_range e)
  {
    std::cerr << "Could not find window " << id << std::endl;
    return 0;
  }
  std::shared_ptr<Workspace> workspace;
  try
  {
    workspace = m_workspaces.at(win->ws_number);
  }
  catch (std::out_of_range e)
  {
    std::cerr << "Removing window from non existing workspace" << std::endl;
    return 0;
  }
  auto it = workspace->window_indices.find(win->name_index);
  if (it == workspace->window_indices.cbegin())
  {
    auto next = it;
    next++;
    if (next == workspace->window_indices.cend())
    {
      //Removing last window
      updateWorkspaceName("", workspace->ws_number);
    }
    else
    {
      std::string new_ws_name = m_name_finder.getName(next->second->name_index);
      if (new_ws_name == "CLASS")
      {
        new_ws_name = next->second->wm_class;
      }
      updateWorkspaceName(new_ws_name, workspace->ws_number);
    }
  }

  m_window_ids.erase(id);
  workspace->window_indices.erase(it);

  return workspace->ws_number;
}

unsigned WorkspaceManager::findWindow(uint64_t win_id)
{
  unsigned ws_number = 0;
  std::function<unsigned(std::shared_ptr<i3ipc::container_t>)> depth_first = [&] (std::shared_ptr<i3ipc::container_t> root) -> unsigned
  {
    if (root->type == "workspace" && root->name != "__i3_scratch")
    {
      if (root->ws_num < 1)
      {
        std::cerr << "Unexpected ws number" << std::endl;
        return 0;
      }
      ws_number = root->ws_num;
    }
    if (root->id == win_id)
    {
      //Found window
      return ws_number;
    }
    for (auto child : root->nodes)
    {
      auto ret = depth_first(child);
      if (ret != 0)
      {
        return ret;
      }
    }
    for (auto floating_child : root->floating_nodes)
    {
      auto ret = depth_first(floating_child);
      if (ret != 0)
      {
        return ret;
      }
    }
    return 0;
  };

  return depth_first(m_i3_conn->get_tree());
}
