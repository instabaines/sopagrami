#include "alg.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <filesystem>

using namespace algo;
namespace fs = std::filesystem;

static std::string sanitize_dot(const std::string& s){
    std::string t; t.reserve(s.size()*2);
    for (char c: s){
        if (c=='"' || c=='\\') t.push_back('\\');
        t.push_back(c);
    }
    return t;
}

static void write_pattern_as_lg(const algo::Pattern& P, const std::string& path){
    std::ofstream out(path);
    if (!out) return;
    for (size_t i=0;i<P.vlab.size();++i) out << "v " << i << " " << P.vlab[i] << "\n";
    for (const auto& e : P.pedges)       out << "e " << e.a << " " << e.b << " " << e.el << "\n";
}

static void write_pattern_as_dot(const algo::Pattern& P, bool directed, const std::string& path){
    std::ofstream out(path);
    if (!out) return;
    out << (directed ? "digraph G {\n" : "graph G {\n");
    // nodes
    for (size_t i=0;i<P.vlab.size();++i){
        out << "  " << i << " [shape=circle,label=\"" << sanitize_dot(P.vlab[i]) << "\"];\n";
    }
    // edges
    for (const auto& e : P.pedges){
        const bool use_arrow = directed || e.dir==1;
        out << "  " << e.a << (use_arrow ? " -> " : " -- ") << e.b
            << " [label=\"" << sanitize_dot(e.el) << "\"];\n";
    }
    out << "}\n";
}

// ---- Dump for static miner output ----
static void dump_patterns_to_dir(const algo::Output& out,
                                 const std::string& dir,
                                 bool directed)
{
    fs::create_directories(dir);
    std::ofstream idx(dir + "/index.tsv");
    idx << "id\tk\tm\tfull_support\tkey\tlg_path\tdot_path\n";

    for (size_t i=0; i<out.frequent_patterns.size(); ++i){
        const auto& f = out.frequent_patterns[i];
        const size_t k = f.pat.vlab.size();
        const size_t m = f.pat.pedges.size();

        std::string base = dir + "/pat_" + std::to_string(i)
                         + "_k" + std::to_string(k)
                         + "_e" + std::to_string(m)
                         + "_full" + std::to_string(f.full_support);
        std::string lgp  = base + ".lg";
        std::string dotp = base + ".dot";

        write_pattern_as_lg (f.pat, lgp);
        write_pattern_as_dot(f.pat, directed, dotp);

        idx << i << '\t' << k << '\t' << m << '\t'
            << f.full_support << '\t' << f.pat.key()
            << '\t' << lgp << '\t' << dotp << "\n";
    }
}
int main(int argc, char** argv){
    // Usage:
    //   run <graph.lg> [tau] [directed(0/1)] [sorted(0/1)] [threads]
    //
    // Defaults:
    //   tau=2, directed=0, sorted=1 (SoGraMi ordering), threads=4
    if (argc < 2){
        std::cerr << "Usage: run <graph.lg> [tau] [directed(0/1)] [sorted(0/1)] [threads]\n";
        return 1;
    }

    const std::string path = argv[1];
    const int   tau      = (argc > 2 ? std::stoi(argv[2]) : 2);
    const bool  directed = (argc > 3 ? (std::stoi(argv[3]) != 0) : false);
    const bool  sorted   = (argc > 4 ? (std::stoi(argv[4]) != 0) : true);   // default: SoGraMi sorted
    const int   threads  = (argc > 5 ? std::stoi(argv[5]) : 4);             // default: 4

    DataGraph G;
    G.load_from_lg(path, directed);

    // Graph stats
    std::cout << "Graph loaded: |V|=" << G.vlabels.size() << ", |E|=";
    long long edge_count = 0;
    for (const auto& adj_list : G.adj) edge_count += (long long)adj_list.size();
    if (!directed) edge_count /= 2;
    std::cout << edge_count << "\n";

    // Params
    Params p;
    p.tau = tau;
    p.directed = directed;
    p.sorted_seeds = sorted;     // SoGraMi ordering toggle
    p.num_threads = threads;     // run_sopagrami  <=0 will default to all available
    p.compute_full_support = true;

    std::cout << "Settings: tau=" << p.tau
              << " directed=" << (p.directed?1:0)
              << " sorted=" << (p.sorted_seeds?1:0)
              << " threads=" << p.num_threads
              << "\n\n";

    // Run
    auto out = run_sopagrami(G, p);

    // Output
    std::cout << "Frequent patterns: " << out.frequent_patterns.size() << "\n";
    for (const auto& f : out.frequent_patterns){
        std::cout << "k=" << f.pat.vlab.size()
                  << " |E|=" << f.pat.pedges.size()
                  << " full=" << f.full_support
                  << " key=" << f.pat.key() << "\n";
    }
    //dump patterns to dir
    std::string dump_dir = (argc > 6 ? argv[6] : "");
    if (!dump_dir.empty()){
        dump_patterns_to_dir(out, dump_dir, p.directed);
        std::cout << "Wrote pattern files to: " << dump_dir << "/ (index.tsv, .lg, .dot)\n";
    }

    return 0;
}
