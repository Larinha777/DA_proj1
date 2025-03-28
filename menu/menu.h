#ifndef MENU_H
#define MENU_H

#include <vector>
#include <string>

#include "../data_structures/Graph.h"

class Menu {
private:
    int selectedItemIndex;
    std::string currentColor;
    Graph graph;
    std::vector<std::string> items;

public:

    Menu();
    void run();
    void displayMenu() const;
    void navigateMenu();
    void handleSelection();

    //Batch
    void batchProcess();
    void askForRouteDetailsDriving();
    void askForRouteDetailsDW();

    void processArrowKeyInput(int& index, int maxIndex) const;

    //Options
    void optionsMenu();
    void handleOptionsSelection(int index);
    void changeDataSet();
    void changeTextColor();
    void displayHelp() const;
};

#endif //MENU_H
