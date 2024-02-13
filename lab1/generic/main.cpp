#include <iostream>
#include <vector>
#include "graph.hpp"





int main(int argc, char* argv[]) {
    // std::ios_base::sync_with_stdio(0);
    // std::cin.tie(NULL);

    if (argc < 3) {
        printf("Error: Invalid arguments\n");
        return 1;
    }

    std::string algorithm = argv[1];
    std::string file_name = argv[2];

    graph::Graph <int> graph = graph::int_graph_from_file(file_name);
    // graph.show();

    // exercise 1
    if (algorithm == "dfs") {
        std::cout << "\nDFS search tree:\n";
        graph::Graph <int> dfs_tree = graph::algorithm::search(graph, true);
        dfs_tree.show();
    }
    else if (algorithm == "bfs") {
        std::cout << "\n\nBFS search tree:\n";
        graph::Graph <int> dfs_tree = graph::algorithm::search(graph, false);
        dfs_tree.show();
    }

    // exercise 2
    else if (algorithm == "ts") {
        std::cout << "\nTopological order:\n";
        try {
            std::vector <int> topological_order = graph::algorithm::topological_sort(graph);
            if (graph.num_vertices() <= 200) {
                for (int v : topological_order)
                    std::cout << v << " ";
                std::cout << "\n";
            }
            else 
                std::cout << "Graph is sortable topologically!\n";
        }
        catch (std::invalid_argument &e) {
            std::cout << e.what() << "\n";
        }

    }

    // exercise 3
    else if (algorithm == "scc") {
        std::cout << "\nStrongly connected componnents:\n";
        graph::graph_t<int>::partition scc = graph::algorithm::scc(graph);
        std::cout << "Number of SCCs: " << scc.size() << "\n";
        if (graph.num_vertices() <= 200) {
            int index = 0;
            for (auto componnent : scc) {
                std::cout << ++index << ": ";
                for (auto v : componnent)
                    std::cout << v << " ";
                std::cout << "\n";
            }
        }
    }

    // exercise 4
    else if (algorithm == "bi") {
        std::cout << "\nBipartite graph:\n";
        try {
            graph::graph_t<int>::partition bp = graph::algorithm::bipartite_partition(graph); 
            std::cout << "Graph is bipartite!\n";
            if (graph.num_vertices() <= 200) {
                std::cout << "Red: ";
                for (int v : bp[0])
                    std::cout << v << " ";
                std::cout << "\nBlue: ";
                for (int v : bp[1])
                    std::cout << v << " ";
                std::cout << "\n";
            }
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