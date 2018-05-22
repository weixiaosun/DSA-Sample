// Submitter: huyanhh(Hoang, Huyanh)
// Partner  : monicadc(Cruz, Monica)
// We certify that we worked cooperatively on this programming
//   assignment, according to the rules for pair programming

#ifndef HASH_SET_HPP_
#define HASH_SET_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "pair.hpp"

using std::cout;
using std::endl;

namespace ics {


#ifndef undefinedhashdefined
#define undefinedhashdefined
template<class T>
int undefinedhash (const T& a) {return 0;}
#endif /* undefinedhashdefined */

//Instantiate the templated class supplying thash(a): produces a hash value for a.
//If thash is defaulted to undefinedhash in the template, then a constructor must supply chash.
//If both thash and chash are supplied, then they must be the same (by ==) function.
//If neither is supplied, or both are supplied but different, TemplateFunctionError is raised.
//The (unique) non-undefinedhash value supplied by thash/chash is stored in the instance variable hash.
template<class T, int (*thash)(const T& a) = undefinedhash<T>> class HashSet {
  public:
    typedef int (*hashfunc) (const T& a);

    //Destructor/Constructors
    ~HashSet ();

    HashSet (double the_load_threshold = 1.0, int (*chash)(const T& a) = undefinedhash<T>);
    explicit HashSet (int initial_bins, double the_load_threshold = 1.0, int (*chash)(const T& k) = undefinedhash<T>);
    HashSet (const HashSet<T,thash>& to_copy, double the_load_threshold = 1.0, int (*chash)(const T& a) = undefinedhash<T>);
    explicit HashSet (const std::initializer_list<T>& il, double the_load_threshold = 1.0, int (*chash)(const T& a) = undefinedhash<T>);

    //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
    template <class Iterable>
    explicit HashSet (const Iterable& i, double the_load_threshold = 1.0, int (*chash)(const T& a) = undefinedhash<T>);


    //Queries
    bool empty      () const;
    int  size       () const;
    bool contains   (const T& element) const;
    std::string str () const; //supplies useful debugging information; contrast to operator <<

    //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
    template <class Iterable>
    bool contains_all (const Iterable& i) const;


    //Commands
    int  insert (const T& element);
    int  erase  (const T& element);
    void clear  ();

    //Iterable class must support "for" loop: .begin()/.end() and prefix ++ on returned result

    template <class Iterable>
    int insert_all(const Iterable& i);

    template <class Iterable>
    int erase_all(const Iterable& i);

    template<class Iterable>
    int retain_all(const Iterable& i);


    //Operators
    HashSet<T,thash>& operator = (const HashSet<T,thash>& rhs);
    bool operator == (const HashSet<T,thash>& rhs) const;
    bool operator != (const HashSet<T,thash>& rhs) const;
    bool operator <= (const HashSet<T,thash>& rhs) const;
    bool operator <  (const HashSet<T,thash>& rhs) const;
    bool operator >= (const HashSet<T,thash>& rhs) const;
    bool operator >  (const HashSet<T,thash>& rhs) const;

    template<class T2, int (*hash2)(const T2& a)>
    friend std::ostream& operator << (std::ostream& outs, const HashSet<T2,hash2>& s);



  private:
    class LN;

  public:
    class Iterator {
      public:
        typedef pair<int,LN*> Cursor;

        //Private constructor called in begin/end, which are friends of HashSet<T,thash>
        ~Iterator();
        T           erase();
        std::string str  () const;
        HashSet<T,thash>::Iterator& operator ++ ();
        HashSet<T,thash>::Iterator  operator ++ (int);
        bool operator == (const HashSet<T,thash>::Iterator& rhs) const;
        bool operator != (const HashSet<T,thash>::Iterator& rhs) const;
        T& operator *  () const;
        T* operator -> () const;
        friend std::ostream& operator << (std::ostream& outs, const HashSet<T,thash>::Iterator& i) {
          outs << i.str(); //Use the same meaning as the debugging .str() method
          return outs;
        }
        friend Iterator HashSet<T,thash>::begin () const;
        friend Iterator HashSet<T,thash>::end   () const;

      private:
        //If can_erase is false, current indexes the "next" value (must ++ to reach it)
        Cursor              current; //Bin Index and Cursor; stops if LN* == nullptr
        HashSet<T,thash>*   ref_set;
        int                 expected_mod_count;
        bool                can_erase = true;

        //Helper methods
        void advance_cursors();

        //Called in friends begin/end
        Iterator(HashSet<T,thash>* iterate_over, bool from_begin);
    };


    Iterator begin () const;
    Iterator end   () const;


  private:
    class LN {
      public:
        LN ()                      {}
        LN (const LN& ln)          : value(ln.value), next(ln.next){}
        LN (T v,  LN* n = nullptr) : value(v), next(n){}

        T   value;
        LN* next   = nullptr;
    };

public:
  int (*hash)(const T& k);   //Hashing function used (from template or constructor)
private:
  LN** set      = nullptr;   //Pointer to array of pointers: each bin stores a list with a trailer node
  double load_threshold;     //used/bins <= load_threshold
  int bins      = 1;         //# bins in array (should start >= 1 so hash_compress doesn't % 0)
  int used      = 0;         //Cache for number of key->value pairs in the hash table
  int mod_count = 0;         //For sensing concurrent modification


  //Helper methods
  int   hash_compress        (const T& key)              const;  //hash function ranged to [0,bins-1]
  LN*   find_element         (const T& element)          const;  //Returns reference to element's node or nullptr
  LN*   copy_list            (LN*   l)                   const;  //Copy the elements in a bin (order irrelevant)
  LN**  copy_hash_table      (LN** ht, int bins)         const;  //Copy the bins/keys/values in ht tree (order in bins irrelevant)

  void  ensure_load_threshold(int new_used);                     //Reallocate if load_threshold > load_threshold
  void  delete_hash_table    (LN**& ht, int bins);               //Deallocate all LN in ht (and the ht itself; ht == nullptr)
};





//HashSet class and related definitions

////////////////////////////////////////////////////////////////////////////////
//
//Destructor/Constructors

template<class T, int (*thash)(const T& a)>
HashSet<T,thash>::~HashSet() {
}


template<class T, int (*thash)(const T& a)>
HashSet<T,thash>::HashSet(double the_load_threshold, int (*chash)(const T& element))
        :hash(thash != (hashfunc)undefinedhash<T> ? thash : chash) {
    if (hash == (hashfunc) undefinedhash<T>)
        throw TemplateFunctionError("HashSet::default constructor: neither specified");
    if (thash != chash && thash != (hashfunc)undefinedhash<T> && chash != (hashfunc)undefinedhash<T>)
        throw TemplateFunctionError("HashSet::default constructor: both specified and different");

    load_threshold = the_load_threshold;
    set = new LN*[bins];
    set[0] = new LN();
}


template<class T, int (*thash)(const T& a)>
HashSet<T,thash>::HashSet(int initial_bins, double the_load_threshold, int (*chash)(const T& element))
        :hash(thash != (hashfunc)undefinedhash<T> ? thash : chash) {
    if (hash == (hashfunc) undefinedhash<T>)
        throw TemplateFunctionError("BSTMap::default constructor: neither specified");
    if (thash != chash && thash!= (hashfunc)undefinedhash<T> && chash != (hashfunc)undefinedhash<T>)
        throw TemplateFunctionError("BSTMap::default constructor: both specified and different");

    load_threshold = the_load_threshold;
    bins = initial_bins;
    set = new LN*[bins];
    for (int i = 0; i < bins; i++)
        set[i] = new LN();
}


template<class T, int (*thash)(const T& a)>
HashSet<T,thash>::HashSet(const HashSet<T,thash>& to_copy, double the_load_threshold, int (*chash)(const T& element))
        :hash(thash != (hashfunc)undefinedhash<T> ? thash : chash) {
    if (hash == (hashfunc) undefinedhash<T>)
        hash = chash;
    if (thash != (hashfunc)undefinedhash<T> && chash != (hashfunc)undefinedhash<T> && thash != chash)
        throw TemplateFunctionError("HashMap::default constructor: both specified and different");

    bins = to_copy.bins;
    load_threshold = the_load_threshold;

    set = new LN*[bins];
    for (int i = 0; i < bins; i++)
        set[i] = new LN();

    if(hash == chash) {
        set = copy_hash_table(to_copy.set, to_copy.bins);
        used = to_copy.used;
    }
    else  { //put_all
        used = insert_all(to_copy);
        mod_count = 0;
    }
}


template<class T, int (*thash)(const T& a)>
HashSet<T,thash>::HashSet(const std::initializer_list<T>& il, double the_load_threshold, int (*chash)(const T& element))
        :hash(thash != (hashfunc)undefinedhash<T> ? thash : chash){

    if (hash == (hashfunc) undefinedhash<T>)
        throw TemplateFunctionError("HashSet::default constructor: neither specified");

    set = new LN*[bins];
    set[0] = new LN();
    load_threshold = the_load_threshold;
    for (const T& m_entry : il)
        insert(m_entry);
}


template<class T, int (*thash)(const T& a)>
template<class Iterable>
HashSet<T,thash>::HashSet(const Iterable& i, double the_load_threshold, int (*chash)(const T& a))
        :hash(thash != (hashfunc)undefinedhash<T> ? thash : chash){
    if (hash == (hashfunc) undefinedhash<T>)
        throw TemplateFunctionError("BSTMap::default constructor: neither specified");

    set = new LN*[bins];
    set[0] = new LN(); //
    load_threshold = the_load_threshold;
    for (const T v : i)
        insert(v);
}


////////////////////////////////////////////////////////////////////////////////
//
//Queries

template<class T, int (*thash)(const T& a)>
bool HashSet<T,thash>::empty() const {
    return used == 0;
}


template<class T, int (*thash)(const T& a)>
int HashSet<T,thash>::size() const {
    return used;
}


template<class T, int (*thash)(const T& a)>
bool HashSet<T,thash>::contains (const T& element) const {
    for (int i = 0; i < bins; i++)
        for (LN* p = set[i]; p->next != nullptr; p = p->next)
            if (p->value == element)
                return true;
    return false;
}


template<class T, int (*thash)(const T& a)>
std::string HashSet<T,thash>::str() const {
    std::ostringstream answer;
    for(int i = 0; i < bins; i++)
        for (LN *p = set[i]; p != nullptr; p = p->next) {
            if (p->next != nullptr)
                answer << p->value;
        }
    return answer.str();
}


template<class T, int (*thash)(const T& a)>
template <class Iterable>
bool HashSet<T,thash>::contains_all(const Iterable& i) const {
    for (const T& m_entry : i)
        if (!contains(m_entry))
            return false;
    return true;
}


////////////////////////////////////////////////////////////////////////////////
//
//Commands

template<class T, int (*thash)(const T& a)>
int HashSet<T,thash>::insert(const T& element) {
    if(!contains(element))
        ensure_load_threshold(++used);
    mod_count++;
    for (LN* p = set[hash_compress(element)]; p != nullptr; p = p->next)
        if (p->value == element && p->value != T())
            return 0;
        else if (p->next == nullptr) {
            p->value = element;
            p->next = new LN(T(), nullptr);
            return 1;
        }
}


template<class T, int (*thash)(const T& a)>
int HashSet<T,thash>::erase(const T& element) {
    if (find_element(element) == nullptr) {
        return 0;
    }

    LN *p = find_element(element);
    LN* to_delete = p->next;
    p->value = to_delete->value;
    p->next = to_delete->next;
    ++mod_count;
    used--;
    delete to_delete;
    return 1;
}


template<class T, int (*thash)(const T& a)>
void HashSet<T,thash>::clear() {

    for (int i = 0; i < bins; i++)
    {
        for (LN *p = set[i]; p != nullptr;) {
            if (p->next == nullptr)
            {
                set[i] = p;
                p = p->next;
            }
            else {
                LN* to_delete = p;
                p = p->next;
                delete to_delete;
            }
        }
    }

    used = 0;
    mod_count++;
}


template<class T, int (*thash)(const T& a)>
template<class Iterable>
int HashSet<T,thash>::insert_all(const Iterable& i) {
    int count = 0;
    for(auto &entry: i) {
        count++;
        insert(entry);
    }
    return count;
}


template<class T, int (*thash)(const T& a)>
template<class Iterable>
int HashSet<T,thash>::erase_all(const Iterable& i) {
    int count = 0;
    for(auto &entry: i) {
        count++;
        erase(entry);
    }
    return count;
}


template<class T, int (*thash)(const T& a)>
template<class Iterable>
int HashSet<T,thash>::retain_all(const Iterable& i) {
    HashSet<T,thash> s(i);
    int count = 0;
    for (int i = 0; i < bins; i++)
        for (LN* p = set[i]; p->next != nullptr;)
        {
            if (!s.contains(p->value)) {
                erase(p->value);
                ++count;
            } else
                p = p->next;
        }
    return count;
}


////////////////////////////////////////////////////////////////////////////////
//
//Operators

template<class T, int (*thash)(const T& a)>
HashSet<T,thash>& HashSet<T,thash>::operator = (const HashSet<T,thash>& rhs) {
    if (this == &rhs)
        return *this;

    hash = rhs.hash;
//
//    cout << "start" << endl;
//    cout << bins << endl;
//    cout << used << endl;
//    cout << mod_count << endl;

    set = new LN*[bins];
    for (int i = 0; i < bins; i++)
        set[i] = new LN();
    load_threshold = rhs.load_threshold;
    bins = rhs.bins;
    used = rhs.used;

//    cout << "----" << endl;
    set = copy_hash_table(rhs.set, rhs.bins);

//    cout << bins << endl;
//    cout << used << endl;
//    cout << mod_count << endl;

    ++mod_count;
    return *this;
}


template<class T, int (*thash)(const T& a)>
bool HashSet<T,thash>::operator == (const HashSet<T,thash>& rhs) const {
    if (this == &rhs)
        return true;

    if (used != rhs.size())
        return false;


    for (int i=0; i<bins; ++i) {
        for (LN* p = set[i]; p != nullptr; p = p->next)
        {
            if(p->next != nullptr)
                if (!contains(p->value))
                    return false;
        }
    }
    return true;
}


template<class T, int (*thash)(const T& a)>
bool HashSet<T,thash>::operator != (const HashSet<T,thash>& rhs) const {
    return !(*this == rhs);
}


template<class T, int (*thash)(const T& a)>
bool HashSet<T,thash>::operator <= (const HashSet<T,thash>& rhs) const {
    if (used <= rhs.used)
        return true;
    return false;
}

template<class T, int (*thash)(const T& a)>
bool HashSet<T,thash>::operator < (const HashSet<T,thash>& rhs) const {
    if (used < rhs.used)
        return true;
    return false;
}


template<class T, int (*thash)(const T& a)>
bool HashSet<T,thash>::operator >= (const HashSet<T,thash>& rhs) const {
    return !(*this < rhs);
}


template<class T, int (*thash)(const T& a)>
bool HashSet<T,thash>::operator > (const HashSet<T,thash>& rhs) const {
    return !(*this <= rhs);
}


template<class T, int (*thash)(const T& a)>
std::ostream& operator << (std::ostream& outs, const HashSet<T,thash>& s) {
    outs << "set[";
    outs << s.str();
    outs << "]";
    return outs;
}


////////////////////////////////////////////////////////////////////////////////
//
//Iterator constructors

template<class T, int (*thash)(const T& a)>
auto HashSet<T,thash>::begin () const -> HashSet<T,thash>::Iterator {
    return Iterator(const_cast<HashSet<T,thash>*>(this), true);
}


template<class T, int (*thash)(const T& a)>
auto HashSet<T,thash>::end () const -> HashSet<T,thash>::Iterator {
    return Iterator(const_cast<HashSet<T,thash>*>(this), false);
}


////////////////////////////////////////////////////////////////////////////////
//
//Private helper methods

template<class T, int (*thash)(const T& a)>
int HashSet<T,thash>::hash_compress (const T& element) const {
    return abs(hash(element)) % bins;
}


template<class T, int (*thash)(const T& a)>
typename HashSet<T,thash>::LN* HashSet<T,thash>::find_element (const T& element) const {
    for (int i = 0;i < bins; i++)
        for (LN* p = set[i];p != nullptr; p = p->next)
            if (p->value == element)
                return p;
    return nullptr;
}

template<class T, int (*thash)(const T& a)>
typename HashSet<T,thash>::LN* HashSet<T,thash>::copy_list (LN* l) const {
    LN* to_return = new LN();
    LN* lhs = to_return;
    for (LN* p = l; p != nullptr; p = p->next, lhs = lhs->next) {
        if(p->next == nullptr)
            lhs = new LN();
        else {
            lhs->value = p->value;
            lhs->next = p->next;
        }
    }
    return to_return;
}


template<class T, int (*thash)(const T& a)>
typename HashSet<T,thash>::LN** HashSet<T,thash>::copy_hash_table (LN** ht, int bins) const {
    LN** return_map = new LN*[bins];
    for(int i = 0; i < bins; i++)
        return_map[i] = copy_list(ht[i]);

    return return_map;
}


template<class T, int (*thash)(const T& a)>
void HashSet<T,thash>::ensure_load_threshold(int new_used) {
    if (new_used / bins <= load_threshold)
        return;
    LN** old_set = set;
    bins *= 2;
    set = new LN*[bins];
    for (int i = 0; i < bins; i++)
        set[i] = new LN();
    for (int i = 0; i < bins/2; i++) {
        for (LN *p = old_set[i]; p != nullptr; p = p->next) {
            if (p->next != nullptr)
                set[hash_compress(p->value)] = new LN(p->value, set[hash_compress(p->value)]);
        }
    }
    delete old_set;
}


template<class T, int (*thash)(const T& a)>
void HashSet<T,thash>::delete_hash_table (LN**& ht, int bins) {
    for (int i = 0; i < bins; i++)
        for (LN *p = ht[i]; p != nullptr;) {
            LN* to_delete = p;
            p = p->next;
            delete to_delete;
        }
    delete[] ht;
    ht = nullptr;
}






////////////////////////////////////////////////////////////////////////////////
//
//Iterator class definitions

template<class T, int (*thash)(const T& a)>
void HashSet<T,thash>::Iterator::advance_cursors() {
    if (current.second->next->next != nullptr) {
        current.second = current.second->next;
    }
    else
    {
        for (int i = current.first + 1; i < ref_set->bins; i++) {
            for (LN* p = ref_set->set[i]; p->next != nullptr; p = p->next)
            {
                current.first = i;
                current.second = p;
                return;
            }
        }
        current.first = -1;
        current.second = nullptr;
    }
}


template<class T, int (*thash)(const T& a)>
HashSet<T,thash>::Iterator::Iterator(HashSet<T,thash>* iterate_over, bool begin)
        : ref_set(iterate_over) {
    expected_mod_count = ref_set->mod_count;

    if(!begin || ref_set->empty())
    {
        current.first = -1;
        current.second = nullptr;
    }
    else
    {
        for(int i = 0; i < ref_set->bins; i++)
        {
            for(LN* p = ref_set->set[i]; p != nullptr; p = p->next)
                if(p->next != nullptr) {
                    current.first = i;
                    current.second = p;
                    goto theEnd; //DANGER
                }
        }
        theEnd:;
    }
}


template<class T, int (*thash)(const T& a)>
HashSet<T,thash>::Iterator::~Iterator()
{}


template<class T, int (*thash)(const T& a)>
T HashSet<T,thash>::Iterator::erase() {
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("HashMap::Iterator::erase");
    if (!can_erase)
        throw CannotEraseError("HashMap::Iterator::erase Iterator cursor already erased");
    if (current.second == nullptr)
        throw CannotEraseError("HashMap::Iterator::erase Iterator cursor beyond data structure");

    can_erase = false;
    T to_return = current.second->value;
    LN* to_erase = current.second;
    if (current.second->next->next == nullptr)
        advance_cursors();
    ref_set->erase(to_erase->value);
    expected_mod_count = ref_set->mod_count;

    return to_return;
}


template<class T, int (*thash)(const T& a)>
std::string HashSet<T,thash>::Iterator::str() const {
    std::ostringstream answer;
    answer << ref_set->str();

    return answer.str();
}


template<class T, int (*thash)(const T& a)>
auto  HashSet<T,thash>::Iterator::operator ++ () -> HashSet<T,thash>::Iterator& {
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("ArrayMap::Iterator::operator ++");

    if(current.second == nullptr)
        return *this;

    if (can_erase)
        advance_cursors();
    else
        can_erase = true;  //current already indexes "one beyond" deleted value

    return *this;
}


template<class T, int (*thash)(const T& a)>
auto  HashSet<T,thash>::Iterator::operator ++ (int) -> HashSet<T,thash>::Iterator {
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("ArrayMap::Iterator::operator ++(int)");

    if (current.second == nullptr)
        return *this;

    Iterator to_return(*this);
    if (can_erase)
        advance_cursors();
    else
        can_erase = true;  //current already indexes "one beyond" deleted value

    return to_return;
}


template<class T, int (*thash)(const T& a)>
bool HashSet<T,thash>::Iterator::operator == (const HashSet<T,thash>::Iterator& rhs) const {
    const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
    if (rhsASI == 0)
        throw IteratorTypeError("ArrayMap::Iterator::operator ==");
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("ArrayMap::Iterator::operator ==");
    if (ref_set != rhsASI->ref_set)
        throw ComparingDifferentIteratorsError("ArrayMap::Iterator::operator ==");


    return current.second == rhsASI->current.second;
}


template<class T, int (*thash)(const T& a)>
bool HashSet<T,thash>::Iterator::operator != (const HashSet<T,thash>::Iterator& rhs) const {
    const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
    if (rhsASI == 0)
        throw IteratorTypeError("ArrayMap::Iterator::operator !=");
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("ArrayMap::Iterator::operator !=");
    if (ref_set != rhsASI->ref_set)
        throw ComparingDifferentIteratorsError("ArrayMap::Iterator::operator !=");

    return current.second != rhsASI->current.second;
}

template<class T, int (*thash)(const T& a)>
T& HashSet<T,thash>::Iterator::operator *() const {
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("ArrayMap::Iterator::operator *");
    if (!can_erase || current.second == nullptr) {
        std::ostringstream where;
        where << current << " when size = " << ref_set->size();
        throw IteratorPositionIllegal("ArrayMap::Iterator::operator * Iterator illegal: "+where.str());
    }
    return current.second->value;
}

template<class T, int (*thash)(const T& a)>
T* HashSet<T,thash>::Iterator::operator ->() const {
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("ArrayMap::Iterator::operator ->");
    if (!can_erase || current.second == nullptr) {
        std::ostringstream where;
        where << current << " when size = " << ref_set->size();
        throw IteratorPositionIllegal("ArrayMap::Iterator::operator -> Iterator illegal: "+where.str());
    }
    return &current.second->value;
}

}

#endif /* HASH_SET_HPP_ */
