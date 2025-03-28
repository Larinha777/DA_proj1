#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <algorithm>
#include <sstream>

#include "../data_structures/Graph.h"
#include "util.h"

/**
 * @brief  Computes the shortest path based on the Dijkstra's Algorithm
 *
 * @details This function computes the shortest path between an origin and destiny node, using the Dijkstra's Algorithm.
 *  It supports different modes of transportation (0->driving, 1->walking), and if driving and maxWalktime != -1, it also
 *  returns the best node to park the car to continue the rest of the route on foot.
 *
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
 * @details Determine the best (fastest) route between a source and destination. Identify the best alternative
 * independent route, ensuring the two routes share no intermediate nodes or segments, except for the source
 * and destination, and that the alternative route is equal to or greater in travel time than the primary route.
 * This provides a robust “Plan-B” option for navigation.The routes calculated will be for driving only. The origin
 * and destination can be parking or non-parking nodes.
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
 * @details Computes the fastest route with specific routing restrictions, namely: excluding specific nodes
 * from the graph (avoidNodes), enabling users to avoid undesirable areas; excluding specific segments from
 * the graph (avoidEdges), enabling users to avoid undesirable road segments; simultaneously excluding the
 * combination of nodes and segments of the graph; including a single specific node (or stop) that the route
 * must include while ensuring that the calculated route remains the fastest possible (includeNode). Those
 * restrictions are optional and can be left with an empty vector or set in the case of the avoidance restrictions.
 * In the case of the include node, if not applicable, it can be left with the origin node id.
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
 * @note Time Complexity: O((V+E)logV + E*N) where V and E are, respectively the number of vertexes and edges
 * of the graph and N is the number of edges to avoid. O((V+E)logV) corresponds to calling the Dijkstra function
 * and O(E*N) to calling avoidNodes(...);

 */
std::string RestrictedDriving(Graph * g, const int &origin, const int &dest, const std::unordered_set<int> &avoidNodes,
                              const std::vector<std::pair<int,int>> &avoidEdges,const int &includeNode);



/**
 * @brief  Best route for driving and walking.
 *
 * @details This function implements the best (shortest overall) route for driving and walking,
 * ensuring that it drives the first section of the route and then parks the vehicle, walking
 * the remaining distance to the destination. The goal is to minimize both driving and walking time and
 * should there be two or more feasible routes with the same overall minimum travel time, the one with
 * the longest walking section (that obviously meets the maximum walking time restriction) should be
 * selected. The origin and destination cannot be adjacent nor parking nodes.
 * If no suitable route is found that satisfies all the requirements, it calls the function Driving Walking
 * to give approximate solutions to the problem
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
 * @note Time Complexity: O((V+E)logV + E*N) where V and E are, respectively the number of vertexes and edges
 * of the graph and N is the number of edges to avoid. O((V+E)logV) corresponds to calling the Dijkstra function
 * and O(E*N) to calling avoidNodes(...);
 */
std::string DrivingWalking(Graph * g, const int &origin, const int &dest, double maxWalkTime,
                           const std::unordered_set<int> &avoidNodes, const std::vector<std::pair<int,int>> &avoidEdges);



/**
 * @brief Approximate Solution
 *
 * @details Called when no suitable route was found using the DrivingWalking(...) that satisfied all the requirements.
 * If possible, it displays (in the output stream oss) 2 suggestions representing the best feasible alternative routes
 * that do not go along with the maximum walking time required, but follow all other requirements.
 * This function also displays a message with the reason for being called, ex: walking time exceeds predefined maximum
 * limit or absence of reachable parking spots.
 *
 * @param g A pointer to the graph that has the origin and destination Vertex.
 * @param origin The id of the origin vertex of the path wanted.
 * @param dest The id of the destination vertex of the path wanted.
 * @param oss The output stream where the path will be stored in the correct format.
 *
 * @note Time Complexity: O((V+E)logV + E*N) where V and E are, respectively the number of vertexes and edges
 * of the graph and N is the number of edges to avoid. O((V+E)logV) corresponds to calling the Dijkstra function
 * and O(E*N) to calling avoidNodes(...);
 */
void DrivingWalkingAlternatives(Graph * g, const int &origin, const int &dest, std::ostringstream &oss);


#endif //ALGORITHMS_H
