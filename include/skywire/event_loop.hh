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

    class event_loop;
    using shutdown_handler_f = std::function<void(event_loop&)>;

    class event_loop {
    public:
        event_loop(int max_events = 10240, bool handle_signals = true);
        ~event_loop();

        void register_handler(int fd, event_handler_f handler, int events = EPOLLIN | EPOLLET | EPOLLOUT);
        void unregister_handler(int fd);

        // this may be called multiple times, so your handler should be idempotent.
        void shutdown_handler(shutdown_handler_f handler) { shutdown_handler_ = handler; };

        // blocks.
        void run();

        void stop(bool graceful = true);

    private:
        std::unordered_map<int, event_handler_f> handlers_;
        shutdown_handler_f shutdown_handler_;
        int fd_wakeup_;
        int fd_signal_;
        int fd_epoll_;
        int max_events_;
        std::atomic<int> exiting_;
    }; // class event_loop

} // namespace skywire


#endif // SKYWIRE__EVENTLOOP_HH