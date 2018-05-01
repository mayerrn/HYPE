#include <SSet.hpp>
#include <algorithm>


auto part::operator>>(std::istream& in, part::NodeHeuristicMode& num)
    -> std::istream&
{
    std::string token;
    in >> token;
    if(token == "exact")
        num = part::NodeHeuristicMode::Exact;
    else if(token == "cached")
        num = part::NodeHeuristicMode::Cached;
    else
        in.setstate(std::ios_base::failbit);
    return in;
}

auto part::operator<<(std::ostream& os, const part::NodeHeuristicMode& num)
    -> std::ostream&
{
    switch(num) {
    case part::NodeHeuristicMode::Exact:
        os << "exact";
        break;
    case part::NodeHeuristicMode::Cached:
        os << "cached";
        break;
    default:
        os.setstate(std::ios_base::failbit);
        break;
    }

    return os;
}



auto part::SSet::addNodes(const std::unordered_set<uint64_t>& nodes_to_add)
    -> void
{
    //create vec with number of neigs pair
    std::vector<std::pair<uint64_t, std::size_t>> nodes_to_add_neig_vec;
    for(auto&& node : nodes_to_add) {
        auto neigs = getNodeHeuristic(node);
        nodes_to_add_neig_vec.emplace_back(node, neigs);
    }

    //sort to get smallest number of neigs first
    std::sort(std::begin(nodes_to_add_neig_vec),
              std::end(nodes_to_add_neig_vec),
              [](auto lhs, auto rhs) { return lhs.second < rhs.second; });

    //do the same with the nodes of this sset
    std::vector<std::pair<uint64_t, std::size_t>> nodes_neig_vec;
    for(auto&& node : _nodes) {
        auto neigs = getNodeHeuristic(node);
        nodes_neig_vec.emplace_back(node, neigs);
    }

    std::sort(std::begin(nodes_neig_vec),
              std::end(nodes_neig_vec),
              [](auto lhs, auto rhs) { return lhs.second < rhs.second; });


    //merge the two sorted vectors
    std::vector<std::pair<uint64_t, std::size_t>> merged_vec;
    std::merge(std::begin(nodes_neig_vec),
               std::end(nodes_neig_vec),
               std::begin(nodes_to_add_neig_vec),
               std::end(nodes_to_add_neig_vec),
               std::back_inserter(merged_vec),
               [](auto&& lhs, auto&& rhs) { return lhs.second < rhs.second; });

    //erase nodes
    _nodes.clear();

    //replace with the smallest n nodes from the merged vector
    auto range = std::min(merged_vec.size(), _max_size);
    for(int i{0}; i < range; ++i) {
        _nodes.insert(merged_vec[i].first);
    }
}

auto part::SSet::getMinElement() const
    -> boost::optional<uint64_t>
{
    boost::optional<std::size_t> neigs;
    boost::optional<std::uint64_t> min_node;

    for(auto&& node : _nodes) {
        auto num_of_neigs = getNodeHeuristic(node);
        if(num_of_neigs <= 1) {
            return node;
        } else if(!neigs || neigs.get() > num_of_neigs) {
            neigs = num_of_neigs;
            min_node = node;
        }
    }

    return min_node;
}

auto part::SSet::getNextNode() const
    -> uint64_t
{
    auto min_node_opt = getMinElement();

    return min_node_opt.get_value_or(_graph.getRandomNode());
}

auto part::SSet::removeNode(const uint64_t& node)
    -> void
{
    auto iter = _nodes.begin();
    for(; iter != _nodes.end(); iter++) {
        if(*iter == node)
            break;
    }
    if(iter != _nodes.end()) {
        _nodes.erase(iter);
    }
}
