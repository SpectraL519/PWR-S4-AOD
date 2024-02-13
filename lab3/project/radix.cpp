#include <iostream>
#include <optional>
#include <vector>
#include <array>
#include <cstdint>

#include "include/graph.hpp"
#include "include/types.hpp"
#include "include/input_parser.hpp"
#include "include/problem_handler.hpp"



namespace {
    struct node_t {
        std::size_t vertex; // index in the graph's adjacency_list
        int64_t distance;
    };

    constexpr static std::size_t int64_bits = sizeof(int64_t) * 8;
    std::size_t bit_width (const int64_t x) {
        return int64_bits - __builtin_clz(x);
    }

    class radix_heap {
        private:
            using bucket_t = std::vector <node_t>;

            // ith bucket contains nodes with labels in range [2^i, 2^(i+1))
            // therefore _num_buckets = num_bits(distance_type) + 1
            constexpr static std::size_t _num_buckets = int64_bits + 1;

            std::array <bucket_t, _num_buckets> _buckets;
            std::array <int64_t, _num_buckets> _bucket_min_distances;
            std::size_t _size = 0;
            int64_t _min_distance = INT64_MAX;

            void _pull_nodes();
            std::size_t _get_bucket (const int64_t distance);

        public: 
            radix_heap();

            const std::size_t size();
            const bool empty();
            void insert (const std::size_t vertex, const int64_t distance);
            const std::size_t extract_first();
    };
}



graph::distances_t graph::radix_shortest_paths (
    graph::int_graph& graph, 
    const std::size_t source
) {
    std::vector <bool> visited(graph.num_vertices(), false);
    graph::distances_t distances(graph.num_vertices(), INT64_MAX);

    radix_heap heap;
    heap.insert(source, 0);
    distances[source] = 0;

    while (!heap.empty()) {
        const std::size_t vertex = heap.extract_first();

        if (visited[vertex])
            continue;

        visited[vertex] = true;
        const int64_t distance = distances[vertex];

        for (const edge_t edge : graph[vertex]) {
            const int64_t new_distance = distance + edge.weight;
            if (new_distance < distances[edge.destination]) {
                distances[edge.destination] = new_distance;
                heap.insert(edge.destination, new_distance);
            }
        }
    }

    return distances;
}



int main(int argc, char **argv) {
    std::optional<data_t> data_opt = parse_input(argc, argv);
    return process_problem(data_opt, graph::radix_shortest_paths);
}



radix_heap::radix_heap () {
    this->_bucket_min_distances.fill(INT64_MAX);
}

const std::size_t radix_heap::size () {
    return this->_size;
}

const bool radix_heap::empty () {
    return this->_size == 0;
}

void radix_heap::insert (const std::size_t vertex, const int64_t distance) {
    const std::size_t bucket_idx = this->_get_bucket(distance);
    this->_buckets[bucket_idx].push_back(node_t{.vertex = vertex, .distance = distance});
    this->_size++;

    if (distance < this->_bucket_min_distances[bucket_idx])
        this->_bucket_min_distances[bucket_idx] = distance;
}

const std::size_t radix_heap::extract_first () {
    this->_pull_nodes();
    node_t node = this->_buckets[0].back();
    this->_buckets[0].pop_back();
    this->_size--;
    return node.vertex;
}

void radix_heap::_pull_nodes () {
    if (!this->_buckets[0].empty())
        return;

    // find first not empty bucket
    std::size_t bucket_idx = 1;
    while (this->_buckets[bucket_idx].empty()) 
        bucket_idx++;

    // update minimum distance
    this->_min_distance = this->_bucket_min_distances[bucket_idx];

    // pull nodes from the bucket to the first buckets
    for (const node_t node : this->_buckets[bucket_idx]) {
        const std::size_t new_bucket = this->_get_bucket(node.distance);
        this->_buckets[new_bucket].push_back(node);

        if (node.distance < this->_bucket_min_distances[new_bucket])
            this->_bucket_min_distances[new_bucket] = node.distance;
    }

    // clear the old bucket
    this->_buckets[bucket_idx].clear();
    this->_bucket_min_distances[bucket_idx] = INT64_MAX;
}

std::size_t radix_heap::_get_bucket (const int64_t distance) {
    return (distance == this->_min_distance) ? 0 : bit_width(distance ^ this->_min_distance);
}