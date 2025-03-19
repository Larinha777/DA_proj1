#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <algorithm>
#include <unordered_set>

#include "Dijkstra.h"
#include "Graph.h"
#include "MutablePriorityQueue.h"

/**
 * @brief Prints the path in the correct order
 *
 * @param v The vector of ints representing the id of each location in the path.
 * The path is in reversed order (from destination to origin)
 */
void printPath(const std::vector<int> &v);


/**
 * @brief Get path from origin to dest and mark all visited nodes to avoid
 *
 * @param g A pointer to the graph that has the origin and destination Vertex
 * @param origin The id of the origin vertex of the path that is being calculated
 * @param dest The id of the destination vertex of the path that is being calculated
 * @param time A reference to an int where the total time of the path is being held
 *
 * @return A vector of ints representing the id of each location in the path. The
 * path is in reversed order (from destination to origin)
 */
std::vector<int> getPath(Graph * g, const int &origin, const int &dest, int &time);



// Fastest Route + Independent Route Planning
void SimpleDriving(Graph * g, const int &origin, const int &dest);


// Restricted Route Planning
void RestrictedDriving(Graph * g, const int &origin, const int &dest, const std::unordered_set<int> &avoidNodes,
    const std::vector<std::pair<int,int>> &avoidEdges,const int &includeNode);
//later will change vector in avoid edges to unordered set

// Best route for driving and walking
void DrivingWalking(Graph * g, const int &origin, const int &dest, const std::unordered_set<int> &avoidNodes,
    const std::vector<std::pair<int,int>> &avoidEdges,const int &includeNode);


// Extra
// Approximate Solution: If no suitable route is found, display a list of suggestions
// representing the best feasible alternative routes that approximate user requirements. For example, if the
// suggestions increase the maximum walking time, indicate the new time. Present 2 alternatives, sorted by
// overall travel time, but always including a driving and a walking segment.


#endif //ALGORITHMS_H
