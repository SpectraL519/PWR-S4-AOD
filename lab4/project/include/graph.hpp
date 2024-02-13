#pragma once

#include <iostream>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <random>
#include <algorithm>
#include <queue>



namespace graph {
    struct edge_descriptor {
        int32_t source = -1;
        int32_t destination = -1;
        int32_t flow = 0;
        int32_t capacity = 0;
        int32_t reverse = -1; 
    };

    using vertex_descriptor = std::vector <edge_descriptor>;

    class graph {
        private:
            std::vector <vertex_descriptor> _adjacency_list;

            bool _dinic_calc_levels (
                const int32_t source, 
                const int32_t sink, 
                std::vector<int32_t>& levels
            );
            int32_t _dinic_send_flow (
                const int32_t source, 
                const int32_t sink,
                int32_t flow,
                std::vector<int32_t>& start,
                std::vector<int32_t>& levels
            );

        public:
            graph() = default;

            void show();

            bool empty();
            std::size_t num_vertices();
            vertex_descriptor& operator [] (const std::size_t index);
            bool has_edge (const int32_t source, const int32_t destination);

            void build_hipercube (const int32_t k);
            void build_bipartite (const int32_t k, const int32_t degree);

            std::pair <int32_t, std::size_t> edmonds_karp_max_flow (const int32_t source, const int32_t sink);
            std::pair <int32_t, std::size_t> dinic_max_flow (const int32_t source, const int32_t sink);
            std::vector <std::pair <int32_t, int32_t>> max_flow_matchings (const int32_t source, const int32_t sink);
    };
} // namespace graph;



void graph::graph::show () {
    std::size_t num_vertices = this->num_vertices();
    for (int v = 0; v < num_vertices; v++) {
        std::cout << v << ": ";
        for (edge_descriptor& edge : this->_adjacency_list[v]) 
            std::cout << "(" << edge.destination 
                      << ", f: " << edge.flow 
                      << ", c: " << edge.capacity << ") ";
        std::cout << std::endl;
    }
}



inline bool graph::graph::empty () {
    return this->_adjacency_list.empty();
}

inline std::size_t graph::graph::num_vertices () {
    return this->_adjacency_list.size();
}

inline graph::vertex_descriptor& graph::graph::operator [] (std::size_t index) {
    return this->_adjacency_list[index];
}

bool graph::graph::has_edge (const int32_t source, const int32_t destination) {
    if (source < 0 || source > this->num_vertices() || destination < 0 || destination > this->num_vertices())
        return false;

    for (const edge_descriptor& edge : this->_adjacency_list[source])
        if (edge.destination == destination)
            return true;
    
    return false;
}



void graph::graph::build_hipercube (const int32_t size) {
    if (size < 1 || size > 16)
        throw std::invalid_argument("Error: size (k) must be in [1, 16] - was " + std::to_string(size));

    if (!this->_adjacency_list.empty())
        throw std::logic_error("Error: graph is not empty");

    std::random_device rd;
    std::mt19937 mt(rd());

    const int32_t num_vertices = 1 << size;
    this->_adjacency_list.resize(num_vertices);

    for (int32_t src = 0; src < num_vertices; src++) {
        vertex_descriptor& vd = this->_adjacency_list[src];

        for (int32_t k = 0; k < size; k++) {
            const int32_t dst = src | (1 << k);
            if (dst == src) 
                continue;

            // calculate capacity
            const int32_t src_hamming = __builtin_popcount(src);
            const int32_t src_zeros = k - src_hamming;

            const int32_t dst_hamming = __builtin_popcount(dst);
            const int32_t dst_zeros = k - dst_hamming;

            const int32_t range = std::max(
                std::max(src_hamming, src_zeros), 
                std::max(dst_hamming, dst_zeros)
            );
            

            std::uniform_int_distribution<int32_t> uniform(1, 1 << range);
            vd.push_back(edge_descriptor {
                .source = src,
                .destination = dst,
                .flow = 0,
                .capacity = uniform(mt)
            });
        }
    }
}

void graph::graph::build_bipartite (const int32_t size, const int32_t degree) {
    if (size < 1 || size > 16)
        throw std::invalid_argument("Error: size (k) must be in [1, 16] - was " + std::to_string(size));

    if (degree < 1 || degree > size)
        throw std::invalid_argument(
            "Error: degree must be in [1, size] - was " + 
            std::to_string(degree) + ", size = " + std::to_string(size)
        );

    if (!this->_adjacency_list.empty())
        throw std::logic_error("Error: graph is not empty");

    const int32_t num_vertices = 1 << size; // number of vertices in a subset: v1, v2
    this->_adjacency_list.resize((num_vertices << 1) + 2);

    vertex_descriptor& source_vd = this->_adjacency_list[0];
    const int32_t sink_idx = 2 * num_vertices + 1;
    vertex_descriptor& sink_vd = this->_adjacency_list[sink_idx];
    for (int32_t dst = 1; dst <= num_vertices; dst++) {
        source_vd.push_back(edge_descriptor {
            .source = 0,
            .destination = dst,
            .capacity = 1,
            .reverse = (int32_t)_adjacency_list[dst].size()
        });
        this->_adjacency_list[dst].push_back(edge_descriptor {
            .source = dst,
            .destination = 0,
            .capacity = 1,
            .reverse = (int32_t)source_vd.size()
        });

        const int32_t v2_dst = num_vertices + dst;
        sink_vd.push_back(edge_descriptor {
            .source = sink_idx,
            .destination = v2_dst,
            .capacity = 1,
            .reverse = (int32_t)_adjacency_list[v2_dst].size()
        });
        this->_adjacency_list[v2_dst].push_back(edge_descriptor {
            .source = v2_dst,
            .destination = sink_idx,
            .capacity = 1,
            .reverse = (int32_t)sink_vd.size()
        });
    }

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution <int32_t> uniform(1, num_vertices);

    std::vector <int32_t> v2(num_vertices);
    std::iota(v2.begin(), v2.end(), num_vertices + 1);

    for (int32_t src = 1; src <= num_vertices; src++) {
        std::shuffle(v2.begin(), v2.end(), mt);

        for (int32_t d = 0; d < degree; d++) {
            const int32_t dst = v2[d];

            this->_adjacency_list[src].push_back(edge_descriptor {
                .source = src,
                .destination = dst,
                .capacity = 1,
                .reverse = (int32_t)_adjacency_list[dst].size()
            });
            this->_adjacency_list[dst].push_back(edge_descriptor {
                .source = dst,
                .destination = src,
                .capacity = 1,
                .reverse = (int32_t)_adjacency_list[src].size()
            });
        }

        std::size_t i = 0;
        while (i != v2.size()) {
            if (this->_adjacency_list[v2[i]].size() == degree + 1) {
                v2.erase(v2.begin() + i);
                continue;
            }
            i++;
        }
    }
}



std::pair <int32_t, std::size_t> graph::graph::edmonds_karp_max_flow (
    const int32_t source, 
    const int32_t sink
) {
    int32_t max_flow = 0;
    std::size_t augmenting_paths = 0;

    while (true) {
        // run the bfs algorithm to find the shortest source - sink path
        std::vector <edge_descriptor*> pred(this->num_vertices(), nullptr);

        std::queue <int32_t> q;
        q.push(source);

        while (!q.empty()) {
            const int32_t u = q.front();
            q.pop();

            for (edge_descriptor& edge : this->_adjacency_list[u]) {
                if (
                    pred[edge.destination] == nullptr /* no predecessor */ &&
                    edge.destination != source &&
                    edge.capacity > edge.flow
                ) {
                    pred[edge.destination] = &edge;
                    q.push(edge.destination);
                }
            }
        }

        if (pred[sink] == nullptr) 
            return std::make_pair(max_flow, augmenting_paths); // cannot reach sink from source

        // augmenting path found
        augmenting_paths++;

        // calculate max flow
        int32_t flow = INT32_MAX;
        for (edge_descriptor* edge = pred[sink]; edge != nullptr; edge = pred[edge->source]) 
            flow = std::min(flow, edge->capacity - edge->flow);

        // update edge flow values
        for (edge_descriptor* edge = pred[sink]; edge != nullptr; edge = pred[edge->source]) {
            edge->flow += flow;

            if (edge->reverse != -1) {
                edge_descriptor& reverse_edge = this->_adjacency_list[edge->destination][edge->reverse];
                reverse_edge.flow -= flow;
            }
        }

        max_flow += flow;
    }

    return std::make_pair(max_flow, augmenting_paths);
}



bool graph::graph::_dinic_calc_levels (
    const int32_t source, 
    const int32_t sink, 
    std::vector<int32_t>& levels
) {
    std::fill(levels.begin(), levels.end(), -1);
    levels[source] = 0;

    std::queue <int32_t> q;
    q.push(source);

    while (!q.empty()) {
        int32_t u = q.front();
        q.pop();

        for (const edge_descriptor& edge : this->_adjacency_list[u]) {
            if (levels[edge.destination] < 0 && edge.flow < edge.capacity) {
                levels[edge.destination] = levels[u] + 1;
                q.push(edge.destination);
            }
        }
    }

    return levels[sink] >= 0;
}

int32_t graph::graph::_dinic_send_flow (
    const int32_t source, 
    const int32_t sink,
    int32_t flow,
    std::vector<int32_t>& start,
    std::vector<int32_t>& levels
) {
    if (source == sink)
        return flow;

    for (; start[source] < this->_adjacency_list[source].size(); start[source]++) {
        edge_descriptor& edge = this->_adjacency_list[source][start[source]];

        if (levels[edge.destination] == levels[edge.source] + 1 && edge.flow < edge.capacity) {
            int32_t curr_flow = std::min(flow, edge.capacity - edge.flow);
            int32_t tmp_flow = this->_dinic_send_flow(edge.destination, sink, curr_flow, start, levels);
        
            if (tmp_flow > 0) {
                edge.flow += tmp_flow;
                
                if (edge.reverse != -1) 
                    this->_adjacency_list[edge.destination][edge.reverse].flow -= tmp_flow;

                return tmp_flow;
            }
        }
    }

    return 0;
}

std::pair <int32_t, std::size_t> graph::graph::dinic_max_flow (
    const int32_t source, 
    const int32_t sink
) {
    int32_t max_flow = 0;
    std::size_t augmenting_paths = 0;

    if (source == sink)
        return std::make_pair(max_flow, augmenting_paths);

    std::vector <int32_t> levels(this->num_vertices());

    while (this->_dinic_calc_levels(source, sink, levels)) {
        std::vector <int32_t> start(this->num_vertices() + 1, 0);

        while (int32_t flow = this->_dinic_send_flow(source, sink, INT32_MAX, start, levels)) {
            max_flow += flow;
            augmenting_paths++;
        }
    }

    return std::make_pair(max_flow, augmenting_paths);
}



std::vector <std::pair <int32_t, int32_t>> graph::graph::max_flow_matchings (
    const int32_t source, 
    const int32_t sink
) {
    std::vector <std::pair <int32_t, int32_t>> matchings;
    for (const vertex_descriptor& vd : this->_adjacency_list) {
        for (const edge_descriptor& edge : vd) {
            if (
                edge.source != source && edge.destination != sink &&
                edge.source < edge.destination && edge.flow == 1
            ) {
                matchings.push_back(std::make_pair(edge.source, edge.destination));
            }
        }
    }

    return std::move(matchings);
}
