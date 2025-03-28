#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <unordered_set>
#include <sstream>

#include "../data_structures/Graph.h"


/**
 * @brief Prints the path with commas between each element to the output stream.
 *
 * @param v The vector of ints representing the id of each location in the path.
 * @param oss The output stream where the path will be stored in the correct format.
 *
 * @note Time complexity: O(n) where n is the size of the vector.
 */
void printPath(const std::vector<int> &v, std::ostringstream &oss);


/**
 * @brief Get path previously calculated from origin to dest and mark all nodes as visited.
 *
 * @param g A pointer to the graph that has the origin and destination Vertex.
 * @param origin The id of the origin vertex of the path that is being calculated.
 * @param dest The id of the destination vertex of the path that is being calculated.
 * @param time A reference to a double where the total time of the path is being held.
 * @param mode Int of the mode of transportation, 0->driving, 1->walking.
 *
 * @return A vector of ints representing the id of each location in the path.
 *
 * @note Time Complexity: O(n) where n is the size of the return vector.
 */
std::vector<int> getPath(Graph * g, const int &origin, const int &dest, double &time, int mode);

/**
 * @brief Initializes all vertex to perform the Dijkstra Algorithm.
 * Checks if any vertex or edge should be avoided.
 *
 * @details Resets distance, walkDistance, path and walkPath. Mark all vertex as not visited.
 * Set vertexes and edges to avoid if needed, otherwise to not avoid.
 *
 * @param g A pointer to the graph that is being worked on.
 * @param avoidNodes Unordered set with the ids of the node that are to be avoided.
 * @param avoidEdges Vector of pairs of ints with the ids of the nodes in each side of the
 * edge that should be avoided.
 *
 * @note Time Complexity: O(V + E * N) where, V is the number of vertex in the graph,
 * E is the number of edges and N is the number of edges to avoid.
 */
void initAvoid(Graph * g,  const std::unordered_set<int> &avoidNodes, const std::vector<std::pair<int,int>> &avoidEdges, int mode);

/**
 * @brief Initializes the graph for the Dijkstra algorithm.
 *
 * @details Set distance, path and visited, to perform the Dijkstra Algorithm.
 * This is usually the second time the graph is being initialized for the same input,
 * so the edges and vertexes to be avoided haven't changed. As such they do not need to
 * be altered.
 *
 * @param g A pointer to the graph that is being worked on.
 *
 * @note Time Complexity: O(V) where V is the number of vertexes in the graph.
 */
void initAgain(Graph * g, int mode);

/**
 * @brief  See if using this edge is a better way to reach vertex v. Set the predecessor
 * of v to u if it is better to use edge e from u to v (edge relaxation).
 *
 * @details Called when doing the Dijkstra Algorithm. Differs a little from the original,
 * as an int variable called mode is passed representing if the person is walking or driving. That way, when we
 * want the distance of the edge (time in this case) it is possible to call a method that
 * gives the time according to the mode of transportation.
 *
 * @param edge Pointer to the edge to be relaxed.
 * @param mode Int of the mode of transportation, 0->driving, 1->walking.
 *
 * @return boolean that is true if edge was relaxed, false otherwise.
 *
 * @note Time Complexity: O(1).
 */
bool relax(Edge *edge, int mode);

/**
 * @brief Compares two vertexes and chooses the one that is a better option to park on.
 * Vertex v is always a parking node.
 *
 * @details If the time needed to walk from v to the destination (walkDist) exceeds the
 * maxWalkingTime, v is not a valid parking spot and can be discarded.
 * If v is a valid parking spot, and it is the first vertex to be so, u will still be the origin
 * node, and as it cannot be a parking spot, v is always the better option.
 *
 * Now that only leaves us on the cases where both vertexes are valid parking spots.
 * The one that takes less time overall (driveDist (driving time from origin to that node) + walkDist(walking
 * time from that node to the destination)) is the better parking spot. Should they have the same overall
 * minimum travel time, the one with the longest walkTime is the better option.
 *
 * @param u pointer to vertex u
 * @param v pointer to vertex v
 * @param maxWalkTime double with maximum time allowed to be walking by the algorithm
 *
 * @return boolean that returns true if v is an invalid parking spot of if u is the better choice. False otherwise
 *
 * @note Time Complexity: O(1).
 */
bool betterPark(Vertex *u, Vertex *v, double maxWalkTime);

#endif //UTIL_H
