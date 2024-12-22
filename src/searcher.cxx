#include <boost/regex.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "searcher.hxx"

using std::endl;

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
        std::cerr << "Path does not exist: " << cmd.path << endl;
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
