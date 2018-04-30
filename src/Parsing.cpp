#include <Parsing.hpp>
#include <Hypergraph.hpp>
#include <algorithm>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <fstream>
#include <iostream>
#include <vector>


namespace {

//generates a parser for the edgelist format
//and filling the given graph with it
auto generate_edgelist_graph_parser(part::Hypergraph& graph)
{
    namespace x3 = boost::spirit::x3;
    namespace fusion = boost::fusion;

    auto parsing_function = [&graph](auto&& ctx) {
        std::vector<uint64_t> edge_list;
        uint64_t vtx;
        auto tup = std::tie(vtx, edge_list);

        fusion::move(std::move(x3::_attr(std::move(ctx))), tup);

        graph.addEdgeList(vtx, edge_list);
    };

    auto line = (x3::uint64 >> ':') > (x3::uint64 % ',');
    auto empty_node = x3::uint64;

    return +(line[parsing_function] | empty_node);
};

//generates a parser for the hmetis format
//and filling the given graph with it
auto generate_hmetis_graph_parser(part::Hypergraph& graph)
{
    namespace x3 = boost::spirit::x3;
    namespace fusion = boost::fusion;

    auto parsing_function = [&graph](auto&& ctx) {
        std::vector<uint64_t> vtx_list;
        uint64_t edge;
        auto tup = std::tie(edge, vtx_list);

        fusion::move(std::move(x3::_attr(std::move(ctx))), tup);

        graph.addNodeList(edge, vtx_list);
    };

    auto line = x3::uint64 > +x3::uint64 > x3::eol;
    auto empty_edge = x3::uint64 > x3::eol;

    return x3::uint64
        > x3::uint64
        > x3::eol // parse the first two numbers of the
        //hmetis file and do nothing with it
        > +(line[parsing_function]
            | empty_edge);
}

//generates a parser parsing bipartite graphs
//and filling the given hypergraph with it
auto generate_bipartite_graph_parser(part::Hypergraph& graph)
{
    namespace x3 = boost::spirit::x3;
    namespace fusion = boost::fusion;

    auto parsing_function = [&graph](auto&& ctx) {
        uint64_t vtx;
        uint64_t edge;
        auto tup = std::tie(edge, vtx);

        fusion::move(std::move(x3::_attr(std::move(ctx))), tup);

        graph.connect(edge, vtx);
    };

    auto line = x3::uint64
        > x3::uint64
        > x3::eol;

    return +(line[parsing_function]);
}

} // namespace

auto part::operator>>(std::istream& in, part::ParsingMode& mode)
    -> std::istream&
{
    std::string token;
    in >> token;
    if(token == "hmetis")
        mode = part::ParsingMode::Hmetis;
    else if(token == "hyperedgelist")
        mode = part::ParsingMode::EdgeList;
    else if(token == "bipartite")
        mode = part::ParsingMode::Bipartite;
    else
        in.setstate(std::ios_base::failbit);
    return in;
}

 auto part::operator<<(std::ostream& os, const part::ParsingMode& mode)
    -> std::ostream&
{
    switch(mode) {
    case part::ParsingMode::Hmetis:
        os << "hmetis";
        break;
    case part::ParsingMode::EdgeList:
        os << "hyperedgelist";
        break;
    case part::ParsingMode::Bipartite:
        os << "bipartite";
    default:
        os.setstate(std::ios_base::failbit);
        break;
    }

    return os;
}

auto part::parseFileIntoHypergraph(const std::string& path,
                                   ParsingMode mode)
    -> part::Hypergraph
{
    namespace x3 = boost::spirit::x3;
    namespace fusion = boost::fusion;

    part::Hypergraph ret_graph{};

    //file handling
    std::ifstream edge_file(path);
    if(!edge_file) {
        std::cout << "file: " << path << "not found\n";
        return {};
    }

    //file iterators
    boost::spirit::istream_iterator file_iterator(edge_file >> std::noskipws),
        eof;

    //parse input file with the specified parser
    switch(mode) {
    case part::ParsingMode::Hmetis:

        x3::phrase_parse(file_iterator,
                         eof,
                         generate_hmetis_graph_parser(ret_graph),
                         x3::space - x3::eol);
        break;

    case part::ParsingMode::EdgeList:

        x3::phrase_parse(file_iterator,
                         eof,
                         generate_edgelist_graph_parser(ret_graph),
                         x3::space);
        break;

    case part::ParsingMode::Bipartite:

        x3::phrase_parse(file_iterator,
                         eof,
                         generate_bipartite_graph_parser(ret_graph),
                         x3::space - x3::eol);
        break;
    }

    return ret_graph;
}
