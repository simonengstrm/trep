#ifndef SEARCHER_HXX
#define SEARCHER_HXX

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "colors.hxx"
#include "parser.hxx"

struct SearchError {
    std::string message;

    // Overload the << operator to print the error message
    friend std::ostream &operator<<(std::ostream &os,
                                    const SearchError &error) {
        os << error.message;
        return os;
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
    int files_searched;
    std::map<std::string, std::vector<Match>> matches;

    std::string to_string() const {
        std::stringstream ss;

        if (cmd.config.verbose) {
            ss << cmd.to_string() << endl;
            ss << cmd.config.to_string() << endl;
        }

        for (const auto &[path, fileMatches] : matches) {
            ss << MAGENTA << path << RESET << "\t" << endl;
            for (const auto &match : fileMatches) {
                ss << match.to_string() << endl;
            }
            ss << endl;
        }

        return ss.str();
    }
};

std::expected<SearchResult, SearchError> search(const Command &cmd);

#endif // SEARCHER_HXX
