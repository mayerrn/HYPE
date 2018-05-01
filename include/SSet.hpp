#pragma once

#include <Hypergraph.hpp>
#include <boost/optional.hpp>
#include <iostream>

namespace part {

enum class NodeHeuristicMode {
    Cached,
    Exact
};

//needed to be able to use the NodeHeuristicNode enum
//as commandline arument
auto operator>>(std::istream& in, part::NodeHeuristicMode& num)
    -> std::istream&;
auto operator<<(std::ostream& os, const part::NodeHeuristicMode& num)
    -> std::ostream&;

class SSet
{
public:
    SSet(const Hypergraph& graph,
         std::size_t max_size,
         NodeHeuristicMode numb_of_neigs_flag)
        : _graph(graph),
          _max_size(max_size),
          _numb_of_neigs_flag(numb_of_neigs_flag) {}

    auto addNodes(const std::unordered_set<uint64_t>& nodes_to_add)
        -> void;

    auto getMinElement() const
        -> boost::optional<uint64_t>;

    auto getNextNode() const
        -> uint64_t;

    auto removeNode(const uint64_t& node)
        -> void;

private:
    std::unordered_set<uint64_t> _nodes;
    const Hypergraph& _graph;
    std::size_t _max_size;
    NodeHeuristicMode _numb_of_neigs_flag;
    const std::function<std::size_t(uint64_t)> getNodeHeuristic =
        [&](uint64_t vtx) {
            switch(_numb_of_neigs_flag) {
            case NodeHeuristicMode::Exact:
                return _graph.getNodeHeuristicExactly(vtx);
            case NodeHeuristicMode::Cached:
                return _graph.getNodeHeuristicEstimate(vtx);
            }

        };
};

} // namespace part
