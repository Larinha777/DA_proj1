#ifndef MENU_H
#define MENU_H

#include <vector>
#include <string>
#include "../data_structures/Graph.h"

/**
 * @brief Menu class for the Route Planning Tool.
 *
 * This class manages the user interface for planning routes. It displays the menu,
 * navigates user input, and calls the appropriate routing functions. Additionally,
 * it provides batch processing and options to change datasets, text color, and display help.
 */
class Menu {
private:
    int selectedItemIndex;    ///< The index of the currently selected menu item.
    std::string currentColor; ///< The current text color code used for highlighting selections.
    Graph graph;              ///< Graph object containing route and location data.
    std::vector<std::string> items; ///< List of menu items displayed to the user.

public:
    /**
     * @brief Constructs a new Menu object.
     *
     * Initializes the menu with default settings, loads a default dataset for the graph,
     * and sets the initial menu items.
     */
    Menu();

    /**
     * @brief Starts the menu interface.
     *
     * Enables raw mode, hides the cursor, displays the menu, and begins processing user input.
     */
    void run();

    /**
     * @brief Displays the menu on the console.
     *
     * Clears the screen and prints all the menu items. The currently selected item is
     * highlighted with the current text color.
     */
    void displayMenu() const;

    /**
     * @brief Handles navigation throughout the menu.
     *
     * Processes keyboard input (arrow keys, numbers, Enter, etc.) to navigate through the
     * menu items and execute selections.
     */
    void navigateMenu();

    /**
     * @brief Executes an action based on the current menu selection.
     *
     * Calls the appropriate function for the selected menu item (e.g., route planning,
     * batch processing, options menu, or exit).
     */
    void handleSelection();

    /**
     * @brief Processes routing requests from a batch file.
     *
     * Reads input parameters from a batch file (input.txt), processes routing based on mode,
     * and writes the output to a file (output.txt).
     */
    void batchProcess();

    /**
     * @brief Asks the user for routing details for a driving route.
     *
     * Prompts for source and destination city IDs, and optionally for nodes/segments to avoid
     * and an intermediate node. Displays the routing results upon completion.
     */
    void askForRouteDetailsDriving();

    /**
     * @brief Asks the user for routing details for a driving-walking (green route) plan.
     *
     * Prompts for source and destination city IDs, maximum walking time, and optionally for
     * nodes/segments to avoid. It validates that the source and destination are not parking nodes.
     * Displays the routing results upon completion.
     */
    void askForRouteDetailsDW();

    /**
     * @brief Processes input from arrow keys to update the selected index.
     *
     * Adjusts the index based on the arrow key input (up or down). After updating the index,
     * the menu is re-displayed.
     *
     * @param index Reference to the index that is currently selected.
     * @param maxIndex The maximum number of menu items.
     */
    void processArrowKeyInput(int& index, int maxIndex) const;

    /**
     * @brief Displays the options menu.
     *
     * Shows additional settings for changing the data set, text color, and displaying help.
     * This menu remains active until the user chooses to return to the main menu.
     */
    void optionsMenu();

    /**
     * @brief Handles selection in the options menu.
     *
     * Based on the index selected in the options menu, it will call the appropriate function
     * (e.g., change dataset, change text color, or display help).
     *
     * @param index The index of the selected options menu item.
     */
    void handleOptionsSelection(int index);

    /**
     * @brief Changes the dataset used for routing.
     *
     * Provides a submenu for selecting from different datasets (Normal, Small, Custom) and
     * loads the selected dataset into the graph.
     */
    void changeDataSet();

    /**
     * @brief Changes the text color of the menu display.
     *
     * Allows the user to choose a different text color from a list of options to highlight the
     * currently selected menu item.
     */
    void changeTextColor();

    /**
     * @brief Displays a help menu.
     *
     * Provides instructions and details for each available routing option including how to
     * input data correctly. The help is divided into multiple pages that can be navigated with
     * arrow keys.
     */
    void displayHelp() const;
};

#endif // MENU_H
