#include <skycoin/coin/client.hh>
#include <skycoin/event_loop.hh>
#include <skycoin/log.hh>

#include <json/json.hpp>
#include <unpause/async>

#include <fstream>

#define VERSION "0.1"


using json = nlohmann::json;

void print_usage() {
    printf("SkyCoin Node " VERSION "\n");
    printf("\nUsage:\n\tnode -c [config_file]\t specify location for configuration.\n\n");
}

int main(int argc, char* argv[]) {

    skycoin::log().info("skycoin node (c++) {}", VERSION);

    std::string config_file;
    char cmd = 0;
    while ((cmd = getopt (argc, argv, "c:")) != -1)
    {
        switch (cmd)
        {
            case 'c':
                config_file = std::string(optarg);
                break;
            case '?':
                print_usage();
                exit(1);
                break;
            default:
                abort();
                break;
        }
    }
    if(config_file.size() == 0) {
        print_usage();
        exit(1);
    }
    json config;

    {
        std::ifstream in(config_file);
        in >> config;
    }

    skycoin::event_loop e; // instantiate before thread pool so children take signal mask

    unpause::async::thread_pool pool(std::thread::hardware_concurrency() * 2);

    skycoin::coin::client coin_client(e, pool, config["coin"].dump());

    coin_client.start();

    e.shutdown_handler([&coin_client](skycoin::event_loop& e, bool graceful) {
        coin_client.stop(graceful);
    });

    // Start our main event loop, this will block until 
    // the program receives a SIGINT (CTRL+C), which will
    // start a graceful shutdown that will wait for all handlers
    // to exit, or a SIGTERM (kill) that will exit cleanly
    // immediately, shutting down any existing handlers.
    
    e.run(); 
    
    return 0;
}