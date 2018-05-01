#pragma once

#include <Hypergraph.hpp>
#include <Partition.hpp>
#include <SSet.hpp>

namespace part {


/**
 * @param graph the hypergraph which will get partitioned.
 * @param number_of_partitions the number of partitions in which the
 *        hypergraph will get partitioned.
 * @param s_set_size the maximum size of the secondary set
 *        high numbers will slow the partitioning down, but can result in
 *        better partitioning quality.
 * @param ignore_biggest_edges_in_percent the percentage of biggest edges
 *        which will be ignored when expanding the secondary set default is
 *        1, which means the biggest 1% of the edges will be ignored.
 *
 * @return a vector of partitions as result of partitioning the graph
 */
auto partitionGraph(Hypergraph&& graph,
                    std::size_t number_of_partitions,
                    std::size_t s_set_size,
                    std::size_t s_set_candidates,
                    double ignore_biggest_edges_in_percent,
                    NodeHeuristicMode num_neigs_flag)
    -> std::vector<Partition>;

/**
 * @param partitions vector of partitions for which the sum of
 *        external degrees will be calculated
 * @return the soed of the given partitions
 */
auto getSumOfExteralDegrees(const std::vector<Partition>& partitions)
    -> std::future<std::size_t>;

auto getHyperedgeCut(const std::vector<Partition>& partitions)
    -> std::future<std::size_t>;

auto getEdgeBalancing(const std::vector<Partition>& partitions)
    -> std::future<double>;

auto getVertexBalancing(const std::vector<Partition>& partitions)
    -> std::future<double>;

auto getKminus1Metric(const std::vector<Partition>& partitions,
                  std::size_t edges_in_graph)
    -> std::future<std::size_t>;

} // namespace part
