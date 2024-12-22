#include <boost/regex.hpp>
#include <iostream>

#include "parser.hxx"
#include "searcher.hxx"

using std::cout, std::endl;

int main(int argc, char *argv[]) {
    // Parse input
    auto cmd = parse_input(argc, argv);
    if (!cmd.has_value()) {
        std::cerr << "Could not parse input." << endl;
        std::cerr << "Usage: " << argv[0] << " <pattern> <path>" << endl;
        return 1;
    }

    // Search for pattern in path
    auto matches = search(cmd.value());
    if (!matches.has_value()) {
        std::cerr << "Could not search for pattern." << endl;
        return 1;
    }

    // Print matches
    cout << matches.value().to_string();

    return 0;
}
