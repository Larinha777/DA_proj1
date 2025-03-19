#include "Algorithms.h"

void printPath(const std::vector<int> &v) {
    for ( int i = 0 ; i <v.size()-1 ; i++ ) {
        std::cout << v[i] << ",";
    }
    std::cout << v[v.size()-1];
}

std::vector<int> getPath(Graph * g, const int &origin, const int &dest, double &time) {
    std::vector<int> res;
    Vertex *v = g->findVertex(dest);
    time += v->getDist();
    while (v->getId() != origin) {

        //marks the route as visited in case it is supposed to get the alternative route later
        if (v->getId() != dest) {
            v->setVisited(true);
        }

        res.push_back(v->getId());
        if (v->getPath() == nullptr) {
            return {};
        }
        v = v->getPath()->getOrig();
    }
    res.push_back(v->getId());
    std::reverse(res.begin(), res.end());
    return res;
}

// Set all Vertex to not visited and see if any should be avoided
void initAvoid(Graph * g,  const std::unordered_set<int> &avoidNodes, const std::vector<std::pair<int,int>> &avoidEdges) {
    for (auto v : g->getVertexSet()) {
        v->setVisited(false);
        v->setAvoiding(avoidNodes.contains(v->getId()));

        for (auto e: v->getAdj()) {
            e->setAvoiding(false);
            for (auto j : avoidEdges) {
                if ((e->getOrig()->getId() == j.first && e->getDest()->getId() == j.second)
                    || (e->getOrig()->getId() == j.second && e->getDest()->getId() == j.first)) {
                    e->setAvoiding(true);
                }
            }
        }
    }
}

// Fastest Route + Independent Route Planning
void SimpleDriving(Graph * g, const int &origin, const int &dest) {
    std::cout << "Source:"<<origin << "\nDestination:" << dest << std::endl;

    for (auto v : g->getVertexSet()) {
            v->setAvoiding(false);
            v->setVisited(false);
            for (auto e : v->getAdj()) {
                e->setAvoiding(false);
            }
    }

    dijkstra(g, origin, dest);
    double time;
    std::vector<int> path1 = getPath(g, origin, dest, time);

    std::cout<<"BestDrivingRoute:";
    if (path1.empty()) {
        std::cout <<"none\n";
        std::cout<<"AlternativeDrivingRoute:none\n";
        return;
    }

    printPath(path1);
    std::cout <<"("<<time<<")\nAlternativeDrivingRoute:";
    dijkstra(g, origin, dest);
    time = 0;
    std::vector<int> path2 = getPath(g, origin, dest, time);

    if (path2.empty()) {
        std::cout <<"none\n";
        return;
    }
    printPath(path2);
    std::cout <<"("<<time<<")\n";

}

// Restricted Route Planning
void RestrictedDriving(Graph * g, const int &origin, const int &dest, const std::unordered_set<int> &avoidNodes,
    const std::vector<std::pair<int,int>> &avoidEdges,const int &includeNode) {

    std::cout << "Source:"<<origin << "\nDestination:" << dest << std::endl;
    std::cout<<"RestrictedDrivingRoute:";

    initAvoid(g, avoidNodes, avoidEdges);
    double time = 0;
    std::vector<int> path1;
    if (origin != includeNode) {
        dijkstra(g, origin, includeNode);
        path1 = getPath(g, origin, includeNode, time);
        if (path1.empty()) {
            std::cout <<"none\n";
            return;
        }
        path1.pop_back();
        //printPath(path1);
        //std::cout<<",";
    }

    dijkstra(g, includeNode, dest);
    std::vector<int> path2 = getPath(g, includeNode, dest, time);

    if (path2.empty()) {
        std::cout <<"none\n";
        return;
    }

    if (origin != includeNode) {
        printPath(path1);
        std::cout <<",";
    }

    printPath(path2);
    std::cout <<"("<<time<<")\n";

}

// Best route for driving and walking
void DrivingWalking(Graph * g, const int &origin, const int &dest, const std::vector<int> &avoidNodes,
    const std::vector<std::pair<int,int>> &avoidEdges,const int &includeNode);



