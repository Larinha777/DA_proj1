#include "menu.h"
#include "tc.h"
#include "../algorithms/Algorithms.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include <stdexcept>
#include <regex>

#include <unistd.h>

#include <vector>
#include <unordered_set>

using namespace std;

// TODO: Split Menu.cpp into multiple files (MenuInput.cpp, MenuBatch.cpp, etc.) if this becomes harder to manage.

//Menu

Menu::Menu() : selectedItemIndex(0), currentColor(TC_MAG),
    graph(initialize("../data/loc.csv", "../data/dist.csv")),
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

unordered_set<int> parseNodeIds(const string& input,  vector<string>& errors) {
    unordered_set<int> nodeIds;
    stringstream ss(input);
    string nodeId;
    while (getline(ss, nodeId, ',')) {
        try {
            nodeIds.insert(stoi(nodeId));
        } catch (const invalid_argument& e) {
            errors.emplace_back("Invalid node ID '" + nodeId + "' skipped.");
        } catch (const out_of_range& e) {
            errors.emplace_back("Node ID out of range '" + nodeId + "' skipped.");
        }
    }
    return nodeIds;
}

vector<pair<int, int>> parseSegmentPairs(const string& input,  vector<string>& errors) {
    vector<pair<int, int>> segments;
    //NOTE: strict regex for a pair of ints and R as raw string
    // \( = ( , (\d+) = number , ',' = comma , (\d+) = number , \) = )
    regex pattern(R"(\((\d+),(\d+)\))");

    //NOTE: regex specific iterator for strings
    auto begin = sregex_iterator(input.begin(), input.end(), pattern);
    auto end = sregex_iterator();

    for (auto it = begin; it != end; ++it) {
        try {
            int a = stoi((*it)[1]);
            int b = stoi((*it)[2]);
            segments.emplace_back(a, b);
        } catch (...) {
            errors.emplace_back("Invalid segment pair.");
        }
    }

    //Detect Junk
    string leftovers = input;
    for (const auto& seg : segments) {
        string pattern = "(" + to_string(seg.first) + "," + to_string(seg.second) + ")";
        size_t pos = leftovers.find(pattern);
        if (pos != string::npos) {
            leftovers.replace(pos, pattern.length(), "");
        }
    }
    // Remove commas and whitespace to detect leftover junk
    leftovers.erase(remove_if(leftovers.begin(), leftovers.end(), [](char c) {
        return c == ',' || isspace(c);
    }), leftovers.end());

    if (!leftovers.empty()) {
        errors.emplace_back("Unrecognized or malformed segment(s): '" + leftovers + "'");
    }

    return segments;
}

bool isParkingNode(const Graph& graph, int nodeId) {
    Vertex* v = graph.findVertex(nodeId);
    return v != nullptr && v->isPark();
}

//Main Functions

void Menu::batchProcess() {
    tc_clear_screen();

    vector<string> errors;

    ifstream inputFile("../batch/input.txt");
    ofstream outputFile("../batch/output.txt");

    if (!inputFile.is_open() || !outputFile.is_open()) {
        cerr << TC_RED << "ERROR: Error opening file(s)." << TC_NRM << endl;
        return; // Exit if file cannot be opened
    }

    string line, mode, temp;
    int source, destination;

    // Read the first line for mode
    if (!getline(inputFile, line) || line.find("Mode:")!=0) errors.emplace_back("Missing or malformed Mode line");
    else mode = line.substr(5);

    // Read the second line for source
    if (!getline(inputFile, line) || line.find("Source:")!=0) errors.emplace_back("Missing or malformed Source line");
    else try {source = stoi(line.substr(7));}catch (...) {errors.emplace_back("Invalid Source ID");}

    // Read the third line for destination
    if (!getline(inputFile, line) || line.find("Destination:")!=0) errors.emplace_back("Missing or malformed Destination line");
    else try {destination = stoi(line.substr(12));} catch (...) {errors.emplace_back("Invalid Destination ID");}


    unordered_set<int> avoidNodes;
    vector<pair<int, int>> avoidEdges;
    int includeNode = -1;
    int maxWalkTime = 0;


    // Read optional parameters
    while (getline(inputFile, line)) {
        if (line.find("AvoidNodes:") != string::npos) {
            avoidNodes = parseNodeIds(line.substr(11), errors);
        } else if (line.find("AvoidSegments:") != string::npos) {
            avoidEdges = parseSegmentPairs(line.substr(14), errors);
        } else if (line.find("IncludeNode:") != string::npos) {
            string value = line.substr(12);
            if (!value.empty()) {try {includeNode = stoi(line.substr(12));} catch (...) {errors.emplace_back("Invalid IncludeNode ID");}}
        } else if (line.find("MaxWalkTime:") != string::npos) {
            string value = line.substr(12);
            if (!value.empty()) {try {maxWalkTime = stoi(line.substr(12));} catch (...) {errors.emplace_back("Invalid MaxWalkTime value");}}
        }
    }

    if (includeNode == -1) {includeNode= source;}
    if (mode != "driving" && mode != "driving-walking") errors.push_back("Unsupported mode: " + mode);
    if (mode == "driving-walking" && source == destination) errors.emplace_back("In mode driving-walking source can not be the same as destination");
    if (mode == "driving-walking" && (isParkingNode(graph, source) || isParkingNode(graph, destination))) errors.emplace_back("In mode driving-walking neither source or destination can be parking spots");

    if (!graph.findVertex(source)) errors.emplace_back("Source node ID " + to_string(source) + " not found in the graph.");
    if (!graph.findVertex(destination)) errors.emplace_back("Destination node ID " + to_string(destination) + " not found in the graph.");
    if (includeNode != source && !graph.findVertex(includeNode)) errors.emplace_back("IncludeNode ID " + to_string(includeNode) + " not found in the graph.");
    for (int nodeId : avoidNodes) if (!graph.findVertex(nodeId)) errors.emplace_back("AvoidNode ID " + to_string(nodeId) + " not found in the graph.");

    //Display and return if error where found
    if (!errors.empty()) {
        cout << TC_RED << "Batch input errors detected:" << TC_NRM << endl;
        for (const auto& err : errors) cout << TC_RED << err << TC_NRM << endl;
        cout << "Press enter to return to the menu..." << endl;
        getchar();
        displayMenu();
        return;
    }

    //Call right algorithm

    string routeDetails;
    if (mode == "driving" && avoidNodes.empty() && avoidEdges.empty()) {
        routeDetails = SimpleDriving(&graph, source, destination);
    } else if (mode == "driving") {
        routeDetails = RestrictedDriving(&graph, source, destination, avoidNodes, avoidEdges, includeNode);
    } else if (mode == "driving-walking") {
        routeDetails = DrivingWalking(&graph, source, destination, maxWalkTime, avoidNodes, avoidEdges);
    }

    // Write the routing details to the output file
    outputFile << routeDetails << endl;

    //Debug avoidEdges parsing
    bool debug = false;
    if (debug) {
        for (const auto& pair : avoidEdges) {
            outputFile << "(" << pair.first << "," << pair.second << ")" << "\n";
        }
    }

    inputFile.close();
    outputFile.close();
    cout << TC_GRN <<"Batch processing completed." << TC_NRM << endl;
    sleep(1);
    displayMenu();
}

void Menu::askForRouteDetailsDriving() {
    int source, destination, includeNode;
    unordered_set<int> avoidNodes;
    vector<pair<int, int>> avoidEdges;

    string answeredSummary;

    // SOURCE
    while (true) {
        tc_clear_screen();
        disable_raw_mode();
        show_cursor();
        cout << answeredSummary;

        string sourceStr = getInfoFromUser("Enter the start city ID:");
        try {
            source = stoi(sourceStr);
            if (!graph.findVertex(source)) {
                cout << TC_RED << "ERROR: Source ID not found in graph." << TC_NRM << endl;
                cout << "\nPress enter to try again..." << endl;
                getchar();
                continue;
            }
            answeredSummary +=  + "Enter the start city ID:\n" + to_string(source);
            break;
        }catch (...) {
            cout << TC_RED << "ERROR: Invalid source ID. Please enter a valid number." << TC_NRM << endl;
            cout << "\nPress enter to try again..." << endl;
            hide_cursor();
            enable_raw_mode();
            getchar();
        }
    }

    //DESTINATION
    while (true) {
        tc_clear_screen();
        disable_raw_mode();
        show_cursor();
        cout << answeredSummary;

        string destStr = getInfoFromUser("\nEnter the destination city ID:");
        try {
            destination = stoi(destStr);
            if (!graph.findVertex(destination)) {
                cout << TC_RED << "ERROR: Destination ID not found in graph." << TC_NRM << endl;
                cout << "\nPress enter to try again..." << endl;
                getchar();
                continue;
            }
            answeredSummary +=  + "\nEnter the destination city ID:\n" + to_string(destination);
            break;
        } catch (...) {
            cout << TC_RED << "ERROR: Invalid destination ID. Please enter a valid number." << TC_NRM << endl;
            cout << "\nPress enter to try again..." << endl;
            hide_cursor();
            enable_raw_mode();
            getchar();
        }
    }

    //AVOID NODES OPTIONAL
    while (true) {
        tc_clear_screen();
        disable_raw_mode();
        show_cursor();
        cout << answeredSummary;

        string avoidStr = getInfoFromUser("\nEnter city IDs to avoid (optional, press enter to skip):");
        vector<string> errors;
        avoidNodes = parseNodeIds(avoidStr, errors);

        for (int id : avoidNodes) {
            if (!graph.findVertex(id)) {
                errors.emplace_back("AvoidNode ID " + to_string(id) + " not found in graph.");
            }else {
                if (id == source) errors.emplace_back("AvoidNode ID " + to_string(id) + " is the same as the source node.");
                if (id == destination) errors.emplace_back("AvoidNode ID " + to_string(id) + " is the same as the destination node.");
                if (id == includeNode) errors.emplace_back("AvoidNode ID " + to_string(id) + " is the same as the include node.");
            }
        }

        if (!avoidStr.empty() && errors.empty()) {
            answeredSummary += "\nEnter city IDs to avoid (optional, press enter to skip):\n" + avoidStr;
            break;
        } else if (errors.empty()) {
            answeredSummary += "\nEnter city IDs to avoid (optional, press enter to skip):\n<none>";
            break;
        }

        for (const auto& err : errors) {
            cout << TC_RED << "ERROR: " << err << TC_NRM << endl;
        }

        cout << "\nPress enter to continue..." << endl;
        hide_cursor();
        enable_raw_mode();
        getchar();
    }

    // AVOID SEGMENTS OPTIONAL
    while (true) {
        tc_clear_screen();
        disable_raw_mode();
        show_cursor();
        cout << answeredSummary;

        string segStr = getInfoFromUser("\nEnter segments to avoid (format: (x,y),(a,b), etc. — optional, press enter to skip):");
        vector<string> errors;
        avoidEdges = parseSegmentPairs(segStr, errors);

        for (const auto& [a, b] : avoidEdges) {
            if (!graph.findVertex(a) || !graph.findVertex(b)) {
                errors.emplace_back("AvoidSegment (" + to_string(a) + "," + to_string(b) + ") contains a node not found in the graph.");
            }
        }

        if (!segStr.empty() && errors.empty()) {
            answeredSummary += "\nEnter segments to avoid (optional):\n" + segStr;
            break;
        } else if (errors.empty()) {
            answeredSummary += "\nEnter segments to avoid (optional):\n<none>";
            break;
        }

        for (const auto& err : errors) {
            cout << TC_RED << "ERROR: " << err << TC_NRM << endl;
        }

        cout << "\nPress enter to continue..." << endl;
        hide_cursor();
        enable_raw_mode();
        getchar();
    }

    //INCLUDE NODE OPTIONAL
    while (true) {
        tc_clear_screen();
        disable_raw_mode();
        show_cursor();
        cout << answeredSummary;

        string includeStr = getInfoFromUser("\nEnter a city ID to route through (optional, press enter to skip):");
        if (includeStr.empty()) {
            includeNode = source;
            answeredSummary += "\nEnter a city ID to route through (optional, press enter to skip):\n<none>";
            break;
        }
        try {
            includeNode = stoi(includeStr);
            if (!graph.findVertex(includeNode)) {
                cout << TC_RED << "ERROR: Include node not found in graph." << TC_NRM << endl;
                cout << "\nPress enter to try again..." << endl;
                getchar();
                continue;
            }
            answeredSummary +=  + "\nEnter a city ID to route through (optional, press enter to skip):\n" + to_string(includeNode);
            break;
        } catch (...) {
            cout << TC_RED << "ERROR: Invalid include node ID." << TC_NRM << endl;
            cout << "\nPress enter to try again..." << endl;
            hide_cursor();
            enable_raw_mode();
            getchar();
        }
    }

    string routeDetails;
    if (avoidNodes.empty() && avoidEdges.empty()) {
        routeDetails = SimpleDriving(&graph, source, destination);
    } else {
        routeDetails = RestrictedDriving(&graph, source, destination, avoidNodes, avoidEdges, includeNode);
    }

    hide_cursor();
    enable_raw_mode();
    tc_clear_screen();
    cout << routeDetails << endl;

    cout << "Press enter to return to the menu..." <<endl;
    getchar();
    displayMenu();
}

void Menu::askForRouteDetailsDW() {
    int source, destination, maxWalkTime;
    unordered_set<int> avoidNodes;
    vector<pair<int, int>> avoidEdges;

    string answeredSummary;

    // SOURCE
    while (true) {
        tc_clear_screen();
        disable_raw_mode();
        show_cursor();
        cout << answeredSummary;

        string sourceStr = getInfoFromUser("Enter the start city ID:");
        try {
            source = stoi(sourceStr);
            if (!graph.findVertex(source)) {
                cout << TC_RED << "ERROR: Source ID not found in graph." << TC_NRM << endl;
                cout << "\nPress enter to try again..." << endl;
                getchar();
                continue;
            }
            if (isParkingNode(graph, source)) {
                cout << TC_RED << "ERROR: Source node cannot be a parking spot in driving-walking mode." << TC_NRM << endl;
                cout << "\nPress enter to try again..." << endl;
                getchar();
                continue;
            }
            answeredSummary +=  + "Enter the start city ID:\n" + to_string(source);
            break;
        }catch (...) {
            cout << TC_RED << "ERROR: Invalid source ID. Please enter a valid number." << TC_NRM << endl;
            cout << "\nPress enter to try again..." << endl;
            hide_cursor();
            enable_raw_mode();
            getchar();
        }
    }

    //DESTINATION
    while (true) {
        tc_clear_screen();
        disable_raw_mode();
        show_cursor();
        cout << answeredSummary;

        string destStr = getInfoFromUser("\nEnter the destination city ID:");
        try {
            destination = stoi(destStr);
            if (!graph.findVertex(destination)) {
                cout << TC_RED << "ERROR: Destination ID not found in graph." << TC_NRM << endl;
                cout << "\nPress enter to try again..." << endl;
                getchar();
                continue;
            }
            if (destination == source) {
                cout << TC_RED << "ERROR: Destination must be different from source." << TC_NRM << endl;
                cout << "\nPress enter to try again..." << endl;
                getchar();
                continue;
            }
            if (isParkingNode(graph, destination)) {
                cout << TC_RED << "ERROR: Destination node cannot be a parking spot in driving-walking mode." << TC_NRM << endl;
                cout << "\nPress enter to try again..." << endl;
                getchar();
                continue;
            }
            answeredSummary +=  + "\nEnter the destination city ID:\n" + to_string(destination);
            break;
        } catch (...) {
            cout << TC_RED << "ERROR: Invalid destination ID. Please enter a valid number." << TC_NRM << endl;
            cout << "\nPress enter to try again..." << endl;
            hide_cursor();
            enable_raw_mode();
            getchar();
        }
    }

    // MAX WALK TIME
    while (true) {
        tc_clear_screen();
        disable_raw_mode();
        show_cursor();
        cout << answeredSummary;

        string walkTimeStr = getInfoFromUser("\nEnter the Max Walk Time (minutes):");
        try {
            maxWalkTime = stoi(walkTimeStr);
            if (maxWalkTime < 0) {
                cout << TC_RED << "ERROR: Max walk time must be a positive number." << TC_NRM << endl;
                getchar(); continue;
            }
            answeredSummary += "\nEnter the Max Walk Time (minutes):\n" + to_string(maxWalkTime);
            break;
        } catch (...) {
            cout << TC_RED << "ERROR: Invalid input. Enter a valid number." << TC_NRM << endl;
            hide_cursor();
            enable_raw_mode();
            getchar();
        }
    }

    //AVOID NODES OPTIONAL
    while (true) {
        tc_clear_screen();
        disable_raw_mode();
        show_cursor();
        cout << answeredSummary;

        string avoidStr = getInfoFromUser("\nEnter city IDs to avoid (optional, press enter to skip):");
        vector<string> errors;
        avoidNodes = parseNodeIds(avoidStr, errors);

        for (int id : avoidNodes) {
            if (!graph.findVertex(id)) {
                errors.emplace_back("AvoidNode ID " + to_string(id) + " not found in graph.");
            }else {
                if (id == source) errors.emplace_back("AvoidNode ID " + to_string(id) + " is the same as the source node.");
                if (id == destination) errors.emplace_back("AvoidNode ID " + to_string(id) + " is the same as the destination node.");
            }
        }

        if (!avoidStr.empty() && errors.empty()) {
            answeredSummary += "\nEnter city IDs to avoid (optional, press enter to skip):\n" + avoidStr;
            break;
        } else if (errors.empty()) {
            answeredSummary += "\nEnter city IDs to avoid (optional, press enter to skip):\n<none>";
            break;
        }

        for (const auto& err : errors) {
            cout << TC_RED << "ERROR: " << err << TC_NRM << endl;
        }

        cout << "\nPress enter to continue..." << endl;
        hide_cursor();
        enable_raw_mode();
        getchar();
    }

    // AVOID SEGMENTS OPTIONAL
    while (true) {
        tc_clear_screen();
        disable_raw_mode();
        show_cursor();
        cout << answeredSummary;

        string segStr = getInfoFromUser("\nEnter segments to avoid (format: (x,y),(a,b), etc. — optional, press enter to skip):");
        vector<string> errors;
        avoidEdges = parseSegmentPairs(segStr, errors);

        for (const auto& [a, b] : avoidEdges) {
            if (!graph.findVertex(a) || !graph.findVertex(b)) {
                errors.emplace_back("AvoidSegment (" + to_string(a) + "," + to_string(b) + ") contains a node not found in the graph.");
            }
        }

        if (!segStr.empty() && errors.empty()) {
            answeredSummary += "\nEnter segments to avoid (optional):\n" + segStr;
            break;
        } else if (errors.empty()) {
            answeredSummary += "\nEnter segments to avoid (optional):\n<none>";
            break;
        }

        for (const auto& err : errors) {
            cout << TC_RED << "ERROR: " << err << TC_NRM << endl;
        }

        cout << "\nPress enter to continue..." << endl;
        hide_cursor();
        enable_raw_mode();
        getchar();
    }

    // Execute route algorithm
    string routeDetails = DrivingWalking(&graph, source, destination, maxWalkTime, avoidNodes, avoidEdges);

    hide_cursor();
    enable_raw_mode();
    tc_clear_screen();
    cout << routeDetails << endl;

    cout << "Press enter to return to the menu..." << endl;
    getchar();
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
     - IncludeNode: City ID that must be passed through
     - AvoidNodes: Comma-separated list of City IDs to avoid
     - AvoidSegments: List of edges in format (x,y),(a,b), etc.
   Example: AvoidSegments: (2,3),(4,5))",

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
     AvoidNodes:1,3,7
     AvoidSegments:(2,3),(4,5)
     IncludeNode:<ID>
     MaxWalkTime:<minutes> (only for driving-walking)
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
