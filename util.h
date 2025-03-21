#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <unordered_set>
#include <sstream>

#include "Graph.h"


/**
 * @brief Prints the path in the correct order
 *
 * @param v The vector of ints representing the id of each location in the path.
 * @param oss
 */
void printPath(const std::vector<int> &v, std::ostringstream &oss);


/**
 * @brief Get path from origin to dest and mark all visited nodes to avoid
 *
 * @param g A pointer to the graph that has the origin and destination Vertex
 * @param origin The id of the origin vertex of the path that is being calculated
 * @param dest The id of the destination vertex of the path that is being calculated
 * @param time A reference to a double where the total time of the path is being held
 *
 * @return A vector of ints representing the id of each location in the path.
 */
std::vector<int> getPath(Graph * g, const int &origin, const int &dest, double &time);


void initAvoid(Graph * g,  const std::unordered_set<int> &avoidNodes, const std::vector<std::pair<int,int>> &avoidEdges);


void initAgain(Graph * g);

bool relax(Edge *edge, const int mode);

bool betterPark(Vertex *u, Vertex *v, double maxWalkTime);
#endif //UTIL_H
