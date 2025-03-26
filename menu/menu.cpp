#include "menu.h"
#include "tc.h"
#include "../algorithms/Algorithms.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include <unistd.h>

#include <vector>
#include <unordered_set>

using namespace std;

//Menu

Menu::Menu() : selectedItemIndex(0), dataLoaded(false), currentColor(TC_MAG),
    graph(initialize("../data/loc.csv", "../data/dist.csv")),
    items({
        "1. Plan Route",
        "2. Plan Green Route",
        "3. Batch Mode",
        "4. Option",
        "0. Exit"
    }) {}

void Menu::displayMenu() {
    tc_clear_screen();
    cout << "================= Route Planning Tool =================\n";
    for (int i = 0; i < items.size(); ++i) {
        if (i == selectedItemIndex)
            cout << currentColor << items[i] << TC_NRM << endl;
        else
            cout << items[i] << endl;
    }
}

void Menu::run() {
    enable_raw_mode();
    hide_cursor();
    displayMenu();
    navigateMenu();
    disable_raw_mode();
    show_cursor();
}

void Menu::navigateMenu() {
  char c;
  bool running = true;
  while (running && (c = getchar())!= EOF) {
      switch (c) {
          case '0':
          case 'q': // quit
              running = false;
              tc_clear_screen();
              break;
          case '\033': // ESC sequence for arrow keys
              processArrowKeyInput(selectedItemIndex, items.size());
              break;
          case '\n':
              handleSelection();
              break;
          case '1':
          case '2':
          case '3':
          case '4':
              selectedItemIndex = c - '1';
              handleSelection();
              break;
          default:
              break;
      }
  }
}

void Menu::handleSelection() {
    switch (selectedItemIndex) {
        case 0:
            askForRouteDetails();
            break;
        case 1:
            break;
        case 2:
            batchProcess();
            break;
        case 3:
            optionsMenu();
            break;
        case 4:
            tc_clear_screen();
            exit(0);
        default:
            break;
    }
}

//Auxiliary Functions

void Menu::processArrowKeyInput(int& index, int maxIndex) {
    getchar(); // Skip the '[' character
    switch (getchar()) {
        case 'A': // Arrow up
            index = (index > 0) ? index - 1 : maxIndex-1;
            break;
        case 'B': // Arrow down
            index = (index + 1) % maxIndex ;
            break;
        default:
            break;
    }
    displayMenu();
}

string Menu::getCityFromUser(const string& prompt) {
    string city;
    cout << prompt << endl;
    getline(cin, city);
    return city;
}

void Menu::batchProcess() {
    tc_clear_screen();

    ifstream inputFile("../batch/input8.txt");
    ofstream outputFile("../batch/output.txt");

    if (!inputFile.is_open() || !outputFile.is_open()) {
        std::cerr << "Error opening file(s)." << std::endl;
        return; // Exit if file cannot be opened
    }

    string line, mode, temp;
    int source, destination;
    unordered_set<int> avoidNodes;
    vector<std::pair<int, int>> avoidEdges;
    int includeNode = -1; // Default initialization when not specified

    // Read the first line for mode
    getline(inputFile, line);
    istringstream modeStream(line);
    getline(modeStream, temp, ':'); // Skip "Mode"
    getline(modeStream, mode); // Read actual mode

    // Read the second line for source
    getline(inputFile, line);
    istringstream sourceStream(line);
    getline(sourceStream, temp, ':'); // Skip "Source"
    sourceStream >> source; // Read actual source

    // Read the third line for destination
    getline(inputFile, line);
    istringstream destStream(line);
    getline(destStream, temp, ':'); // Skip "Destination"
    destStream >> destination; // Read actual destination

    // Read optional parameters if present
    while (getline(inputFile, line)) {
        if (line.find("AvoidNodes:") != string::npos) {
            istringstream nodeStream(line.substr(line.find(':') + 1));
            string node;
            while (getline(nodeStream, node, ',')) {
                if (!node.empty()) {
                    avoidNodes.insert(stoi(node));
                }
            }
        } else if (line.find("AvoidSegments:") != string::npos) {
            string segmentPairs = line.substr(line.find(':') + 1);
            segmentPairs = segmentPairs.substr(1, segmentPairs.length() - 2);  // Remove the first '(' and last ')'

            istringstream segmentStream(segmentPairs);
            string segment;

            // First, extract the string until the first comma, which is part of the pair (first,second),
            // then use ')' as a delimiter to close each segment pair.
            while (getline(segmentStream, segment, ',')) {
                // Strip any potential leading '(' from the line
                if (segment.front() == '(') {
                    segment = segment.substr(1);
                }

                int first = stoi(segment);  // Convert the first part of the pair to an integer

                // Now read until the next ')' to get the second part of the pair
                getline(segmentStream, segment, ')');
                int second = stoi(segment);  // Convert the second part of the pair to an integer

                avoidEdges.emplace_back(first, second);

                // Skip the next ',' which comes after ')' in patterns like (3,4),
                segmentStream.get();  // This consumes the ',' delimiter after each pair except the last one
            }
        } else if (line.find("IncludeNode:") != string::npos) {
            istringstream includeStream(line.substr(line.find(':') + 1));
            includeStream >> includeNode;
        }
    }

    if (includeNode == -1) {includeNode= source;}

    // Now that all data is parsed, perform the appropriate routing function based on the mode

    // Variable to hold the output of routing functions
    std::string routeDetails;

    if (mode == "driving" && avoidNodes.empty() && avoidEdges.empty()) {
        routeDetails = SimpleDriving(&graph, source, destination);
    } else if (mode == "driving") {
        routeDetails = RestrictedDriving(&graph, source, destination, avoidNodes, avoidEdges, includeNode);
    } else if (mode == "driving-walking") {
        // routeDetails = DrivingWalking(&graph, source, destination, avoidNodes, avoidEdges, includeNode);
    }


    // Write the routing details to the output file
    outputFile << routeDetails << endl;

    /*
    for (const auto& pair : avoidEdges) {
        outputFile << "(" << pair.first << "," << pair.second << ")" << std::endl;
    }
    */

    // Close files
    inputFile.close();
    outputFile.close();

    cout << TC_GRN <<"Batch processing completed." << TC_NRM << endl;
    sleep(1);

    displayMenu();
}


//SubMenus

void Menu::askForRouteDetails() {
    tc_clear_screen();
    show_cursor();
    disable_raw_mode();

    string source = getCityFromUser("Enter the start city:");
    tc_clear_screen();

    string destination = getCityFromUser("Enter the destination city:");
    tc_clear_screen();

    string includeNode = getCityFromUser("Enter a city(s) to route through (optional, press enter to skip):");
    tc_clear_screen();

    string avoidNodes = getCityFromUser("Enter a city(s) to avoid (optional, press enter to skip):");
    tc_clear_screen();

    string avoidSegments = getCityFromUser("Enter a segment(s) to avoid (optional, press enter to skip):");
    hide_cursor();



    cout << "Calculated route: " << endl;
    cout << "Press enter to return to the menu..." << endl;

    enable_raw_mode();
    displayMenu();
}

void Menu::changeTextColor() {
    const vector<string> colorOptions = {"Red", "Green", "Yellow", "Blue", "Magenta", "Cyan", "White"};
    int colorChoice = 0;

    while (true) {
        tc_clear_screen();
        cout << "Select text color:\n";
        for (int i = 0; i < colorOptions.size(); ++i) {
            if (i == colorChoice){
                cout << "> " << colorOptions[i] << "\n";
            }else{
                cout << "  " << colorOptions[i] << "\n";
            }
        }

        char c = getchar();
        switch (c) {
            case '\033': // ESC sequence for arrow keys
                processArrowKeyInput(colorChoice, colorOptions.size());
                break;
            case '\n': // Enter key
                switch (colorChoice) {
                    case 0:
                        currentColor = TC_RED;
                        break;
                    case 1:
                        currentColor = TC_GRN;
                        break;
                    case 2:
                        currentColor = TC_YEL;
                        break;
                    case 3:
                        currentColor = TC_BLU;
                        break;
                    case 4:
                        currentColor = TC_MAG;
                        break;
                    case 5:
                        currentColor = TC_CYN;
                        break;
                    case 6:
                        currentColor = TC_WHT;
                        break;
                    default:
                        break;
                }
                displayMenu();
                return;
        }
    }
}

void Menu::changeDataSet(){
    const vector<string> dataOptions = {"Normal", "Small", "Custom"};
    int choice = 0;
    string locFile, distFile;

    while (true) {
        tc_clear_screen();
        cout << "Choose data Set for route finding:" << endl;
        for (int i = 0; i < dataOptions.size(); ++i) {
            if(i == choice){
                cout << "> " << dataOptions[i] << "\n";
            }else{
              cout << "  " << dataOptions[i] << "\n";
            }
        }

        char c = getchar();
        switch (c) {
            case '\033': // ESC sequence for arrow keys
                processArrowKeyInput(choice, dataOptions.size());
                break;
            case '\n': // Enter key
                switch (choice) {
                    case 0:
                        try {
                            graph = initialize("../data/loc.csv", "../data/dist.csv");
                            cout << endl << TC_GRN << "Dataset loaded successfully." << TC_NRM << endl;
                            sleep(1);
                        }catch (exception& e) {
                            cout << endl << TC_RED << e.what() << TC_NRM << endl;
                            cout << endl << "Press any key to continue...";
                            getchar();
                        }
                        break;
                    case 1:
                        try{
                            graph= initialize("../data/Locations.csv", "../data/Distances.csv");
                            cout << endl << TC_GRN << "Dataset loaded successfully." << TC_NRM << endl;
                            sleep(1);
                        }catch (exception& e) {
                            cout << endl  << TC_RED << e.what() << TC_NRM << endl;
                            cout << endl << "Press any key to continue...";
                            getchar();
                        }
                        break;
                    case 2:
                        tc_clear_screen();
                        show_cursor();
                        disable_raw_mode();

                        locFile = getCityFromUser("File path Vertices:");
                        distFile = getCityFromUser("File path Edges:");

                        enable_raw_mode();
                        hide_cursor();

                        try {
                            graph = initialize(locFile, distFile);
                            cout << endl << TC_GRN <<  "Custom dataset loaded successfully." << TC_NRM << endl;
                            sleep(1);
                        }catch (exception& e) {
                            cout << endl << TC_RED << e.what() << TC_NRM << endl;
                            cout << endl << "Press any key to continue...";
                            getchar();
                        }


                        tc_clear_screen();
                        displayMenu();
                        break;
                    default:
                        break;
                }
                displayMenu();
                return;
            default:
                break;
        }
    }
}

void Menu::optionsMenu() {
    vector<string> optionsItems = {
        "1. Change Data Set",
        "2. Change Text Color",
        "3. Help",
        "4. Run Tests",
        "0. Return to Menu"
    };

    int optionsIndex = 0;
    bool optionsRunning = true;

    while (optionsRunning) {
        tc_clear_screen();
        cout << "================= Options =================\n";
        for (int i = 0; i < optionsItems.size(); ++i) {
            if (i == optionsIndex)
                cout << currentColor << optionsItems[i] << TC_NRM << endl;
            else
                cout << optionsItems[i] << endl;
        }

        char c = getchar();
        switch (c) {
            case '0':
                optionsRunning = false;
                break;
            case '\033': // Handle arrow keys
                processArrowKeyInput(optionsIndex, optionsItems.size());
                break;
            case '\n':// Enter key
                if (optionsIndex == 4) {optionsRunning = false;}
                handleOptionsSelection(optionsIndex);
                break;
            default:
                break;
        }
    }
    displayMenu();
}

void Menu::handleOptionsSelection(int index) {
    switch (index) {
        case 0:
            changeDataSet();
            break;
        case 1:
            changeTextColor();
            break;
        case 2:
            displayHelp();
            break;
        case 3:
            runTests();
            break;
        case 4:
            break;
        default:
            break;
    }
}

void Menu::displayHelp() {
    tc_clear_screen();
    cout << "============================== Help Menu ==============================\n";
    cout << "\nAvailable Commands:\n";

    // Details for 'Plan Route'
    cout << "1. Plan Route\n";
    cout << "   Usage: Select 'Plan Route' from the main menu to start route planning.\n";
    cout << "   Description: Calculates the fastest route between two points.\n";
    cout << "   Format: Provide city IDs for the start and destination cities, and optionally specify segments to avoid.\n";
    cout << "   Example: Start City ID: 1, Destination City ID: 5, Avoid Segments: (2,3),(4,5)\n";

    // Details for 'Plan Green Route'
    cout << "\n2. Plan Green Route\n";
    cout << "   Usage: Select 'Plan Green Route' to calculate an environmentally friendly route that combines driving and walking.\n";
    cout << "   Description: Calculates routes that minimize driving and maximize walking, according to user preference.\n";
    cout << "   Format: In addition to city IDs, specify how much you are willing to walk.\n";
    cout << "   Example: Start City ID: 1, Destination City ID: 5, Max Walking Time: 20 minutes\n";

    // Details for 'Batch Mode'
    cout << "\n3. Batch Mode\n";
    cout << "   Usage: Select 'Batch Mode' to process routing requests from a file (input.txt) and write results in a file (output.txt).\n";
    cout << "   Description: Automates the processing of multiple routes using predefined input and output formats.\n";

    // Details for 'Options'
    cout << "\n4. Options\n";
    cout << "   - Change Data Set: Load different datasets for routing.\n";
    cout << "   - Change Text Color: Customize the color of the text in the console.\n";
    cout << "   - Run Tests: Execute predefined tests to check system functionality.(at the moment it is not implemented) \n";

    // Exit command
    cout << "\n5. Exit\n";
    cout << "   Usage: Exit the application.\n";

    // General tips
    cout << "\nGeneral Tips:\n";
    cout << "   - Use the arrow keys to navigate through the menu options.\n";
    cout << "   - You can also use number keys to navigate through the menu options.\n";
    cout << "   - Press Enter to select an option.\n";
    cout << "\nPress any key to return to the main menu...";

    getchar();
    displayMenu();
}

void Menu::runTests() {
    tc_clear_screen();
    cout << "Running system tests..." << endl << endl;
    sleep(2); // Simulate time taken to run tests
    cout << TC_GRN << "Tests completed successfully." << TC_NRM << endl;
    sleep(2);
    optionsMenu();
}

//HELP