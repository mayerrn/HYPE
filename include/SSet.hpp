#pragma once

#include <Hypergraph.hpp>
#include <iostream>
#include <optional>

namespace part {

enum class NodeHeuristicMode {
    Cached,
    Exact
};

enum class NodeSelectionMode {
    TrulyRandom,
    NextBest
};

//needed to be able to use the NodeHeuristicNode enum
//as commandline arument
auto operator>>(std::istream& in, part::NodeHeuristicMode& num)
    -> std::istream&;
auto operator<<(std::ostream& os, const part::NodeHeuristicMode& num)
    -> std::ostream&;
auto operator>>(std::istream& in, part::NodeSelectionMode& num)
    -> std::istream&;
auto operator<<(std::ostream& os, const part::NodeSelectionMode& num)
    -> std::ostream&;

class SSet
{
public:
    SSet(const Hypergraph& graph,
         std::size_t max_size,
         NodeHeuristicMode numb_of_neigs_flag,
         NodeSelectionMode node_select_flag)
        : _graph(graph),
          _max_size(max_size),
          _numb_of_neigs_flag(numb_of_neigs_flag),
          _node_select_flag(node_select_flag) {}

    auto addNodes(const std::unordered_set<uint64_t>& nodes_to_add)
        -> void;

    auto getMinElement() const
        -> std::optional<uint64_t>;

    auto getNextNode() const
        -> uint64_t;

    auto removeNode(const uint64_t& node)
        -> void;

private:
    auto getNodeHeuristic(std::uint64_t vtx) const
        -> std::size_t;

    auto selectANode() const
        -> std::uint64_t;

private:
    std::unordered_set<uint64_t> _nodes;
    const Hypergraph& _graph;
    std::size_t _max_size;
    NodeHeuristicMode _numb_of_neigs_flag;
    NodeSelectionMode _node_select_flag;
};

} // namespace part
