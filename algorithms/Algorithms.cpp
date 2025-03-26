#include "Algorithms.h"



void dijkstra(Graph * g, const int &origin, const int &dest, const int mode, const double maxWalkTime, Vertex **u) {

    //graph is already initialized to perform this algorithm

    //get the origin
    Vertex* s = g->findVertex(origin);
    s->setDist(0);

    //initialize a priority queue and add origin to it
    MutablePriorityQueue<Vertex> q;
    q.insert(s);

    while (!q.empty()) {

        Vertex *v = q.extractMin();
        if (mode == 0) {
            if (v->getId() == dest) {
                break;
            }
            if (v->isPark() && (u != nullptr || maxWalkTime != -1)) {
                *u = betterPark(*u, v, maxWalkTime) ? *u : v;
            }
        }
        if (mode==1) {
            if (v->getDist() > maxWalkTime) {
                return;
            }
        }

        for (auto e : v->getAdj()) {

            if (e->isAvoiding() || e->getTime(mode)==-1) {continue;}
            Vertex *w = e->getDest();

            if (w->isAvoiding() || w->isVisited()) {
                continue;
            } //skips vertex that were used in the first route (visited) + the ones to avoid

            double oldDist = w->getDist();;
            if (relax(e, mode)) {
                if (oldDist == INF) {
                    q.insert(w);
                }else {
                    q.decreaseKey(w);
                }
            }
        }
    }
}



// Fastest Route + Independent Route Planning
std::string SimpleDriving(Graph * g, const int &origin, const int &dest) {
    std::ostringstream oss;
    oss << "Source:" << origin << "\nDestination:" << dest << "\n";

    // Initialize all nodes to perform the Dijkstra algorithm
    // Visited set to false
    initAvoid(g,{},{});
    dijkstra(g, origin, dest, 0); //perform dijkstra

    //get the path of the fastest route
    double time = 0;
    std::vector<int> path1 = getPath(g, origin, dest, time, 0);

    oss<<"BestDrivingRoute:";

    //if there is no route
    if (path1.empty()) {
        oss <<"none\n";
        oss<<"AlternativeDrivingRoute:none\n";
        return oss.str();
    }

    printPath(path1, oss);
    oss <<"("<<time<<")\n";
    oss<<"AlternativeDrivingRoute:";

    // Initialize all nodes to perform the Dijkstra algorithm
    // Visited not altered, nodes and edges to be avoided are also not altered
    for (auto v : g->getVertexSet()) {
        v->setDist(INF);
        v->setPath(nullptr);
    }

    dijkstra(g, origin, dest, 0);

    time = 0;
    std::vector<int> path2 = getPath(g, origin, dest, time, 0);

    if (path2.empty()) {
        oss <<"none\n";
        return oss.str();
    }
    printPath(path2, oss);
    oss <<"("<<time<<")\n";
    return oss.str();
}

// Restricted Route Planning
std::string RestrictedDriving(Graph * g, const int &origin, const int &dest, const std::unordered_set<int> &avoidNodes,
    const std::vector<std::pair<int,int>> &avoidEdges,const int &includeNode) {
    std::ostringstream oss;
    oss << "Source:"<<origin << "\nDestination:" << dest << std::endl;
    oss<<"RestrictedDrivingRoute:";

    initAvoid(g, avoidNodes, avoidEdges);
    double time = 0;
    std::vector<int> path1;
    if (origin != includeNode) {
        dijkstra(g, origin, includeNode, 0);
        path1 = getPath(g, origin, includeNode, time, 0);
        if (path1.empty()) {
            oss <<"none\n";
            return "";
        }
        path1.pop_back();
        initAgain(g);
    }

    dijkstra(g, includeNode, dest, 0);
    std::vector<int> path2 = getPath(g, includeNode, dest, time, 0);

    if (path2.empty()) {
        oss <<"none\n";
        return "";
    }

    if (origin != includeNode) {
        printPath(path1, oss);
        oss <<",";
    }

    printPath(path2, oss);
    oss <<"("<<time<<")\n";
    return oss.str();
}


// Best route for driving and walking
std::string DrivingWalking(Graph * g, const int &origin, const int &dest, const double maxWalkTime,
    const std::unordered_set<int> &avoidNodes, const std::vector<std::pair<int,int>> &avoidEdges) {
    std::ostringstream oss;
    oss << "Source:"<<origin << "\nDestination:" << dest << std::endl;


    // Mark the time needed to walk from parking spots to the destination, but just the ones with
    // the time below the maxWalkingTime allowed
    initAvoid(g, avoidNodes, avoidEdges);
    dijkstra(g, dest, -1, 1, maxWalkTime);

    // Get the better parking spot p
    initAgain(g);
    Vertex* park_spot = g->findVertex(origin);
    dijkstra(g, origin, -1,0, maxWalkTime, &park_spot); // checks all vertexes

    // Is the parking spot viable
    if (park_spot->getId()==origin || !park_spot->isPark() || park_spot->getWalkDist() > maxWalkTime) {
        //oss << "ParkingNode:\nWalkingRoute:\nTotalTime:\n";

        //get approximate solution
        DrivingWalkingAlternatives(g, origin, dest, oss);

        //rever melhor isto
        oss << "Message:No possible route with max. walking time of "<<maxWalkTime<<" minutes.\n";
        //oss << "Message:No possible route because of an absence of parking.\n";

        return oss.str();
    }
    oss<<"DrivingRoute:";

    //get driving route from origin to parking spot
    double time1 = 0;
    std::vector<int> path1 = getPath(g, origin, park_spot->getId(), time1, 0);
    printPath(path1, oss);
    oss<<"("<<time1<<")\n";
    oss<<"ParkingNode:"<<path1.back()<<"\n";
    oss<<"WalkingRoute:";

    //get walking route from parking spot to destination
    double time2 = 0;
    std::vector<int> path2 = getPath(g, dest, park_spot->getId(), time2, 1);
    std::reverse(path2.begin(), path2.end());
    printPath(path2, oss);
    oss<<"("<<time2<<")\n";
    oss<<"TotalTime:"<<time1+time2<<"\n";
    return oss.str();
}


//not working
//second alternative wrong
void DrivingWalkingAlternatives(Graph * g, const int &origin, const int &dest, std::ostringstream &oss) {
    //oss << "Source:"<<origin << "\nDestination:" << dest << std::endl;

    // Mark the time needed to walk from parking spots to the destination, but just the ones with
    // the time below the maxWalkingTime allowed
    for (auto v : g->getVertexSet()) {
        v->setDist(INF);
        v->setPath(nullptr);
        v->setVisited(false);
        v->setWalkDist(INF);
        v->setPath(nullptr);
    }
    dijkstra(g, dest, -1, 1, INF); // checks all vertexes

    // Get the better parking spot p
    initAgain(g);
    Vertex* park_spot = g->findVertex(origin);
    dijkstra(g, origin, -1,0, INF, &park_spot);

    if (park_spot->getId()==origin || !park_spot->isPark()) {
        std::cout<<"n deu :( \n";
    }

    //get driving route from origin to parking spot
    double time1 = 0;
    std::vector<int> path1 = getPath(g, origin, park_spot->getId(), time1, 0);
    oss<<"DrivingRoute1:";
    printPath(path1, oss);
    oss<<"("<<time1<<")\n";
    oss<<"ParkingNode1:"<<path1.back()<<"\n";
    oss<<"WalkingRoute1:";

    //get walking route from parking spot to destination
    double time2 = 0;
    std::vector<int> path2 = getPath(g, dest, park_spot->getId(), time2, 1);
    std::reverse(path2.begin(), path2.end());
    printPath(path2, oss);
    oss<<"("<<time2<<")\n";
    oss<<"TotalTime1:"<<time1+time2<<"\n";

    Vertex *secondPark = g->findVertex(origin);
    for (auto v : g->getVertexSet()) {
        if (v->isPark() && v->getId() != park_spot->getId()) {
            secondPark = betterPark(secondPark, v, INF) ? secondPark : v;
        }
    }

    //get driving route from origin to parking spot
     time1 = 0;
    path1 = getPath(g, origin, secondPark->getId(), time1, 0);
    oss<<"DrivingRoute2:";
    printPath(path1, oss);
    oss<<"("<<time1<<")\n";
    oss<<"ParkingNode2:"<<path1.back()<<"\n";
    oss<<"WalkingRoute2:";

    //get walking route from parking spot to destination
     time2 = 0;
    path2 = getPath(g, dest, secondPark->getId(), time2, 1);
    std::reverse(path2.begin(), path2.end());
    printPath(path2, oss);
    oss<<"("<<time2<<")\n";
    oss<<"TotalTime2:"<<time1+time2<<"\n";


}