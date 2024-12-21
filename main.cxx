#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <regex>
#include <sstream>
#include <string>

#include "colors.hxx"

using std::cout, std::endl;

struct Command {
    std::string pattern;
    std::regex regex;
    std::string path;

    std::string to_string() const {
        std::stringstream ss;
        ss << "Pattern: " << pattern << "\tPath: " << path << std::endl;
        return ss.str();
    }
};

struct Match {
    std::string line;
    int line_number;
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
        command.regex = std::regex(command.pattern);
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
            if (std::regex_search(line, cmd.regex)) {
                fileMatches.push_back({line, line_number});
            }
        }
    }

    file.close();
    return fileMatches;
}

std::optional<std::map<std::string, std::vector<Match>>>
search(const Command &cmd) {
    std::map<std::string, std::vector<Match>> matches;

    // If cmd path is a file, search the file, else search all files in the
    // directory. Only perform search_file on path if it exists

    // Check if path exists
    if (std::filesystem::exists(cmd.path)) {
        if (std::filesystem::is_regular_file(cmd.path)) {
            // Search file
            auto fileMatches = search_file(cmd, cmd.path);
            matches[cmd.path] = fileMatches;
        } else if (std::filesystem::is_directory(cmd.path)) {
            // Search all files in directory
            for (const auto &entry :
                 std::filesystem::recursive_directory_iterator(cmd.path)) {
                if (std::filesystem::is_regular_file(entry.path())) {
                    auto fileMatches = search_file(cmd, entry.path());
                    if (!fileMatches.empty())
                        matches[entry.path()] = fileMatches;
                }
            }
        }
    } else {
        std::cerr << "Path does not exist: " << cmd.path << std::endl;
        return std::nullopt;
    }

    return matches;
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
    for (const auto &[path, fileMatches] : matches.value()) {
        cout << MAGENTA "File: " << RESET << path << "\t" << endl;
        for (const auto &match : fileMatches) {
            cout << GREEN << match.line_number << RESET << ": " << match.line
                 << endl;
        }
        cout << endl;
    }
    return 0;
}
