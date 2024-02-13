#include <iostream>
#include <vector>
#include <chrono>
#include "graph.hpp"





int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Error: Invalid arguments\n");
        return 1;
    }

    std::string algorithm = argv[1];
    std::string file_name = argv[2];

    graph::IntGraph graph;
    try {
        graph.from_file(file_name);
        if (graph.num_vertices() <= 20)
            graph.show();
    }
    catch (std::invalid_argument& e) {
        std::cout << "Error: Could not read '" << file_name << "'!\n\t";
        std::cout << e.what() << "\n";
        std::exit(1);
    }
    catch (std::exception& e) {
        std::cout << "Error: Could not read '" << file_name << "'!\n\t";
        std::cout << e.what() << "\n";
        std::exit(1);
    }

    // exercise 1
    if (algorithm == "dfs") {
        auto start = std::chrono::high_resolution_clock::now();
        std::pair <std::vector <int>, IntGraph> search = graph::algorithm::search(graph, true);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        
        std::cout << "\nDFS vertex visiting order:\n";
        for (int v : search.first) 
            std::cout << v + 1 << " ";

        std::cout << "\n\nDFS search tree:\n";
        search.second.show();
        std::cout << "Execution time: " << (float)duration.count() / 1000 << "s\n";
    }
    else if (algorithm == "bfs") {
        auto start = std::chrono::high_resolution_clock::now();
        std::pair <std::vector <int>, IntGraph> search = graph::algorithm::search(graph, false);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        
        std::cout << "\nBFS vertex visiting order:\n";
        for (int v : search.first) 
            std::cout << v + 1 << " ";

        std::cout << "\n\nBFS search tree:\n";
        search.second.show();
        std::cout << "Execution time: " << (float)duration.count() / 1000 << "s\n";
    }

    // exercise 2
    else if (algorithm == "ts") {
        std::cout << "\nTopological order:\n";
        try {
            auto start = std::chrono::high_resolution_clock::now();
            std::vector <int> topological_order = graph::algorithm::topological_sort(graph);
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

            if (graph.num_vertices() <= 200) {
                for (int v : topological_order)
                    std::cout << v + 1 << " ";
                std::cout << "\n";
            }
            else 
                std::cout << "Graph is sortable topologically!\n";
            std::cout << "Execution time: " << (float)duration.count() / 1000 << "s\n";
        }
        catch (std::invalid_argument &e) {
            std::cout << e.what() << "\n";
        }

    }

    // exercise 3
    else if (algorithm == "scc") {
        std::cout << "\nStrongly connected componnents:\n";
        auto start = std::chrono::high_resolution_clock::now();
        std::vector <std::vector <int>> scc = graph::algorithm::scc(graph);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

        std::cout << "Number of SCCs: " << scc.size() << "\n";
        if (graph.num_vertices() <= 200) {
            int index = 0;
            for (std::vector <int> componnent : scc) {
                std::cout << ++index << ": ";
                for (int v : componnent)
                    std::cout << v + 1 << " ";
                std::cout << "\n";
            }
        }
        std::cout << "Execution time: " << (float)duration.count() / 1000 << "s\n";
    }

    // exercise 4
    else if (algorithm == "bi") {
        std::cout << "\nBipartite graph:\n";
        try {
            auto start = std::chrono::high_resolution_clock::now();
            std::pair<std::vector <int>, std::vector <int>> bp = graph::algorithm::bipartite_partition(graph); 
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

            if (graph.num_vertices() <= 200) {
                std::cout << "Red: ";
                for (int v : bp.first)
                    std::cout << v << " ";
                std::cout << "\nBlue: ";
                for (int v : bp.second)
                    std::cout << v << " ";
                std::cout << "\n";
            }
            else 
                std::cout << "Graph is bipartite!\n";
            std::cout << "Execution time: " << (float)duration.count() / 1000 << "s\n";
        } 
        catch (std::invalid_argument &e) {
            std::cout << e.what() << "\n";
        }
    }
    else {
        std::cout << "Error: Invalid value of `algorithm` - must be ['dfs', 'bfs', 'to', 'scc', 'bi']!\n";
    }
    
    return 0;
}