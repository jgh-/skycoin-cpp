#ifndef SKYWIRE__EVENTLOOP_HH
#define SKYWIRE__EVENTLOOP_HH


#include <skywire/event_handler.hh>

#include <unordered_map>
#include <functional>
#include <atomic>

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/types.h>
#include <sys/socket.h>

namespace skywire {

    class event_loop {

    }; // class event_loop

} // namespace skywire


#endif // SKYWIRE__EVENTLOOP_HH