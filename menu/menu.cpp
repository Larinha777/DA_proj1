#include "menu.h"
#include "tc.h"
#include "../algorithms/Algorithms.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

#include <unistd.h>

#include <vector>
#include <unordered_set>

using namespace std;

//Menu

Menu::Menu() : selectedItemIndex(0), currentColor(TC_MAG),
    graph(initialize("data/loc.csv", "data/dist.csv")),
    items({
        "1. Plan Route",
        "2. Plan Green Route",
        "3. Batch Mode",
        "4. Option",
        "0. Exit"
    }) {}

void Menu::displayMenu() const {
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
            askForRouteDetailsDriving();
            break;
        case 1:
            askForRouteDetailsDW();
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

void Menu::processArrowKeyInput(int& index, int maxIndex) const {
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

string getInfoFromUser(const string& prompt) {
    string city;
    cout << prompt << endl;
    getline(cin, city);
    return city;
}

unordered_set<int> parseNodeIds(const string& input) {
    unordered_set<int> nodeIds;
    stringstream ss(input);
    string nodeId;
    while (getline(ss, nodeId, ',')) {
        try {
            nodeIds.insert(stoi(nodeId));
        } catch (const std::invalid_argument& e) {
            cerr << "ERROR: Invalid node ID '" << nodeId << "' skipped." << endl;
        } catch (const std::out_of_range& e) {
            cerr << "ERROR: Node ID out of range '" << nodeId << "' skipped." << endl;
        }
    }
    return nodeIds;
}

vector<pair<int, int>> parseSegmentPairs(const string& input) {
    vector<pair<int, int>> avoidEdges;
    // Ensure the input is not empty and has the expected format before proceeding
    if (input.empty()) return avoidEdges;

    // Prepare the input string by removing the outer parentheses
    string segmentPairs = input.substr(1, input.length() - 2);

    // Use a stream to parse each segment defined by commas
    istringstream segmentStream(segmentPairs);
    string segment;

    // Process each segment
    while (getline(segmentStream, segment, ',')) {
        // Ensure the segment starts as expected with '(' and remove it
        if (segment.front() == '(') {
            segment = segment.substr(1);  // Remove the '('
        }

        int first = stoi(segment);

        // Find the position of the comma separating the two numbers
        size_t commaPos = segment.find(',');

        getline(segmentStream, segment, ')');
        int second = stoi(segment);

        avoidEdges.emplace_back(first, second);

        // If the next character in the stream is a comma, move past it
        if (segmentStream.peek() == ',') segmentStream.get(); // This handles the delimiter after each pair
    }
    return avoidEdges;
}

bool isParkingNode(const Graph& graph, int nodeId) {
    Vertex* v = graph.findVertex(nodeId);
    return v != nullptr && v->isPark();
}

//Main Functions

void Menu::batchProcess() {
    tc_clear_screen();

    ifstream inputFile("../batch/input.txt");
    ofstream outputFile("../batch/output.txt");

    if (!inputFile.is_open() || !outputFile.is_open()) {
        std::cerr << "Error opening file(s)." << std::endl;
        return; // Exit if file cannot be opened
    }

    string line, mode, temp;
    int source, destination;

    // Read the first line for mode
    getline(inputFile, line);
    mode = line.substr(line.find(':') + 1);

    // Read the second line for source
    getline(inputFile, line);
    source = stoi(line.substr(line.find(':') + 1));

    // Read the third line for destination
    getline(inputFile, line);
    destination = stoi(line.substr(line.find(':') + 1));


    unordered_set<int> avoidNodes;
    vector<std::pair<int, int>> avoidEdges;
    int includeNode = -1;
    int maxWalkTime = 0;


    // Read optional parameters if present
    while (getline(inputFile, line)) {
        if (line.find("AvoidNodes:") != string::npos) {
            avoidNodes = parseNodeIds(line.substr(line.find(':') + 1));
        } else if (line.find("AvoidSegments:") != string::npos) {
            avoidEdges = parseSegmentPairs(line.substr(line.find(':') + 1));
        } else if (line.find("IncludeNode:") != string::npos) {
            istringstream includeStream(line.substr(line.find(':') + 1));
            includeStream >> includeNode;
        } else if (line.find("MaxWalkTime:") != string::npos) {
            istringstream includeStream(line.substr(line.find(':') + 1));
            includeStream >> maxWalkTime;
        }
    }

    if (includeNode == -1) {includeNode= source;}

    // Now that all data is parsed, perform the appropriate routing function based on the mode
    std::string routeDetails;
    if (mode == "driving" && avoidNodes.empty() && avoidEdges.empty()) {
        routeDetails = SimpleDriving(&graph, source, destination);
    } else if (mode == "driving") {
        routeDetails = RestrictedDriving(&graph, source, destination, avoidNodes, avoidEdges, includeNode);
    } else if (mode == "driving-walking") {
        routeDetails = DrivingWalking(&graph, source, destination, maxWalkTime, avoidNodes, avoidEdges);
    }

    // Write the routing details to the output file
    outputFile << routeDetails << endl;
    /*
    //Debug avoidEdges parsing
    for (const auto& pair : avoidEdges) {
        outputFile << "(" << pair.first << "," << pair.second << ")" << "\n";
    }
    */

    inputFile.close();
    outputFile.close();
    cout << TC_GRN <<"Batch processing completed." << TC_NRM << endl;
    sleep(1);
    displayMenu();
}

void Menu::askForRouteDetailsDriving() {
    tc_clear_screen();
    show_cursor();
    disable_raw_mode();

    try {
        string sourceStr = getInfoFromUser("Enter the start city ID:");
        int source = stoi(sourceStr);

        string destinationStr = getInfoFromUser("\nEnter the destination city ID:");
        int destination = stoi(destinationStr);

        string avoidNodesStr = getInfoFromUser("\nEnter city IDs to avoid (optional, press enter to skip):");
        unordered_set<int> avoidNodes = parseNodeIds(avoidNodesStr);

        string avoidSegmentsStr = getInfoFromUser("\nEnter segments to avoid (optional, format (x,y),(a,b),etc.):");
        vector<pair<int, int>> avoidEdges = parseSegmentPairs(avoidSegmentsStr);

        string includeNodeStr = getInfoFromUser("\nEnter a city ID to route through (optional, press enter to skip):");
        int includeNode = includeNodeStr.empty() ? source : stoi(includeNodeStr);

        hide_cursor();

        string routeDetails;
        if (avoidNodes.empty() && avoidEdges.empty()) {
            routeDetails = SimpleDriving(&graph, source, destination);
        } else {
            routeDetails = RestrictedDriving(&graph, source, destination, avoidNodes, avoidEdges, includeNode);
        }

        tc_clear_screen();
        cout << routeDetails << endl;

        cout << "Press enter to return to the menu..." <<endl;
        getchar();

    } catch (const std::invalid_argument& e) {
        hide_cursor();
        cout << TC_RED << "Invalid input. Please enter values with correct format." << TC_NRM << endl;
        getchar();
    }catch (const std::out_of_range& e) {
        hide_cursor();
        cout << TC_RED << "Input out of range. Ensure numbers are within the valid range." << TC_NRM << endl;
        getchar();
    }

    enable_raw_mode();
    displayMenu();
}

void Menu::askForRouteDetailsDW() {
    tc_clear_screen();
    show_cursor();
    disable_raw_mode();

    try {
        string sourceStr = getInfoFromUser("Enter the start city ID:");
        int source = stoi(sourceStr);

        string destinationStr = getInfoFromUser("\nEnter the destination city ID:");
        int destination = stoi(destinationStr);

        // Check if source and destination are the same or designated as parking nodes
        if (source == destination){
            throw runtime_error("Source and destination cannot be the same.");
        }else if (isParkingNode(graph, source) || isParkingNode(graph, destination)) {
            throw runtime_error("Neither the source nor the destination can be parking spots.");
        }

        string maxWalkTimeStr = getInfoFromUser("\nEnter the Max Walk Time:");
        int maxWalkTime = stoi(maxWalkTimeStr);

        string avoidNodesStr = getInfoFromUser("\nEnter city IDs to avoid (optional, press enter to skip):");
        unordered_set<int> avoidNodes = parseNodeIds(avoidNodesStr);

        string avoidSegmentsStr = getInfoFromUser("\nEnter segments to avoid (optional, format (x,y),(a,b),etc.):");
        vector<pair<int, int>> avoidEdges = parseSegmentPairs(avoidSegmentsStr);

        hide_cursor();

        string routeDetails = DrivingWalking(&graph, source, destination, maxWalkTime, avoidNodes, avoidEdges);
        tc_clear_screen();
        cout << routeDetails << endl;

        cout << "Press enter to return to the menu..." <<endl;
        getchar();

    } catch (const std::invalid_argument& e) {
        hide_cursor();
        cout << TC_RED << "Invalid input. Please enter values with correct format." << TC_NRM << endl;
        getchar();

    } catch (const std::out_of_range& e) {
        hide_cursor();
        cout << TC_RED << "Input out of range. Please enter smaller numbers." << TC_NRM << endl;
        getchar();

    }catch (const std::runtime_error& e) {
        hide_cursor();
        cout << TC_RED << e.what() << TC_NRM << endl;
        getchar();
    }

    enable_raw_mode();
    displayMenu();
}

// Optional (Menu experience enhancers)

void Menu::optionsMenu() {
    vector<string> optionsItems = {
        "1. Change Data Set",
        "2. Change Text Color",
        "3. Help",
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

        char input = getchar();
        switch (input) {
            case '0':
                optionsRunning = false;
                break;
            case '1':
            case '2':
            case '3':
                // Set the index based on the numeric key pressed and execute the selection immediately.
                optionsIndex = input - '1';
                handleOptionsSelection(optionsIndex);
            break;
            case '\033': // Handle arrow keys
                processArrowKeyInput(optionsIndex, optionsItems.size());
                break;
            case '\n':// Enter key
                if (optionsIndex == 3) {optionsRunning = false;}
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
        default:
            break;
    }
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
            } else {
                cout << "  " << colorOptions[i] << "\n";
            }
        }

        switch (getchar()) {
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
            default:
                break;
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


        switch (getchar()) {
            case '\033': // ESC sequence for arrow keys
                processArrowKeyInput(choice, dataOptions.size());
                break;
            case '\n': // Enter key
                switch (choice) {
                    case 0:
                        try {
                            graph= initialize("../data/Locations.csv", "../data/Distances.csv");
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
                            graph = initialize("../data/loc.csv", "../data/dist.csv");
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

                        locFile = getInfoFromUser("File path Vertices:");
                        distFile = getInfoFromUser("File path Edges:");

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

void Menu::displayHelp() const {
    vector<string> helpPages = {
        // Page 1: Plan Route
        R"(================================ Help Menu ================================
1. Plan Route
   Description: Calculates the fastest driving route between two cities.
   Required: Start City ID, Destination City ID
   Optional:
     - AvoidNodes: Comma-separated list of City IDs to avoid
     - AvoidSegments: List of edges in format (x,y),(a,b), etc.
           Example: AvoidSegments: (2,3),(4,5)
     - IncludeNode: City ID that must be passed through)",

        // Page 2: Plan Green Route
        R"(================================ Help Menu ================================
2. Plan Green Route
   Description: Calculates a greener route by combining driving and walking.
   Required: Start City ID, Destination City ID, Max Walk Time (in minutes)
   Optional:
     - AvoidNodes: Comma-separated list of City IDs to avoid
     - AvoidSegments: (x,y),(a,b), etc.)",

        // Page 3: Batch Mode
        R"(================================ Help Menu ================================
3. Batch Mode
   Description: Processes a routing task from a text file input.
   File Format (input.txt):
     Mode:driving OR Mode:driving-walking
     Source:<ID>
     Destination:<ID>
     MaxWalkTime:<minutes> (only for driving-walking)
     AvoidNodes:1,3,7
     AvoidSegments:(2,3),(4,5)
     IncludeNode:<ID>
   Output is written to: output.txt)",

        // Page 4: Options, Exit, Tips
        R"(================================ Help Menu ================================
4. Options
   - Change Data Set: Load different datasets for routing.
   - Change Text Color: Customize the console text color.

5. Exit
   - Closes the application.

General Tips:
   - Use arrow keys or number keys to navigate.
   - Press Enter to select a menu option.
   - Input must be numeric IDs. Format errors are handled.)"
    };

    int currentPage = 0;
    const int totalPages = helpPages.size();

    while (true) {
        tc_clear_screen();
        cout << helpPages[currentPage] << "\n";
        cout << "\n< Page " << currentPage + 1 << "/" << totalPages << " >" <<" | q to exit";

        switch (getchar()) {
            case 'q':
                return; // Exit
            case '\033': {
                getchar();
                switch (getchar()) {
                    case 'C': // Right arrow
                        if (currentPage < totalPages - 1) currentPage++;
                        break;
                    case 'D': // Left arrow
                        if (currentPage > 0) currentPage--;
                        break;
                    default:
                        break;
                }
                break;
            }
            default:
                break;
        }
    }
}
