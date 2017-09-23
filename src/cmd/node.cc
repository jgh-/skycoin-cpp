#include <skywire/event_loop.hh>
#include <skywire/log.hh>
#include <unpause/async>

int main(int argc, char* argv[]) {

    skywire::log().info("skywire node (c++) 0.1");

    skywire::event_loop l; // instantiate before thread pool so children take signal mask

    unpause::async::thread_pool pool(std::thread::hardware_concurrency() * 2);


    // Start our main event loop, this will block until 
    // the program receives a SIGINT (CTRL+C), which will
    // start a graceful shutdown that will wait for all handlers
    // to exit, or a SIGTERM (kill) that will exit cleanly
    // immediately, shutting down any existing handlers.
    
    l.run(); 
    
    return 0;
}