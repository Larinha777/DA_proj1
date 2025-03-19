// Original code by Gonçalo Leão
// Updated by DA 2024/2025 Team

#ifndef DA_TP_CLASSES_GRAPH
#define DA_TP_CLASSES_GRAPH

#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <limits>
#include <string>
#include "MutablePriorityQueue.h" // not needed for now

#define INF std::numeric_limits<double>::max()

class Edge;

/************************* Vertex  **************************/

class Vertex {
public:
    Vertex(const std::string& name, const int& id, const std::string& code, const bool& park);

    bool operator<(const Vertex &vertex) const; //required by the MutablePriorityQueue

    std::string getName() const;
    int getId() const;
    std::string getCode() const;
    bool isPark() const;

    std::vector<Edge *> getAdj() const;
    bool isVisited() const;
    // bool isProcessing() const;
    // unsigned int getIndegree() const;
    double getDist() const;
    Edge *getPath() const;
    std::vector<Edge *> getIncoming() const;

    double getWalkTime() const;
    bool isAvoiding() const;

    void setName(const std::string& newName);
    void setId(const int& newId);
    void setCode(const std::string& newCode);
    void setPark(bool newPark);

    void setVisited(bool visited);
    // void setProcessing(bool processing);
    // void setIndegree(unsigned int indegree);

    void setWalkTime(double time);
    void setAvoiding(bool avoid);

    void setDist(double dist);
    void setPath(Edge *path);

    Edge *addEdge(Vertex *dest, double walk, double drive);

    bool removeEdge(const std::string &destName);
    void removeOutgoingEdges();

    friend class MutablePriorityQueue<Vertex>;
protected:
    std::string name;
    int id;
    std::string code;
    bool park;

    double walkTime;
    bool avoid;

    // Outgoing edges
    std::vector<Edge *> adj;

    // auxiliary fields
    bool visited = false; // used by DFS, BFS, Prim ...
    // bool processing = false; // used by isDAG (in addition to the visited attribute)
    //unsigned int indegree; // used by topsort
    double dist = 0;
    Edge *path = nullptr;

    std::vector<Edge *> incoming; // incoming edges

    int queueIndex = 0; // required by MutablePriorityQueue and UFDS

    void deleteEdge(const Edge *edge) const;
};

/********************** Edge  ****************************/

class Edge {
public:
    Edge(Vertex *orig, Vertex *dest, double walk, double drive);

    Vertex *getDest() const;
    // bool isSelected() const;
    Vertex *getOrig() const;
    Edge *getReverse() const;

    bool isAvoiding() const;
    void setAvoiding(bool avoid);

    // void setSelected(bool selected);
    void setReverse(Edge *reverse);

    // Functions to modify or return the weight parameters.
    void setWalk(double walk);
    double getWalk() const;
    void setDrive(double drive);
    double getDrive() const;

protected:
    Vertex *dest;  // destination vertex
    //bool selected = false;
    Vertex *orig;  // origin vertex
    Edge *reverse = nullptr;  // pointer to the reverse edge (if bidirectional)
    bool avoid;
    double drive, walk;
};

/********************** Graph  ****************************/

class Graph {
public:
    ~Graph();

    Vertex *findVertex(const std::string &code) const;
    Vertex *findVertex(const int &id) const;

    // Add a vertex with the given properties.
    // Returns false if a vertex with that name already exists.
    bool addVertex(const std::string &name, const int &id, const std::string &code, const bool& park);

    // Remove a vertex by its name.
    bool removeVertex(const std::string &name);

    // Adds an edge to the graph, using vertex names to identify source and destination.
    bool addEdge(const std::string &sourceName, const std::string &destName, double walk, double drive) const;
    bool removeEdge(const std::string &sourceName, const std::string &destName) const;
    bool addBidirectionalEdge(const std::string &sourceName, const std::string &destName, double walk, double drive) const;

    int getNumVertex() const;
    std::vector<Vertex *> getVertexSet() const;


protected:
    std::vector<Vertex *> vertexSet; // set of vertices

    double **distMatrix = nullptr;  // distance matrix (e.g., for Floyd-Warshall)
    int **pathMatrix = nullptr;     // path matrix for reconstruction of shortest paths

    // Finds the index of a vertex given its name.
    int findVertexIdx(const std::string &name) const;
};

// Utility functions to delete allocated matrices.
void deleteMatrix(int **m, int n);
void deleteMatrix(double **m, int n);

// New project-specific function
Graph initialize(const std::string& locs, const std::string& dists);

#endif // GRAPH_H
#endif /* DA_TP_CLASSES_GRAPH */