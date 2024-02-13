#pragma once

#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <numeric>



namespace graph {
    struct edge_t {
        std::size_t destination;
        int32_t weight;
    };

    typedef std::vector <edge_t> vertex_t;

    class int_graph {
        private:
            std::vector <vertex_t> _adjacency_list;
            int32_t _min_weight = INT32_MAX;
            int32_t _max_weight = INT32_MIN;

        public:
            int_graph() = default;
            int_graph (const std::size_t num_vertices);
            ~int_graph() = default;

            std::size_t num_vertices();
            int32_t min_weight();
            int32_t max_weight();
            vertex_t& operator [] (const std::size_t index);
            void add_edge (const std::size_t u, const std::size_t v, const int32_t weight);

            void show();
    };

    // shortest path finding
    typedef std::vector <int64_t> distances_t;

    distances_t dijkstra_shortest_paths (int_graph& graph, const std::size_t source);
    distances_t dial_shortest_paths (int_graph& graph, const std::size_t source);
    distances_t radix_shortest_paths (int_graph& graph, const std::size_t source);
}


graph::int_graph::int_graph (const std::size_t num_vertices) {
    this->_adjacency_list = std::vector<vertex_t>(num_vertices);
}


std::size_t graph::int_graph::num_vertices () {
    return this->_adjacency_list.size();
}

int32_t graph::int_graph::min_weight () {
    return this->_min_weight;
}

int32_t graph::int_graph::max_weight () {
    return this->_max_weight;
}

graph::vertex_t& graph::int_graph::operator[] (const std::size_t vertex) {
    return this->_adjacency_list[vertex];
}

void graph::int_graph::add_edge (const std::size_t u, const std::size_t v, const int32_t weight) {
    this->_adjacency_list[u].push_back(edge_t{.destination = v, .weight = weight});

    if (weight > this->_max_weight)
        this->_max_weight = weight;
    if (weight < this->_min_weight)
        this->_min_weight = weight;
}

void graph::int_graph::show() {
    int num_vertices = this->num_vertices();
    for (int32_t v = 0; v < num_vertices; v++) {
        std::cout << v + 1 << ": ";
        for (edge_t adj : this->_adjacency_list[v]) 
            std::cout << "(" << adj.destination + 1 << ": " << adj.weight << ") ";
        std::cout << "\n";
    }
}