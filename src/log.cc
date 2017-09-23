#include <skywire/log.hh>
#include <memory>

namespace skywire { 

    static std::shared_ptr<spdlog::logger> logger_;

    spdlog::logger& 
    log() {

        // TODO: Handle logging to file based on build parameters
        if(!logger_) {
            logger_ = spdlog::stdout_color_mt("skywire");
        }

        return *logger_;
    }
}