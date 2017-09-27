#include <skycoin/net/tcp/listener.hh>
#include <skycoin/net/udp/listener.hh>
#include <skycoin/event_loop.hh>
#include <skycoin/log.hh>

#include <unpause/async>

int main(int argc, char* argv[]) {

    skycoin::log().info("skycoin node (c++) 0.1");

    skycoin::event_loop e; // instantiate before thread pool so children take signal mask

    unpause::async::thread_pool pool(std::thread::hardware_concurrency() * 2);

    skycoin::tcp::listener tcp("*", 2020, pool);

    tcp.set_register_handler([&e](int fd, skycoin::event_handler_f handler) {
        e.register_handler(fd, handler);
    });

    tcp.set_unregister_handler([&e](int fd) {
        e.unregister_handler(fd);
    });

    skycoin::tcp::connection c("localhost", 2021);
    c.set_can_read_handler([](skycoin::i_connection& conn){ 
        uint8_t buf[1024] {};
        ssize_t res =  conn.read(buf, sizeof(buf));
        if(res > 0) {
            skycoin::log().info("got {}", buf);
            std::string f("fartington\n");

            conn.write((uint8_t*)f.data(), f.size());
        }
        return res;
    });

    tcp.set_can_read_handler([](skycoin::i_connection& conn){ 
        uint8_t buf[1024] {};
        ssize_t res =  conn.read(buf, sizeof(buf));
        if(res > 0) {
            skycoin::log().info("got {}", buf);
            std::string f("fartington\n");

            conn.write((uint8_t*)f.data(), f.size());
        }
        return res;
    });
    
    c.connect();
    e.register_handler(c.fd(), c.handler());

    e.shutdown_handler([&tcp] (skycoin::event_loop& e) {
        e.unregister_handler(tcp.fd());
    });
    
    e.register_handler(tcp.fd(), tcp.handler());

    // Start our main event loop, this will block until 
    // the program receives a SIGINT (CTRL+C), which will
    // start a graceful shutdown that will wait for all handlers
    // to exit, or a SIGTERM (kill) that will exit cleanly
    // immediately, shutting down any existing handlers.
    
    e.run(); 
    
    return 0;
}