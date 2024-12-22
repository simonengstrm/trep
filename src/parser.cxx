#include "parser.hxx"

using std::endl;

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
                  << endl;
        return std::nullopt;
    }

    // Try parsing regex
    try {
        command.regex = boost::regex(command.pattern);
    } catch (boost::regex_error &e) {
        std::cerr << "Invalid regex pattern: " << command.pattern << endl;
        return std::nullopt;
    }

    return command;
}
