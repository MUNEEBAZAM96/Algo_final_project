#!/usr/bin/env python3
"""
Generate realistic disaster relief datasets
"""

import json
import random
import math

def generate_realistic_dataset(
    num_nodes=20,
    num_vehicles=3,
    grid_size=100.0,
    edge_density=0.3,
    output_file='realistic_input.json'
):
    """
    Generate a realistic disaster relief scenario
    
    Parameters:
    - num_nodes: Number of disaster locations (excluding depot)
    - num_vehicles: Number of rescue vehicles
    - grid_size: Size of the coordinate grid
    - edge_density: Probability of road between nearby locations
    - output_file: Output JSON filename
    """
    
    print(f"Generating dataset with {num_nodes} locations...")
    
    # Set random seed for reproducibility
    random.seed(42)
    
    nodes = []
    
    # Add depot at center
    depot = {
        "id": 0,
        "demand": 0,
        "priority": 0,
        "x": grid_size / 2,
        "y": grid_size / 2
    }
    nodes.append(depot)
    
    # Generate disaster zones with realistic distribution
    # Priority zones: 30% high (4-5), 40% medium (2-3), 30% low (1)
    priority_distribution = [5, 5, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 1, 1]
    
    for i in range(1, num_nodes + 1):
        # Cluster some nodes (disasters often affect nearby areas)
        if i % 3 == 0 and i > 3:
            # Create clusters near previous nodes
            base_node = nodes[random.randint(1, len(nodes)-1)]
            x = base_node['x'] + random.uniform(-15, 15)
            y = base_node['y'] + random.uniform(-15, 15)
        else:
            # Random distribution
            x = random.uniform(5, grid_size - 5)
            y = random.uniform(5, grid_size - 5)
        
        # Clamp coordinates
        x = max(5, min(grid_size - 5, x))
        y = max(5, min(grid_size - 5, y))
        
        priority = random.choice(priority_distribution)
        demand = random.randint(1, min(8, priority * 2))  # Higher priority = higher demand
        
        node = {
            "id": i,
            "demand": demand,
            "priority": priority,
            "x": round(x, 2),
            "y": round(y, 2)
        }
        nodes.append(node)
    
    print(f"✅ Generated {len(nodes)} nodes (including depot)")
    
    # Generate edges based on distance and connectivity
    edges = []
    
    # Calculate distance between all pairs
    def euclidean_dist(n1, n2):
        return math.sqrt((n1['x'] - n2['x'])**2 + (n1['y'] - n2['y'])**2)
    
    # Create connectivity graph
    # Strategy: Connect each node to nearest neighbors + depot
    for i, node_i in enumerate(nodes):
        # Calculate distances to all other nodes
        distances = []
        for j, node_j in enumerate(nodes):
            if i != j:
                dist = euclidean_dist(node_i, node_j)
                distances.append((j, dist))
        
        # Sort by distance
        distances.sort(key=lambda x: x[1])
        
        # Connect to depot (always)
        if i != 0:
            depot_dist = euclidean_dist(node_i, nodes[0])
            reliability = random.uniform(0.75, 0.95)
            
            # Check if edge already exists
            edge_exists = any(
                (e['u'] == 0 and e['v'] == i) or (e['u'] == i and e['v'] == 0)
                for e in edges
            )
            
            if not edge_exists:
                edges.append({
                    "u": min(0, i),
                    "v": max(0, i),
                    "cost": round(depot_dist, 2),
                    "reliability": round(reliability, 2)
                })
        
        # Connect to 2-4 nearest neighbors
        num_neighbors = random.randint(2, 4)
        for neighbor_idx, dist in distances[:num_neighbors]:
            # Add edge with some probability
            if random.random() < edge_density + 0.4:  # Higher connectivity
                reliability = random.uniform(0.7, 1.0)
                
                # Check if edge already exists
                u, v = min(i, neighbor_idx), max(i, neighbor_idx)
                edge_exists = any(
                    e['u'] == u and e['v'] == v
                    for e in edges
                )
                
                if not edge_exists:
                    edges.append({
                        "u": u,
                        "v": v,
                        "cost": round(dist, 2),
                        "reliability": round(reliability, 2)
                    })
    
    # Add some random long-distance roads (highways)
    num_highways = max(3, num_nodes // 8)
    for _ in range(num_highways):
        i = random.randint(1, num_nodes)
        j = random.randint(1, num_nodes)
        if i != j:
            u, v = min(i, j), max(i, j)
            edge_exists = any(e['u'] == u and e['v'] == v for e in edges)
            
            if not edge_exists:
                dist = euclidean_dist(nodes[i], nodes[j])
                edges.append({
                    "u": u,
                    "v": v,
                    "cost": round(dist * 0.8, 2),  # Highways are faster
                    "reliability": round(random.uniform(0.85, 0.98), 2)
                })
    
    print(f"✅ Generated {len(edges)} road connections")
    
    # Generate vehicles with varying capacities
    vehicles = []
    base_capacity = 10
    for i in range(1, num_vehicles + 1):
        capacity = base_capacity + random.randint(0, 10)
        vehicles.append({
            "id": i,
            "capacity": capacity
        })
    
    print(f"✅ Generated {len(vehicles)} vehicles")
    
    # Create final data structure
    data = {
        "nodes": nodes,
        "edges": edges,
        "vehicles": vehicles
    }
    
    # Save to file
    with open(output_file, 'w') as f:
        json.dump(data, f, indent=2)
    
    print(f"\n✅ Dataset saved to: {output_file}")
    
    # Print statistics
    print(f"\n" + "="*70)
    print("DATASET STATISTICS")
    print("="*70)
    print(f"Total Locations: {num_nodes} (+ 1 depot)")
    print(f"Total Roads: {len(edges)}")
    print(f"Vehicles: {num_vehicles}")
    print(f"Average Edges per Node: {len(edges) * 2 / len(nodes):.1f}")
    
    # Priority distribution
    priorities = [n['priority'] for n in nodes if n['priority'] > 0]
    high_priority = sum(1 for p in priorities if p >= 4)
    medium_priority = sum(1 for p in priorities if p == 3)
    low_priority = sum(1 for p in priorities if p < 3)
    
    print(f"\nPriority Distribution:")
    print(f"  High Priority (4-5): {high_priority} locations")
    print(f"  Medium Priority (3): {medium_priority} locations")
    print(f"  Low Priority (1-2): {low_priority} locations")
    
    # Demand statistics
    total_demand = sum(n['demand'] for n in nodes)
    total_capacity = sum(v['capacity'] for v in vehicles)
    print(f"\nResource Requirements:")
    print(f"  Total Demand: {total_demand} units")
    print(f"  Total Vehicle Capacity: {total_capacity} units")
    print(f"  Coverage: {(total_capacity / total_demand * 100):.1f}%")
    
    # Reliability statistics
    avg_reliability = sum(e['reliability'] for e in edges) / len(edges)
    print(f"\nRoad Quality:")
    print(f"  Average Reliability: {avg_reliability:.2f}")
    print(f"  Damaged Roads (<0.8): {sum(1 for e in edges if e['reliability'] < 0.8)}")
    
    print("="*70)
    
    return data

def main():
    """Generate multiple dataset sizes"""
    
    print("\n" + "="*70)
    print("DISASTER RELIEF DATASET GENERATOR")
    print("="*70 + "\n")
    
    # Generate small dataset (good for visualization)
    print("1. Generating SMALL dataset (20 locations)...")
    generate_realistic_dataset(
        num_nodes=20,
        num_vehicles=3,
        output_file='small_dataset.json'
    )
    
    print("\n" + "-"*70 + "\n")
    
    # Generate medium dataset (demonstration)
    print("2. Generating MEDIUM dataset (50 locations)...")
    generate_realistic_dataset(
        num_nodes=50,
        num_vehicles=5,
        output_file='medium_dataset.json'
    )
    
    print("\n" + "-"*70 + "\n")
    
    # Generate large dataset (scalability test)
    print("3. Generating LARGE dataset (100 locations)...")
    generate_realistic_dataset(
        num_nodes=100,
        num_vehicles=8,
        output_file='large_dataset.json'
    )
    
    print("\n" + "="*70)
    print("ALL DATASETS GENERATED SUCCESSFULLY! ✅")
    print("="*70)
    print("\nGenerated files:")
    print("  - small_dataset.json   (20 locations, 3 vehicles)")
    print("  - medium_dataset.json  (50 locations, 5 vehicles)")
    print("  - large_dataset.json   (100 locations, 8 vehicles)")
    print("\nUse these with your disaster_relief program!")
    print("="*70 + "\n")

if __name__ == "__main__":
    main()

