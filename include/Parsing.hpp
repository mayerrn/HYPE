#include <Hypergraph.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace part {

enum class ParsingMode {
    Hmetis,
    EdgeList,
    Bipartite
};

//needed to be able to parse commandline arguments
//into the ParseMode struct
auto operator>>(std::istream& in, part::ParsingMode& mode)
    -> std::istream&;
auto operator<<(std::ostream& os, const part::ParsingMode& mode)
    -> std::ostream&;


auto parseFileIntoHypergraph(const std::string& path,
                             ParsingMode mode)
    -> part::Hypergraph;

} // namespace part
