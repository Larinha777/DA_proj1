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
#include "MutablePriorityQueue.h"

#define INF std::numeric_limits<double>::max()

class Edge;

/************************* Vertex  **************************/

/**
 * @brief Class representing a vertex in the graph.
 */
class Vertex {
public:
    /**
     * @brief Constructor for Vertex.
     *
     * @param name The name of the vertex.
     * @param id The unique identifier for the vertex.
     * @param code The code associated with the vertex.
     * @param park Boolean indicating if the vertex is a park.
     */
    Vertex(const std::string& name, const int& id, const std::string& code, const bool& park);

    /**
     * @brief Comparison operator for Vertex.
     *
     * Required by the MutablePriorityQueue.
     *
     * @param vertex The vertex to compare with.
     * @return true if this vertex is considered less than the other.
     */
    bool operator<(const Vertex &vertex) const;

    /**
     * @brief Gets the name of the vertex.
     *
     * @return The name of the vertex.
     */
    std::string getName() const;

    /**
     * @brief Gets the identifier of the vertex.
     *
     * @return The id of the vertex.
     */
    int getId() const;

    /**
     * @brief Gets the code associated with the vertex.
     *
     * @return The code as a string.
     */
    std::string getCode() const;

    /**
     * @brief Checks if the vertex represents a park.
     *
     * @return True if the vertex is a park, false otherwise.
     */
    bool isPark() const;

    /**
     * @brief Gets the outgoing edges from the vertex.
     *
     * @return A vector of pointers to Edge representing adjacent edges.
     */
    std::vector<Edge *> getAdj() const;

    /**
     * @brief Checks if the vertex has been visited.
     *
     * @return True if visited, false otherwise.
     */
    bool isVisited() const;

    /**
     * @brief Gets the distance from the source vertex.
     *
     * @param mode The mode of travel (e.g., 0 for driving, 1 for walking).
     * @return The distance as a double.
     */
    double getDist(int mode) const;

    /**
     * @brief Gets the previous edge in the shortest path.
     *
     * @param mode The mode of travel (e.g., 0 for driving, 1 for walking).
     * @return Pointer to the Edge in the path.
     */
    Edge *getPath(int mode) const;

    /**
     * @brief Gets the incoming edges to the vertex.
     *
     * @return A vector of pointers to Edge representing incoming edges.
     */
    std::vector<Edge *> getIncoming() const;

    /**
     * @brief Checks if the vertex is marked to be avoided.
     *
     * @return True if marked as avoiding, false otherwise.
     */
    bool isAvoiding() const;

    /**
     * @brief Sets the name of the vertex.
     *
     * @param newName The new name for the vertex.
     */
    void setName(const std::string& newName);

    /**
     * @brief Sets the id of the vertex.
     *
     * @param newId The new id for the vertex.
     */
    void setId(const int& newId);

    /**
     * @brief Sets the code of the vertex.
     *
     * @param newCode The new code for the vertex.
     */
    void setCode(const std::string& newCode);

    /**
     * @brief Sets whether the vertex is a park.
     *
     * @param newPark Boolean indicating the new park status.
     */
    void setPark(bool newPark);

    /**
     * @brief Sets the visited flag for the vertex.
     *
     * @param visited Boolean indicating if the vertex has been visited.
     */
    void setVisited(bool visited);

    /**
     * @brief Sets the distance for the vertex.
     *
     * @param dist The distance to be set.
     * @param mode The mode of travel (e.g., 0 for driving, 1 for walking).
     */
    void setDist(double dist, int mode);

    /**
     * @brief Sets the previous edge in the path.
     *
     * @param path Pointer to the edge that precedes this vertex.
     * @param mode The mode of travel (e.g., 0 for driving, 1 for walking).
     */
    void setPath(Edge *path, int mode);

    /**
     * @brief Sets the avoiding flag for the vertex.
     *
     * @param avoid Boolean indicating whether to avoid this vertex.
     */
    void setAvoiding(bool avoid);

    /**
     * @brief Adds an edge from this vertex to a destination vertex.
     *
     * @param dest Pointer to the destination vertex.
     * @param walk Weight for walking mode.
     * @param drive Weight for driving mode.
     * @return Pointer to the created edge.
     */
    Edge *addEdge(Vertex *dest, double walk, double drive);

    /**
     * @brief Removes an edge from this vertex to the specified destination.
     *
     * @param destName The name of the destination vertex.
     * @return True if the edge was successfully removed, false otherwise.
     */
    bool removeEdge(const std::string &destName);

    /**
     * @brief Removes all outgoing edges from this vertex.
     */
    void removeOutgoingEdges();

    // Friend declaration for MutablePriorityQueue
    friend class MutablePriorityQueue<Vertex>;

protected:
    std::string name;                 ///< Name of the vertex.
    int id;                           ///< Unique identifier.
    std::string code;                 ///< Code associated with the vertex.
    bool park;                        ///< Indicates if the vertex is a park.

    std::vector<Edge *> adj;          ///< Outgoing edges.

    bool visited = false;             ///< Visited flag.
    double driveDist = INF;           ///< Distance for driving mode.
    Edge *drivePath = nullptr;        ///< Previous edge in driving mode.
    double walkDist = INF;            ///< Distance for walking mode.
    Edge *walkPath = nullptr;         ///< Previous edge in walking mode.
    bool avoid = false;               ///< Flag to indicate avoiding this vertex.

    std::vector<Edge *> incoming;     ///< Incoming edges.

    int queueIndex = 0;               ///< Required by MutablePriorityQueue and UFDS.

    /**
     * @brief Deletes a specified edge.
     *
     * @param edge Pointer to the edge to be deleted.
     */
    void deleteEdge(const Edge *edge) const;
};

/********************** Edge  ****************************/

/**
 * @brief Class representing an edge in the graph.
 */
class Edge {
public:
    /**
     * @brief Constructor for Edge.
     *
     * @param orig Pointer to the origin vertex.
     * @param dest Pointer to the destination vertex.
     * @param walk Weight for walking mode.
     * @param drive Weight for driving mode.
     */
    Edge(Vertex *orig, Vertex *dest, double walk, double drive);

    /**
     * @brief Gets the destination vertex of the edge.
     *
     * @return Pointer to the destination vertex.
     */
    Vertex *getDest() const;

    /**
     * @brief Gets the origin vertex of the edge.
     *
     * @return Pointer to the origin vertex.
     */
    Vertex *getOrig() const;

    /**
     * @brief Gets the reverse edge.
     *
     * @return Pointer to the reverse edge if it exists, otherwise nullptr.
     */
    Edge *getReverse() const;

    /**
     * @brief Checks if the edge is marked to be avoided.
     *
     * @return True if the edge is to be avoided, false otherwise.
     */
    bool isAvoiding() const;

    /**
     * @brief Sets the avoiding flag for the edge.
     *
     * @param avoid Boolean indicating whether to avoid this edge.
     */
    void setAvoiding(bool avoid);

    /**
     * @brief Sets the reverse edge pointer.
     *
     * @param reverse Pointer to the reverse edge.
     */
    void setReverse(Edge *reverse);

    /**
     * @brief Sets the walking weight of the edge.
     *
     * @param walk New walking weight.
     */
    void setWalk(double walk);

    /**
     * @brief Gets the walking weight of the edge.
     *
     * @return The walking weight as a double.
     */
    double getWalk() const;

    /**
     * @brief Sets the driving weight of the edge.
     *
     * @param drive New driving weight.
     */
    void setDrive(double drive);

    /**
     * @brief Gets the driving weight of the edge.
     *
     * @return The driving weight as a double.
     */
    double getDrive() const;

    /**
     * @brief Sets the time for the edge for a given mode.
     *
     * @param time The time to be set.
     * @param mode The mode of travel (e.g., 0 for driving, 1 for walking).
     */
    void setTime (double time, int mode);

    /**
     * @brief Gets the time for the edge for a given mode.
     *
     * @param mode The mode of travel (e.g., 0 for driving, 1 for walking).
     * @return The time as a double.
     */
    double getTime(int mode) const;
protected:
    Vertex *dest;  ///< Destination vertex.
    Vertex *orig;  ///< Origin vertex.
    Edge *reverse = nullptr;  ///< Pointer to the reverse edge (if bidirectional).
    bool avoid;    ///< Flag to indicate if the edge should be avoided.
    double drive;  ///< Driving weight.
    double walk;   ///< Walking weight.
};

/********************** Graph  ****************************/

/**
 * @brief Class representing a graph.
 */
class Graph {
public:
    /**
     * @brief Destructor for the Graph.
     */
    ~Graph();

    /**
     * @brief Finds a vertex by its code.
     *
     * @param code The code to search for.
     * @return Pointer to the Vertex if found, otherwise nullptr.
     */
    Vertex *findVertex(const std::string &code) const;

    /**
     * @brief Finds a vertex by its identifier.
     *
     * @param id The identifier to search for.
     * @return Pointer to the Vertex if found, otherwise nullptr.
     */
    Vertex *findVertex(const int &id) const;

    /**
     * @brief Adds a vertex to the graph.
     *
     * @param name The name of the vertex.
     * @param id The unique identifier for the vertex.
     * @param code The code associated with the vertex.
     * @param park Boolean indicating if the vertex is a park.
     * @return True if the vertex was added successfully, false if a vertex with the same name exists.
     */
    bool addVertex(const std::string &name, const int &id, const std::string &code, const bool& park);

    /**
     * @brief Removes a vertex from the graph by its name.
     *
     * @param name The name of the vertex to remove.
     * @return True if the vertex was removed, false otherwise.
     */
    bool removeVertex(const std::string &name);

    /**
     * @brief Adds an edge to the graph using vertex names.
     *
     * @param sourceName The name of the source vertex.
     * @param destName The name of the destination vertex.
     * @param walk Weight for walking mode.
     * @param drive Weight for driving mode.
     * @return True if the edge was added successfully, false otherwise.
     */
    bool addEdge(const std::string &sourceName, const std::string &destName, double walk, double drive) const;

    /**
     * @brief Removes an edge from the graph using vertex names.
     *
     * @param sourceName The name of the source vertex.
     * @param destName The name of the destination vertex.
     * @return True if the edge was removed successfully, false otherwise.
     */
    bool removeEdge(const std::string &sourceName, const std::string &destName) const;

    /**
     * @brief Adds a bidirectional edge between two vertices.
     *
     * @param sourceName The name of the source vertex.
     * @param destName The name of the destination vertex.
     * @param walk Weight for walking mode.
     * @param drive Weight for driving mode.
     * @return True if both edges were added successfully, false otherwise.
     */
    bool addBidirectionalEdge(const std::string &sourceName, const std::string &destName, double walk, double drive) const;

    /**
     * @brief Gets the number of vertices in the graph.
     *
     * @return The total number of vertices.
     */
    int getNumVertex() const;

    /**
     * @brief Retrieves the set of vertices in the graph.
     *
     * @return A vector of pointers to Vertex representing the vertex set.
     */
    std::vector<Vertex *> getVertexSet() const;

protected:
    std::vector<Vertex *> vertexSet; ///< Set of vertices in the graph.

    double **distMatrix = nullptr;  ///< Distance matrix (e.g., for Floyd-Warshall).
    int **pathMatrix = nullptr;     ///< Path matrix for reconstruction of shortest paths.

    /**
     * @brief Finds the index of a vertex by its name.
     *
     * @param name The name of the vertex.
     * @return The index of the vertex if found, otherwise -1.
     */
    int findVertexIdx(const std::string &name) const;
};

/**
 * @brief Utility function to delete a matrix of integers.
 *
 * @param m Pointer to the integer matrix.
 * @param n Number of rows in the matrix.
 */
void deleteMatrix(int **m, int n);

/**
 * @brief Utility function to delete a matrix of doubles.
 *
 * @param m Pointer to the double matrix.
 * @param n Number of rows in the matrix.
 */
void deleteMatrix(double **m, int n);

/**
 * @brief Project-specific function to initialize the graph.
 *
 * @param locs The locations file.
 * @param dists The distances file.
 * @return An initialized Graph object.
 */
Graph initialize(const std::string& locs, const std::string& dists);

#endif // GRAPH_H
#endif /* DA_TP_CLASSES_GRAPH */
