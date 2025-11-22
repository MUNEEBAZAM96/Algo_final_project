/**
 * algorithms.h - Graph algorithms for routing
 * 
 * This module implements:
 * - Dijkstra's algorithm for shortest paths
 * - A* search algorithm with heuristic
 * - 2-opt route optimization
 */

#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "graph.h"
#include <vector>
#include <unordered_map>
#include <queue>
#include <limits>
#include <algorithm>
#include <cmath>

using namespace std;

/**
 * Dijkstra's Algorithm
 * Time Complexity: O((V + E) log V) using priority queue
 * Returns: unordered_map of minimum distances from source to all reachable nodes
 */
unordered_map<int, double> dijkstra(const Graph& graph, int source) {
    unordered_map<int, double> distMap;
    unordered_map<int, bool> visited;
    
    // Initialize distances to infinity for all nodes
    for (int nodeId : graph.getAllNodeIds()) {
        distMap[nodeId] = numeric_limits<double>::max();
        visited[nodeId] = false;
    }
    distMap[source] = 0.0;
    
    // Priority queue: (distance, node_id) - smaller distance has higher priority
    priority_queue<pair<double, int>, 
                   vector<pair<double, int>>,
                   greater<pair<double, int>>> pq;
    pq.push({0.0, source});
    
    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();
        
        // Skip if already visited
        if (visited[u]) continue;
        visited[u] = true;
        
        // Check all neighbors of current node
        for (const auto& neighbor : graph.getNeighbors(u)) {
            int v = neighbor.first;
            double edgeCost = neighbor.second.cost;
            
            // Update distance if we found a shorter path
            if (!visited[v] && distMap[u] + edgeCost < distMap[v]) {
                distMap[v] = distMap[u] + edgeCost;
                pq.push({distMap[v], v});
            }
        }
    }
    
    return distMap;
}

/**
 * A* Search Algorithm
 * Time Complexity: O((V + E) log V) in worst case
 * Heuristic: simple absolute difference |a - b|
 * Returns: path from source to target, or empty vector if no path exists
 */
vector<int> astar(const Graph& graph, int source, int target) {
    unordered_map<int, double> gScore;  // Cost from start to this node
    unordered_map<int, double> fScore;  // gScore + heuristic (total estimated cost)
    unordered_map<int, int> cameFrom;   // For path reconstruction
    unordered_map<int, bool> visited;
    
    // Initialize scores to infinity
    for (int nodeId : graph.getAllNodeIds()) {
        gScore[nodeId] = numeric_limits<double>::max();
        fScore[nodeId] = numeric_limits<double>::max();
        visited[nodeId] = false;
    }
    
    // Start node has zero cost from start
    gScore[source] = 0.0;
    fScore[source] = abs(source - target); // Simple heuristic: |source - target|
    
    // Priority queue: (fScore, node_id)
    priority_queue<pair<double, int>,
                   vector<pair<double, int>>,
                   greater<pair<double, int>>> pq;
    pq.push({fScore[source], source});
    
    while (!pq.empty()) {
        int current = pq.top().second;
        pq.pop();
        
        if (visited[current]) continue;
        visited[current] = true;
        
        // If we reached the target, reconstruct and return path
        if (current == target) {
            vector<int> path;
            int node = target;
            while (node != -1) {
                path.push_back(node);
                auto it = cameFrom.find(node);
                node = (it != cameFrom.end()) ? it->second : -1;
            }
            reverse(path.begin(), path.end());
            return path;
        }
        
        // Check all neighbors
        for (const auto& neighbor : graph.getNeighbors(current)) {
            int v = neighbor.first;
            double edgeCost = neighbor.second.cost;
            double tentativeGScore = gScore[current] + edgeCost;
            
            // If we found a better path to neighbor, update it
            if (tentativeGScore < gScore[v]) {
                cameFrom[v] = current;
                gScore[v] = tentativeGScore;
                fScore[v] = gScore[v] + abs(v - target); // Heuristic
                pq.push({fScore[v], v});
            }
        }
    }
    
    return {}; // No path found
}

/**
 * 2-Opt Route Optimization
 * Time Complexity: O(n^2) per iteration, typically converges quickly
 * Improves route by reversing segments when it reduces total cost
 * Returns: optimized route
 */
vector<int> twoOpt(const Graph& graph, const vector<int>& route) {
    // Routes with 3 or fewer nodes can't be optimized
    if (route.size() <= 3) return route;
    
    vector<int> bestRoute = route;
    bool improved = true;
    
    // Keep trying improvements until no improvement is found
    while (improved) {
        improved = false;
        
        // Try all possible segment reversals
        for (int i = 1; i < (int)bestRoute.size() - 2; i++) {
            for (int j = i + 1; j < (int)bestRoute.size() - 1; j++) {
                // Calculate current route cost
                double currentCost = 0.0;
                bool currentValid = true;
                for (int k = 0; k < (int)bestRoute.size() - 1; k++) {
                    double cost = graph.getEdgeCost(bestRoute[k], bestRoute[k + 1]);
                    if (cost < 0) {
                        currentValid = false;
                        break;
                    }
                    currentCost += cost;
                }
                if (!currentValid) continue;
                
                // Try reversing segment from i to j
                vector<int> newRoute = bestRoute;
                reverse(newRoute.begin() + i, newRoute.begin() + j + 1);
                
                // Calculate new route cost
                double newCost = 0.0;
                bool valid = true;
                for (int k = 0; k < (int)newRoute.size() - 1; k++) {
                    double cost = graph.getEdgeCost(newRoute[k], newRoute[k + 1]);
                    if (cost < 0) {
                        valid = false;
                        break;
                    }
                    newCost += cost;
                }
                
                // If new route is better, use it
                if (valid && newCost < currentCost) {
                    bestRoute = newRoute;
                    improved = true;
                    break;
                }
            }
            if (improved) break;
        }
    }
    
    return bestRoute;
}

#endif // ALGORITHMS_H
