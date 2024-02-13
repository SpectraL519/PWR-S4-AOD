#include <iostream>
#include <fstream>
#include <chrono>
#include <tuple>
#include <cmath>
#include <optional>

#include <argparse.hpp>
#include <graph.hpp>



namespace {
    void gen_lp_script (
        graph::graph& g, 
        const std::string file_name, 
        const bool gen_matchings
    ) {
        std::ofstream file(file_name);
        if (!file.is_open()) {
            std::cerr << "Error: cannot open file - " << file_name << std::endl;
            return;
        }

        std::size_t n = g.num_vertices();

        file << "import LinearAlgebra" << std::endl;
        file << "using JuMP" << std::endl;
        file << "using GLPK" << std::endl;

        file << std::endl << "model = Model(GLPK.Optimizer)" << std::endl;

        file << std::endl << "# x[u, v] - flow from vertex u to vertex v" << std::endl;
        file << "# vertices indexed 1 to 2 ^ k" << std::endl;
        file << "n = " << n << std::endl;
        file << "@variable(model, x[1:" << n << ", 1:" << n << "] >= 0, Int)" << std::endl;

        file << std::endl << "# capacities of given arcs" << std::endl;
        file << "@constraints(model, begin" << std::endl;
        for (std::size_t u = 0; u < n; u++) {
            for (const graph::edge_descriptor& edge : g[u]) 
                file << "\tx[" << u + 1 << ", " << edge.destination + 1 << "] <= " << edge.capacity << std::endl;
                
            for (std::size_t v = 0; v < n; v++) {
                if (!g.has_edge(u, v)) 
                    file << "\tx[" << u + 1 << ", " << v + 1 << "] == 0" << std::endl;
            }
        } 
        file << "end)" << std::endl;   

        file << std::endl << "# flow must be balanced" << std::endl;
        file << "@constraint(model, [u = 2:" << n - 1 << "], sum(x[u, :]) == sum(x[:, u]))" << std::endl;
        file << "@constraint(model, sum(x[1, :])  == sum(x[:, " << n << "]))" << std::endl;
        
        file << std::endl << "# maximize the flow" << std::endl;
        file << "@objective(model, Max, sum(x[1, :]))" << std::endl;
        file << "optimize!(model)" << std::endl;

        file << std::endl << "println(Int64(objective_value(model)))" << std::endl;
        file << "println(solution_summary(model))" << std::endl;

        file << std::endl << "println(\"maximum flow = \", Int32(value(sum(x[1, :]))))" << std::endl;
        file << std::endl << "for u in 1:n" << std::endl;
        file << "\tfor v in 1:n" << std::endl;
        file << "\t\tif value(x[u, v]) > 0" << std::endl;
        file << "\t\t\tprintln(u - 1, \" - \", v - 1, \": \", Int64(value(x[u, v])))" << std::endl;
        file << "\t\tend" << std::endl << "\tend" << std::endl << "end" << std::endl;

        if (gen_matchings) {
            file << std::endl << "matchings = Vector{Pair}()" << std::endl;
            file << "for u in 2:" << n - 1 << std::endl;
            file << "\tfor v in (u + 1):" << n - 1 << std::endl;
            file << "\t\tif value(x[u, v]) > 0" << std::endl;
            file << "\t\t\tpush!(matchings, Pair(u, v))" << std::endl;
            file << "\t\tend" << std::endl << "\tend" << std::endl << "end" << std::endl;

            file << std::endl << "println(\"maximum matching = \", length(matchings))" << std::endl;
            file << "for matching in matchings" << std::endl;
            file << "\tu, v = matching" << std::endl;
            file << "\tprintln(u - 1, \" -> \", v - 1)" << std::endl;
            file << "end" << std::endl;
        }

        file.close();
    }



    void flow (
        const std::size_t size, 
        const bool print_detail,
        const std::optional <std::string> result,
        const std::optional <std::string> glpk
    ) {
        auto start = std::chrono::high_resolution_clock::now();

        graph::graph g;
        g.build_hipercube(size);

        const int32_t source = 0;
        const int32_t sink = (1 << size) - 1;
        auto [max_flow, augmenting_paths] = g.edmonds_karp_max_flow(source, sink);

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        float time = (float)duration.count() / 1000.0;

        std::cerr << "execution time = " << time << " (ms)" << std::endl;

        std::cout << std::endl << "source = " << source << ", sink = " << sink << std::endl;
        std::cout << "maximum flow = " << max_flow << std::endl;
        std::cerr << "augmenting paths = " << augmenting_paths << std::endl;

        if (print_detail) {
            std::cout << std::endl;
            g.show();
            std::cout << std::endl;
        }

        // print results into a result file
        if (result) {
            std::ofstream file(result.value(), std::ios_base::app);
            if (!file.is_open()) {
                std::cerr << "Error: cannot open file - " << result.value() << std::endl;
                return;
            }

            file << size << "," << max_flow << "," << augmenting_paths << "," << time << std::endl;

            file.close();
        }

        // generate linear programming model code
        if (glpk) 
            gen_lp_script(g, glpk.value(), false);
    }

    void dinic_flow (
        const std::size_t size, 
        const bool print_detail,
        const std::optional <std::string> result,
        const std::optional <std::string> glpk
    ) {
        auto start = std::chrono::high_resolution_clock::now();

        graph::graph g;
        g.build_hipercube(size);

        const int32_t source = 0;
        const int32_t sink = (1 << size) - 1;
        auto [max_flow, augmenting_paths] = g.dinic_max_flow(source, sink);

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        float time = (float)duration.count() / 1000.0;

        std::cerr << "execution time = " << time << " (ms)" << std::endl;

        std::cout << std::endl << "source = " << source << ", sink = " << sink << std::endl;
        std::cout << "maximum flow = " << max_flow << std::endl;
        std::cerr << "augmenting paths = " << augmenting_paths << std::endl;

        if (print_detail) {
            std::cout << std::endl;
            g.show();
            std::cout << std::endl;
        }

        // print results into a result file
        if (result) {
            std::ofstream file(result.value(), std::ios_base::app);
            if (!file.is_open()) {
                std::cerr << "Error: cannot open file - " << result.value() << std::endl;
                return;
            }

            file << size << "," << max_flow << "," << augmenting_paths << "," << time << std::endl;

            file.close();
        }


        // generate linear programming model code
        if (glpk) 
            gen_lp_script(g, glpk.value(), false);
    }

    void matchings (
        const std::size_t size, 
        const std::size_t degree, 
        const bool print_detail,
        const std::optional <std::string> result,
        const std::optional <std::string> glpk
    ) {
        auto start = std::chrono::high_resolution_clock::now();

        graph::graph g;
        g.build_bipartite(size, degree);

        const int32_t source = 0;
        const int32_t sink = g.num_vertices() - 1;

        auto [max_flow, augmenting_paths] = g.dinic_max_flow(0, sink);

        std::vector <std::pair <int32_t, int32_t>> matchings = g.max_flow_matchings(source, sink);

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        float time = (float)duration.count() / 1000.0;

        std::cerr << "execution time = " << time << " (ms)" << std::endl;

        std::cout << std::endl << "source = " << source << ", sink = " << sink << std::endl;
        std::cout << "maximum flow = " << max_flow << std::endl;
        std::cerr << "augmenting paths = " << augmenting_paths << std::endl;

        if (print_detail) {
            std::cout << std::endl;
            g.show();
            std::cout << std::endl;
        }

        std::cout << std::endl << "maximum matching size = " << matchings.size() << std::endl;
        if (print_detail && matchings.size() > 0) {
            for (const std::pair <int32_t, int32_t> matching : matchings)
                std::cout << matching.first << " -> " << matching.second << std::endl;
        }

        // print results into a result file
        if (result) {
            std::ofstream file(result.value(), std::ios_base::app);
            if (!file.is_open()) {
                std::cerr << "Error: cannot open file - " << result.value() << std::endl;
                return;
            }

            file << size << "," << degree << "," << max_flow << "," << augmenting_paths << "," << matchings.size() << "," << time << std::endl;

            file.close();
        }


        // generate linear programming model code
        if (glpk) 
            gen_lp_script(g, glpk.value(), true);
    }
}



int main (int argc, char* argv[]) {
    // specify the program args
    argparse::ArgumentParser parser("maximum flow");
    parser
        .add_argument("-p", "--problem")
        .default_value("flow")
        .action([] (const std::string& value) {
            static const std::vector <std::string> choices = { "flow", "dinic_flow", "matchings" };
            if (std::find(choices.begin(), choices.end(), value) != choices.end())
                return value;
            return std::string("flow");
        })
        .help("problem type");
    parser
        .add_argument("-p", "--print-detail")
        .default_value(false)
        .implicit_value(true)
        .help("prints the detailed output of the problem computation to the terminal");
    parser
        .add_argument("-s", "--size")
        .scan<'i', std::size_t>()
        .help("graph size specifier");
    parser
        .add_argument("-d", "--degree")
        .scan<'i', std::size_t>()
        .help("degree of each vertex in the matching problem");
    parser
        .add_argument("--save")
        .help("optional - path to `.csv` file to which the results will be saaved");
    parser
        .add_argument("--glpk")
        .help("optional - path to a `.jl` file to which a linear programming model code will be generated");

    // parse args
    try {
        parser.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << parser;
        std::exit(1);
    }

    // process args
    try {
        const std::string problem = parser.get("problem");
        const std::size_t size = parser.get<std::size_t>("size");
        const bool print_detail = parser.get<bool>("print-detail");

        std::optional <std::string> save = std::nullopt;
        if (parser.present("save"))
            save = parser.get("save");

        std::optional <std::string> glpk = std::nullopt;
        if (parser.present("glpk"))
            glpk = parser.get("glpk");

        if (problem == "flow") {
            flow(size, print_detail, save, glpk);
        }
        else if (problem == "dinic_flow") {
            dinic_flow(size, print_detail, save, glpk);
        }
        else {
            const std::size_t degree = parser.get<std::size_t>("degree");
            matchings(size, degree, print_detail, save, glpk);
        }
    }
    catch (const std::logic_error& err) {
        std::cerr << err.what() << std::endl;
        std::exit(1);
    }

    return 0;
}