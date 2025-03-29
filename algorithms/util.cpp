#include "util.h"

#include <algorithm>


void printPath(const std::vector<int> &v, std::ostringstream &oss) {
    if (v.empty()) {
        return;
    }
    for ( int i = 0 ; i <v.size()-1 ; i++ ) {
        oss << v[i] << ",";
    }
    oss << v[v.size()-1];
}


std::vector<int> getPath(Graph * g, const int &origin, const int &dest, double &time, const int mode) {
    std::vector<int> res;
    Vertex *v = g->findVertex(dest);

    time += v->getDist(mode);

    while (v->getId() != origin) {

        //marks the route as visited in case it is supposed to get the alternative route later
        if (v->getId() != dest) {
            v->setVisited(true);
        }

        res.push_back(v->getId());
        if ( v->getPath(mode) == nullptr) {
            time=-1;
            return {};
        }

        v = v->getPath(mode)->getOrig();

    }
    res.push_back(v->getId());
    std::reverse(res.begin(), res.end());
    return res;
}


void initAvoid(Graph * g,  const std::unordered_set<int> &avoidNodes, const std::vector<std::pair<int,int>> &avoidEdges, int mode) {
    for (auto v : g->getVertexSet()) {
        v->setDist(INF, mode);
        v->setPath(nullptr, mode);
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


void initAgain(Graph * g, int mode) {
    for (auto v : g->getVertexSet()) {
        v->setDist(INF, mode);
        v->setPath(nullptr, mode);
        v->setVisited(false);
    }
}


bool relax(Edge *edge, const int mode) { // d[u] + w(u,v) < d[v]
    Vertex *u = edge->getOrig();
    Vertex *v = edge->getDest();
    if (v->getDist(mode) > u->getDist(mode) + edge->getTime(mode)) {
        v->setDist(u->getDist(mode) + edge->getTime(mode), mode);
        v->setPath(edge, mode);
        return true;
    }
    return false;
}


bool betterPark(Vertex *u, Vertex *v, double maxWalkTime) { //is u a better parking spot thant v?
    if (v->getDist(1) > maxWalkTime) {  // v is not a valid parking spot because it exceeds max walking time
        return true;
    }
    if (!u->isPark()) {// Vertex u exceeds max time but v doesnt
        return false;
    }

    if (u->getDist(0) + u->getDist(1) == v->getDist(0) + v->getDist(1)) {// if both have the same time
        return u->getDist(1) > v->getDist(0) + v->getDist(1); //the one that walks more is the better choice
    }

    // the one that that takes the least time is better
    return (u->getDist(0) + u->getDist(1) < v->getDist(0) + v->getDist(1));
}



