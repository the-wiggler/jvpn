#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <filesystem>
#include <unistd.h>

namespace fs = std::filesystem;

void showHelp() {
    std::cout << "Usage:\n";
    std::cout << "  a / activate <option>          | Activate VPN\n";
    std::cout << "  d / deactivate <option>        | Deactivate VPN\n";
    std::cout << "  l / list                       | VPNs Available to Toggle\n";
    std::cout << "  c / configure                  | Install a new wireguard .conf file\n";
    std::cout << "  r / remove                     | Remove a wireguard .conf file\n";
    std::cout << "  h / help                       | Show this help message\n";
    std::cout << "  q / quit                       | Quit the program\n\n";
}

std::string vpn_path = "/etc/wireguard/";
void listwg() {
    std::cout << "Available VPNs :";
    for (const auto & entry : fs::directory_iterator(vpn_path)) {
        std::cout << entry.path();
    }
    std::cout << std::endl << std::endl;
}

void checkRoot() {
    if (geteuid() != 0) {
        std::cerr << "This program must be run as root.\n";
        std::exit(EXIT_FAILURE);
    }
}

void checkDependencies() {
    // check to see if there is a folder called /etc/wireguard
    if (!(fs::exists(vpn_path))) {
        std::cout << "It appears that you dont have a wireguard config folder. Would you like to configure one? (y / n): ";
        std::string line;
        std::getline(std::cin, line);

        if (line == "y" || line == "yes") {
            fs::create_directory(vpn_path);
            std::cout << "\n/etc/wireguard created successfull!\n";
        }
        else if (line == "n" || line == "no") {
            std::cout << "\nOk. Continuing without configuration. Be sure you have wireguard installed!\n";
        }
    }

    // checks to see if there are any .conf files in /etc/wireuard
    bool found_config = false;
    for (const auto & entry : fs::directory_iterator(vpn_path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".conf") {
            found_config = true;
        }
    }
    if (!found_config) {
        std::cout << "Found no VPN configuration files!\n";
    } 
    else {
        listwg();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
    checkRoot();

	std::cout << "\nWelcome to the jvpn Tool!\n\n"
			  << "Currently Toggled VPNs: \n\n";
    showHelp();

    checkDependencies();

    bool running = true;

    while (running) {

        std::cout << "> ";

        std::string line; // the whole line
        std::getline(std::cin, line); // read the entire input line

        std::istringstream iss(line);
        std::string command, argument;
        iss >> command >> argument; // splits input into command and argument
        /////////////////////////////////////////////
        // activate command
        /////////////////////////////////////////////
        if (command == "a" || command == "activate") {
            if (!argument.empty()) {
                std::string cmd = "wg-quick up " + argument;
                int result = std::system(cmd.c_str());
            } else {
                std::cerr << "Missing argument for activation.\n";
            }
        }
        /////////////////////////////////////////////
        // deactivate command 
        /////////////////////////////////////////////
        else if (command == "d" || command == "deactivate") {
            if (!argument.empty()) {
                std::string cmd = "wg-quick down " + argument;
                int result = std::system(cmd.c_str());
            } else {
                std::cerr << "Missing argument for deactivation.\n";
            }
        } 
        /////////////////////////////////////////////
        // configure new vpn command
        /////////////////////////////////////////////
        else if (command == "c" || command == "configure") {
            std::cout << "Are you installing one (1) or multiple (2) .conf files? (1 / 2): ";
            std::string ot;
            std::cin >> ot;

            // single .conf
            if (ot == "1") {
                std::string new_conf_path;
                std::cout << "full filepath to wireguard .conf file: ";
                std::cin >> new_conf_path;
                std::cout << "\ncopying config file to /etc/wireguard...\n";
                std::string cmd = "sudo cp " + new_conf_path + " /etc/wireguard/";
                int result = std::system(cmd.c_str());
            }

            // directory containing .conf files
            else if (ot == "2") {
                std::string new_conf_path;
                std::cout << "full filepath to directory containing .conf files: ";
                std::cin >> new_conf_path;
                std::cout << "\ncopying config files to /etc/wireguard...\n";
                    bool conf_exists = false;
                    for (const auto & entry : fs::directory_iterator(new_conf_path)) {
                        if (entry.is_regular_file() && entry.path().extension() == ".conf") {
                            std::cout << "Copying configuration file: " << entry.path() << std::endl;
                            std::string cmd = "sudo cp " + entry.path().string() + " /etc/wireguard/";
                            int result = std::system(cmd.c_str());
                            conf_exists = true;
                        }
                    }
                    if (!conf_exists) {
                        std::cout << "Found no VPN configuration files!\n";
                    }
            }

            // exits if entered # isnt 1 or 2
            else std::cout << "Not an option. Please enter '1' or '2'\n";
        }
        /////////////////////////////////////////////
        // remove wireguard .conf file
        /////////////////////////////////////////////
        else if (command == "r" || command == "remove") {
            std::cout << "Which configuration would you like to remove?:\n";
            for (const auto & entry : fs::directory_iterator(vpn_path)) {
                std::cout << entry.path() << std::endl;
            }
            std::cout << "Enter '<filename>' or 'all' to remove all .conf files: ";
            std::string inp;
            std::cin >> inp;
            if (inp == "all") {
                for (const auto & entry : fs::directory_iterator(vpn_path)) {
                    if (entry.is_regular_file() && entry.path().extension() == ".conf") {
                        std::cout << "Removing " << entry.path() << std::endl;
                        std::string cmd = "sudo rm " + entry.path().string();
                        int result = std::system(cmd.c_str());
                    }
                }
            }
            else {
                std::string cmd = "sudo rm /etc/wireguard/" + inp;
                int result = std::system(cmd.c_str());
                std::cout << "Removed " << inp << std::endl;
            }

        }
        /////////////////////////////////////////////
        // list available vpn configs
        /////////////////////////////////////////////
        else if (command == "l" || command == "list") {
            listwg();
        }
        /////////////////////////////////////////////
        // help command
        /////////////////////////////////////////////
        else if (command == "h" || command == "help") {
            showHelp();
        } 
        /////////////////////////////////////////////
        // quit command ( I hate to see you go :( )
        /////////////////////////////////////////////
        else if (command == "q" || command == "quit") {
            running = false;
        } 

        else if (command == "") {
            continue;
        }

        // error
        else {
            std::cerr << "Error: Invalid input.\n";
        }
    }

    return 0;
}
