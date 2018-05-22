////// Submitter: huyanhh(Hoang, Huyanh)
////// Partner: monicadc(Cruz, Monica)
////// We certify that we worked cooperatively on this programming
////// assignment, according to the rules for pair programming
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
//typedef ics::ArrayQueue<std::string>                InputsQueue;
//typedef ics::ArrayMap<std::string,std::string>      InputStateMap;
//
//typedef ics::ArrayMap<std::string,InputStateMap>    FA;
//typedef ics::pair<std::string,InputStateMap>        FAEntry;
//
//bool gt_FAEntry (const FAEntry& a, const FAEntry& b)
//{return a.first<b.first;}
//
//typedef ics::ArrayPriorityQueue<FAEntry,gt_FAEntry> FAPQ;
//
//typedef ics::pair<std::string,std::string>          Transition;
//typedef ics::ArrayQueue<Transition>                 TransitionQueue;
//
//
////Read an open file describing the finite automaton (each line starts with
////  a state name followed by pairs of transitions from that state: (input
////  followed by new state, all separated by semicolons), and return a Map
////  whose keys are states and whose associated values are another Map with
////  each input in that state (keys) and the resulting state it leads to.
//const FA read_fa(std::ifstream &file) {
//    FA fa;
//    std::string line;
//    while (getline(file,line)) {
//        std::vector<std::string> words = ics::split(line," ");
//        for (const std::string& word : words) {
//            std::vector<std::string> states = ics::split(word, ";");
//            std::string key = states[0];
//            for (int i = 1; i < states.size(); i++) {
//                ics::pair<std::string, std::string> pair(states.at(i++), states.at(i));
//                fa[key].put(pair.first, pair.second);
//            }
//        }
//    }
//    file.close();
//    return fa;
//}
//
//
////Print a label and all the entries in the finite automaton Map, in
////  alphabetical order of the states: each line has a state, the text
////  "transitions:" and the Map of its transitions.
//void print_fa(const FA& fa) {
//    FAPQ fapq;
//    fapq.enqueue_all(fa);
//    cout << "The Finite Automaton's Description" << endl;
//    for (auto i: fapq) {
//        cout << "  " << i.first << " transitions: " << i.second << endl;
//    }
//    cout << endl;
//}
//
//
////Return a queue of the calculated transition pairs, based on the finite
////  automaton, initial state, and queue of inputs; each pair in the returned
////  queue is of the form: input, new state.
////The first pair contains "" as the input and the initial state.
////If any input i is illegal (does not lead to a state in the finite
////  automaton), then the last pair in the returned queue is i,"None".
//TransitionQueue process(const FA& fa, std::string state, const InputsQueue& inputs) {
//    TransitionQueue transitionQueue({ics::make_pair(std::string(), state)});
//    for (auto i: inputs) {
//        try {
//            std::string newState(fa[state][i]);
//            transitionQueue.enqueue(ics::make_pair(i, newState));
//            state = newState;
//        } catch (ics::IcsError& e){
//            transitionQueue.enqueue(ics::make_pair(i, std::string("None")));
//            break;
//        }
//    }
//    return transitionQueue;
//}
//
//
////Print a TransitionQueue (the result of calling the process function above)
//// in a nice form.
////Print the Start state on the first line; then print each input and the
////  resulting new state (or "illegal input: terminated", if the state is
////  "None") indented on subsequent lines; on the last line, print the Stop
////  state (which may be "None").
//void interpret(TransitionQueue& tq) {  //or TransitionQueue or TransitionQueue&&
//    std::string state;
//    for (auto i: tq) {
//        if (i.first == "")
//            continue;
//        if (i.second == "None") {
//            cout << "  Input = " << i.first << "; " << "illegal input: terminated" << endl;
//        } else {
//            cout << "  Input = " << i.first << "; " << "new state = " << i.second << endl;
//        }
//        state = i.second;
//    }
//    cout << "Stop state = " << state << endl << endl;
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
//      ics::safe_open(text_file," Enter a finite automaton's file","faparity.txt");
//      FA fa = read_fa(text_file);
//      print_fa(fa);
//      std::ifstream text_input_file;
//      ics::safe_open(text_input_file, "Enter a start-state and input file", "fainputparity.txt");
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
//              TransitionQueue tq = process(fa, state, inputsQueue);
//              cout << "Starting a new simulation with description: " << line << endl;
//              cout << "Start state = " << state;
//              interpret(tq);
//          }
//      }
//
//  } catch (ics::IcsError& e) {
//    std::cout << e.what() << std::endl;
//  }
//
//  return 0;
//}
