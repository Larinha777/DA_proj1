#include <iostream>
#include "Graph.h"
#include "Algorithms.h"
int main() {
    Graph g = initialize("../loc.csv", "../dist.csv");

    //RestrictedDriving(&g, 5, 4, {2},{},5);
    /*
            SimpleDriving(&g, 3, 8);
        Source:3
        Destination:8
        BestDrivingRoute:3,2,4,8(19)
        AlternativeDrivingRoute:3,7,8(34)

            SimpleDriving(&g,8,1);
        Source:8
        Destination:1
        BestDrivingRoute:8,4,2,1(24)
        AlternativeDrivingRoute:none

            RestrictedDriving(&g, 5, 4, {2},{},5);
        Source:5
        Destination:4
        RestrictedDrivingRoute:5,3,7,8,4(52)
    */


    /* Output the content
    std::cout << "Vertices in the graph:\n";
    for (Vertex* v : g.getVertexSet()) {
        std::cout << "Name: " << v->getName()
                  << ", ID: " << v->getId()
                  << ", Code: " << v->getCode()
                  << ", Parking: " << (v->isPark() ? "Yes" : "No") << '\n';
    }

    std::cout << "\nEdges in the graph:\n";
    for (Vertex* v : g.getVertexSet()) {
        for (Edge* e : v->getAdj()) {
            std::cout << "From " << e->getOrig()->getName()
                      << " to " << e->getDest()->getName()
                      << " | Walk: " << e->getWalk()
                      << " | Drive: " << e->getDrive() << '\n';
        }
    }
    */
    return 0;
}
