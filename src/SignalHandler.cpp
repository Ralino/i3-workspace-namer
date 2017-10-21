#include "SignalHandler.h"
#include <iostream>

SignalHandler::SignalHandler(NameFinder name_finder)
  : m_i3_conn()
  , m_name_finder(name_finder)
{
  m_i3_conn.subscribe(i3ipc::ET_WORKSPACE | i3ipc::ET_WINDOW);

  m_i3_conn.signal_workspace_event.connect(std::bind(&SignalHandler::handleWorkspaceEvent,
                                                     this,
                                                     std::placeholders::_1));
  m_i3_conn.signal_window_event.connect(std::bind(&SignalHandler::handleWindowEvent,
                                                  this,
                                                  std::placeholders::_1));
}

void SignalHandler::spin()
{
  while (true)
  {
    m_i3_conn.handle_event();
  }
}

void SignalHandler::handleWorkspaceEvent(const i3ipc::workspace_event_t& event)
{
  //TODO

}

void SignalHandler::handleWindowEvent(const i3ipc::window_event_t& event)
{
  std::cout << "Got window event: " << event.container->name << std::endl;
}
