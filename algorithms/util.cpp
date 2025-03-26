#include "util.h"

#include <algorithm>

#include "Algorithms.h"


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

    if (mode == 0) {
        time += v->getDist();
    } else if (mode == 1) {
        time += v->getWalkDist();
    } else {
        return {};
    }

    while (v->getId() != origin) {

        //marks the route as visited in case it is supposed to get the alternative route later
        if (v->getId() != dest) {
            v->setVisited(true);
        }

        res.push_back(v->getId());
        if ( (mode == 0 && v->getPath() == nullptr) || (mode == 1 && v->getWalkPath() == nullptr) ) {
            time=-1;
            return {};
        }

        if (mode == 0) {
            v = v->getPath()->getOrig();
        } else if (mode == 1) {
            v = v->getWalkPath()->getOrig();
        }

    }
    res.push_back(v->getId());
    std::reverse(res.begin(), res.end());
    return res;
}


void initAvoid(Graph * g,  const std::unordered_set<int> &avoidNodes, const std::vector<std::pair<int,int>> &avoidEdges) {
    for (auto v : g->getVertexSet()) {
        v->setDist(INF);
        v->setPath(nullptr);
        v->setWalkDist(INF);
        v->setWalkPath(nullptr);
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


void initAgain(Graph * g) {
    for (auto v : g->getVertexSet()) {
        v->setDist(INF);
        v->setPath(nullptr);
        v->setVisited(false);
    }
}


bool relax(Edge *edge, const int mode) { // d[u] + w(u,v) < d[v]
    Vertex *u = edge->getOrig();
    Vertex *v = edge->getDest();
    if (v->getDist() > u->getDist() + edge->getTime(mode)) {
        v->setDist(u->getDist() + edge->getTime(mode));
        v->setPath(edge);

        if (mode == 1) {
            v->setWalkPath(edge);
            v->setWalkDist(v->getDist());
        }

        return true;
    }
    return false;
}

bool betterPark(Vertex *u, Vertex *v, double maxWalkTime) { //is u a better parking spot thant v?
    if (!u->isPark()) return false; // first time
    if (v->getWalkDist() > maxWalkTime) {
        if ( u->getWalkDist() > maxWalkTime ) {  //both vertexes exceed maxWalkingTime
            if (u->getDist() + u->getWalkDist() == v->getDist() + v->getWalkDist()) { //same overall time
                return u->getWalkDist() > v->getDist() + v->getWalkDist();
            }
            return (u->getDist() + u->getWalkDist() < v->getDist() + v->getWalkDist());
        }
        return true; // just v exceeds max walking time
    }
    if (u->getWalkDist() > maxWalkTime) return false;    // Vertex u exceeds max time but v doesnt


    if (u->getDist() + u->getWalkDist() == v->getDist() + v->getWalkDist()) {// if both have the same time, the one that walks more is better
        return u->getWalkDist() > v->getDist() + v->getWalkDist();
    }

    // the one that that takes the least time is better
    return (u->getDist() + u->getWalkDist() < v->getDist() + v->getWalkDist());
}



