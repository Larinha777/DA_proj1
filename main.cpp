#include <iostream>
#include "data_structures/Graph.h"
#include "algorithms/Algorithms.h"
int main() {
    Graph g = initialize("../data/loc.csv", "../data/dist.csv");
    // Graph g = initialize("../data/Locations.csv", "../data/Distances.csv");


    /*
    std::cout<< SimpleDriving(&g, 8, 1) <<"\n";
    std::cout<<RestrictedDriving(&g, 5, 4, {2},{},5)<<"\n";
    std::cout<<RestrictedDriving(&g, 5, 4, {2},{{4,7}},5)<<"\n";
    std::cout<<RestrictedDriving(&g, 5, 4, {},{{3,2},{7,8}},5)<<"\n";
    std::cout<<RestrictedDriving(&g, 5, 4, {},{},7)<<"\n";
    std::cout<<DrivingWalking(&g, 8, 5, 18, {}, {} )<<"\n";

    std::cout<<DrivingWalking(&g, 4, 8, 20, {}, {} )<<"\n";
    std::cout<<DrivingWalking(&g, 1, 8, 13, {}, {} )<<"\n";

    */

    /*

    ***Edge Cases*** not necessary anymore, they will not happen
    SimpleDriving(&g,3,3); // dont know what happens in alternative route
    RestrictedDriving(&g, 3,5, {5},{},3 ); // works but checks everything first
    RestrictedDriving(&g, 3,3, {},{},8 ); // does not work, the path is supposed to be shorter, it does not repeat nodes
    RestrictedDriving(&g, 3,3, {3},{},3 ); // what is the priority? Avoid or include? Should return none I think

    */

   return 0;
}





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
