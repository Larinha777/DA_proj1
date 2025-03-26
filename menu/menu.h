#ifndef MENU_H
#define MENU_H

#include <vector>

#include <string>

#include "../data_structures/Graph.h"

class Menu {
private:
    int selectedItemIndex;
    bool dataLoaded;
    std::vector<std::string> items;
    std::string currentColor;
    Graph graph;

public:
    Menu();
    void run();
    void displayMenu();

    void navigateMenu();
    void handleSelection();

    //Options
    void optionsMenu();
    void handleOptionsSelection(int index);
    void changeDataSet();
    void changeTextColor();
    void displayHelp();
    void runTests();

    //Batch
    void batchProcess();

    //Aux
    void processArrowKeyInput(int& index, int maxIndex);
    void askForRouteDetails();
    static std::string getCityFromUser(const std::string& prompt);


};

#endif //MENU_H
