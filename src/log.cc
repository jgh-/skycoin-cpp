#include <skycoin/log.hh>
#include <memory>

namespace skycoin { 

    static std::shared_ptr<spdlog::logger> logger_;

    spdlog::logger& 
    log() {

        // TODO: Handle logging to file based on build parameters
        if(!logger_) {
            logger_ = spdlog::stdout_color_mt("skycoin");
        }

        return *logger_;
    }
}