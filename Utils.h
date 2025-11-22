/**
 * Utils.h - Utility functions and cost calculations
 * 
 * This module provides:
 * - Multi-objective cost function
 * - Helper functions for route cost calculation
 */

#ifndef UTILS_H
#define UTILS_H

#include "graph.h"
#include "Allocation.h"
#include <vector>
#include <cmath>

using namespace std;

// Cost function parameters (defaults)
const double ALPHA = 0.6;  // Weight for total time
const double BETA = 0.3;   // Weight for reliability penalty
const double GAMMA = 0.1;  // Weight for idle time

/**
 * RouteCost structure - stores cost breakdown for a route
 */
struct RouteCost {
    double totalTime;           // Total travel time
    double reliabilityPenalty;  // Penalty for low reliability
    double idleTime;            // Unused capacity penalty
    double finalScore;          // Weighted final score
    
    RouteCost() : totalTime(0.0), reliabilityPenalty(0.0), 
                  idleTime(0.0), finalScore(0.0) {}
};

/**
 * Calculate multi-objective cost for a route
 * Cost = α * total_time + β * (1 - reliability_sum) + γ * idle_time
 * 
 * Time Complexity: O(n) where n is route length
 */
RouteCost calculateRouteCost(const Graph& graph, 
                             const vector<int>& route,
                             int vehicleCapacity,
                             int vehicleLoad) {
    RouteCost cost;
    
    // Empty or single-node routes have zero cost
    if (route.size() < 2) {
        return cost;
    }
    
    double totalReliability = 1.0;
    
    // Calculate total time and reliability product
    for (int i = 0; i < (int)route.size() - 1; i++) {
        int u = route[i];
        int v = route[i + 1];
        
        double edgeCost = graph.getEdgeCost(u, v);
        double edgeReliability = graph.getEdgeReliability(u, v);
        
        if (edgeCost >= 0) {
            cost.totalTime += edgeCost;
            totalReliability *= edgeReliability;
        }
    }
    
    // Reliability penalty: (1 - product of all edge reliabilities)
    // Higher reliability = lower penalty
    cost.reliabilityPenalty = 1.0 - totalReliability;
    
    // Idle time: unused capacity as a penalty
    // Vehicles should use their capacity efficiently
    int unusedCapacity = vehicleCapacity - vehicleLoad;
    cost.idleTime = unusedCapacity;
    
    // Final weighted score using multi-objective function
    cost.finalScore = ALPHA * cost.totalTime + 
                      BETA * cost.reliabilityPenalty + 
                      GAMMA * cost.idleTime;
    
    return cost;
}

/**
 * Calculate total cost for all vehicles
 */
double calculateTotalCost(const Graph& graph, 
                         const vector<Vehicle>& vehicles) {
    double totalCost = 0.0;
    
    for (const auto& vehicle : vehicles) {
        RouteCost cost = calculateRouteCost(graph, vehicle.route, 
                                           vehicle.capacity, vehicle.currentLoad);
        totalCost += cost.finalScore;
    }
    
    return totalCost;
}

#endif // UTILS_H
