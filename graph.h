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

#define INF std::numeric_limits<double>::max()

// Forward declaration
class Edge;

/************************* Vertex  **************************/

class Vertex {
public:
    // The constructor now directly stores the vertex information.
    Vertex(const std::string& name, const int& id, const std::string& code, const bool& park);

    // Operator required by the MutablePriorityQueue (implementation not shown)
    bool operator<(const Vertex &vertex) const;

    // Getter functions to return each piece of vertex information
    std::string getName() const;
    int getId() const;
    std::string getCode() const;
    bool isPark() const;

    // Functions that previously returned a Location now return individual pieces or work on the fields directly.
    std::vector<Edge *> getAdj() const;
    bool isVisited() const;
    bool isProcessing() const;
    unsigned int getIndegree() const;
    double getDist() const;
    Edge *getPath() const;
    std::vector<Edge *> getIncoming() const;

    // Setter functions for individual fields (setLoc is removed in favor of these)
    void setName(const std::string& newName);
    void setId(const int& newId);
    void setCode(const std::string& newCode);
    void setPark(bool newPark);

    void setVisited(bool visited);
    void setProcessing(bool processing);

    int getLow() const;
    void setLow(int value);
    int getNum() const;
    void setNum(int value);

    void setIndegree(unsigned int indegree);
    void setDist(double dist);
    void setPath(Edge *path);

    // Adds an edge from this vertex to destination vertex.
    Edge *addEdge(Vertex *dest, double walk, double drive);

    // Instead of passing a Location, removeEdge now takes the destination vertex’s name.
    bool removeEdge(const std::string &destName);
    void removeOutgoingEdges();

protected:
    // Directly storing the properties that used to be in Location.
    std::string name;
    int id;
    std::string code;
    bool park;

    // Outgoing edges
    std::vector<Edge *> adj;

    // Auxiliary fields for graph algorithms
    bool visited = false;
    bool processing = false;
    int low = -1, num = -1;
    unsigned int indegree = 0;
    double dist = 0;
    Edge *path = nullptr;

    std::vector<Edge *> incoming; // incoming edges

    int queueIndex = 0; // required by MutablePriorityQueue and UFDS

    // Helper function to delete a specific edge.
    void deleteEdge(const Edge *edge) const;
};

/********************** Edge  ****************************/

class Edge {
public:
    // Edge constructor: note that the vertices are passed directly.
    Edge(Vertex *orig, Vertex *dest, double walk, double drive);

    Vertex *getDest() const;
    bool isSelected() const;
    Vertex *getOrig() const;
    Edge *getReverse() const;

    void setSelected(bool selected);
    void setReverse(Edge *reverse);

    // Functions to modify or return the weight parameters.
    void setWalk(double walk);
    double getWalk() const;
    void setDrive(double drive);
    double getDrive() const;

protected:
    Vertex *dest;  // destination vertex
    bool selected = false;
    Vertex *orig;  // origin vertex
    Edge *reverse = nullptr;  // pointer to the reverse edge (if bidirectional)

    double drive, walk;
};

/********************** Graph  ****************************/

class Graph {
public:
    ~Graph();

    // Find a vertex using its name (previously this used a Location).
    Vertex *findVertex(const std::string &name) const;

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

// Optionally, an initialize function to build the graph from data.
Graph initialize(const std::string& locs, const std::string& dists);

#endif // GRAPH_H
#endif /* DA_TP_CLASSES_GRAPH */