#ifndef PARSER_HXX
#define PARSER_HXX

#include <boost/regex.hpp>
#include <expected>
#include <iostream>
#include <sstream>
#include <string>

using std::endl;

struct ParseError {
    std::string message;

    // Overload the << operator to print the error message
    friend std::ostream &operator<<(std::ostream &os, const ParseError &error) {
        os << error.message;
        return os;
    }
};

struct Config {
    bool ignore_case;
    bool recursive;
    bool verbose;

    std::string to_string() const {
        std::stringstream ss;
        ss << "Ignore case: " << ignore_case << "\tRecursive: " << recursive
           << "\tVerbose: " << verbose << endl;
        return ss.str();
    }
};

struct Command {
    std::string pattern;
    std::string path;
    boost::regex regex;
    Config config;

    std::string to_string() const {
        std::stringstream ss;
        ss << "Pattern: " << pattern << "\tPath: " << path << endl;
        return ss.str();
    }
};

std::expected<Command, ParseError> parse_input(int argc, char *argv[]);

#endif // PARSER_HXX
