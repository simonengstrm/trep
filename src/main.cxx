#include <boost/regex.hpp>
#include <iostream>

#include "parser.hxx"
#include "searcher.hxx"

using std::cout, std::endl;

int main(int argc, char *argv[]) {
    // Parse input
    auto result = parse_input(argc, argv);
    if (!result.has_value()) {
        std::cerr << result.error() << endl;
        return 1;
    }

    // Search for pattern in path
    auto matches = search(result.value());
    if (!matches.has_value()) {
        std::cerr << matches.error() << endl;
        return 1;
    }

    // Print matches
    cout << matches.value().to_string();

    return 0;
}
