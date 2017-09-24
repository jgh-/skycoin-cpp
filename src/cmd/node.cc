#include <skywire/net/tcp/listener.hh>
#include <skywire/net/udp/listener.hh>
#include <skywire/event_loop.hh>
#include <skywire/log.hh>

#include <unpause/async>

int main(int argc, char* argv[]) {

    skywire::log().info("skywire node (c++) 0.1");

    skywire::event_loop e; // instantiate before thread pool so children take signal mask

    unpause::async::thread_pool pool(std::thread::hardware_concurrency() * 2);

    skywire::tcp::listener tcp("*", 2020, "localhost", 2021, pool);
    skywire::udp::listener udp("*", 2000, "localhost", 2001, pool);

    tcp.set_register_handler([&e](int fd, skywire::event_handler_f handler) {
        e.register_handler(fd, handler);
    });

    tcp.set_unregister_handler([&e](int fd) {
        e.unregister_handler(fd);
    });

    udp.set_register_handler([&e](int fd, skywire::event_handler_f handler) {
        e.register_handler(fd, handler);
    });

    udp.set_unregister_handler([&e](int fd) {
        e.unregister_handler(fd);
    });

    e.shutdown_handler([&tcp, &udp] (skywire::event_loop& e) {
        e.unregister_handler(tcp.fd());
        e.unregister_handler(udp.fd());
    });
    
    e.register_handler(tcp.fd(), tcp.handler());
    e.register_handler(udp.fd(), udp.handler(), EPOLLIN);

    // Start our main event loop, this will block until 
    // the program receives a SIGINT (CTRL+C), which will
    // start a graceful shutdown that will wait for all handlers
    // to exit, or a SIGTERM (kill) that will exit cleanly
    // immediately, shutting down any existing handlers.
    
    e.run(); 
    
    return 0;
}