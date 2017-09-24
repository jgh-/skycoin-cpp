#include <skycoin/net/tcp/listener.hh>
#include <skycoin/net/udp/listener.hh>
#include <skycoin/event_loop.hh>
#include <skycoin/log.hh>

#include <unpause/async>

int main(int argc, char* argv[]) {

    skycoin::log().info("skycoin node (c++) 0.1");

    skycoin::event_loop e; // instantiate before thread pool so children take signal mask

    unpause::async::thread_pool pool(std::thread::hardware_concurrency() * 2);

    skycoin::tcp::listener tcp("*", 2020, "localhost", 2021, pool);
    skycoin::udp::listener udp("*", 2000, "localhost", 2001, pool);

    tcp.set_register_handler([&e](int fd, skycoin::event_handler_f handler) {
        e.register_handler(fd, handler);
    });

    tcp.set_unregister_handler([&e](int fd) {
        e.unregister_handler(fd);
    });

    udp.set_register_handler([&e](int fd, skycoin::event_handler_f handler) {
        e.register_handler(fd, handler);
    });

    udp.set_unregister_handler([&e](int fd) {
        e.unregister_handler(fd);
    });

    e.shutdown_handler([&tcp, &udp] (skycoin::event_loop& e) {
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