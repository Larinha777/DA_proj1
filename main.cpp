#include <iostream>
#include "data_structures/Graph.h"
#include "algorithms/Algorithms.h"
int main() {
    Graph g = initialize("../data/loc.csv", "../data/dist.csv");

    // Graph g = initialize("../data/extra_loc.csv", "../data/extra_dis.csv");
    // Graph g = initialize("../data/Locations.csv", "../data/Distances.csv");

/* Alternative Routes
    * Source:8
    Destination:5
    DrivingRoute1:8,4,2,3(19)
    ParkingNode1:3
    WalkingRoute1:3,5(10)
    TotalTime1:29
    DrivingRoute2:8,4,6(21)
    ParkingNode2:6
    WalkingRoute2:6,5(10)
    TotalTime2:31
 */

    std::cout<<DrivingWalking(&g, 8, 5, 18, {}, {} )<<"\n";
    std::cout<<DrivingWalking(&g, 8, 5, 5, {}, {} )<<"\n";

    /*
    std::cout<<SimpleDriving(&g, 3, 8 )<<"\n";
    std::cout<< SimpleDriving(&g, 8, 1) <<"\n";

    std::cout<<RestrictedDriving(&g, 5, 4, {2},{},5)<<"\n";
    std::cout<<RestrictedDriving(&g, 5, 4, {},{{3,2},{7,8}},5)<<"\n";
    std::cout<<RestrictedDriving(&g, 5, 4, {2},{{4,7}},5)<<"\n";
    std::cout<<RestrictedDriving(&g, 5, 4, {},{},7)<<"\n";


    std::cout<<DrivingWalking(&g, 1, 8, 13, {}, {} )<<"\n";

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
