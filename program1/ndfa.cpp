//////// Submitter: huyanhh(Hoang, Huyanh)
//////// Partner: monicadc(Cruz, Monica)
//////// We certify that we worked cooperatively on this programming
//////// assignment, according to the rules for pair programming
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
//using std::endl;
//
//typedef ics::ArraySet<std::string>                     States;
//typedef ics::ArrayQueue<std::string>                   InputsQueue;
//typedef ics::ArrayMap<std::string,States>              InputStatesMap;
//
//typedef ics::ArrayMap<std::string,InputStatesMap>       NDFA;
//typedef ics::pair<std::string,InputStatesMap>           NDFAEntry;
//
//bool gt_NDFAEntry (const NDFAEntry& a, const NDFAEntry& b)
//{return a.first<b.first;}
//
//typedef ics::ArrayPriorityQueue<NDFAEntry,gt_NDFAEntry> NDFAPQ;
//
//typedef ics::pair<std::string,States>                   Transitions;
//typedef ics::ArrayQueue<Transitions>                    TransitionsQueue;
//
//
////Read an open file describing the non-deterministic finite automaton (each
////  line starts with a state name followed by pairs of transitions from that
////  state: (input followed by a new state, all separated by semicolons), and
////  return a Map whose keys are states and whose associated values are another
////  Map with each input in that state (keys) and the resulting set of states it
////  can lead to.
//const NDFA read_ndfa(std::ifstream &file) {
//    NDFA ndfa;
//    std::string line;
//    while (getline(file,line)) {
//        std::vector<std::string> words = ics::split(line," ");
//        for (const std::string& word : words) {
//            std::vector<std::string> states = ics::split(word, ";");
//            std::string key = states[0];
//
//            for (int i = 0; i < states.size(); i++) {
//                if (i != 0) {
//                    InputStatesMap inputStatesMap;
//                    inputStatesMap.put(states.at(i++), States({states.at(i)}));
//                    ndfa[key][states.at(i - 1)].insert(states.at(i));
//                } else {
//                    ndfa.put(key, InputStatesMap());
//                }
//            }
//        }
//    }
//    file.close();
//    return ndfa;
//}
//
//
////Print a label and all the entries in the finite automaton Map, in
////  alphabetical order of the states: each line has a state, the text
////  "transitions:" and the Map of its transitions.
//void print_ndfa(const NDFA& ndfa) {
//    NDFAPQ ndfapq;
//    ndfapq.enqueue_all(ndfa);
//    cout << "The Finite Automaton's Description" << endl;
//    for (auto i: ndfapq) {
//        cout << "  " << i.first << " transitions: " << i.second << endl;
//    }
//    cout << endl;
//}
//
//
////Return a queue of the calculated transition pairs, based on the non-deterministic
////  finite automaton, initial state, and queue of inputs; each pair in the returned
////  queue is of the form: input, set of new states.
////The first pair contains "" as the input and the initial state.
////If any input i is illegal (does not lead to any state in the non-deterministic finite
////  automaton), ignore it.
//TransitionsQueue process(const NDFA& ndfa, std::string state, const InputsQueue& inputs) {
//    States newStates({state});
//    TransitionsQueue transitionsQueue({ics::make_pair(std::string(), newStates)});
//    for (auto input: inputs) {
//        States tempStates;
//        for (auto state: newStates) {
//            if (ndfa[state].has_key(input)) {
//                tempStates.insert_all(ndfa[state][input]);
//            }
//        }
//        Transitions transitions = ics::make_pair(input, tempStates);
//        newStates = tempStates;
//        transitionsQueue.enqueue(transitions);
//    }
//    return transitionsQueue;
//}
//
//
////Print a TransitionsQueue (the result of calling process) in a nice form.
////Print the Start state on the first line; then print each input and the
////  resulting new states indented on subsequent lines; on the last line, print
////  the Stop state.
//void interpret(TransitionsQueue& tq) {  //or TransitionsQueue or TransitionsQueue&&
//    States states;
//    for (auto i: tq) {
//        if (i.first == "")
//            continue;
//        cout << "  Input = " << i.first << "; " << "new states = " << i.second << endl;
//        states = i.second;
//    }
//    cout << "Stop state(s) = " << states << endl << endl;
//}
//
//
//
////Prompt the user for a file, create a finite automaton Map, and print it.
////Prompt the user for a file containing any number of simulation descriptions
////  for the finite automaton to process, one description per line; each
////  description contains a start state followed by its inputs, all separated by
////  semicolons.
////Repeatedly read a description, print that description, put each input in a
////  Queue, process the Queue and print the results in a nice form.
//int main() {
//  try {
//      std::ifstream text_file;
//      ics::safe_open(text_file," Enter a non-deterministic finite automaton's file",
//                     "ndfaendin01.txt");
//      NDFA ndfa = read_ndfa(text_file);
//      print_ndfa(ndfa);
//
//      std::ifstream text_input_file;
//      ics::safe_open(text_input_file, "Enter the name of a file with the start-states and input",
//                     "ndfainputendin01.txt");
//      cout << endl;
//      std::string line;
//      while (getline(text_input_file,line)) {
//          std::vector<std::string> words = ics::split(line," ");
//          InputsQueue inputsQueue;
//          std::string state;
//          for (const std::string& word : words) {
//              std::vector<std::string> inputs = ics::split(word, ";");
//              inputsQueue.enqueue_all(inputs);
//              state = inputsQueue.dequeue();
//              TransitionsQueue tq = process(ndfa, state, inputsQueue);
//              cout << "Starting a new simulation with description: " << line << endl;
//              cout << "Start state = " << state << endl;
//              interpret(tq);
//          }
//      }
//  } catch (ics::IcsError& e) {
//    std::cout << e.what() << std::endl;
//  }
//
//  return 0;
//}
