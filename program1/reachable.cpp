//// Submitter: huyanhh(Hoang, Huyanh)
//// Partner: monicadc(Cruz, Monica)
//// We certify that we worked cooperatively on this programming
//// assignment, according to the rules for pair programming
//
//// http://stackoverflow.com/questions/121162/what-does-the-explicit-keyword-in-c-mean
//// http://www.cprogramming.com/tutorial/function-pointers.html
//// pass by value: you pass it in and then mutate it without changing the original, (3)
//// pass by const ref: you pass it in but you can't mutate it and it's faster, (2)
//// pass by ref: mutate it, (2)
//
//#include <string>
//#include <iostream>
//#include <fstream>
//#include <sstream>
//#include <vector>
//#include "ics46goody.hpp"
//#include "array_queue.hpp"
//#include "array_priority_queue.hpp"
//#include "array_set.hpp"
//#include "array_map.hpp"
//
//using std::cout;
//
//typedef ics::ArraySet<std::string>          NodeSet;
//typedef ics::pair<std::string,NodeSet>      GraphEntry;
//
//bool graph_entry_gt (const GraphEntry& a, const GraphEntry& b)
//{return a.first<b.first;}
//
//typedef ics::ArrayPriorityQueue<GraphEntry,graph_entry_gt> GraphPQ;
//typedef ics::ArrayMap<std::string,NodeSet>  Graph;
//
//
////Read an open file of edges (node names separated by semicolons, with an
////  edge going from the first node name to the second node name) and return a
////  Graph (Map) of each node name associated with the Set of all node names to
////  which there is an edge from the key node name.
//
//Graph read_graph(std::ifstream &file) {
//	Graph graph;
//
//    std::string line;
//    int line_number = 0;
//    while (getline(file,line)) {
//        line_number++;
//        std::vector<std::string> words = ics::split(line," ");
//
//        for (const std::string& word : words) {
//            std::vector<std::string> nodes = ics::split(word, ";");
//            graph[nodes[0]].insert(nodes[1]);
////            std::cout << graph[0] << std::endl;
////            std::cout << graph << std::endl;
//        }
//
//    }
//
//    file.close();
//    return graph;
//}
//
//
////Print a label and all the entries in the Graph in alphabetical order
////  (by source node).
////Use a "->" to separate the source node name from the Set of destination
////  node names to which it has an edge.
//void print_graph(const Graph& graph) {
//    GraphPQ pq;
//    pq.enqueue_all(graph);
//    cout << "Graph: source -> {destination} edges" << std::endl;
//
//    for (auto i : pq) {
//        cout << "  " << i.first << " -> " << i.second << std::endl;
//    }
//}
//
//
////Return the Set of node names reaching in the Graph starting at the
////  specified (start) node.
////Use a local Set and a Queue to respectively store the reachable nodes and
////  the nodes that are being explored.
//NodeSet reachable(const Graph& graph, std::string start) {
//    NodeSet reachedSet;
//    ics::ArrayQueue<std::string> exploringQueue;
//    exploringQueue.enqueue(start);
//    while (exploringQueue.size() > 0) {
//        std::string dequeued = exploringQueue.dequeue();
//        reachedSet.insert(dequeued);
//        if (graph.has_key(dequeued)) {
//            for (auto node : graph[dequeued]) {
//                if (!reachedSet.contains(node)) {
//                    exploringQueue.enqueue(node);
//                }
//            }
//        }
//    }
//    cout << "Reachable from node name: " << start << " = " << reachedSet << std::endl;
//    return reachedSet;
//}
//
//
////Prompt the user for a file, create a graph from its edges, print the graph,
////  and then repeatedly (until the user enters "quit") prompt the user for a
////  starting node name and then either print an error (if that the node name
////  is not a source node in the graph) or print the Set of node names
////  reachable from it by using the edges in the Graph.
//int main() {
//  try {
//      std::ifstream text_file;
//      ics::safe_open(text_file,"Enter file name to analyze","graph1.txt");
//
//      Graph graph = read_graph(text_file);
//      print_graph(graph);
//
//      std::string nodeName;
//      while (nodeName != "quit") {
//          nodeName = ics::prompt_string("Enter a starting node's name");
//          if (!graph.has_key(nodeName)) {
//              cout << "  " << nodeName << " is not a source node name in the graph" << std::endl;
//          } else {
//              reachable(graph, nodeName);
//          }
//      }
//
//
//
//  } catch (ics::IcsError& e) {
//    cout << e.what() << std::endl;
//  }
//
//  return 0;
//}
