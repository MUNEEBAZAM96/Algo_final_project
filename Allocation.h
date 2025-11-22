/**
 * Allocation.h - Vehicle allocation and route planning
 * 
 * This module implements priority-based greedy allocation:
 * 1. Sort nodes by priority (descending)
 * 2. For each node, assign to vehicle with minimum extra cost
 * 3. Use Dijkstra to compute shortest paths
 */

#ifndef ALLOCATION_H
#define ALLOCATION_H

#include "graph.h"
#include "algorithms.h"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <limits>

using namespace std;

// Vehicle structure - represents a rescue vehicle
struct Vehicle {
    int id;
    int capacity;          // Maximum capacity
    int currentLoad;        // Current load (sum of demands served)
    vector<int> route;      // Route as sequence of node IDs
    
    Vehicle() : id(0), capacity(0), currentLoad(0) {}
    Vehicle(int id, int capacity) 
        : id(id), capacity(capacity), currentLoad(0) {
        route.push_back(0); // Start at depot (node 0)
    }
    
    // Check if vehicle can serve a node with given demand
    bool canServe(int demand) const {
        return currentLoad + demand <= capacity;
    }
    
    // Add a node to the route and update load
    void addNode(int nodeId, int demand) {
        route.push_back(nodeId);
        currentLoad += demand;
    }
    
    // Finalize route by returning to depot
    void finalizeRoute() {
        route.push_back(0); // Return to depot
    }
};

/**
 * Priority-Based Greedy Allocation
 * Time Complexity: O(V * V * log V) for Dijkstra calls
 * 
 * Algorithm:
 * 1. Sort all nodes by priority (descending) - higher priority first
 * 2. For each high-priority node:
 *    - Find vehicle with minimum extra cost to serve it
 *    - Use Dijkstra to compute shortest path from vehicle's last position
 *    - Assign node to best vehicle
 */
vector<Vehicle> allocateVehicles(const Graph& graph, 
                                 const vector<Vehicle>& vehicles) {
    vector<Vehicle> assignedVehicles = vehicles;
    
    // Get all nodes except depot (node 0) that have demand
    vector<int> nodesToAssign;
    for (int nodeId : graph.getAllNodeIds()) {
        if (nodeId != 0) {
            const Node* node = graph.getNode(nodeId);
            if (node && node->demand > 0) {
                nodesToAssign.push_back(nodeId);
            }
        }
    }
    
    // Sort nodes by priority (descending) - highest priority first
    sort(nodesToAssign.begin(), nodesToAssign.end(),
         [&graph](int a, int b) {
             const Node* nodeA = graph.getNode(a);
             const Node* nodeB = graph.getNode(b);
             if (!nodeA || !nodeB) return false;
             return nodeA->priority > nodeB->priority;
         });
    
    // Assign nodes to vehicles using greedy approach
    for (int nodeId : nodesToAssign) {
        const Node* node = graph.getNode(nodeId);
        if (!node) continue;
        
        int bestVehicle = -1;
        double minExtraCost = numeric_limits<double>::max();
        
        // Find vehicle with minimum extra cost to serve this node
        for (int i = 0; i < (int)assignedVehicles.size(); i++) {
            Vehicle& vehicle = assignedVehicles[i];
            
            // Check if vehicle has capacity
            if (!vehicle.canServe(node->demand)) {
                continue;
            }
            
            // Get last node in vehicle's current route
            int lastNode = vehicle.route.back();
            
            // Compute shortest path cost from last node to new node using Dijkstra
            unordered_map<int, double> distances = dijkstra(graph, lastNode);
            
            // Get distance to target node
            double extraCost = numeric_limits<double>::max();
            if (distances.find(nodeId) != distances.end()) {
                extraCost = distances[nodeId];
            }
            
            // Update best vehicle if this one has lower cost
            if (extraCost < minExtraCost && 
                extraCost < numeric_limits<double>::max()) {
                minExtraCost = extraCost;
                bestVehicle = i;
            }
        }
        
        // Assign node to best vehicle if found
        if (bestVehicle >= 0) {
            assignedVehicles[bestVehicle].addNode(nodeId, node->demand);
        }
    }
    
    // Finalize all routes (return to depot)
    for (auto& vehicle : assignedVehicles) {
        vehicle.finalizeRoute();
    }
    
    return assignedVehicles;
}

#endif // ALLOCATION_H
