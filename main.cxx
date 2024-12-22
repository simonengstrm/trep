#include <boost/regex.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "colors.hxx"

using std::cout, std::endl;

struct Command {
    std::string pattern;
    std::string path;
    boost::regex regex;

    std::string to_string() const {
        std::stringstream ss;
        ss << "Pattern: " << pattern << "\tPath: " << path << std::endl;
        return ss.str();
    }
};

struct Match {
    std::string line;
    int line_number;
    std::vector<std::pair<int, int>> positions;

    std::string to_string() const {
        std::string highlight_line = line;
        for (const auto &[start, end] : positions) {
            highlight_line.replace(start, end - start,
                                   BOLDRED + line.substr(start, end - start) +
                                       RESET);
        }
        return GREEN + std::to_string(line_number) + RESET + ": " +
               highlight_line;
    }
};

struct SearchResult {
    const Command &cmd;
    std::map<std::string, std::vector<Match>> matches;

    std::string to_string() const {
        std::stringstream ss;
        for (const auto &[path, fileMatches] : matches) {
            ss << MAGENTA << path << RESET << "\t" << std::endl;
            for (const auto &match : fileMatches) {
                ss << match.to_string() << std::endl;
            }
            ss << std::endl;
        }

        return ss.str();
    }
};

std::optional<Command> parse_input(int argc, char *argv[]) {
    Command command;
    if (argc == 2) { // Only pattern entered
        command.pattern = argv[1];
        command.path = "."; // Default path
    } else if (argc == 3) { // Pattern and path entered
        command.pattern = argv[1];
        command.path = argv[2];
    } else {
        std::cerr << "Invalid input. Usage: " << argv[0] << " <pattern> <path>"
                  << std::endl;
        return std::nullopt;
    }

    // Try parsing regex
    try {
        command.regex = boost::regex(command.pattern);
    } catch (std::regex_error &e) {
        std::cerr << "Invalid regex pattern: " << command.pattern << std::endl;
        return std::nullopt;
    }

    return command;
}

/*
 * Searches a file for a regex pattern
 * Assumes that the file exists
 */
std::vector<Match> search_file(const Command &cmd, const std::string &path) {
    std::vector<Match> fileMatches;

    // Open file and search for regex pattern
    std::ifstream file(path);
    if (file.is_open()) {
        std::string line;
        int line_number = 0;
        while (std::getline(file, line)) {
            line_number++;
            // Search for pattern in line
            // If found, add to fileMatches
            boost::smatch m;
            if (boost::regex_search(line, m, cmd.regex)) {
                std::vector<std::pair<int, int>> positions;
                for (const auto &sm : m) {
                    positions.push_back(
                        {sm.first - line.begin(), sm.second - line.begin()});
                }
                fileMatches.push_back({line, line_number, positions});
            }
        }
    }
    file.close();
    return fileMatches;
}

std::optional<SearchResult> search(const Command &cmd) {
    // If cmd path is a file, search the file, else search all files in the
    // directory. Only perform search_file on path if it exists
    if (!std::filesystem::exists(cmd.path)) {
        std::cerr << "Path does not exist: " << cmd.path << std::endl;
        return std::nullopt;
    }

    SearchResult searchResult{cmd};
    if (std::filesystem::is_regular_file(cmd.path)) {
        // Search file
        auto fileMatches = search_file(cmd, cmd.path);
        if (!fileMatches.empty())
            searchResult.matches[cmd.path] = fileMatches;
    } else if (std::filesystem::is_directory(cmd.path)) {
        for (const auto &entry :
             std::filesystem::recursive_directory_iterator(cmd.path)) {
            if (std::filesystem::is_regular_file(entry.path())) {
                auto fileMatches = search_file(cmd, entry.path());
                if (!fileMatches.empty())
                    searchResult.matches[entry.path()] = fileMatches;
            }
        }
    }

    return searchResult;
}

int main(int argc, char *argv[]) {
    // Parse input
    auto cmd = parse_input(argc, argv);
    if (!cmd.has_value()) {
        std::cerr << "Could not parse input." << std::endl;
        std::cerr << "Usage: " << argv[0] << " <pattern> <path>" << std::endl;
        return 1;
    }

    // Search for pattern in path
    auto matches = search(cmd.value());
    if (!matches.has_value()) {
        std::cerr << "Could not search for pattern." << std::endl;
        return 1;
    }

    // Print matches
    cout << matches.value().to_string();

    return 0;
}
