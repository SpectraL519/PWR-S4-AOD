#pragma once

#include <optional>
#include <functional>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <chrono>

#include "graph.hpp"
#include "types.hpp"



int process_problem (
    std::optional<data_t>& data_opt,
    std::function <graph::distances_t(graph::int_graph&, const std::size_t)> shortest_paths
) {
    if (!data_opt) 
        return 1;

    data_t data = data_opt.value();
    if (data.problem == problem_t::ss) {
        if (!data.ss) {
            std::cerr << "Error: cannot access ss problem data" << std::endl;
            return 1;
        }

        std::ofstream out_file(data.out_file_name);
        if (!out_file) {
            std::cerr << "Error: Invalid file name: " + data.out_file_name << std::endl;
            return 1;
        }

        ss_t ss = data.ss.value();

        auto start = std::chrono::high_resolution_clock::now();

        for (std::size_t src : ss.sources) 
            shortest_paths(data.graph, src);

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        float avg_time = (float)duration.count() / (float)ss.sources.size() / 1000.0f;

        out_file << "p res sp ss dijkstra" << std::endl;
        out_file << "f " << data.graph_file_name << " " << data.problem_file_name << std::endl;
        out_file << "g " << data.graph.num_vertices() << " " 
                         << data.num_edges << " " 
                         << data.graph.min_weight() << " " 
                         << data.graph.max_weight() << std::endl;
        out_file << "t " << avg_time << std::endl;
        out_file.close();
    }
    else {
        if (!data.p2p) {
            std::cerr << "Error: cannot access p2p problem data" << std::endl;
            return 1;
        }

        std::ofstream out_file(data.out_file_name);
        if (!out_file) {
            std::cerr << "Error: Invalid file name: " + data.out_file_name << std::endl;
            return 1;
        }

        out_file << "f " << data.graph_file_name << " " << data.problem_file_name << std::endl;
        out_file << "g " << data.graph.num_vertices() << " " 
                         << data.num_edges << " " 
                         << data.graph.min_weight() << " " 
                         << data.graph.max_weight() << std::endl;

        p2p_t p2p = data.p2p.value();
        std::unordered_map <std::size_t, graph::distances_t> distances_map;

        for (std::pair <std::size_t, std::size_t> pair : p2p.pairs) {
            if (distances_map.find(pair.first) == distances_map.end())
                distances_map[pair.first] = shortest_paths(data.graph, pair.first);
            
            out_file << "d " << pair.first + 1 << " " 
                             << pair.second + 1 << " " 
                             << distances_map[pair.first][pair.second] << std::endl;
        }
    }

    return 0;
}