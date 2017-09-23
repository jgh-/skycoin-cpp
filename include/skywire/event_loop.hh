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
    public:
        event_loop(int max_events = 10240);
        ~event_loop();

        void register_handler(int fd, event_handler_f handler, int events = EPOLLIN | EPOLLET | EPOLLOUT);
        void unregister_handler(int fd);
        
        // blocks.
        void run();

        void stop(bool graceful = true);

    private:
        std::unordered_map<int, event_handler_f> handlers_;
        int fd_signal_;
        int fd_epoll_;
        int max_events_;
        std::atomic<int> exiting_;
    }; // class event_loop

} // namespace skywire


#endif // SKYWIRE__EVENTLOOP_HH