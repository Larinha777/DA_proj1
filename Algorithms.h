#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <algorithm>
#include <sstream>

#include "Graph.h"
#include "MutablePriorityQueue.h"
#include "util.h"

void dijkstra(Graph * g, const int &origin, const int &dest, const int mode, Vertex **u = nullptr, const double maxWalkTime = -1);




// Fastest Route + Independent Route Planning
std::string SimpleDriving(Graph * g, const int &origin, const int &dest);


// Restricted Route Planning
std::string RestrictedDriving(Graph * g, const int &origin, const int &dest, const std::unordered_set<int> &avoidNodes,
    const std::vector<std::pair<int,int>> &avoidEdges,const int &includeNode);
//later will change vector in avoid edges to unordered set

// Best route for driving and walking
std::string DrivingWalking(Graph * g, const int &origin, const int &dest, double maxWalkTime,
    const std::unordered_set<int> &avoidNodes, const std::vector<std::pair<int,int>> &avoidEdges);


// Extra
// Approximate Solution: If no suitable route is found, display a list of suggestions
// representing the best feasible alternative routes that approximate user requirements. For example, if the
// suggestions increase the maximum walking time, indicate the new time. Present 2 alternatives, sorted by
// overall travel time, but always including a driving and a walking segment.


#endif //ALGORITHMS_H
