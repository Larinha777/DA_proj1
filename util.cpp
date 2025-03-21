#include "util.h"

#include <algorithm>


void printPath(const std::vector<int> &v, std::ostringstream &oss) {
    for ( int i = 0 ; i <v.size()-1 ; i++ ) {
        oss << v[i] << ",";
    }
    oss << v[v.size()-1];
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

// Set all Verteces to not visited and see if any should be avoided
// Works for initializing all verteces to perform the Dijkstra Algorithm
void initAvoid(Graph * g,  const std::unordered_set<int> &avoidNodes, const std::vector<std::pair<int,int>> &avoidEdges) {
    for (auto v : g->getVertexSet()) {
        v->setDist(INF);
        v->setPath(nullptr);
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


// Initialize all vertex for a second time inside the same algorithm
// Prepares the graph for the Dijkstra algorithm but does not alter avoiding edges and nodes
void initAgain(Graph * g) {
    for (auto v : g->getVertexSet()) {
        v->setDist(INF);
        v->setPath(nullptr);
        v->setVisited(false);
    }
}


//edge relaxation
bool relax(Edge *edge, const int mode) { // d[u] + w(u,v) < d[v]
    Vertex *u = edge->getOrig();
    Vertex *v = edge->getDest();
    if (v->getDist() > u->getDist() + edge->getTime(mode)) {
        v->setDist(u->getDist() + edge->getTime(mode));
        v->setPath(edge);
        return true;
    }
    return false;
}

bool betterPark(Vertex *u, Vertex *v, double maxWalkTime) { //is u a better parking spot thant v?
    if (!u->isPark()) return false; // first time
    if (v->getWalkTime() > maxWalkTime) {
        if ( u->getWalkTime() > maxWalkTime ) {
            return u->getDist() + u->getWalkTime() < v->getDist() + v->getWalkTime();
        }
        return true;
    }
    if (u->getWalkTime() > maxWalkTime) return false;
    return u->getDist() + u->getWalkTime() < v->getDist() + v->getWalkTime();
}

