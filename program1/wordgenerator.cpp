//// Submitter: huyanhh(Hoang, Huyanh)
//// Partner: monicadc(Cruz, Monica)
//// We certify that we worked cooperatively on this programming
//// assignment, according to the rules for pair programming

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>                           //I used std::numeric_limits<int>::max()
#include "ics46goody.hpp"
#include "array_queue.hpp"
#include "array_priority_queue.hpp"
#include "array_set.hpp"
#include "array_map.hpp"

using std::cout;
using std::endl;

typedef ics::ArrayQueue<std::string>         WordQueue;
typedef ics::ArraySet<std::string>           FollowSet;
typedef ics::pair<WordQueue,FollowSet>       CorpusEntry;
typedef ics::ArrayPriorityQueue<CorpusEntry> CorpusPQ;     //Convenient to supply gt at construction
typedef ics::ArrayMap<WordQueue,FollowSet>   Corpus;


//Return a random word in the words set (use in produce_text)
std::string random_in_set(const FollowSet& words) {
  int index = ics::rand_range(1, words.size());
  int i = 0;
  for (const std::string& s : words)
    if (++i == index)
      return s;
  return "?";
}


//Read an open file of lines of words (separated by spaces) and return a
//  Corpus (Map) of each sequence (Queue) of os (Order-Statistic) words
//  associated with the Set of all words that follow them somewhere in the
//  file.
Corpus read_corpus(int os, std::ifstream &file) {
    Corpus corpus;
    std::string line;
    WordQueue wordQueue;
    std::string wordsToRead;
    while (getline(file,line)) {
        wordsToRead += line + " ";
    }
    std::vector<std::string> words = ics::split(wordsToRead, " ");
    for (int i = 0; i < words.size(); i++) {
        while (wordQueue.size() != os) {
            wordQueue.enqueue(words[i]);
        }
        if (i != words.size() - 1)
            corpus[wordQueue].insert(words[i + 1]);
            wordQueue.dequeue();
            wordQueue.enqueue(words[i + 1]);
    }
    file.close();
    return corpus;
}


//Print "Corpus" and all entries in the Corpus, in lexical alphabetical order
//  (with the minimum and maximum set sizes at the end).
//Use a "can be followed by any of" to separate the key word from the Set of words
//  that can follow it.

//One queue is lexically greater than another, if its first value is smaller; or if
//  its first value is the same and its second value is smaller; or if its first
//  and second values are the same and its third value is smaller...
//If any of its values is greater than the corresponding value in the other queue,
//  the first queue is not greater.
//Note that the queues sizes are the same: each stores Order-Statistic words
//Important: Use iterators for examining the queue values: DO NOT CALL DEQUEUE.

bool queue_gt(const CorpusEntry& a, const CorpusEntry& b) {
    ics::ArrayQueue<std::string>::Iterator ita = a.first.begin();
    ics::ArrayQueue<std::string>::Iterator itb = b.first.begin();
    for (; ita != a.first.end() && itb != b.first.end(); ++ita, ++itb){
        if (*ita != *itb) {
            return *ita < *itb;
        }
    }
}

void print_corpus(const Corpus& corpus) {
    CorpusPQ corpusPQ(queue_gt);
    corpusPQ.enqueue_all(corpus);
    cout << "Corpus has " << corpus.size() << " Entries" << endl;
    int min = corpusPQ.peek().second.size();
    int max = min;
    for (auto i: corpusPQ) {
        cout << i.first << " -> " << i.second << endl;
        min = (i.second.size() < min ? i.second.size() : min);
        max = (i.second.size() > max ? i.second.size() : max);
    }
    cout << "Corpus has " << corpus.size() << " Entries" << endl;
    cout << "min/max = " << min << "/" << max << endl;
}


//Return a Queue of words, starting with those in start and including count more
//  randomly selected words using corpus to decide which word comes next.
//If there is no word that follows the previous ones, put "None" into the queue
//  and return immediately this list (whose size is <= start.size() + count).
WordQueue produce_text(const Corpus& corpus, const WordQueue& start, int count) {
    WordQueue generatedText(start);
    WordQueue key(start);
    for (int i = 0; i < count; i++) {
        if (!corpus.has_key(key)) {
            generatedText.enqueue("None");
            return generatedText;
        }
        std::string word = random_in_set(corpus[key]);
        generatedText.enqueue(word);
        key.dequeue();
        key.enqueue(word);
    }
    return generatedText;
}



//Prompt the user for (a) the order statistic and (b) the file storing the text.
//Read the text as a Corpus and print it appropriately.
//Prompt the user for order statistic words from the text.
//Prompt the user for number of random words to generate
//Call the above functions to solve the problem, and print the appropriate information
int main() {
  try {
      std::ifstream text_file;
      int os = ics::prompt_int("Enter an order statistic", 2);
      ics::safe_open(text_file,"Enter a file to process",
                     "wginput1.txt");
      Corpus corpus = read_corpus(os, text_file);
      cout << endl;
      print_corpus(corpus);
      cout << endl;
      cout << "Enter 2 words to start with " << endl;
      std::string word1 = ics::prompt_string("Enter word 1");
      std::string word2 = ics::prompt_string("Enter word 2");
      WordQueue start({word1, word2});
      int count = ics::prompt_int("Enter # of words to generate");
      cout << "Random text = " << produce_text(corpus, start, count) << endl;
  } catch (ics::IcsError& e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}
