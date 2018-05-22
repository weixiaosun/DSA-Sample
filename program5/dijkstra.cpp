#include <string>
#include <iostream>
#include <fstream>
#include "ics46goody.hpp"
#include "array_queue.hpp"
#include "hash_graph.hpp"
#include "dijkstra.hpp"



std::string get_node_in_graph(const ics::DistGraph& g, std::string prompt, bool allow_QUIT) {
  std::string node;
  for(;;) {
    node = ics::prompt_string(prompt + " (must be in graph" + (allow_QUIT ? " or QUIT" : "") + ")");
    if ((allow_QUIT && node == "QUIT") || g.has_node(node))
      break;
  }
  return node;
}

void get_path(ics::CostMap answer_map, std::string prompt, bool allow_QUIT) {
    std::string node;
    for(;;) {
        node = ics::prompt_string(prompt + " (must be in graph" + (allow_QUIT ? " or QUIT" : "") + ")");
        if ((allow_QUIT && node == "QUIT"))
            break;

        std::cout << "Cost is " << answer_map[node].cost << "; path is "
                  << ics::recover_path(answer_map, node) << std::endl << std::endl;
    }
}


int main() {
  try {
      ics::DistGraph graph;
      std::ifstream f;
      ics::safe_open(f, "Enter graph filename", "flightdist.txt");
      graph.load(f, ";");
      std::cout << graph << std::endl;

      std::string start_node = get_node_in_graph(graph, "Enter start node", true);
      ics::CostMap answer_map = ics::extended_dijkstra(graph, start_node);
      std::cout << answer_map << std::endl;

      get_path(answer_map, "Enter stop node", true);

  } catch (ics::IcsError& e) {
    std::cout << e.what() << std::endl;
  }

  return 0;
}
