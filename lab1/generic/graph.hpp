#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <queue>
#include <stack>
#include <algorithm>
#include <cmath>
#include <functional>
#include <stdexcept>





// Declarations 
namespace graph {
    template <typename T>
    struct graph_t {
        typedef std::pair <T, T> edge;
        struct vertex_descriptor {
            std::vector <T> adjacent_in;
            std::vector <T> adjacent_out;
            // TODO: vector<T> -> vector<edge_descriptor<T>>
        };
        typedef std::vector <std::vector <T>> partition;
    };



    template <typename T>
    class Graph {
        private:
            bool directed;
            std::vector <T> vertices;
            std::vector <typename graph_t<int>::vertex_descriptor> adjacency_list;

        public:
            Graph() = default;
            Graph (bool directed);
            ~Graph() = default;

            void show();
            bool is_directed();
            bool is_empty();
            int num_vertices();
            std::vector <T> get_vertices();
            int index_of (T vertex);
            T& operator [] (int index); // returns vertex 'name'
            std::vector <int> adjacent_in (int index); // returns vector of indices
            std::vector <int> adjacent_out (int index); // returns vector of indices
            int in_deg (int index);
            int out_deg (int index);
            void add_vertex (T vertex);
            void add_edge (typename graph_t<T>::edge edge);
    };



    namespace algorithm {
        // `_` prefixed members should be considered private

        // dfs, bfs
        template <typename T>
        struct _container_f {
            // stack and queue container operation functions
            // vertex indices containers
            std::function <T(std::deque<T>&)> first;
            std::function <void(std::deque<T>&)> pop_first;
        };

        template <typename T>
        Graph <T> search (Graph <T> graph, bool depth_first);


        // finding graph's topological order or acyclicity
        template <typename T>
        std::vector <T> topological_sort (Graph <T> graph);


        // finding graph's strongly connected componnents
        template <typename T>
        struct _scc_s {
            // structures required for the strong connecting algorithm
            std::vector <int> componnent_index;
            std::vector <int> low_link;
            std::vector <bool> on_stack;
            std::stack <int> stack;
            int componnent;
            graph_t<T>::partition scc;
        };

        template <typename T>
        void _scc_unit (Graph <T> graph, _scc_s <T> &cs, std::stack <int> &dfs_stack);

        template <typename T>
        graph_t<T>::partition scc (Graph <T> graph);


        // checking if a graph is bipartite
        template <typename T>
        graph_t<T>::partition bipartite_partition (Graph <T> graph);
    };



    // Graph utils (reading from file)
    /*
    Reading a graph with vertices of type <int>
    File structure:
    [D|U] - dorected / undirected
    n - number of vertices (the graph will have vertices numbered [1 ... n])
    m - number of edges
    list of m edges in a "u v" format
    */
    Graph <int> int_graph_from_file (std::string file_name) {
        try {
            std::cout << "Reading data...\n";
            std::ifstream infile;
            infile.open(file_name);

            Graph <int> graph;
            char g_type;

            infile >> g_type;
            switch (g_type) {
                case 'D': {
                    graph = Graph<int>(true);
                    break;
                }
                case 'U': {
                    graph = Graph<int>(false);
                    break;
                }
                default: {
                    std::string message = "Invalid graph type (" + std::to_string(g_type) + ") - must be 'U' or 'D'";
                    throw std::invalid_argument(message);
                }
            }
            
            int n_vertices, n_edges;
            infile >> n_vertices >> n_edges;
            for (int i = 0; i < n_vertices; i++) 
                graph.add_vertex(i + 1);

            int v, u;
            for (int i = 0; i < n_edges; i++) {
                infile >> v >> u;
                graph.add_edge(std::make_pair(v, u));
            }

            std::cout << "Success!\n";
            return graph;
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
    }
}

// Definitions
using namespace graph;

// Graph container
template <typename T>
Graph<T>::Graph (bool directed) {
    this->directed = directed;
}

template <typename T>
void Graph<T>::show() {
    int num_vertices = this->vertices.size();
    for (int v_idx = 0; v_idx < num_vertices; v_idx++) {
        std::cout << this->vertices[v_idx] << ": ";
        for (int u_idx : this->adjacent_out(v_idx)) 
            std::cout << this->vertices[u_idx] << " ";
        std::cout << "\n";
    }
}

template <typename T>
bool Graph<T>::is_directed () {
    return this->directed;
}

template <typename T>
bool Graph<T>::is_empty () {
    return this->vertices.empty();
}

template <typename T>
int Graph<T>::num_vertices () {
    return this->vertices.size();
}

template <typename T>
std::vector <T> Graph<T>::get_vertices () {
    return this->vertices;
}

template <typename T>
int Graph<T>::index_of (T vertex) {
    std::vector<int>::iterator it = std::find(this->vertices.begin(), this->vertices.end(), vertex);
    return (it - this->vertices.begin());
}

template <typename T>
T& Graph<T>::operator[] (int index) {
    return this->vertices[index];
}

template <typename T>
std::vector <int> Graph<T>::adjacent_in (int index) {
    return this->adjacency_list[index].adjacent_in;
}

template <typename T>
std::vector <int> Graph<T>::adjacent_out (int index) {
    return this->adjacency_list[index].adjacent_out;
}

template <typename T>
int Graph<T>::in_deg (int index) {
    return this->adjacency_list[index].adjacent_in.size();
}

template <typename T>
int Graph<T>::out_deg (int index) {
    return this->adjacency_list[index].adjacent_out.size();
}

template <typename T>
void Graph<T>::add_vertex (T vertex) {
    if (this->index_of(vertex) == this->vertices.size()) {
        this->vertices.push_back(vertex);
        this->adjacency_list.push_back(typename graph_t<T>::vertex_descriptor{});
    }
}

template <typename T>
void Graph<T>::add_edge (typename graph_t<T>::edge edge) {
    try {
        int first_idx = this->index_of(edge.first);
        int second_idx = this->index_of(edge.second);

        this->adjacency_list[first_idx].adjacent_out.push_back(second_idx);
        this->adjacency_list[second_idx].adjacent_in.push_back(first_idx);
        if (!this->directed) {
            this->adjacency_list[second_idx].adjacent_out.push_back(first_idx);
            this->adjacency_list[first_idx].adjacent_in.push_back(second_idx);
        }
    }
    catch (std::exception &e) {
        std::cout << "Error: Cannot add edge!\n" << e.what();
        std::exit(1);
    }
}



// Graph algorithms
// dfs, bfs
template <typename T>
Graph <T> algorithm::search (Graph <T> graph, bool depth_first) {
    // TODO: vertices stack -> vertex indices stack
    int num_vertices = graph.num_vertices();
    std::vector <bool> visited = std::vector<bool>(num_vertices, false);
    std::vector <int> parent_idx = std::vector<int>(num_vertices, -1);
    Graph <T> search_tree(true);

    std::deque <int> container; // dfs: stack, bfs: queue - index container
    algorithm::_container_f <int> cf;
    if (depth_first) {
        cf.first = [](std::deque<int>& container) { return container.back(); };
        cf.pop_first = [](std::deque<int>& container) { container.pop_back(); };
    }
    else {
        cf.first = [](std::deque<int>& container) { return container.front(); };
        cf.pop_first = [](std::deque<int>& container) { container.pop_front(); };
    }

    for (int vertex_idx = 0; vertex_idx < num_vertices; vertex_idx++) 
        if (!visited[vertex_idx]) {
            container.push_back(vertex_idx);

            while (!container.empty()) {
                // get first element from the container
                int v_idx = cf.first(container);
                cf.pop_first(container);
                
                if (!visited[v_idx]) {
                    visited[v_idx] = true;
                    search_tree.add_vertex(graph[v_idx]);

                    // push adjacent vertices to the container
                    for (int adj_idx : graph.adjacent_out(v_idx)) {
                        if (!visited[adj_idx]) {
                            container.push_back(adj_idx);
                            if (parent_idx[adj_idx] == -1) 
                                parent_idx[adj_idx] = v_idx;
                        }
                    }
                }
            }
        }
            

    for (int v = 0; v < num_vertices; v++) 
        if (parent_idx[v] != -1) 
            search_tree.add_edge(std::make_pair(graph[parent_idx[v]], graph[v]));

    return search_tree;
}


// finding graph's topological order or acyclicity
template <typename T>
std::vector <T> algorithm::topological_sort (Graph <T> graph) {
    if (!graph.is_directed()) 
        throw std::invalid_argument("Graph is NOT directed!");

    int num_vertices = graph.num_vertices();

    std::vector <int> in_deg(num_vertices);
    for (int v_idx = 0; v_idx < num_vertices; v_idx++) 
        in_deg[v_idx] = graph.in_deg(v_idx);

    std::queue <int> src_queue; // indices of source vertices
    for (int v_idx = 0; v_idx < num_vertices; v_idx++)
        if (!in_deg[v_idx])
            src_queue.push(v_idx);

    std::vector <T> topological_order;
    while (!src_queue.empty()) {
        int src_idx = src_queue.front();
        src_queue.pop();
        topological_order.push_back(graph[src_idx]);

        for (int adj_idx : graph.adjacent_out(src_idx)) {
            if (--in_deg[adj_idx] == 0)
                src_queue.push(adj_idx);
        }
    }

    if (topological_order.size() != num_vertices) 
        throw std::invalid_argument("Graph is NOT acyclic!");

    return topological_order;
}


// finding graph's strongly connected componnents
template <typename T>
void algorithm::_scc_unit (Graph <T> graph, algorithm::_scc_s <T> &cs, std::stack <int> &dfs_stack) {
    // Tarjan's strongly connected components algorithm (iterative)
    while (!dfs_stack.empty()) {
        int v_idx = dfs_stack.top();

        if (!cs.on_stack[v_idx]) {
            cs.stack.push(v_idx);
            cs.on_stack[v_idx] = true;
        }

        if (cs.componnent_index[v_idx] == -1) 
            cs.componnent_index[v_idx] = cs.low_link[v_idx] = cs.componnent++;

        for (int adj_idx : graph.adjacent_out(v_idx)) {
            if (cs.componnent_index[adj_idx] == -1) {
                dfs_stack.push(adj_idx);
                break;
            }
            else if (cs.on_stack[adj_idx])
                cs.low_link[v_idx] = std::min(cs.low_link[v_idx], cs.low_link[adj_idx]);
        }

        // If there are still vertices adjacent to v on the (dfs)stack 
        if (dfs_stack.top()!= v_idx) 
            continue;

        // if vertex is the root of a strongly connected componnent 
        // push stacked vertices to the new_componnent (until vertex is found)
        if (cs.low_link[v_idx] == cs.componnent_index[v_idx]) {
            std::vector <T> new_componnent;
            while (cs.on_stack[v_idx]) {
                int w_idx = cs.stack.top();
                cs.stack.pop();
                cs.on_stack[w_idx] = false;
                new_componnent.push_back(graph[w_idx]);
            }
            cs.scc.push_back(new_componnent);
        }

        dfs_stack.pop();
    }
}

template <typename T>
graph_t<T>::partition algorithm::scc (Graph <T> graph) {
    int num_vertices = graph.num_vertices();
    algorithm::_scc_s <T> cs = {
        .componnent_index = std::vector<int>(num_vertices, -1),
        .low_link = std::vector<int>(num_vertices, -1),
        .on_stack = std::vector<bool>(num_vertices, false),
        .stack = std::stack<int>(), 
        .componnent = 0,
        .scc = typename graph_t<T>::partition()
    };

    std::stack <int> dfs_stack; 

    for (int v_idx = 0; v_idx < num_vertices; v_idx++) 
        if (cs.componnent_index[v_idx] == -1) {
            dfs_stack.push(graph[v_idx]);
            algorithm::_scc_unit(graph, cs, dfs_stack);
        }

    return cs.scc;
}


// checking if a graph is bipartite
template <typename T>
graph_t<T>::partition algorithm::bipartite_partition (Graph <T> graph) {
    // O(|V| + |E|) time complexity
    const int gray = 0; // not yet visited
    const int red = 1;

    int num_vertices = graph.num_vertices();
    std::vector <int> colors(num_vertices, gray); // 0: gray, 1: red, -1: blue
    std::queue <int> queue;

    for (int vertex_idx = 0; vertex_idx < num_vertices; vertex_idx++) {
        if (colors[vertex_idx] == gray) {
            colors[vertex_idx] = red;
            queue.push(vertex_idx);

            while (!queue.empty()) {
                int v_idx = queue.front();
                queue.pop();

                for (int adj_idx : graph.adjacent_out(v_idx)) {
                    if (colors[adj_idx] == colors[v_idx])
                        throw std::invalid_argument("Graph is NOT bipartite!");
                    
                    if (colors[adj_idx] == gray) {
                        colors[adj_idx] = -colors[v_idx];
                        queue.push(adj_idx);
                    }
                }
            }
        }
    }

    std::vector <T> red_vertices, blue_vertices;
    for (int v_idx = 0; v_idx < num_vertices; v_idx++) {
        if (colors[v_idx] == red)
            red_vertices.push_back(graph[v_idx]);
        else 
            blue_vertices.push_back(graph[v_idx]);
    }

    typename graph_t<T>::partition bp;
    bp.push_back(red_vertices);
    bp.push_back(blue_vertices);
    return bp;
}