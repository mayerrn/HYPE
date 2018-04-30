#pragma once

#include <future>
#include <unordered_set>
#include <vector>

namespace part {

class Partition
{
public:
    //make partitions move only
    Partition(Partition&&) = default;
    Partition() = delete;
    auto operator=(Partition &&)
        -> Partition& = default;
    auto operator=(const Partition&)
        -> Partition& = delete;

    Partition(std::size_t);

    //add node and its hyperedges to the partition
    auto addNode(uint64_t node,
                 const std::vector<uint64_t>& edges)
        -> void;

    //same as above
    auto addNode(uint64_t node,
                 const std::unordered_set<uint64_t>& edges)
        -> void;

    //checks if the partitions holds a vertex
    //which is connected to the given edge
    auto hasEdge(uint64_t edge) const
        -> bool;
    //return reference to the node set
    auto getNodes() const
        -> const std::unordered_set<uint64_t>&;
    //same as above
    auto getNodes()
        -> std::unordered_set<uint64_t>&;

    //return reference to the edge set
    auto getEdges() const
        -> const std::unordered_set<uint64_t>&;
    //same as above
    auto getEdges()
        -> std::unordered_set<uint64_t>&;

    //clears edge and node set
    auto clear()
        -> void;

    //returns the id of the partition
    auto getId() const
        -> std::size_t;

    //returns the number of hyperedges in the partition
    auto numberOfEdges() const
        -> std::size_t;

    //return the number of nodes in the partition
    auto numberOfNodes() const
        -> std::size_t;

    //returns a future which will hold the number of external degrees
    //of the partition to the other given partition
    auto externalDegree(const std::vector<Partition>& parts) const
        -> std::future<std::size_t>;

private:
    std::size_t _id; //id of the partition
    std::unordered_set<uint64_t> _nodes; //nodes in partition
    std::unordered_set<uint64_t> _edges; //edges in partition
};

} // namespace part
