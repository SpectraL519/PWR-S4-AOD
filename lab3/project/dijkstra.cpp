#include <cstdint>
#include <vector>
#include <queue>
#include <optional>

#include "include/graph.hpp"
#include "include/types.hpp"
#include "include/input_parser.hpp"
#include "include/problem_handler.hpp"



namespace {
    struct node_t {
        std::size_t vertex; // index in the graph's adjacency_list
        int64_t distance;
    };

    static bool node_distance_compare (const node_t& lhs, const node_t& rhs) {
        return lhs.distance > rhs.distance;
    }
}



graph::distances_t graph::dijkstra_shortest_paths (
    graph::int_graph& graph, 
    const std::size_t source
) {
    std::vector <bool> visited(graph.num_vertices(), false);
    graph::distances_t distances(graph.num_vertices(), INT64_MAX);

    std::priority_queue <
        node_t, 
        std::vector <node_t>,
        bool (*)(const node_t&, const node_t&)
    > queue(node_distance_compare);

    queue.push(node_t{.vertex = source, .distance = 0});
    distances[source] = 0;

    while (!queue.empty()) {
        const node_t node = queue.top();
        queue.pop();

        if (visited[node.vertex])
            continue;

        visited[node.vertex] = true;
        const int64_t distance = distances[node.vertex];
        
        for (const edge_t edge : graph[node.vertex]) {
            const int64_t new_distance = distance + edge.weight;
            if (new_distance < distances[edge.destination]) {
                distances[edge.destination] = new_distance;
                queue.push(node_t{.vertex = edge.destination, .distance = new_distance});
            }
        }
    }

    return distances;
}



int main(int argc, char **argv) {
    std::optional<data_t> data_opt = parse_input(argc, argv);
    return process_problem(data_opt, graph::dijkstra_shortest_paths);
}