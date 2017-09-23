#include <skywire/net/udp/listener.hh>

namespace skywire { namespace udp {

    listener::listener(std::string addr, int in_port, std::string out_addr, int out_port, unpause::async::thread_pool& pool)
    : addr_in_(in_addr)
    , addr_out_(out_addr)
    , pool_(pool)
    , fd_(-1)
    , port_in_(in_port)
    , port_out_(out_port)
    {}

    int 
    listener::handle_events(uint32_t events)
    {
        
    }
}
}