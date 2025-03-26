#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <algorithm>
#include <sstream>

#include "../data_structures/Graph.h"
#include "../data_structures/MutablePriorityQueue.h"
#include "util.h"

/**
 * @param g A pointer to the graph that has the origin and destination Vertex.
 * @param origin The id of the origin vertex of the path wanted.
 * @param dest The id of the destination vertex of the path wanted.
 * @param mode Int of the mode of transportation, 0->driving, 1->walking.
 * @param maxWalkTime Double with maximum time allowed to be walking by the algorithm. (not mandatory)
 * @param u Pointer to a pointer of a vertex of the better parking spot for the requested route, default value nullptr,
 * when the function is called, the vertex is the origin.
 *
 * @note Time Complexity: O((V+E)logV) where V and E are the number of vertexes and edges respectively.
 */
void dijkstra(Graph * g, const int &origin, const int &dest, int mode, double maxWalkTime = -1, Vertex **u = nullptr);



/**
 * @brief Fastest Route + Independent Route Planning.
 *
 * @details
*
 * @param g A pointer to the graph that has the origin and destination Vertex.
 * @param origin The id of the origin vertex of the path wanted.
 * @param dest The id of the destination vertex of the path wanted.
 *
 * @return A string where the correct output for the route that was asked.
 *
 * @note Time Complexity: O((V+E)logV) where V and E are the number of vertexes and edges respectively.
 */
std::string SimpleDriving(Graph * g, const int &origin, const int &dest);



/**
 * @brief Restricted Route Planning.
 *
 * @details
*
 * @param g A pointer to the graph that has the origin and destination Vertex.
 * @param origin The id of the origin vertex of the path wanted.
 * @param dest The id of the destination vertex of the path wanted.
 * @param avoidNodes Unordered set with the ids of the node that are to be avoided.
 * @param avoidEdges Vector of pairs of ints with the ids of the nodes in each side of the
 * edge that should be avoided.
 * @param includeNode int with the id of the node that is to be included in the desired path.
 *
 * @return A string where the correct output for the route that was asked.
 *
 * @note Time Complexity:
 */
std::string RestrictedDriving(Graph * g, const int &origin, const int &dest, const std::unordered_set<int> &avoidNodes,
                              const std::vector<std::pair<int,int>> &avoidEdges,const int &includeNode);



/**
 * @brief  Best route for driving and walking.
 *
 * @details
 *
 * @param g A pointer to the graph that has the origin and destination Vertex.
 * @param origin The id of the origin vertex of the path wanted.
 * @param dest The id of the destination vertex of the path wanted.
 * @param maxWalkTime Double with the maximum time of walking allowed from the park to the
 * destination.
 * @param avoidNodes Unordered set with the ids of the node that are to be avoided.
 * @param avoidEdges Vector of pairs of ints with the ids of the nodes in each side of the
 * edge that should be avoided.
 *
 * @return A string where the correct output for the route that was asked.
 *
 * @note Time Complexity:
 */
std::string DrivingWalking(Graph * g, const int &origin, const int &dest, double maxWalkTime,
                           const std::unordered_set<int> &avoidNodes, const std::vector<std::pair<int,int>> &avoidEdges);


// Extra
// Approximate Solution: If no suitable route is found, display a list of suggestions
// representing the best feasible alternative routes that approximate user requirements. For example, if the
// suggestions increase the maximum walking time, indicate the new time. Present 2 alternatives, sorted by
// overall travel time, but always including a driving and a walking segment.

void DrivingWalkingAlternatives(Graph * g, const int &origin, const int &dest, std::ostringstream &oss);


#endif //ALGORITHMS_H
