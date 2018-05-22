#ifndef DIJKSTRA_HPP_
#define DIJKSTRA_HPP_

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>                    //Biggest int: std::numeric_limits<int>::max()
#include "array_queue.hpp"
#include "array_stack.hpp"
#include "heap_priority_queue.hpp"
#include "hash_graph.hpp"


namespace ics {


class Info {
  public:
    Info() { }

    Info(std::string a_node) : node(a_node) { }

    bool operator==(const Info &rhs) const { return cost == rhs.cost && from == rhs.from; }

    bool operator!=(const Info &rhs) const { return !(*this == rhs); }

    friend std::ostream &operator<<(std::ostream &outs, const Info &i) {
      outs << "Info[" << i.node << "," << i.cost << "," << i.from << "]" << std::endl;
      return outs;
    }

    //Public instance variable definitions
    std::string node = "?";
    int cost = std::numeric_limits<int>::max();
    std::string from = "?";
  };


  bool gt_info(const Info &a, const Info &b) { return a.cost < b.cost; }

  typedef ics::HashGraph<int> DistGraph;
  typedef ics::HeapPriorityQueue<Info, gt_info> CostPQ;
  typedef ics::HashMap<std::string, Info, DistGraph::hash_str> CostMap;
  typedef ics::pair<std::string, Info> CostMapEntry;


//Return the final_map as specified in the lecture-node description of
//  extended Dijkstra algorithm
  CostMap extended_dijkstra(const DistGraph &g, std::string start_node) {
        CostMap answer_map;
        CostMap info_map;

        for (auto node: g.all_nodes()) {
            Info info(node.first);
            info_map[node.first] = info;
        }

        info_map[start_node].cost = 0;

        CostPQ info_pq(gt_info);
        for (const CostMapEntry node: info_map)
            info_pq.enqueue(node.second);
        std::cout << info_pq << std::endl;

        while (!info_map.empty()) {
            Info min_node = info_pq.dequeue();
            int min_cost = min_node.cost;
            if (min_cost == std::numeric_limits<int>::max())
                return answer_map;
            if (answer_map.has_key(min_node.node))
                continue;

            info_map.erase(min_node.node);
            answer_map.put(min_node.node, min_node);

            for (auto node: g.out_nodes(min_node.node))
                if (info_map[node].cost > min_cost + g.edge_value(min_node.node, node)
                        && !answer_map.has_key(node)) {
                    info_map[node].cost = min_cost + g.edge_value(min_node.node, node);
                    info_map[node].from = min_node.node;
                    info_pq.enqueue(info_map[node]);
                }
        }

        return answer_map;
  }


//Return a queue whose front is the start node (implicit in answer_map) and whose
//  rear is the end node
  ArrayQueue <std::string> recover_path(const CostMap &answer_map, std::string end_node) {
        ics::ArrayQueue<std::string> to_return;
        ics::ArrayStack<std::string> tracker;
        tracker.push(end_node);
        while (answer_map[tracker.peek()].from != "?") {
            tracker.push(answer_map[tracker.peek()].from);
        }

        to_return.enqueue_all(tracker);
        return to_return;
  }


}

#endif /* DIJKSTRA_HPP_ */
