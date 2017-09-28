#ifndef SKYCOIN__URI_HH
#define SKYCOIN__URI_HH

#include <skycoin/log.hh>

#include <vector>
#include <string>
#include <regex>
namespace skycoin {

    namespace detail {
        inline std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
            std::stringstream ss(s);
            std::string item;
            while (std::getline(ss, item, delim)) {
                if(!item.empty()) {
                    elems.push_back(item);
                }
            }
            return elems;
        }
        inline std::vector<std::string> split(const std::string &s, char delim) {
            std::vector<std::string> elems;
            split(s, delim, elems);
            return elems;
        }    
    }
    
    struct uri {

        uri(std::string str) 
        : port(-1)
        {
            // Todo: probably need to find a better regex.
            std::string regex_str = "^([a-zA-Z0-9\\-\\.\\+]+):\\/\\/([a-zA-Z0-9\\-\\.\\_\\*]+):?([0-9]+)?([a-zA-Z0-9\\-\\.%\\/\\+\\?\\#\\_\\=]+)?$";
            std::regex regex(regex_str);
            std::smatch matches;
        
            if(std::regex_match(str, matches, regex)) {
                // Matches 0 = full uri
                // 1 = scheme
                // 2 = host
                // 3 = port (maybe)
                // 4 = path (maybe)
                scheme = matches[1].str();
                host = matches[2].str();
                if(matches[3].str().size() > 0) { 
                    port = std::stoi(matches[3].str());
                }
                path = matches[4].str();
            }
        }
        std::string path;
        std::string scheme;
        std::string host;
        int port;
    };
}
#endif