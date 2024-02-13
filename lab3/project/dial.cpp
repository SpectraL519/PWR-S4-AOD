#include <iostream>
#include <optional>
#include <vector>
#include <queue>
#include <cstdint>

#include "include/graph.hpp"
#include "include/types.hpp"
#include "include/input_parser.hpp"
#include "include/problem_handler.hpp"



namespace {
    class bucket_queue {
        private:
            using bucket_t = std::queue <std::size_t>;
            std::vector <bucket_t> _buckets;
            std::size_t _num_buckets;
            std::size_t _size = 0;
            std::size_t _min_label_idx = 0;

        public:
            bucket_queue (int32_t max_label);

            const std::size_t size();
            const bool empty();
            void push (const std::size_t vertex, const int64_t label);
            const std::size_t extract_first();
    };
}



graph::distances_t graph::dial_shortest_paths (
    graph::int_graph& graph, 
    const std::size_t source
) {
    std::vector <bool> visited(graph.num_vertices(), false);
    graph::distances_t distances(graph.num_vertices(), INT64_MAX);

    bucket_queue queue(graph.max_weight());
    queue.push(source, 0);
    distances[source] = 0;

    while (!queue.empty()) {
        const std::size_t vertex = queue.extract_first();
        
        if (visited[vertex])
            continue;

        visited[vertex] = true;
        const int64_t distance = distances[vertex];

        for (const edge_t edge : graph[vertex]) {
            const int64_t new_distance = distance + edge.weight;
            if (new_distance < distances[edge.destination]) {
                distances[edge.destination] = new_distance;
                queue.push(edge.destination, new_distance);
            }
        }
    }

    return distances;
}



int main(int argc, char **argv) {
    std::optional<data_t> data_opt = parse_input(argc, argv);
    return process_problem(data_opt, graph::dial_shortest_paths);
}



bucket_queue::bucket_queue (int32_t max_label) {
    this->_num_buckets = max_label + 1;
    this->_buckets.resize(this->_num_buckets);
}

const std::size_t bucket_queue::size () {
    return this->_size;
}

const bool bucket_queue::empty () {
    return this->_size == 0;
}

void bucket_queue::push (const std::size_t vertex, const int64_t label) {
    this->_buckets[label % this->_num_buckets].push(vertex);
    this->_size++;
}

const std::size_t bucket_queue::extract_first () {
    if (this->_buckets[this->_min_label_idx].empty()) 
        while (this->_buckets[this->_min_label_idx].empty()) {
            this->_min_label_idx++;
            if (this->_min_label_idx == this->_num_buckets) 
                this->_min_label_idx = 0;
        }

    const std::size_t vertex = this->_buckets[this->_min_label_idx].front();
    this->_buckets[this->_min_label_idx].pop();
    this->_size--;

    return vertex;
}