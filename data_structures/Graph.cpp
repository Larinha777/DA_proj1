//
// Created by Vasco Lemos on 06/03/2025.
//

#include "Graph.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>

/************************* Vertex  **************************/

Vertex::Vertex(const std::string &name, const int &id, const std::string &code, const bool &park)
    : name(name), id(id), code(code), park(park) { }

Edge* Vertex::addEdge(Vertex *d, double walk, double drive) {
    auto newEdge = new Edge(this, d, walk, drive);
    adj.push_back(newEdge);
    d->incoming.push_back(newEdge);
    return newEdge;
}

bool Vertex::removeEdge(const std::string &destName) {
    bool removedEdge = false;
    auto it = adj.begin();
    while (it != adj.end()) {
        Edge *edge = *it;
        Vertex *dest = edge->getDest();
        if (dest->getName() == destName) {
            it = adj.erase(it);
            deleteEdge(edge);
            removedEdge = true; // allows for multiple edges between the same pair of vertices
        }
        else {
            ++it;
        }
    }
    return removedEdge;
}

void Vertex::removeOutgoingEdges() {
    auto it = adj.begin();
    while (it != adj.end()) {
        Edge *edge = *it;
        it = adj.erase(it);
        deleteEdge(edge);
    }
}

bool Vertex::operator<(const Vertex &vertex) const {
    return this->dist < vertex.dist;
}

std::string Vertex::getName() const {
    return this->name;
}

int Vertex::getId() const {
    return this->id;
}

std::string Vertex::getCode() const {
    return this->code;
}

bool Vertex::isPark() const {
    return this->park;
}


std::vector<Edge*> Vertex::getAdj() const {
    return this->adj;
}
bool Vertex::isVisited() const {
    return this->visited;
}
/*

bool Vertex::isProcessing() const {
    return this->processing;
}

unsigned int Vertex::getIndegree() const {
    return this->indegree;
}
*/
double Vertex::getDist() const {
    return this->dist;
}

Edge *Vertex::getPath() const {
    return this->path;
}

std::vector<Edge *> Vertex::getIncoming() const {
    return this->incoming;
}

double Vertex::getWalkTime() const{
    return this->walkTime;
}

bool Vertex::isAvoiding() const{
    return this->avoid;
}

void Vertex::setName(const std::string& newName) {this->name = newName;}
void Vertex::setId(const int& newId) {this->id = newId;}
void Vertex::setCode(const std::string& newCode) {this->code = newCode;}
void Vertex::setPark(const bool newPark) {this->park = newPark;}
void Vertex::setVisited(bool visited) {
    this->visited = visited;
}
/*

void Vertex::setProcessing(bool processing) {
    this->processing = processing;
}

void Vertex::setIndegree(unsigned int indegree) {
    this->indegree = indegree;
}
*/

void Vertex::setWalkTime(double time) {
    this->walkTime = time;
}

void Vertex::setAvoiding(bool avoid) {
    this->avoid = avoid;
}

void Vertex::setDist(double dist) {
    this->dist = dist;
}

void Vertex::setPath(Edge *path) {
    this->path = path;
}

void Vertex::deleteEdge(const Edge *edge) const {
    Vertex *dest = edge->getDest();
    // Remove the corresponding edge from the destination's incoming list.
    auto it = dest->incoming.begin();
    while (it != dest->incoming.end()) {
        if ((*it)->getOrig()->getName() == this->name) {
            it = dest->incoming.erase(it);
        }
        else {
            ++it;
        }
    }
    delete edge;
}

/********************** Edge  ****************************/

Edge::Edge(Vertex *orig, Vertex *dest, double w, double drive)
    : orig(orig), dest(dest), walk(w), drive(drive) { }

Vertex *Edge::getDest() const {
    return this->dest;
}

Vertex *Edge::getOrig() const {
    return this->orig;
}
Edge *Edge::getReverse() const {
    return this->reverse;
}
/*

bool Edge::isSelected() const {
    return this->selected;
}

void Edge::setSelected(bool selected) {
    this->selected = selected;
}
*/

void Edge::setReverse(Edge *reverse) {
    this->reverse = reverse;
}
bool Edge::isAvoiding() const {
    return this->avoid;
}

void Edge::setAvoiding(bool avoid) {
    this->avoid = avoid;
}


void Edge::setWalk(const double walk) { this->walk = walk; }
double Edge::getWalk() const { return this->walk; }
void Edge::setDrive(const double drive) { this->drive = drive; }
double Edge::getDrive() const { return drive; }

void Edge::setTime(const double time, const int mode) {
    switch (mode) {
        case 0:
            this->drive = time;
            break;
        case 1:
            this->walk = time;
                break;
        default:
            std::cout<<"Mode not found"<<std::endl;
    }
}
double Edge::getTime(const int mode) const {
    switch (mode) {
        case 0:
            return this->drive;
        break;
        case 1:
            return this->walk;
        break;
        default:
            std::cout<<"Mode not found"<<std::endl;
        return -1;
    }
}

/********************** Graph  ****************************/

int Graph::getNumVertex() const {
    return vertexSet.size();
}

std::vector<Vertex *> Graph::getVertexSet() const {
    return vertexSet;
}

// Finds a vertex by its code (assumed to be unique).
Vertex *Graph::findVertex(const std::string &code) const {
    for (auto v : vertexSet)
        if (v->getCode() == code)
            return v;
    return nullptr;
}

// Finds a vertex by its id.
Vertex *Graph::findVertex(const int &id) const {
    for (auto v : vertexSet)
        if (v->getId() == id)
            return v;
    return nullptr;
}

int Graph::findVertexIdx(const std::string &name) const {
    for (unsigned i = 0; i < vertexSet.size(); i++)
        if (vertexSet[i]->getName() == name)
            return i;
    return -1;
}

// Adds a vertex with the given properties.
// Returns false if a vertex with the same code already exists.
bool Graph::addVertex(const std::string &name, const int &id, const std::string &code, const bool &park) {
    if (findVertex(code) != nullptr)
        return false;
    vertexSet.push_back(new Vertex(name, id, code, park));
    return true;
}

// Removes a vertex identified by its name and all its edges.
bool Graph::removeVertex(const std::string &name) {
    for (auto it = vertexSet.begin(); it != vertexSet.end(); ++it) {
        if ((*it)->getName() == name) {
            Vertex *v = *it;
            v->removeOutgoingEdges();
            // Remove edges in other vertices that point to v.
            for (auto u : vertexSet) {
                u->removeEdge(v->getName());
            }
            vertexSet.erase(it);
            delete v;
            return true;
        }
    }
    return false;
}

// Adds an edge between vertices identified by their names.
// The parameters are: source name, destination name, walk weight, drive weight.
bool Graph::addEdge(const std::string &sourceName, const std::string &destName, double walk, double drive) const {
    auto v1 = findVertex(sourceName);
    auto v2 = findVertex(destName);
    if (v1 == nullptr || v2 == nullptr)
        return false;
    v1->addEdge(v2, walk, drive);
    return true;
}

// Removes an edge from the source vertex to the destination vertex.
bool Graph::removeEdge(const std::string &sourceName, const std::string &destName) const {
    Vertex *srcVertex = findVertex(sourceName);
    if (srcVertex == nullptr)
        return false;
    return srcVertex->removeEdge(destName);
}

bool Graph::addBidirectionalEdge(const std::string &sourceName, const std::string &destName, double walk, double drive) const {
    auto v1 = findVertex(sourceName);
    auto v2 = findVertex(destName);
    if (v1 == nullptr || v2 == nullptr)
        return false;
    auto e1 = v1->addEdge(v2, walk, drive);
    auto e2 = v2->addEdge(v1, walk, drive);
    e1->setReverse(e2);
    e2->setReverse(e1);
    return true;
}


inline void deleteMatrix(int **m, int n) {
    if (m != nullptr) {
        for (int i = 0; i < n; i++)
            if (m[i] != nullptr)
                delete [] m[i];
        delete [] m;
    }
}

inline void deleteMatrix(double **m, int n) {
    if (m != nullptr) {
        for (int i = 0; i < n; i++)
            if (m[i] != nullptr)
                delete [] m[i];
        delete [] m;
    }
}

Graph::~Graph() {
    deleteMatrix(distMatrix, vertexSet.size());
    deleteMatrix(pathMatrix, vertexSet.size());
}

Graph initialize(const std::string &locs, const std::string &dists) {
    Graph g;
    std::string line;

    std::ifstream locFile(locs);
    if (!locFile.is_open()) {
        std::cout << "Error opening locations file: " << locs << std::endl;
        return g;
    }
    std::getline(locFile, line); // Skip header line
    while (std::getline(locFile, line)) {
        if (line.empty())
            continue;
        std::stringstream ss(line);
        std::string nameStr, idStr, codeStr, parkStr;
        if (std::getline(ss, nameStr, ',') && std::getline(ss, idStr, ',') &&
            std::getline(ss, codeStr, ',') && std::getline(ss, parkStr, ',')) {
            int id = std::stoi(idStr);
            bool park = (std::stoi(parkStr) != 0);
            if (g.addVertex(nameStr, id, codeStr, park) == false) {
                std::cout<<"Problem adding vertex "<<nameStr<<std::endl;
            }
        }
    }
    locFile.close();

    std::ifstream distFile(dists);
    if (!distFile.is_open()) {
        std::cout << "Error opening distances file: " << dists << std::endl;
        return g;
    }
    std::getline(distFile, line); // Skip header line
    while (std::getline(distFile, line)) {
        if (line.empty())
            continue;
        std::stringstream ss(line);
        std::string loc1Str, loc2Str, driveStr, walkStr;
        if (std::getline(ss, loc1Str, ',') &&
            std::getline(ss, loc2Str, ',') &&
            std::getline(ss, driveStr, ',') &&
            std::getline(ss, walkStr, ',')) {
            double drive = (driveStr == "X") ? -1 : std::stod(driveStr); //driving might not be available ("X")
            double walk = std::stod(walkStr);

            if (g.addEdge(loc1Str, loc2Str, walk, drive) == 0) {
                std::cout<<"Problem adding Edge"<<std::endl;
            }
            if (g.addEdge(loc2Str, loc1Str, walk, drive) == 0) {
                std::cout<<"Problem adding Edge"<<std::endl;
            }
        }
    }
    distFile.close();

    return g;
}
