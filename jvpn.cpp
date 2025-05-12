#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <filesystem>

void showHelp() {
    std::cout << "Usage:\n";
    std::cout << "  a / activate <option>      | Activate VPN\n";
    std::cout << "  d / deactivate <option>    | Deactivate VPN\n";
    std::cout << "  h / help                   | Show this help message\n";
    std::cout << "  q / quit                   | Quit the program\n";
    std::cout << "  l / list                   | VPNs Available to Toggle\n";
}

namespace fs = std::filesystem;

int main() {
	std::cout << "\nWelcome to the jvpn Tool!\n\n"
			  << "Currently Toggled VPNs: \n\n";

	std::string vpn_path = "/etc/wireguard/";
	std::cout << "Available VPNs :";
	for (const auto & entry : fs::directory_iterator(vpn_path)) {
		std::cout << entry.path();
    }
    std::cout << std::endl << std::endl;


    showHelp();

    bool running = true;

    while (running) {

        std::cout << "> ";

        std::string line; // the whole line
        std::getline(std::cin, line); // read the entire input line

        std::istringstream iss(line);
        std::string command, argument;
        iss >> command >> argument; // splits input into command and argument

        // activate command
        if (command == "a" || command == "activate") {
            if (!argument.empty()) {
                std::string cmd = "wg-quick up " + argument;
                int result = std::system(cmd.c_str());
            } else {
                std::cerr << "Missing argument for activation.\n";
            }
        } 

        // deactivate command 
        else if (command == "d" || command == "deactivate") {
            if (!argument.empty()) {
                std::string cmd = "wg-quick down " + argument;
                int result = std::system(cmd.c_str());
            } else {
                std::cerr << "Missing argument for deactivation.\n";
            }
        } 

        // help command
        else if (command == "h" || command == "help") {
            showHelp();
        } 

        // quit command ( I hate to see you go :( )
        else if (command == "q" || command == "quit") {
            running = false;
        } 

        // error
        else {
            std::cerr << "ERROR: Invalid input.\n";
            showHelp();
        }
    }

    return 0;
}
