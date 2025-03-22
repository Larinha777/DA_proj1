#include "Algorithms.h"



void dijkstra(Graph * g, const int &origin, const int &dest, const int mode, const double maxWalkTime, Vertex **u) {

    //initialize all vertex
    //remove from here in future modifications
    for (auto v : g->getVertexSet()) {
        v->setDist(INF);
        v->setPath(nullptr);
    }

    Vertex* s = g->findVertex(origin);
    s->setDist(0);

    MutablePriorityQueue<Vertex> q;
    q.insert(s);

    while (!q.empty()) {

        Vertex *v = q.extractMin();
        //std::cout << "Analyzing "<<v->getId()<<std::endl;
        if (mode == 0) {
            if (v->getId() == dest) {
                std::cout << "Got it"<<std::endl;
                break;
            }
            if (v->isPark() && (u != nullptr || maxWalkTime != -1)) {
                *u = betterPark(*u, v, maxWalkTime) ? *u : v;
            }
        }
        if (mode==1) {
            if (v->getDist() > maxWalkTime) {
                //std::cout<<v->getDist()<<"Too much walking\n";
                return;
            }
            if (v->isPark()) {
                v->setWalkTime(v->getDist());
            }
        }

        for (auto e : v->getAdj()) {

            if (e->isAvoiding() || e->getTime(mode)==-1) {continue;}
            Vertex *w = e->getDest();
            //std::cout << "loop "<<w->getId()<<std::endl;

            if (w->isAvoiding() || w->isVisited()) {
                continue;
            } //skips vertex that were used in the first route + the ones to avoid

            double oldDist = w->getDist();;
            if (relax(e, mode)) {
                //std::cout << "relax "<<w->getId()<<std::endl;

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
    dijkstra(g, origin, dest, 0);


    double time = 0;
    std::vector<int> path1 = getPath(g, origin, dest, time);

    oss<<"BestDrivingRoute:";
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
    std::vector<int> path2 = getPath(g, origin, dest, time);

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
        path1 = getPath(g, origin, includeNode, time);
        if (path1.empty()) {
            oss <<"none\n";
            return "";
        }
        path1.pop_back();
        initAgain(g);
    }

    dijkstra(g, includeNode, dest, 0);
    std::vector<int> path2 = getPath(g, includeNode, dest, time);

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
    oss<<"DrivingRoute:";


    // Mark the time needed to walk from parking spots to the destination, but just the ones with
    // the time below the maxWalkingTime allowed
    initAvoid(g, avoidNodes, avoidEdges);
    dijkstra(g, dest, origin, 1, maxWalkTime);

    // Get the better parking spot p
    initAgain(g);
    Vertex* p = g->findVertex(origin);
    dijkstra(g, origin, -1,0, maxWalkTime, &p);

    // Is the parking spot viable
    if (p->getId()==origin || !p->isPark() || p->getWalkTime() > maxWalkTime) {
        oss << "ParkingNode:\nWalkingRoute:\nTotalTime:\n";

        //rever melhor isto
        if (hasParks(g)) {
            oss << "Message:No possible route with max. walking time of "<<maxWalkTime<<" minutes.\n";
        } else {
            oss << "Message:No possible route because of an absence of parking.\n";
        }

        //chamar funcao para calcular aproximados

        return oss.str();
    }

    //get driving route from origin to parking spot
    double time1 = 0;
    std::vector<int> path1 = getPath(g, origin, p->getId(), time1);
    printPath(path1, oss);
    oss<<"("<<time1<<")\n";
    oss<<"ParkingNode:"<<path1.back()<<"\n";
    oss<<"WalkingRoute:";

    //get walking route from parking spot to destination
    double time2 = 0;
    initAgain(g);
    dijkstra(g, p->getId(), dest, 1, p->getWalkTime());
    std::vector<int> path2 = getPath(g, p->getId(), dest, time2);
    printPath(path2, oss);
    oss<<"("<<time2<<")\n";
    oss<<"TotalTime:"<<time1+time2<<"\n";
    return oss.str();
}



