#include <skywire/event_loop.hh>
#include <skywire/log.hh>
int main(int argc, char* argv[]) {

    skywire::log().info("skywire node (c++) 0.1");

    skywire::event_loop l;

    l.run();
    
    return 0;
}