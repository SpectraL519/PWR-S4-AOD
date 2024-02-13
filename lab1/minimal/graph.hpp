#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <deque>
#include <queue>
#include <stack>
#include <algorithm>
#include <cmath>
#include <functional>
#include <stdexcept>





// Declarations 
namespace graph {
    class IntGraph {
        private:
            bool directed;
            std::vector <std::vector <int>> adjacency_list;
            std::vector <int> in_deg_list;

        public:
            IntGraph() = default;
            IntGraph (bool directed);
            ~IntGraph() = default;

            void show();
            bool is_directed();
            bool is_empty();
            int num_vertices();
            std::vector <int> vertices();
            std::vector<int>& operator [] (int index);
            int in_deg (int vertex);
            void push_vertices (int max_vertex);
            bool add_edge (int u, int v);

            void from_file (std::string file_name);
    };



    namespace algorithm {
        // `_` prefixed members should be considered private

        // dfs, bfs
        struct _container_f {
            // stack and queue container operation functions
            std::function <int(std::deque<int>&)> first;
            std::function <void(std::deque<int>&)> pop_first;
        };

        std::pair <std::vector <int>, IntGraph>  search (IntGraph graph, bool depth_first);


        // finding IntGraph's topological order or acyclicity
        std::vector <int> topological_sort (IntGraph graph);


        // finding IntGraph's strongly connected componnents
        struct _scc_s {
            // structures required for the strong connecting algorithm
            std::vector <int> componnent_index;
            std::vector <int> low_link;
            std::vector <bool> on_stack;
            std::stack <int> stack;
            int componnent;
            std::vector <std::vector<int>> scc;
        };

        void _scc_unit (IntGraph &graph, _scc_s &cs, std::stack<int> &dfs_stack);

        std::vector <std::vector <int>> scc (IntGraph graph);


        // checking if a IntGraph is bipartite
        std::pair<std::vector <int>, std::vector <int>> bipartite_partition (IntGraph graph);
    };
}

// Definitions
using namespace graph;

// IntGraph container
IntGraph::IntGraph (bool directed) {
    this->directed = directed;
}

void IntGraph::show() {
    int num_vertices = this->num_vertices();
    for (int v = 0; v < num_vertices; v++) {
        std::cout << v + 1 << ": ";
        for (int adj : this->adjacency_list[v]) 
            std::cout << adj + 1 << " ";
        std::cout << "\n";
    }
}

bool IntGraph::is_directed () {
    return this->directed;
}

bool IntGraph::is_empty () {
    return this->adjacency_list.empty();
}

int IntGraph::num_vertices () {
    return this->adjacency_list.size();
}

std::vector <int> IntGraph::vertices () {
    std::vector <int> vertices(this->num_vertices());
    std::iota(vertices.begin(), vertices.end(), 0);
    return vertices;
}

std::vector<int>& IntGraph::operator[] (int vertex) {
    return this->adjacency_list[vertex];
}

int IntGraph::in_deg (int vertex) {
    return this->in_deg_list[vertex];
}


void IntGraph::push_vertices (int max_vertex) {
    this->adjacency_list.resize(max_vertex, std::vector<int>());
    this->in_deg_list.resize(max_vertex, 0);
}


bool IntGraph::add_edge (int u, int v) {
    if (u < 0 || u >= this->num_vertices())
        return false;

    if (v < 0 || v >= this->num_vertices())
        return false;

    this->adjacency_list[u].push_back(v);
    this->in_deg_list[v]++;
    if (!this->directed) {
        this->adjacency_list[v].push_back(u);
        this->in_deg_list[u]++;
    }

    return true;
}

// IntGraph utils (reading from file)
/*
Reading a IntGraph with vertices of type <int>
File structure:
[D|U] - dorected / undirected
n - number of vertices (the IntGraph will have vertices numbered [1 ... n])
m - number of edges
list of m edges in a "u v" format
*/
void IntGraph::from_file (std::string file_name) {
    std::cout << "Reading data...\n";
    std::ifstream infile;
    infile.open(file_name);

    if (!infile.is_open())
        throw std::invalid_argument("Could not open: " + file_name + "!");

    char g_type;
    infile >> g_type;
    switch (g_type) {
        case 'D': {
            this->directed = true;
            break;
        }
        case 'U': {
            this->directed = false;
            break;
        }
        default: {
            std::string message = "Invalid Graph type (" + std::to_string(g_type) + ") - must be 'U' or 'D'";
            throw std::invalid_argument(message);
        }
    }

    int n_vertices, n_edges;
    infile >> n_vertices >> n_edges;
    this->push_vertices(n_vertices);

    int u, v;
    for (int i = 0; i < n_edges; i++) {
        infile >> u >> v;
        if (!this->add_edge(u - 1, v - 1)) {
            std::string message = "Error: Could not add edge (" + std::to_string(u) + "," + std::to_string(v) + ")";
            throw std::invalid_argument(message);
        }
    }

    std::cout << "Success!\n";
}



// Graph algorithms
// dfs, bfs
std::pair <std::vector <int>, IntGraph> algorithm::search (IntGraph graph, bool depth_first) {
    int num_vertices = graph.num_vertices();
    std::vector <bool> visited = std::vector<bool>(num_vertices, false);
    std::vector <int> parent_idx = std::vector<int>(num_vertices, -1);
    std::vector <int> search_order;

    algorithm::_container_f cf;
    std::deque <int> container; // dfs: stack, bfs: queue
    if (depth_first) {
        cf.first = [](std::deque<int>& container) { return container.back(); };
        cf.pop_first = [](std::deque<int>& container) { container.pop_back(); };
    }
    else {
        cf.first = [](std::deque<int>& container) { return container.front(); };
        cf.pop_first = [](std::deque<int>& container) { container.pop_front(); };
    }

    for (int vertex = 0; vertex < num_vertices; vertex++) 
        if (!visited[vertex]) {
            container.push_back(vertex);
            while (!container.empty()) {
                // get first element from the container
                int v = cf.first(container);
                cf.pop_first(container);
                
                if (!visited[v]) {
                    visited[v] = true;
                    search_order.push_back(v);

                    // push adjacent vertices to the container
                    for (int adj_v : graph[v]) {
                        if (!visited[adj_v]) {
                            container.push_back(adj_v);
                            if (parent_idx[adj_v] == -1) 
                                parent_idx[adj_v] = v;
                        }
                    }
                }
            }
        }
            
    IntGraph search_tree(true);
    search_tree.push_vertices(num_vertices);
    for (int v = 0; v < num_vertices; v++) 
        search_tree.add_edge(parent_idx[v], v);

    return std::make_pair(search_order, search_tree);
}


// finding IntGraph's topological order or acyclicity
std::vector <int> algorithm::topological_sort (IntGraph graph) {
    if (!graph.is_directed()) 
        throw std::invalid_argument("Graph is NOT directed!");

    int num_vertices = graph.num_vertices();

    std::vector <int> in_deg(num_vertices);
    for (int v = 0; v < num_vertices; v++) 
        in_deg[v] = graph.in_deg(v);

    std::queue <int> src_queue; // indeices of source vertices
    for (int v = 0; v < num_vertices; v++)
        if (!in_deg[v])
            src_queue.push(v);

    std::vector <int> topological_order;
    while (!src_queue.empty()) {
        int src = src_queue.front();
        src_queue.pop();
        topological_order.push_back(src);

        for (int adj : graph[src]) {
            if (--in_deg[adj] == 0)
                src_queue.push(adj);
        }
    }

    if (topological_order.size() != num_vertices) 
        throw std::invalid_argument("Graph is NOT acyclic!");

    return topological_order;
}


// finding IntGraph's strongly connected componnents
void algorithm::_scc_unit (IntGraph &graph, algorithm::_scc_s &cs, std::stack<int> &dfs_stack) {
    // Tarjan's strongly connected components algorithm (iterative)
    while (!dfs_stack.empty()) {
        int v = dfs_stack.top();

        if (!cs.on_stack[v]) {
            cs.stack.push(v);
            cs.on_stack[v] = true;
        }

        if (cs.componnent_index[v] == -1) 
            cs.componnent_index[v] = cs.low_link[v] = cs.componnent++;

        for (int adj : graph[v]) {
            if (cs.componnent_index[adj] == -1) {
                dfs_stack.push(adj);
                break;
            }
            else if (cs.on_stack[adj])
                cs.low_link[v] = std::min(cs.low_link[v], cs.low_link[adj]);
        }

        // If there are still vertices adjacent to v on the (dfs)stack 
        if (dfs_stack.top()!= v) 
            continue;

        // if vertex is the root of a strongly connected componnent 
        // push stacked vertices to the new_componnent (until vertex is found)
        if (cs.low_link[v] == cs.componnent_index[v]) {
            std::vector <int> new_componnent;
            while (cs.on_stack[v]) {
                int w = cs.stack.top();
                cs.stack.pop();
                cs.on_stack[w] = false;
                new_componnent.push_back(w);
            }
            cs.scc.push_back(new_componnent);
        }

        dfs_stack.pop();
    }
}

std::vector <std::vector <int>> algorithm::scc (IntGraph graph) {
    int num_vertices = graph.num_vertices();
    algorithm::_scc_s cs = {
        .componnent_index = std::vector<int>(num_vertices, -1),
        .low_link = std::vector<int>(num_vertices, -1),
        .on_stack = std::vector<bool>(num_vertices, false),
        .stack = std::stack<int>(), 
        .componnent = 0,
        .scc = std::vector<std::vector<int>>()
    };

    std::stack <int> dfs_stack;

    for (int vertex = 0; vertex < num_vertices; vertex++) 
        if (cs.componnent_index[vertex] == -1) {
            dfs_stack.push(vertex);
            algorithm::_scc_unit(graph, cs, dfs_stack);
        }

    return cs.scc;
}


// checking if a IntGraph is bipartite
std::pair<std::vector <int>, std::vector <int>> algorithm::bipartite_partition (IntGraph graph) {
    // O(|V| + |E|) time complexity
    const int gray = 0; // not yet visited
    const int red = 1;

    int num_vertices = graph.num_vertices();
    std::vector <int> colors(num_vertices, gray); // 0: gray, 1: red, -1: blue
    std::queue <int> queue;
    
    for (int vertex = 0; vertex < num_vertices; vertex++) {
        if (colors[vertex] == gray) {
            colors[vertex] = red;
            queue.push(vertex);
            
            while (!queue.empty()) {
                int v = queue.front();
                queue.pop();

                for (int adj : graph[v]) {
                    if (colors[adj] == colors[v])
                        throw std::invalid_argument("Graph is NOT bipartite!");
                    
                    if (colors[adj] == gray) {
                        colors[adj] = -colors[v];
                        queue.push(adj);
                    }
                }
            }
        }
    }

    std::vector <int> red_vertices, blue_vertices;
    for (int v = 0; v < num_vertices; v++) {
        if (colors[v] == red)
            red_vertices.push_back(v);
        else 
            blue_vertices.push_back(v);
    }

    return std::make_pair(red_vertices, blue_vertices);
}