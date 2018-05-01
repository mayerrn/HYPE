#include <Hypergraph.hpp>
#include <Parsing.hpp>
#include <Partition.hpp>
#include <Partitioning.hpp>
#include <SSet.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <string>


auto main(int argc, char const* argv[])
    -> int
{
    namespace po = boost::program_options;

    po::options_description description{"Edgelist converter usage"};


    // clang-format off
    description.add_options()
        ("help,h", "display this help message")

        ("raw,r",
         po::bool_switch()->default_value(false),
         "output raw numbers to make it easier to redirect output into files")

        ("input,i",
         po::value<std::string>(),
         "input hypergraph file")

        ("format,f",
         po::value<part::ParsingMode>()->default_value(part::ParsingMode::EdgeList),
         "specify the input format of the hypergraph file")

        ("partitions,p",
         po::value<std::size_t>(),
         "number of partitions")

        ("sset-size,s",
         po::value<std::size_t>()->default_value(10),
         "maximum size of the secondary set")

        ("nh-expand-candidates,n",
         po::value<std::size_t>()->default_value(2),
         "number of candidates explored during neighbouhood expantion")

        ("percent-of-edges-ignored,e",
         po::value<double>()->default_value(0),
         "how many percent of the biggest edges will be removed")

        ("heuristic-calc-method,c",
         po::value<part::NodeHeuristicMode>()->default_value(part::NodeHeuristicMode::Cached),
         "Switch to choose between exact and cached calculation for the node heuristic");
    // clang-format on

    po::variables_map vm{};
    po::store(po::command_line_parser(argc, argv).options(description).run(), vm);
    po::notify(vm);

    if(!vm.count("input")
       || !vm.count("partitions")
       || !vm.count("format")
       || vm.count("help")) {

        std::cout << description;
        return 0;
    }

    auto input_path = vm["input"].as<std::string>();
    auto format = vm["format"].as<part::ParsingMode>();
    auto partitions = vm["partitions"].as<std::size_t>();
    auto ssize = vm["sset-size"].as<std::size_t>();
    auto percent = vm["percent-of-edges-ignored"].as<double>();
    auto numb_of_neigs_flag = vm["heuristic-calc-method"].as<part::NodeHeuristicMode>();
    auto raw = vm["thesis"].as<bool>();
    auto numb_of_can = vm["nh-expand-candidates"].as<std::size_t>();



    if(!raw) {
        std::cout << "----------------------------------------------------------------------------\n"
                  << "Partitioning Graph: "
                  << input_path
                  << "\n"
                  << "into "
                  << partitions
                  << " partitions\n"
                  << "max secondary set size: "
                  << ssize
                  << "\n"
                  << "while the secondary set expantion, the biggest "
                  << percent
                  << "% of edges will be ignored\n"
                  << "the neighbourhood heuristic of a node will be "
                  << (numb_of_neigs_flag == part::NodeHeuristicMode::Exact ? "exact" : "estimated")
                  << "\n"
                  << "----------------------------------------------------------------------------\n";

        std::cout << "parsing graph ...\n";
    }

    auto begin = std::chrono::steady_clock::now();

    auto graph = part::parseFileIntoHypergraph(input_path, format);
    auto number_of_nodes = graph.getVertices().size();
    auto number_of_edges = graph.getEdges().size();

    auto end = std::chrono::steady_clock::now();
    auto parsing_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
            .count();

    if(!raw) {
        std::cout << "graph parsed in "
                  << parsing_time
                  << " milliseconds\n"
                  << "#Nodes:\t"
                  << number_of_nodes
                  << "\n"
                  << "#HyperEdges:\t"
                  << number_of_edges
                  << "\n"
                  << "----------------------------------------------------------------------------\n";

        std::cout << "partitioning graph\n";
    }

    begin = std::chrono::steady_clock::now();
    auto parts = part::partitionGraph(std::move(graph),
                                      partitions,
                                      ssize,
                                      numb_of_can,
                                      percent,
                                      numb_of_neigs_flag);
    end = std::chrono::steady_clock::now();

    auto partitioning_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
            .count();

    //start parallel futures
    auto soed_fut = part::getSumOfExteralDegrees(parts);
    auto vtx_balance_fut = part::getVertexBalancing(parts);
    auto edge_balance_fut = part::getEdgeBalancing(parts);
    auto edge_cut_fut = part::getHyperedgeCut(parts);
    auto k_minus_1_fut = part::getKminus1Metric(parts, number_of_edges);

    //wait for all results
    auto soed = soed_fut.get();
    auto vtx_balance = vtx_balance_fut.get();
    auto edge_balance = edge_balance_fut.get();
    auto edge_cut = edge_cut_fut.get();
    auto k_minus_1 = k_minus_1_fut.get();

    if(raw) {
        std::cout << partitions
                  << "\t\t"
                  << soed
                  << "\t\t"
                  << vtx_balance
                  << "\t\t"
                  << edge_balance
                  << "\t\t"
                  << edge_cut
                  << "\t\t"
                  << k_minus_1
                  << "\t\t"
                  << (parsing_time + partitioning_time)
                  << std::endl;

    } else {
        std::cout << "partitioning done in "
                  << partitioning_time
                  << " milliseconds\n"
                  << "----------------------------------------------------------------------------\n"
                  << "sum of external degrees: " << soed << "\n"
                  << "Hyperedges cut: " << edge_cut << "\n"
                  << "K-1: " << k_minus_1 << "\n"
                  << "node balancing: " << vtx_balance << "\n"
                  << "edge balancing: " << edge_balance << "\n"
                  << "parsing time: " << parsing_time << "\n"
                  << "partition time: " << partitioning_time << "\n"
                  << "total time: " << (parsing_time + partitioning_time)
                  << std::endl;
    }

    return 0;
}
