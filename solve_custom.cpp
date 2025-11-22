/**
 * Solve custom dataset - Standalone solver for any input file
 */

#include "graph.h"
#include "solver.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using namespace DisasterRelief;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input_json_file> [output_json_file]" << endl;
        cerr << "Example: " << argv[0] << " small_dataset.json small_output.json" << endl;
        return 1;
    }
    
    string input_file = argv[1];
    string output_file = (argc >= 3) ? argv[2] : "custom_output.json";
    
    try {
        cout << "\n" << string(70, '=') << endl;
        cout << "DISASTER RELIEF OPTIMIZATION - CUSTOM DATASET" << endl;
        cout << string(70, '=') << "\n" << endl;
        
        cout << "Loading dataset: " << input_file << endl;
        Graph graph = Graph::fromJsonFile(input_file);
        
        cout << "Graph loaded successfully!" << endl;
        cout << "  Nodes: " << graph.numNodes() << endl;
        cout << "  Edges: " << graph.numEdges() << endl;
        
        // Load vehicles from the input file
        // Parse JSON to get vehicles
        ifstream file(input_file);
        if (!file.is_open()) {
            throw runtime_error("Cannot open input file");
        }
        
        string content, line;
        while (getline(file, line)) {
            content += line;
        }
        file.close();
        
        // Simple JSON parsing for vehicles
        vector<Vehicle> vehicles;
        size_t vehicles_pos = content.find("\"vehicles\"");
        if (vehicles_pos != string::npos) {
            size_t start = content.find('[', vehicles_pos);
            size_t end = content.find(']', start);
            string vehicles_str = content.substr(start + 1, end - start - 1);
            
            size_t pos = 0;
            while ((pos = vehicles_str.find('{', pos)) != string::npos) {
                size_t vehicle_end = vehicles_str.find('}', pos);
                string vehicle_str = vehicles_str.substr(pos, vehicle_end - pos + 1);
                
                // Extract id
                size_t id_pos = vehicle_str.find("\"id\"");
                int id = 0;
                if (id_pos != string::npos) {
                    size_t colon = vehicle_str.find(':', id_pos);
                    size_t comma = vehicle_str.find_first_of(",}", colon);
                    id = stoi(vehicle_str.substr(colon + 1, comma - colon - 1));
                }
                
                // Extract capacity
                size_t cap_pos = vehicle_str.find("\"capacity\"");
                int capacity = 0;
                if (cap_pos != string::npos) {
                    size_t colon = vehicle_str.find(':', cap_pos);
                    size_t comma = vehicle_str.find_first_of(",}", colon);
                    capacity = stoi(vehicle_str.substr(colon + 1, comma - colon - 1));
                }
                
                vehicles.emplace_back(id, capacity);
                pos = vehicle_end + 1;
            }
        }
        
        cout << "  Vehicles: " << vehicles.size() << endl;
        for (const auto& v : vehicles) {
            cout << "    - Vehicle " << v.id << " (Capacity: " << v.capacity << ")" << endl;
        }
        
        cout << "\nSolving..." << endl;
        
        DisasterReliefSolver solver(graph, vehicles, 0, 1.0, 0.5, 0.3);
        auto solution = solver.solve(true);
        
        cout << "\nSolution found!" << endl;
        solver.printSolution();
        
        solver.exportSolution(output_file);
        
        cout << "\n✅ Solution exported to: " << output_file << endl;
        cout << string(70, '=') << endl;
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}

