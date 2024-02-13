#pragma once

#include <fstream>
#include <optional>

#include "argparse.hpp"
#include "graph.hpp"
#include "types.hpp"



std::optional<data_t> parse_input(const int argc, const char *const argv[]) {
    argparse::ArgumentParser parser("shortest paths");
    parser.add_argument("-d").help("graph specification file path");
    parser.add_argument("-ss").help("sources file path");
    parser.add_argument("-oss").help("shortest path problem with one source result file path");
    parser.add_argument("-p2p").help("p2p problem (pairs of vertices) file path");
    parser.add_argument("-op2p").help("p2p problem (pairs of vertices) result file path"); 

    try {
        parser.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << parser;
        return std::nullopt;
    }

    std::string graph_file_name;
    problem_t problem;
    std::string problem_file_name;
    std::string output_file_name;

    try {
        graph_file_name = parser.get("-d");

        if (parser.present("-ss")) {
            problem = problem_t::ss;
            problem_file_name = parser.get("-ss");
            output_file_name = parser.get("-oss");
        }
        else if (parser.present("-p2p")) {
            problem = problem_t::p2p;
            problem_file_name = parser.get("-p2p");
            output_file_name = parser.get("-op2p");
        }
        else 
            throw std::logic_error("Error: missing arguments: required [-ss, -oss] or [-p2p, -op2p]");
    }
    catch (const std::logic_error& err) {
        std::cerr << err.what() << std::endl;
        return std::nullopt;
    }


    std::string line, dummy;
    std::stringstream ss;


    // parse graph file
    std::ifstream graph_file(graph_file_name);
    if (!graph_file) {
        std::cerr << "Error: Invalid file name: " + graph_file_name << std::endl;
        return std::nullopt;
    }

    graph::int_graph graph;
    int32_t min_weight = INT32_MAX, max_weight = INT32_MIN;
    std::size_t num_vertices, num_edges = INT32_MAX, edge = 0;

    while (std::getline(graph_file, line) && edge < num_edges) {
        switch (line[0]) {
            case 'p': {
                ss << line;
                ss >> dummy >> dummy >> num_vertices >> num_edges;
                graph = graph::int_graph(num_vertices);
                std::stringstream().swap(ss); // clear buffer
                break;
            }

            case 'a': {
                std::size_t u, v;
                int32_t weight;
                ss << line;
                ss >> dummy >> u >> v >> weight;
                graph.add_edge(u - 1, v - 1, weight);
                edge++;

                std::stringstream().swap(ss); // clear buffer
                break;
            }
        }
    }
    graph_file.close();

    // parse problem file
    std::ifstream problem_file(problem_file_name);
    if (!problem_file) {
        std::cerr << "Error: Invalid file name: " + problem_file_name << std::endl;
        return std::nullopt;
    }

    std::optional <ss_t> ss_opt = std::nullopt;
    std::optional <p2p_t> p2p_opt = std::nullopt;
    
    if (problem == problem_t::ss) {
        std::size_t num_sources = INT32_MAX;
        ss_t ss_;

        while (std::getline(problem_file, line) && ss_.sources.size() < num_sources) {
            switch (line[0]) {
                case 'p': {
                    ss << line;
                    ss >> dummy >> dummy >> dummy >> dummy >> num_sources;
                    std::stringstream().swap(ss); // clear buffer
                    break;
                }

                case 's': {
                    std::size_t src;
                    ss << line;
                    ss >> dummy >> src;
                    ss_.sources.push_back(src - 1);
                    std::stringstream().swap(ss); // clear buffer
                    break;
                }
            }
        }
        ss_opt = ss_;
    }
    else {
        std::size_t num_pairs = INT32_MAX;
        p2p_t p2p_;

        while (std::getline(problem_file, line) && p2p_.pairs.size() < num_pairs) {
            switch (line[0]) {
                case 'p': {
                    ss << line;
                    ss >> dummy >> dummy >> dummy >> dummy >> num_pairs;
                    std::stringstream().swap(ss); // clear buffer
                    break;
                }

                case 'q': {
                    std::size_t u, v;
                    ss << line;
                    ss >> dummy >> u >> v;
                    p2p_.pairs.push_back(std::make_pair(u - 1, v - 1));
                    std::stringstream().swap(ss); // clear buffer
                    break;
                }
            }
        }
        p2p_opt = p2p_;
    }
    problem_file.close();


    return data_t {
        .graph = graph,
        .num_edges = num_edges,

        .problem = problem,
        .ss = ss_opt,
        .p2p = p2p_opt,

        .graph_file_name = graph_file_name,
        .problem_file_name = problem_file_name,
        .out_file_name = output_file_name
    };
}