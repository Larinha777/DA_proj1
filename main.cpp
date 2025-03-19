#include <iostream>
#include "Graph.h"
#include "Algorithms.h"
int main() {
    Graph g = initialize("../loc.csv", "../dist.csv");

    /*
    SimpleDriving(&g, 3, 8);
    std::cout<<"\n";
    SimpleDriving(&g,8,1);
    std::cout<<"\n";

    RestrictedDriving(&g, 5, 4, {2},{},5);
    std::cout<<"\n";
    RestrictedDriving(&g, 5, 4, {},{{3,2},{7,8}},5);
    std::cout<<"\n";
    RestrictedDriving(&g, 5, 4, {2},{{4,7}},5);
    std::cout<<"\n";
    RestrictedDriving(&g, 5, 4, {},{},7);
    std::cout<<"\n";
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
