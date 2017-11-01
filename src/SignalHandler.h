#pragma once

#include <memory>

//#include <i3ipc++/ipc.hpp>

#include "WorkspaceManager.h"

class SignalHandler
{
public:
  SignalHandler(std::shared_ptr<i3ipc::connection>, std::shared_ptr<WorkspaceManager>);

private:
  void handleWorkspaceEvent(const i3ipc::workspace_event_t& event);
  void handleWindowEvent(const i3ipc::window_event_t& event);

  std::shared_ptr<i3ipc::connection> m_i3_conn;
  std::shared_ptr<WorkspaceManager> m_ws_manager;
};
