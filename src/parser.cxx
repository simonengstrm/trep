#include "parser.hxx"
#include <__expected/expected.h>
#include <__expected/unexpected.h>

using std::cout, std::endl;

void printUsage() {
    cout << "Usage: trep [OPTIONS] <pattern> [path]" << endl;
    cout << "Options:" << endl;
    cout << "-i, --ignore-case" << "\tIgnore case when searching" << endl;
    cout << "-r, --recursive" << "\tSearch recursively through subdirectories"
         << endl;
    cout << "-v, --verbose" << "\tPrint verbose output" << endl;
    cout << "-h, --help" << "\tPrint this help message" << endl;
}

std::expected<void, ParseError> parse_flag(const std::string &flag,
                                           Config &config) {
    if (flag == "-i" || flag == "--ignore-case") {
        config.ignore_case = true;
    } else if (flag == "-r" || flag == "--recursive") {
        config.recursive = true;
    } else if (flag == "-v" || flag == "--verbose") {
        config.verbose = true;
    } else {
        return std::unexpected(ParseError{"Unknown flag: " + flag});
    }

    return {};
}

std::expected<Command, ParseError> parse_input(int argc, char *argv[]) {

    // Usage is ./trep [OPTIONS] <pattern> [path]
    //
    // Options defined by the Config struct in parser.hxx
    // -i, --ignore-case
    // -r, --recursive
    // -v, --verbose
    //
    // Pattern is a regular expression
    // Path is a directory or file, if no path is given, use current directory

    if (argc < 2) {
        return std::unexpected(ParseError{"Not enough arguments."});
    }

    Command command;
    command.path = "."; // By default, use current directory
    std::vector<std::string> args(argv + 1, argv + argc);
    std::string pattern;
    std::string path;
    for (const auto &arg : args) {
        if (arg == "-h" || arg == "--help") {
            printUsage();
            return std::unexpected(ParseError{""});
        } else if (arg.starts_with("-")) {
            auto result = parse_flag(arg, command.config);
            if (!result.has_value()) {
                return std::unexpected(result.error());
            }
        } else if (pattern.empty()) {
            pattern = arg;
            try {
                if (command.config.ignore_case) {
                    command.regex = boost::regex(pattern, boost::regex::icase);
                } else {
                    command.regex = boost::regex(pattern);
                }
            } catch (const boost::regex_error &e) {
                return std::unexpected(ParseError{e.what()});
            }
            command.pattern = pattern;
        } else if (path.empty()) {
            path = arg;
            command.path = path;
        } else {
            return std::unexpected(ParseError{"Invalid input: " + arg});
        }
    }

    return command;
}
