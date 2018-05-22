//// Submitter: huyanhh(Hoang, Huyanh)
//// Partner  : monicadc(Cruz, Monica)
//// We certify that we worked cooperatively on this programming
////   assignment, according to the rules for pair programming
//
//#include <string>
//#include <iostream>
//#include <fstream>
//#include <sstream>
//#include <vector>
//#include <limits>                    //Biggest int: std::numeric_limits<int>::max()
//#include "ics46goody.hpp"
//#include "array_queue.hpp"
//#include "array_priority_queue.hpp"
//#include "array_set.hpp"
//#include "array_map.hpp"
//
//using std::cout;
//using std::endl;
//
//typedef ics::ArrayQueue<std::string>              CandidateQueue;
//typedef ics::ArraySet<std::string>                CandidateSet;
//typedef ics::ArrayMap<std::string,int>            CandidateTally;
//
//typedef ics::ArrayMap<std::string,CandidateQueue> Preferences;
//typedef ics::pair<std::string,CandidateQueue>     PreferencesEntry;
//typedef ics::ArrayPriorityQueue<PreferencesEntry> PreferencesEntryPQ; //Must supply gt at construction
//
//typedef ics::pair<std::string,int>                TallyEntry;
//typedef ics::ArrayPriorityQueue<TallyEntry>       TallyEntryPQ;
//
//bool preferences_entry_gt (const PreferencesEntry& a, const PreferencesEntry& b)
//{return a.first<b.first;}
//
//bool tally_entry_gt_num (const TallyEntry& a, const TallyEntry& b)
//{return a.second<b.second;}
//
//bool tally_entry_lt_num (const TallyEntry& a, const TallyEntry& b)
//{return a.second>b.second;}
//
//bool tally_entry_gt (const TallyEntry& a, const TallyEntry& b)
//{return a.first<b.first;}
//
////Read an open file stating voter preferences (each line is (a) a voter
////  followed by (b) all the candidates the voter would vote for, in
////  preference order (from most to least preferred candidate, separated
////  by semicolons), and return a Map of preferences: a Map whose keys are
////  voter names and whose values are a queue of candidate preferences.
//Preferences read_voter_preferences(std::ifstream &file) {
//    Preferences preferences;
//
//    std::string line;
//    int line_number = 0;
//    while (getline(file,line)) {
//        line_number++;
//        std::vector<std::string> words = ics::split(line," ");
//        for (const std::string& word : words) {
//            std::vector<std::string> nodes = ics::split(word, ";");
//            CandidateSet candidateSet({nodes[1], nodes[2], nodes[3]});
//            preferences[nodes[0]].enqueue_all(candidateSet);
//        }
//    }
//    file.close();
//    return preferences;
//}
//
//
////Print a label and all the entries in the preferences Map, in alphabetical
////  order according to the voter.
////Use a "->" to separate the voter name from the Queue of candidates.
//void print_voter_preferences(const Preferences& preferences) {
//    PreferencesEntryPQ preferencesEntryPQ(preferences_entry_gt);
//    preferencesEntryPQ.enqueue_all(preferences);
//
//    cout << "Voter -> queue[Preferences]" << std::endl;
//    for (auto i : preferencesEntryPQ) {
//        cout << "  " << i.first << " -> " << i.second << std::endl;
//    }
//    cout << endl;
//}
//
//
////Print the message followed by all the entries in the CandidateTally, in
////  the order specified by has_higher_priority: i is printed before j, if
////  has_higher_priority(i,j) returns true: sometimes alphabetically by candidate,
////  other times by decreasing votes for the candidate.
////Use a "->" to separate the candidate name from the number of votes they
////  received.
//void print_tally(std::string message, const CandidateTally& tally, bool (*has_higher_priority)(const TallyEntry& i,const TallyEntry& j)) {
//    CandidateSet setToPrint;
//    for (auto i: tally) {
//        setToPrint.insert(i.first);
//    }
//    cout << message << setToPrint << endl;
//    TallyEntryPQ tallyEntryPQ(has_higher_priority);
//    tallyEntryPQ.enqueue_all(tally);
//
//    for (auto i: tallyEntryPQ) {
//        cout << "  " << i.first << " -> " << i.second << endl;
//    }
//    cout << endl;
//}
//
//
////Return the CandidateTally: a Map of candidates (as keys) and the number of
////  votes they received, based on the unchanging Preferences (read from the
////  file) and the candidates who are currently still in the election (which changes).
////Every possible candidate should appear as a key in the resulting tally.
////Each voter should tally one vote: for their highest-ranked candidate who is
////  still in the the election.
//CandidateTally evaluate_ballot(const Preferences& preferences, const CandidateSet& candidates) {
//    CandidateTally candidateTally;
//    for (auto i: preferences) {
//        for (auto j: i.second) {
//            if (candidates.contains(j)) {
//                candidateTally[j] += 1;
//                break;
//            }
//        }
//        //cout << candidateTally << endl;
//    }
//    return candidateTally;
//}
//
//
////Return the Set of candidates who are still in the election, based on the
////  tally of votes: compute the minimum number of votes and return a Set of
////  all candidates receiving more than that minimum; if all candidates
////  receive the same number of votes (that would be the minimum), the empty
////  Set is returned.
//CandidateSet remaining_candidates(const CandidateTally& tally) {
//    CandidateSet candidateSet;
//    TallyEntryPQ tallyEntryPQ(tally_entry_gt_num);
//    tallyEntryPQ.enqueue_all(tally);
//    TallyEntry tallyEntry = tallyEntryPQ.peek();
//    for (auto i: tally) {
//        if (i.second > tallyEntry.second) {
//            candidateSet.insert(i.first);
//        }
//    }
//    return candidateSet;
//}
//
//
////Prompt the user for a file, create a voter preference Map, and print it.
////Determine the Set of all the candidates in the election, from this Map.
////Repeatedly evaluate the ballot based on the candidates (still) in the
////  election, printing the vote count (tally) two ways: with the candidates
////  (a) shown alphabetically increasing and (b) shown with the vote count
////  decreasing (candidates with equal vote counts are shown alphabetically
////  increasing); from this tally, compute which candidates remain in the
////  election: all candidates receiving more than the minimum number of votes;
////  continue this process until there are less than 2 candidates.
////Print the final result: there may 1 candidate left (the winner) or 0 left
////   (no winner).
//int main() {
//  try {
//      std::ifstream text_file;
//      ics::safe_open(text_file,"Enter file name to analyze","votepref1.txt");
//
//      Preferences preferences = read_voter_preferences(text_file);
//      print_voter_preferences(preferences);
//      CandidateSet candidateSet;
//      for (auto i : preferences) {
//          candidateSet.insert_all(i.second);
//          break;
//      }
//
//      int ballotCount = 1;
//      while (candidateSet.size() > 1) {
//          std::string message = "Vote count on ballot #" + std::to_string(ballotCount)
//               + " with candidates (alphabetically ordered); "
//               + "remaining candidates = ";
//          std::string message2 = "Vote count on ballot #" + std::to_string(ballotCount)
//                                 + " with candidates (numerically ordered); "
//                                 + "remaining candidates = ";
//          CandidateTally tally = evaluate_ballot(preferences, candidateSet);
//          print_tally(message, tally, tally_entry_gt);
//          print_tally(message2, tally, tally_entry_lt_num);
//          candidateSet = remaining_candidates(tally);
//          ballotCount++;
//          if (candidateSet.size() == 1) {
//              for (auto i: candidateSet) { cout << "Winner is " << i << endl; }
//          }
//      }
//      if (candidateSet.size() == 0) {
//          cout << "No winner: election is a tie among all candidates remaining on the last ballot";
//      }
//
//
//  } catch (ics::IcsError& e) {
//    std::cout << e.what() << std::endl;
//  }
//  return 0;
//}
