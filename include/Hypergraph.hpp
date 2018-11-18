#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace part {

class Hypergraph
{
public:
    using EdgeMap = std::unordered_map<int64_t, std::unordered_set<int64_t>>;
    using VertexMap = std::unordered_map<int64_t, std::unordered_set<int64_t>>;
    using VertexMapIter = typename VertexMap::iterator;
    using EdgeMapIter = typename EdgeMap::iterator;
    using VertexMapConstIter = typename VertexMap::const_iterator;
    using EdgeMapConstIter = typename EdgeMap::const_iterator;

public:
    //delete copy ctor
    Hypergraph() = default;
    Hypergraph(Hypergraph&&) = default;
    Hypergraph(const Hypergraph&) = delete;

    //delete copy assignemt operator
    auto operator=(Hypergraph &&)
        -> Hypergraph& = default;
    auto operator=(const Hypergraph&)
        -> Hypergraph& = delete;

    //adds vertex to graph
    //O(1)
    auto addVertex(int64_t)
        -> std::pair<VertexMapIter, bool>;

    //adds edge to graph
    //O(1)
    auto addEdge(int64_t)
        -> std::pair<EdgeMapIter, bool>;

    //adds vertex and and given edges to the graph
    //the vertex will be connected to all edges
    auto addEdgeList(const int64_t&,
                     const std::vector<int64_t>& edge_list)
        -> void;


    //adds new edge and given vertexlist
    //which are connected to the edge to the hypergraph
    auto addNodeList(const int64_t& edge,
                     const std::vector<int64_t>& node_list)
        -> void;

    auto getEdgesizeOfPercentBiggestEdge(double percent) const
        -> std::size_t;

    //connects the given vertex with the given edge
    //if vertex or edge dont exist in the graph
    //they get inserted
    auto connect(const int64_t& vtx,
                 const int64_t& edge)
        -> void;

    //return reference to the edge map
    auto getEdges() const
        -> const EdgeMap&;
    auto getEdges()
        -> EdgeMap&;

    //returns a reference to vertex map
    auto getVertices() const
        -> const VertexMap&;
    auto getVertices()
        -> VertexMap&;

    //deletes given vertex
    //also makes sure no edge holds a reference to the
    //deleted vertex
    auto deleteVertex(int64_t vtx)
        -> void;

    //returns a set of all edges a given vertex is connected to
    //returns an empty set when the vertex is not in the graph
    auto getEdgesOf(const int64_t&) const
        -> const std::unordered_set<int64_t>&;

    //returns a set of all vertices whcih are conntecte to the given edge
    //returns an empty set if the edge doesnt exist
    auto getVerticesOf(const int64_t&) const
        -> const std::unordered_set<int64_t>&;

    //returns @param n random neigbours of @param vtx
    //but ignores neigbours which are connected to vtx through
    //edges greater than @param ignore_edges
    auto getSSetCandidates(const int64_t& vtx,
                           std::size_t n,
                           std::size_t ignore_edges) const
        -> std::unordered_set<int64_t>;

    //returns the heuristic if the node is a good next node to add
    auto getNodeHeuristicExactly(const int64_t& vtx) const
        -> double;

    //if a nodes heuristic was never calculated then return exact number
    //and cache this number
    //if then the function gets ever called again with the same vtx
    //return the cached number instead of recalculating the number of neigs again
    //attention: since the graph changes while partitioning the cached number must not
    //be the right number, but mostly ever a good estimate
    auto getNodeHeuristicEstimate(const int64_t& vtx) const
        -> double;

    //returns a random node of the graph
    auto getRandomNode() const
        -> int64_t;
    //returns any node of the graph
    auto getANode() const
        -> int64_t;

    static auto setSeed(uint32_t seed)
        -> void;

private:
    EdgeMap _edges;
    VertexMap _vertices;
    mutable std::unordered_map<int64_t, double> _neigbour_map;
    inline static uint32_t random_seed;
};

} // namespace part
