#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <i3ipc++/ipc.hpp>
#include "NameFinder.h"

class SignalHandler
{
public:
  SignalHandler(NameFinder name_finder);

  void spin();

private:
  void handleWorkspaceEvent(const i3ipc::workspace_event_t& event);
  void handleWindowEvent(const i3ipc::window_event_t& event);

  SignalHandler();
  i3ipc::connection m_i3_conn;
  NameFinder m_name_finder;
  //WorkspaceManager

};
#endif //SIGNAL_HANDLER_H
