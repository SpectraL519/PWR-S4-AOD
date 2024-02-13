#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <optional>

#include "graph.hpp"



enum class problem_t {
    ss, p2p    
};

struct ss_t {
    std::vector <std::size_t> sources;
};

struct p2p_t {
    std::vector <std::pair <std::size_t, std::size_t>> pairs;
};

struct data_t {
    graph::int_graph graph;
    std::size_t num_edges;

    problem_t problem;
    std::optional <ss_t> ss;
    std::optional <p2p_t> p2p;

    std::string graph_file_name;
    std::string problem_file_name;
    std::string out_file_name;
};