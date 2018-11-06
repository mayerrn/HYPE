#include <Hypergraph.hpp>
#include <algorithm>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>


auto part::Hypergraph::addVertex(uint64_t id)
    -> std::pair<part::Hypergraph::VertexMapIter, bool>
{
    if(auto iter = _vertices.find(id);
       iter != _vertices.end()) {
        return {iter, false};
    } else {
        return _vertices.insert({id, {}});
    }
}

auto part::Hypergraph::addEdge(uint64_t id)
    -> std::pair<part::Hypergraph::EdgeMapIter, bool>
{
    if(auto iter = _edges.find(id);
       iter != _edges.end()) {
        return {iter, false};
    } else {
        return _edges.insert({id, {}});
    }
}

auto part::Hypergraph::addEdgeList(const uint64_t& vtx,
                                   const std::vector<uint64_t>& edge_list)
    -> void
{
    for(auto&& edge : edge_list) {
        connect(vtx, edge);
    }
}


auto part::Hypergraph::addNodeList(const uint64_t& edge,
                                   const std::vector<uint64_t>& node_list)
    -> void
{
    for(auto&& node : node_list) {
        connect(node, edge);
    }
}

auto part::Hypergraph::connect(const uint64_t& vertex,
                               const uint64_t& edge) -> void
{
    auto [vertex_iter, dummy1] = addVertex(vertex);
    auto [edge_iter, dummy2] = addEdge(edge);

    edge_iter->second.insert(vertex);
    vertex_iter->second.insert(edge);
}

auto part::Hypergraph::getNodeHeuristicExactly(const uint64_t& vtx) const
    -> double
{
    const auto& edges = getEdgesOf(vtx);

    //we need this to not divide by zero later
    if(edges.empty())
        return 0;

    return std::accumulate(std::begin(edges),
                           std::end(edges),
                           std::size_t{0},
                           [this](auto init, auto edge) {
                               return init + getVerticesOf(edge).size() - 1;
                           })
        / edges.size();
}

auto part::Hypergraph::getNodeHeuristicEstimate(const uint64_t& vtx) const
    -> double
{
    if(auto iter = _neigbour_map.find(vtx);
       iter != _neigbour_map.end()) {
        return iter->second;
    }

    auto neigs = getNodeHeuristicExactly(vtx);

    _neigbour_map.insert({vtx, neigs});
    return neigs;
}

namespace {

//adds elements of @param from into @param to
//as long as @param to does not have more than @param upto
//elements. Only elements for which the given predicate @param pred is true
template<class From, class To, class Predicate>
auto addAtMostN(From&& from,
                std::size_t upto,
                To&& to,
                Predicate&& pred)
{
    //set end iterator based on wether size(to) + size(from)
    //is bigger than allowed
    auto end = from.begin();
    if(to.size() + from.size() > upto)
        std::advance(end, upto - to.size());
    else
        end = from.end();

    //add elements if predicate is true
    auto iter = from.begin();
    while(iter != end) {
        if(pred(*iter)) {
            to.insert(*iter);
        }
        iter++;
    }

    //return new container with at most @param upto
    //elements
    return std::move(to);
}

} // namespace

auto part::Hypergraph::getSSetCandidates(const uint64_t& vtx,
                                         std::size_t n,
                                         std::size_t max_edge_size) const
    -> std::unordered_set<uint64_t>
{
    //while we dont have reached the limit
    //and we dont have enough sset-candidates found
    //keep searching for them in bigger edges
    const auto& edges = getEdgesOf(vtx);
    std::unordered_set<uint64_t> neigbors;
    std::size_t current_max{2};

    while(current_max < max_edge_size
          && neigbors.size() <= n) {

        //TODO:maybe refactor to something faster
        for(auto&& edge : edges) {
            const auto& vtxs = getVerticesOf(edge);

            // if edge is bigger then max_edge_size then skip
            if(vtxs.size() > current_max)
                continue;

            neigbors = addAtMostN(vtxs,
                                  n,
                                  std::move(neigbors), //i like to move it move it
                                  [&vtx](auto&& elem) {
                                      return elem != vtx;
                                  });

            if(neigbors.size() >= n)
                return neigbors;
        }

        current_max *= 2;
    }
    return neigbors;
}

auto part::Hypergraph::getEdgesOf(const uint64_t& vtx) const
    -> const std::unordered_set<uint64_t>&
{
    if(auto iter = _vertices.find(vtx); iter != _vertices.end()) {
        return iter->second;
    } else {
        //needed to return a reference
        static const std::unordered_set<uint64_t> empty_set;
        return empty_set;
    }
}

auto part::Hypergraph::getVerticesOf(const uint64_t& edge) const
    -> const std::unordered_set<uint64_t>&
{
    if(auto iter = _edges.find(edge); iter != _edges.end()) {
        return iter->second;
    } else {
        //needed to return a reference
        static const std::unordered_set<uint64_t> empty_set;
        return empty_set;
    }
}

auto part::Hypergraph::getEdges() const
    -> const EdgeMap&
{
    return _edges;
}

auto part::Hypergraph::getEdges()
    -> EdgeMap&
{
    return _edges;
}

auto part::Hypergraph::getVertices() const
    -> const VertexMap&
{
    return _vertices;
}

auto part::Hypergraph::getVertices()
    -> VertexMap&
{
    return _vertices;
}

auto part::Hypergraph::getEdgesizeOfPercentBiggestEdge(double percent) const
    -> std::size_t
{
    const auto factor = 1 - percent / 100;
    std::vector<std::size_t> size_vec;
    for(auto&& [key, value] : _edges) {
        size_vec.push_back(value.size());
    }

    std::nth_element(size_vec.begin(),
                     size_vec.begin() + (size_vec.size() - 1) * factor,
                     size_vec.end());

    return size_vec[(size_vec.size() - 1) * factor];
}

auto part::Hypergraph::getRandomNode() const
    -> uint64_t
{
    static std::mt19937 engine{Hypergraph::random_seed};
    std::uniform_int_distribution<std::size_t> dist(0, _vertices.size() - 1);

    auto iter = std::begin(_vertices);
    std::advance(iter, dist(engine));

    return iter->first;
}

auto part::Hypergraph::getANode() const
    -> uint64_t
{
    return _vertices.begin()->first;
}

auto part::Hypergraph::deleteVertex(uint64_t vertex)
    -> void
{
    if(auto vertex_iter = _vertices.find(vertex);
       vertex_iter != _vertices.end()) {
        for(auto&& edge : vertex_iter->second) {
            if(auto edge_iter = _edges.find(edge);
               edge_iter != _edges.end()) {
                edge_iter->second.erase(vertex);

                // delete edges without nodes
                if(edge_iter->second.empty()) {
                    _edges.erase(edge_iter);
                }
            }
        }

        _neigbour_map.erase(vertex);
        _vertices.erase(vertex_iter);
    }
}


auto part::Hypergraph::setSeed(int64_t seed)
    -> void
{
    Hypergraph::random_seed = seed;
}
