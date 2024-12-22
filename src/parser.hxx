#ifndef PARSER_HXX
#define PARSER_HXX

#include <boost/regex.hpp>
#include <iostream>
#include <sstream>
#include <string>

using std::endl;

struct Command {
    std::string pattern;
    std::string path;
    boost::regex regex;

    std::string to_string() const {
        std::stringstream ss;
        ss << "Pattern: " << pattern << "\tPath: " << path << endl;
        return ss.str();
    }
};

std::optional<Command> parse_input(int argc, char *argv[]);

#endif // PARSER_HXX
