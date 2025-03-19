#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "Graph.h"

bool relax(Edge *edge);

void dijkstra(Graph * g, const int &origin, const int &dest);


#endif //DIJKSTRA_H
