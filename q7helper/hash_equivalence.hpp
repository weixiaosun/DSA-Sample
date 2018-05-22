#ifndef HASH_EQUIVALENCE_HPP_
#define HASH_EQUIVALENCE_HPP_

#include <sstream>
#include "ics_exceptions.hpp"
#include "hash_map.hpp"
#include "hash_set.hpp"

using namespace std;

namespace ics {


//HashEquivalence must be instantiated with a legal thash function (there is no default)
//  The classes() method must use a hash that in instantiated in the template
template<class T, int (*thash) (const T& a)> class HashEquivalence {
  public:
    //Destructor/Constructors
    ~HashEquivalence ();

    HashEquivalence          (double the_load_threshold = 1.0);
    HashEquivalence          (const HashEquivalence<T,thash>& to_copy, double the_load_threshold = 1.0);
    explicit HashEquivalence (const std::initializer_list<T>& il, double the_load_threshold = 1.0);

    //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
    template <class Iterable>
    explicit HashEquivalence (const Iterable& i, double the_load_threshold = 1.0);


    //Queries
    bool                in_same_class (const T& a, const T& b); // Not const because compression
    int                 size          () const;                 // number of values in all Equivalences
    int                 class_count   () const;                 // number of Equivalences
    HashSet<HashSet<T,thash>> classes ();                       // Not const because compression used
    std::string str                   () const; //supplies useful debugging information; contrast to operator <<


    //Commands
    void add_singleton    (const T& a);
    void merge_classes_of (const T& a, const T& b);


    //Operators
    template<class T2, int (*thash2) (const T2& a)>
    friend std::ostream& operator << (std::ostream& outs, const HashEquivalence<T2,thash2>& e);


    //Miscellaneous methods (useful for testing/debugging)
    int max_height               () const;
    HashMap<T,int> heights       () const;
    std::string equivalence_info () const;

  private:
    HashMap<T,T>   parent;
    HashMap<T,int> root_size;

    //Helper methods
    T compress_to_root (T a);
};





////////////////////////////////////////////////////////////////////////////////
//
//HashMap class and related definitions

//Destructor/Constructor

template<class T, int (*thash) (const T& a)>
HashEquivalence<T,thash>::~HashEquivalence ()
{}


template<class T, int (*thash) (const T& a)>
HashEquivalence<T,thash>::HashEquivalence(double the_load_threshold)
: parent(the_load_threshold,thash), root_size(the_load_threshold,thash) {
}


template<class T, int (*thash) (const T& a)>
HashEquivalence<T,thash>::HashEquivalence (const HashEquivalence<T,thash>& to_copy, double the_load_threshold)
: parent(to_copy.parent,the_load_threshold,thash), root_size(to_copy.root_size,the_load_threshold,thash) {
}


template<class T, int (*thash) (const T& a)>
HashEquivalence<T,thash>::HashEquivalence (const std::initializer_list<T>& il, double the_load_threshold)
: parent(the_load_threshold,thash), root_size(the_load_threshold,thash) {
  for (const T& v : il)
    add_singleton(v);
}


template<class T, int (*thash) (const T& a)>
template <class Iterable>
HashEquivalence<T,thash>::HashEquivalence (const Iterable& i, double the_load_threshold)
:  parent(the_load_threshold,thash), root_size(the_load_threshold,thash) {
  for (const T& v : i)
    add_singleton(v);
}


////////////////////////////////////////////////////////////////////////////////
//
//Queries

//Two values are in the same class if their equivalence trees have the same roots.
//In the process of finding the roots and comparing them, compress all the values
//  on the path to the root: make the parents of a and all its ancestors (not
//  descendants) directly refer to the root of a's equivalence class (same for b).
//Throw an EquivalenceError (with a descriptive message) if the parameter a or b
//  is not already a value in the Equivalence (were never added as singletons).
template<class T, int (*thash) (const T& a)>
bool HashEquivalence<T,thash>::in_same_class (const T& a, const T& b) {
  //Write code here
      if (!parent.has_key(a) || !parent.has_key(b))
        throw EquivalenceError("Parameters (1) or (2) were never added as singletons");
      return compress_to_root(a) == compress_to_root(b);
}


//The number of values in all Equivalences; if a,b,c are equivalent, and
//  d,e are equivalent, size returns 5.
template<class T, int (*thash) (const T& a)>
int HashEquivalence<T,thash>::size () const{
  return parent.size();
}


//The number of different equivalent classes; if a,b,c are equivalent, and
//  d,e are equivalent, class_count returns 2.
template<class T, int (*thash) (const T& a)>
int HashEquivalence<T,thash>::class_count () const{
  return root_size.size();
}




//For every value in the Equivalence, compress it to its root and
//  insert it into the set associated with the root in a local map
//Then, insert into a Set all the sets collected previously in the map
//Use thash to write a lambda for hashing Set<T>, which is used in the
//  constructor for the returned HashSet<HashSet<T,thash>>.
template<class T, int (*thash) (const T& a)>
HashSet<HashSet<T,thash>> HashEquivalence<T,thash>::classes () {
  //Write code here
        auto lambda = [](const HashSet<T,thash>& set) -> int {
            int hashed = 1;
            for (auto value: set)
                hashed *= thash(value);
            return hashed;
        };

        HashMap<T, HashSet<T, thash>> map(1,thash);
        HashSet<HashSet<T,thash>> to_return(1,lambda);
        for (auto value: parent) {
            T root = compress_to_root(value.first);
            map[root].insert(value.first);
        }
        for (auto value: map)
            to_return.insert(value.second);
        return to_return;
}


template<class T, int (*thash) (const T& a)>
std::string HashEquivalence<T,thash>::str () const {
  std::ostringstream answer;
  answer << "HashEquivalence [" << std::endl;
  answer << "  parent   : " << parent.str() << std::endl;
  answer << "  root_size: " << root_size.str() << "]" << std::endl;
  return answer.str();
}


////////////////////////////////////////////////////////////////////////////////
//
//Commands

//Add the singleton a to the Equivalence.
//Throw an EquivalenceError (with a descriptive message) if the parameter a
//  already a value in the Equivalence (was previously added as a singleton).
template<class T, int (*thash) (const T& a)>
void HashEquivalence<T,thash>::add_singleton (const T& a) {
  //Write code here
        if (parent.has_key(a))
            throw EquivalenceError("Parameter was previously added as a singleton");
        parent[a] = a;
        root_size[a] = 1;
}


//Compress a and b to their roots.
//If they are in different equivalence classes, make the parent of the
//  root of the smaller-sized equivalence class refer to the root of the larger-
//  sized equivalence class; update the size of the root of the larger equivalence
//  class and remove the root of the smaller equivalence class from the root_size
//Throw an EquivalenceError (with a descriptive message) if the parameter a or b
//  is not already a value in the Equivalence (were never added as singletons)
template<class T, int (*thash) (const T& a)>
void HashEquivalence<T,thash>::merge_classes_of (const T& a, const T& b) {
  //Write code here
        if (!parent.has_key(a) || !parent.has_key(b))
            throw EquivalenceError("Parameters (1) or (2) were never added as singletons");
        if (in_same_class(a, b))
            return;
        T root_a = compress_to_root(a);
        T root_b = compress_to_root(b);
        if (root_size[root_a] > root_size[root_b]) {
            parent[root_b] = root_a;
            root_size[root_a] += root_size[root_b];
            root_size.erase(root_b);
        } else {
            parent[root_a] = root_b;
            root_size[root_b] += root_size[root_a];
            root_size.erase(root_a);
        }
}



////////////////////////////////////////////////////////////////////////////////
//
//Operators

template<class T, int (*thash) (const T& a)>
std::ostream& operator << (std::ostream& outs, const HashEquivalence<T,thash>& e) {
  outs << "HashEquivalence [" << std::endl;
  outs << "  parent map   : " << e.parent << std::endl;
  outs << "  root_size map: " << e.root_size << "]" << std::endl;
  return outs;
}


////////////////////////////////////////////////////////////////////////////////
//
//Helper methods

//Call compress_to_root as a helper method in_same_class and merge_classes_of.
//When finished, a and all its ancestors (but not descendants) should refer
//  (in the parent map) directly to the root of a's equivalence class.
//Throw an EquivalenceError (with a descriptive message) if the parameter a
//  is not already a value in the Equivalence (was never added as a singleton).
template<class T, int (*thash) (const T& a)>
T HashEquivalence<T,thash>::compress_to_root (T a) {
  //Write code here
        if (!parent.has_key(a))
            throw EquivalenceError("Parameter (1) was never added as a singleton");

        ics::HashSet<T, thash> set;
        while (parent[a] != a) {
            set.insert(a);
            a = parent[a];
        }
        T root = a;
        for (auto value: set)
            parent[value] = root;
        return root;
}


////////////////////////////////////////////////////////////////////////////////
//
//Miscellaneous methods (useful for testing/debugging)

//Compute all root heights (see method below), then locate/return the largest
template<class T, int (*thash) (const T& a)>
int HashEquivalence<T,thash>::max_height () const{
  int mh = 0;
  for (const pair<T,int>& h : heights())
    if (h.second > mh)
      mh = h.second;
  return mh;
}


//Compute/Return a map of all root heights.
//Don't compress any nodes here
template<class T, int (*thash) (const T& a)>
HashMap<T,int> HashEquivalence<T,thash>::heights () const {
  //Compute the depth of every node by tracing a path to its root;
  //  update the answer/height of the root if it is larger
  HashMap<T,int> answer(1,thash);
  for (const pair<T,T>& np : parent) {
    T e = np.first;
    int depth = 0;
    while (parent[e] != e) {
      e = parent[e];
      depth++;
    }
    if ( answer[e] < depth)
      answer[e] = depth;
  }

  return answer;
}


//Return string containing the parent, root_size, and height maps amd the maximum
//  height of any equivalence tree
template<class T, int (*thash) (const T& a)>
std::string HashEquivalence<T,thash>::equivalence_info () const {
  std::ostringstream answer;
  answer << "  parent map   : " << parent       << std::endl;
  answer << "  root_size map: " << root_size    << std::endl;
  answer << "  heights map  : " << heights()    << std::endl;
  answer << "  max height   : " << max_height() << std::endl;

  return answer.str();
}


}

#endif /* HASH_EQUIVALENCE_HPP_ */
