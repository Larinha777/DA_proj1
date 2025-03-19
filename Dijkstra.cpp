#include "Dijkstra.h"

//edge relaxation
bool relax(Edge *edge) { // d[u] + w(u,v) < d[v]
    Vertex *u = edge->getOrig();
    Vertex *v = edge->getDest();
    if (v->getDist() > u->getDist() + edge->getDrive()) {
        v->setDist(u->getDist() + edge->getDrive());
        v->setPath(edge);
        return true;
    }
    return false;
}


void dijkstra(Graph * g, const int &origin, const int &dest) {

    //initialize all vertex
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
        //std::cout << "Analizing "<<v->getId()<<std::endl;
        if (v->getId() == dest) {
            //std::cout << "Got it"<<std::endl;

            break;
        }
        for (auto e : v->getAdj()) {

            if (e->isAvoiding() || e->getDrive()==-1) {continue;}
            Vertex *w = e->getDest();
            //std::cout << "loop "<<w->getId()<<std::endl;

            if (w->isAvoiding() || w->isVisited()) {
                continue;
            } //skips vertex that were used in the first route + the ones to avoid

            double oldDist = w->getDist();;
            if (relax(e)) {
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
