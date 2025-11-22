/**
 * Simple Solver - All code in header file
 */

#ifndef SOLVER_H
#define SOLVER_H

#include "graph.h"
#include "algorithms.h"
#include <unordered_map>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>

using namespace std;

namespace DisasterRelief {

class DisasterReliefSolver {
private:
    Graph graph;
    vector<Vehicle> vehicles;
    int depot;
    unordered_map<int, vector<int>> routes;
    
public:
    DisasterReliefSolver(const Graph& g, const vector<Vehicle>& v, int depot = 0)
        : graph(g), vehicles(v), depot(depot) {
    }
    
    void solve() {
        // Step 1: Compute shortest paths from depot
        cout << "Step 1: Computing shortest paths..." << endl;
        auto [distances, parents] = ShortestPathAlgorithms::dijkstra(graph, depot, true);
        
        // Step 2: Build routes using simple greedy approach
        cout << "Step 2: Building routes..." << endl;
        
        // Get all locations sorted by priority (highest first)
        vector<int> locations;
        for (int nodeId : graph.getAllNodeIds()) {
            if (nodeId != depot) {
                const Node* node = graph.getNode(nodeId);
                if (node && node->priority > 0) {
                    locations.push_back(nodeId);
                }
            }
        }
        
        // Sort by priority (descending)
        sort(locations.begin(), locations.end(), [this](int a, int b) {
            const Node* nodeA = this->graph.getNode(a);
            const Node* nodeB = this->graph.getNode(b);
            if (!nodeA || !nodeB) return false;
            return nodeA->priority > nodeB->priority;
        });
        
        // Initialize routes
        for (auto& vehicle : vehicles) {
            routes[vehicle.id] = {depot};
            vehicle.currentLoad = 0;
        }
        
        // Assign locations to vehicles
        vector<bool> visited(locations.size(), false);
        
        for (auto& vehicle : vehicles) {
            for (size_t i = 0; i < locations.size(); i++) {
                if (visited[i]) continue;
                
                int locId = locations[i];
                const Node* node = graph.getNode(locId);
                if (!node) continue;
                
                // Check if vehicle can serve this location
                if (vehicle.currentLoad + node->demand <= vehicle.capacity) {
                    routes[vehicle.id].push_back(locId);
                    vehicle.currentLoad += node->demand;
                    visited[i] = true;
                }
            }
            
            // Return to depot
            routes[vehicle.id].push_back(depot);
        }
    }
    
    void printSolution() const {
        cout << "\n========================================" << endl;
        cout << "SOLUTION" << endl;
        cout << "========================================" << endl;
        
        for (const auto& vehicle : vehicles) {
            auto it = routes.find(vehicle.id);
            if (it == routes.end()) continue;
            
            const vector<int>& route = it->second;
            
            cout << "\nVehicle " << vehicle.id << " (Capacity: " << vehicle.capacity << "):" << endl;
            cout << "  Route: ";
            for (size_t i = 0; i < route.size(); i++) {
                cout << route[i];
                if (i < route.size() - 1) cout << " -> ";
            }
            cout << endl;
            
            // Calculate metrics
            int totalDemand = 0;
            int totalPriority = 0;
            double totalDistance = 0.0;
            
            for (size_t i = 0; i < route.size() - 1; i++) {
                int u = route[i];
                int v = route[i + 1];
                
                EdgeCostResult cost = graph.getEdgeCost(u, v, false);
                if (cost.found) {
                    totalDistance += cost.cost;
                }
            }
            
            for (int nodeId : route) {
                if (nodeId != depot) {
                    const Node* node = graph.getNode(nodeId);
                    if (node) {
                        totalDemand += node->demand;
                        totalPriority += node->priority;
                    }
                }
            }
            
            cout << "  Locations Served: " << (route.size() - 2) << endl;
            cout << "  Total Distance: " << fixed << setprecision(2) << totalDistance << endl;
            cout << "  Total Demand: " << totalDemand << "/" << vehicle.capacity << endl;
            cout << "  Total Priority: " << totalPriority << endl;
        }
        
        cout << "\n========================================" << endl;
    }
    
    void exportSolution(const string& filename) const {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Cannot create file: " << filename << endl;
            return;
        }
        
        file << "{\n";
        file << "  \"routes\": {\n";
        
        bool first = true;
        for (const auto& vehicle : vehicles) {
            auto it = routes.find(vehicle.id);
            if (it == routes.end()) continue;
            
            if (!first) file << ",\n";
            first = false;
            
            file << "    \"" << vehicle.id << "\": [";
            const vector<int>& route = it->second;
            for (size_t i = 0; i < route.size(); i++) {
                file << route[i];
                if (i < route.size() - 1) file << ", ";
            }
            file << "]";
        }
        
        file << "\n  }\n";
        file << "}\n";
        file.close();
    }
    
    const unordered_map<int, vector<int>>& getRoutes() const {
        return routes;
    }
};

} // namespace DisasterRelief

#endif // SOLVER_H
