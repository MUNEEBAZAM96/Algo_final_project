/**
 * graph.h - Graph data structure for disaster response routing
 * 
 * This module defines the graph structure representing the disaster region.
 * Each node represents a location with demand and priority.
 * Each edge represents a road connection with cost and reliability.
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <unordered_map>
#include <string>

using namespace std;

// Node represents a location in the disaster area
struct Node {
    int id;
    int demand;      // Resource demand at this location
    int priority;    // Priority level (higher = more urgent)
    
    Node() : id(0), demand(0), priority(0) {}
    Node(int id, int demand, int priority) 
        : id(id), demand(demand), priority(priority) {}
};

// Edge represents a road connection between locations
struct Edge {
    int u;              // Source node
    int v;              // Destination node
    double cost;        // Travel time/cost
    double reliability;  // Edge reliability (0.0 to 1.0)
    
    Edge() : u(0), v(0), cost(0.0), reliability(1.0) {}
    Edge(int u, int v, double cost, double reliability)
        : u(u), v(v), cost(cost), reliability(reliability) {}
};

// Graph class using adjacency list representation
// Time complexity: O(V + E) for construction, O(1) for neighbor queries
class Graph {
private:
    unordered_map<int, Node> nodes;
    unordered_map<int, vector<pair<int, Edge>>> adjacencyList;
    vector<Edge> edges;

public:
    Graph() = default;
    
    // Add a node to the graph
    void addNode(const Node& node) {
        nodes[node.id] = node;
        if (adjacencyList.find(node.id) == adjacencyList.end()) {
            adjacencyList[node.id] = vector<pair<int, Edge>>();
        }
    }
    
    // Add an edge to the graph (undirected)
    void addEdge(const Edge& edge) {
        edges.push_back(edge);
        
        // Add to adjacency list for both directions (undirected graph)
        adjacencyList[edge.u].push_back({edge.v, edge});
        adjacencyList[edge.v].push_back({edge.u, edge});
    }
    
    // Get all neighbors of a node
    const vector<pair<int, Edge>>& getNeighbors(int nodeId) const {
        static const vector<pair<int, Edge>> empty;
        auto it = adjacencyList.find(nodeId);
        if (it != adjacencyList.end()) {
            return it->second;
        }
        return empty;
    }
    
    // Get node by ID
    const Node* getNode(int nodeId) const {
        auto it = nodes.find(nodeId);
        if (it != nodes.end()) {
            return &(it->second);
        }
        return nullptr;
    }
    
    // Get all node IDs
    vector<int> getAllNodeIds() const {
        vector<int> ids;
        for (const auto& pair : nodes) {
            ids.push_back(pair.first);
        }
        return ids;
    }
    
    // Get edge cost between two nodes
    double getEdgeCost(int u, int v) const {
        for (const auto& neighbor : getNeighbors(u)) {
            if (neighbor.first == v) {
                return neighbor.second.cost;
            }
        }
        return -1.0; // Not found
    }
    
    // Get edge reliability between two nodes
    double getEdgeReliability(int u, int v) const {
        for (const auto& neighbor : getNeighbors(u)) {
            if (neighbor.first == v) {
                return neighbor.second.reliability;
            }
        }
        return 0.0; // Not found
    }
    
    // Get all edges
    const vector<Edge>& getEdges() const {
        return edges;
    }
    
    // Get number of nodes
    int numNodes() const {
        return nodes.size();
    }
    
    // Get number of edges
    int numEdges() const {
        return edges.size();
    }
};

#endif // GRAPH_H
