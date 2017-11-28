#include "SignalHandler.h"

#include <iostream>

SignalHandler::SignalHandler(std::shared_ptr<i3ipc::connection> i3_conn,
                             std::shared_ptr<WorkspaceManager> ws_manager)
  : m_i3_conn(i3_conn)
  , m_ws_manager(ws_manager)
{
  m_i3_conn->subscribe(i3ipc::ET_WORKSPACE | i3ipc::ET_WINDOW);

  m_i3_conn->signal_workspace_event.connect(std::bind(&SignalHandler::handleWorkspaceEvent,
                                                     this,
                                                     std::placeholders::_1));
  m_i3_conn->signal_window_event.connect(std::bind(&SignalHandler::handleWindowEvent,
                                                  this,
                                                  std::placeholders::_1));
}

void SignalHandler::handleWorkspaceEvent(const i3ipc::workspace_event_t& event)
{
  std::cout << "Got workspace event: " << (char) event.type << ", old: " << (event.old? event.old->num : 0)
                                                            << ", current: " << (event.current? event.current->num : 0)
                                                            << (event.current? ", name: " + event.current->name : "")
                                                            << std::endl;

  switch (event.type)
  {
    case decltype(event.type)::EMPTY:
      m_ws_manager->removeWorkspace(event.current->num);
      break;
    case decltype(event.type)::INIT:
      m_ws_manager->initWorkspace(event.current->num, event.current->name);
      break;
    case decltype(event.type)::RENAME:
      m_ws_manager->changeWorkspaceName(event.current->num, event.current->name);
      break;
    default:
      //Ignore other events
      break;
  }
}

void SignalHandler::handleWindowEvent(const i3ipc::window_event_t& event)
{
  std::cout << "Got window event: " << (char) event.type << ",  window name: " << event.container->name << ", id: " << event.container->id << std::endl;

  switch (event.type)
  {
    case decltype(event.type)::CLOSE:
      m_ws_manager->removeWindow(event.container->id);
      break;
    case decltype(event.type)::MOVE:
      m_ws_manager->moveWindow(event.container->id);
      break;
    case decltype(event.type)::NEW:
      m_ws_manager->addWindow(event.container->id, event.container->window_properties.wm_class, event.container->window_properties.title);
      break;
    case decltype(event.type)::TITLE:
      m_ws_manager->changeWindowTitle(event.container->id, event.container->window_properties.title);
      break;
    default:
      //Ignore other events
      break;
  }
}
