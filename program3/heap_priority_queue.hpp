// Submitter: huyanhh(Hoang, Huyanh)
// Partner  : monicadc(Cruz, Monica)
// We certify that we worked cooperatively on this programming
//   assignment, according to the rules for pair programming

#ifndef HEAP_PRIORITY_QUEUE_HPP_
#define HEAP_PRIORITY_QUEUE_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include <utility>              //For std::swap function
#include "array_stack.hpp"      //See operator <<

using std::cout;
using std::endl;

namespace ics {


#ifndef undefinedgtdefined
#define undefinedgtdefined
template<class T>
bool undefinedgt (const T& a, const T& b) {return false;}
#endif /* undefinedgtdefined */

//Instantiate the templated class supplying tgt(a,b): true, iff a has higher priority than b.
//If tgt is defaulted to undefinedgt in the template, then a constructor must supply cgt.
//If both tgt and cgt are supplied, then they must be the same (by ==) function.
//If neither is supplied, or both are supplied but different, TemplateFunctionError is raised.
//The (unique) non-undefinedgt value supplied by tgt/cgt is stored in the instance variable gt.
template<class T, bool (*tgt)(const T &a, const T &b) = undefinedgt < T>>

class HeapPriorityQueue {
public:
    typedef bool (*gtfunc)(const T &a, const T &b);

    //Destructor/Constructors
    ~HeapPriorityQueue();

    HeapPriorityQueue(bool (*cgt)(const T &a, const T &b) = undefinedgt < T >);

    explicit HeapPriorityQueue(int initial_length, bool (*cgt)(const T &a, const T &b) = undefinedgt < T >);

    HeapPriorityQueue(const HeapPriorityQueue<T, tgt> &to_copy,
                      bool (*cgt)(const T &a, const T &b) = undefinedgt < T >);

    explicit HeapPriorityQueue(const std::initializer_list<T> &il,
                               bool (*cgt)(const T &a, const T &b) = undefinedgt < T >);

    //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
    template<class Iterable>
    explicit HeapPriorityQueue(const Iterable &i, bool (*cgt)(const T &a, const T &b) = undefinedgt < T >);


    //Queries
    bool empty() const;

    int size() const;

    T &peek() const;

    std::string str() const; //supplies useful debugging information; contrast to operator <<


    //Commands
    int enqueue(const T &element);

    T dequeue();

    void clear();

    //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
    template<class Iterable>
    int enqueue_all(const Iterable &i);


    //Operators
    HeapPriorityQueue<T, tgt> &operator=(const HeapPriorityQueue<T, tgt> &rhs);

    bool operator==(const HeapPriorityQueue<T, tgt> &rhs) const;

    bool operator!=(const HeapPriorityQueue<T, tgt> &rhs) const;

    template<class T2, bool (*gt2)(const T2 &a, const T2 &b)>
    friend std::ostream &operator<<(std::ostream &outs, const HeapPriorityQueue<T2, gt2> &pq);


    class Iterator {
    public:
        //Private constructor called in begin/end, which are friends of HeapPriorityQueue<T,tgt>
        ~Iterator();

        T erase();

        std::string str() const;

        HeapPriorityQueue<T, tgt>::Iterator &operator++();

        HeapPriorityQueue<T, tgt>::Iterator operator++(int);

        bool operator==(const HeapPriorityQueue<T, tgt>::Iterator &rhs) const;

        bool operator!=(const HeapPriorityQueue<T, tgt>::Iterator &rhs) const;

        T &operator*() const;

        T *operator->() const;

        friend std::ostream &operator<<(std::ostream &outs, const HeapPriorityQueue<T, tgt>::Iterator &i) {
            outs << i.str(); //Use the same meaning as the debugging .str() method
            return outs;
        }

        friend Iterator HeapPriorityQueue<T, tgt>::begin() const;

        friend Iterator HeapPriorityQueue<T, tgt>::end() const;

    private:
        //If can_erase is false, the value has been removed from "it" (++ does nothing)
        HeapPriorityQueue<T, tgt> it;                 //copy of HPQ (from begin), to use as iterator via dequeue
        HeapPriorityQueue<T, tgt> *ref_pq;
        int expected_mod_count;
        bool can_erase = true;

        //Called in friends begin/end
        //These constructors have different initializers (see it(...) in first one)
        Iterator(HeapPriorityQueue<T, tgt> *iterate_over, bool from_begin);    // Called by begin
        Iterator(HeapPriorityQueue<T, tgt> *iterate_over);                     // Called by end
    };


    Iterator begin() const;

    Iterator end() const;


private:
    bool (*gt)(const T &a, const T &b); //The gt used by enqueue (from template or constructor)
    T *pq;                              //Array represents a heap, so it uses the heap ordering property
    int length = 0;                   //Physical length of array: must be >= .size()
    int used = 0;                   //Amount of array used:  invariant: 0 <= used <= length
    int mod_count = 0;                   //For sensing concurrent modification


    //Helper methods
    void ensure_length(int new_length);
    int left_child(int i) const;         //Useful abstractions for heaps as arrays
    int right_child(int i) const;
    int parent(int i) const;
    bool is_root(int i) const;
    bool in_heap(int i) const;
    void percolate_up(int i);
    void percolate_down(int i);
    void heapify();                   // Percolate down all value is array (from indexes used-1 to 0): O(N)
};





////////////////////////////////////////////////////////////////////////////////
//
//HeapPriorityQueue class and related definitions

//Destructor/Constructors

template<class T, bool (*tgt)(const T &a, const T &b)>
HeapPriorityQueue<T, tgt>::~HeapPriorityQueue() {
    delete[] pq;
}


template<class T, bool (*tgt)(const T &a, const T &b)>
HeapPriorityQueue<T, tgt>::HeapPriorityQueue(bool (*cgt)(const T &a, const T &b))
        : gt(tgt != (gtfunc) undefinedgt<T> ? tgt : cgt) {
    if (gt == (gtfunc) undefinedgt<T>)
        throw TemplateFunctionError("HeapPriorityQueue::default constructor: neither specified");
    if (tgt != (gtfunc) undefinedgt<T> && cgt != (gtfunc) undefinedgt<T> && tgt != cgt)
        throw TemplateFunctionError("HeapPriorityQueue::default constructor: both specified and different");

    pq = new T[length];
}


template<class T, bool (*tgt)(const T &a, const T &b)>
HeapPriorityQueue<T, tgt>::HeapPriorityQueue(int initial_length, bool (*cgt)(const T &a, const T &b))
        : gt(tgt != (gtfunc) undefinedgt<T> ? tgt : cgt), length(initial_length) {
    if (gt == (gtfunc) undefinedgt<T>)
        throw TemplateFunctionError("HeapPriorityQueue::length constructor: neither specified");
    if (tgt != (gtfunc) undefinedgt<T> && cgt != (gtfunc) undefinedgt<T> && tgt != cgt)
        throw TemplateFunctionError("HeapPriorityQueue::length constructor: both specified and different");

    if (length < 0)
        length = 0;
    pq = new T[length];
}


template<class T, bool (*tgt)(const T &a, const T &b)>
HeapPriorityQueue<T, tgt>::HeapPriorityQueue(const HeapPriorityQueue<T, tgt> &to_copy,
                                             bool (*cgt)(const T &a, const T &b))
: gt(tgt != (gtfunc)undefinedgt<T> ? tgt : cgt), length(to_copy.length) {
    if (tgt != (gtfunc)undefinedgt<T> && cgt != (gtfunc)undefinedgt<T> && tgt != cgt)
        throw TemplateFunctionError("HeapPriorityQueue::copy constructor: both specified and different");

    used = to_copy.used;
    pq = new T[length];

    for (int i = 0; i < used; ++i)
        pq[i] = to_copy.pq[i];

    if (gt == (gtfunc)undefinedgt<T>) { // if one gt function only
        gt = to_copy.gt;

    } else if (gt != to_copy.gt) { // otherwise if two diff gt functions
        heapify();
    }
}


template<class T, bool (*tgt)(const T &a, const T &b)>
HeapPriorityQueue<T, tgt>::HeapPriorityQueue(const std::initializer_list<T> &il,
                                             bool (*cgt)(const T &a, const T &b))
        : gt(tgt != (gtfunc)undefinedgt<T> ? tgt : cgt), length(il.size()){
    std::cout << "by the way, our initializer list function takes ~111814ms to run, please wait" << std::endl;
    if (gt == (gtfunc)undefinedgt<T>)
        throw TemplateFunctionError("HeapPriorityQueue::initializer_list constructor: neither specified");
    if (tgt != (gtfunc)undefinedgt<T> && cgt != (gtfunc)undefinedgt<T> && tgt != cgt)
        throw TemplateFunctionError("HeapPriorityQueue::initializer_list constructor: both specified and different");

    pq = new T[length];
    int i = 0;
    for (const T& pq_elem : il)
        pq[i++] = pq_elem;

    used = length;
    heapify();
}


template<class T, bool (*tgt)(const T &a, const T &b)>
template<class Iterable>
HeapPriorityQueue<T, tgt>::HeapPriorityQueue(const Iterable &i, bool (*cgt)(const T &a, const T &b))
    : gt(tgt != (gtfunc)undefinedgt<T> ? tgt : cgt), length(i.size()) {
        if (gt == (gtfunc)undefinedgt<T>)
            throw TemplateFunctionError("HeapPriorityQueue::Iterable constructor: neither specified");
        if (tgt != (gtfunc)undefinedgt<T> && cgt != (gtfunc)undefinedgt<T> && tgt != cgt)
            throw TemplateFunctionError("HeapPriorityQueue::Iterable constructor: both specified and different");

        pq = new T[length];
        for (const T& v : i)
            enqueue(v);

        used = length;
        heapify();
    }


////////////////////////////////////////////////////////////////////////////////
//
//Queries

template<class T, bool (*tgt)(const T &a, const T &b)>
bool HeapPriorityQueue<T, tgt>::empty() const {
    return used == 0;
}


template<class T, bool (*tgt)(const T &a, const T &b)>
int HeapPriorityQueue<T, tgt>::size() const {
    return used;
}


template<class T, bool (*tgt)(const T &a, const T &b)>
T &HeapPriorityQueue<T, tgt>::peek() const {
    if (empty())
        throw EmptyError("HeapPriorityQueue::peek");

    return pq[0];
}


template<class T, bool (*tgt)(const T &a, const T &b)>
std::string HeapPriorityQueue<T, tgt>::str() const {
    std::ostringstream answer;
    answer << "HeapPriorityQueue[";

    if (length != 0) {
        answer << "0:" << pq[0];
        for (int i = 1; i < length; ++i)
            answer << "," << i << ":" << pq[i];
    }

    answer << "](length=" << length << ",used=" << used << ",mod_count=" << mod_count << ")";
    return answer.str();
}


////////////////////////////////////////////////////////////////////////////////
//
//Commands

template<class T, bool (*tgt)(const T &a, const T &b)>
int HeapPriorityQueue<T, tgt>::enqueue(const T &element) {
    ensure_length(length + 1);
    pq[used++] = element;
    percolate_up(used - 1);
    ++mod_count;
    return 1;
}


template<class T, bool (*tgt)(const T &a, const T &b)>
T HeapPriorityQueue<T, tgt>::dequeue() {
    if (empty()) {
        throw EmptyError("HeapPriorityQueue::dequeue");
    }
    T to_return = pq[0];
    pq[0] = pq[used - 1];
    used--;
    percolate_down(0);
    ++mod_count;
    return to_return;
}


template<class T, bool (*tgt)(const T &a, const T &b)>
void HeapPriorityQueue<T, tgt>::clear() {
    used = 0;
    ++mod_count;
}


template<class T, bool (*tgt)(const T &a, const T &b)>
template<class Iterable>
int HeapPriorityQueue<T, tgt>::enqueue_all(const Iterable &i) {
    int count = 0;
    for (const T& v : i)
        count += enqueue(v);

    return count;
}


////////////////////////////////////////////////////////////////////////////////
//
//Operators

template<class T, bool (*tgt)(const T &a, const T &b)>
HeapPriorityQueue<T, tgt> &HeapPriorityQueue<T, tgt>::operator=(const HeapPriorityQueue<T, tgt> &rhs) {
    if (this == &rhs)
        return *this;

    gt = rhs.gt;
    this->ensure_length(rhs.used);
    used = rhs.used;
    for (int i = 0; i < used; ++i)
        pq[i] = rhs.pq[i];

    heapify();

    ++mod_count;
    return *this;
}


template<class T, bool (*tgt)(const T &a, const T &b)>
bool HeapPriorityQueue<T, tgt>::operator==(const HeapPriorityQueue<T, tgt> &rhs) const {
    if (this == &rhs)
        return true;

    if (gt != rhs.gt) //For PriorityQueues to be equal, they need the same gt function, and values
        return false;

    if (used != rhs.size())
        return false;

    HeapPriorityQueue<T, tgt>::Iterator rhs_i = rhs.begin();
    HeapPriorityQueue<T, tgt>::Iterator lhs_i = this->begin();
    for (; lhs_i != this->end(); ++lhs_i, ++rhs_i)
        if (*lhs_i != *rhs_i)
            return false;

    return true;
}


template<class T, bool (*tgt)(const T &a, const T &b)>
bool HeapPriorityQueue<T, tgt>::operator!=(const HeapPriorityQueue<T, tgt> &rhs) const {
    return !(*this == rhs);
}


template<class T, bool (*tgt)(const T &a, const T &b)>
std::ostream &operator<<(std::ostream &outs, const HeapPriorityQueue<T, tgt> &p) {
    outs << "priority_queue[";
    ArrayStack<T> stack;

    for (auto i = p.begin(); i != p.end(); i++)
        stack.push(*i);

    if (!p.empty()) {
        outs << stack.pop();
        for (int j = 1; j < p.used; j++)
            outs << "," << stack.pop();
    }

    outs << "]:highest";
    return outs;
}


////////////////////////////////////////////////////////////////////////////////
//
//Iterator constructors

template<class T, bool (*tgt)(const T &a, const T &b)>
auto HeapPriorityQueue<T, tgt>::begin() const -> HeapPriorityQueue<T, tgt>::Iterator {
    return Iterator(const_cast<HeapPriorityQueue<T, tgt> *>(this), true);
}


template<class T, bool (*tgt)(const T &a, const T &b)>
auto HeapPriorityQueue<T, tgt>::end() const -> HeapPriorityQueue<T, tgt>::Iterator {
    return Iterator(const_cast<HeapPriorityQueue<T, tgt> *>(this));
}


////////////////////////////////////////////////////////////////////////////////
//
//Private helper methods

template<class T, bool (*tgt)(const T &a, const T &b)>
void HeapPriorityQueue<T, tgt>::ensure_length(int new_length) {
    if (length >= new_length)
        return;
    T *old_pq = pq;
    length = std::max(new_length, 2 * length);
    pq = new T[length];
    for (int i = 0; i < used; ++i)
        pq[i] = old_pq[i];

    delete[] old_pq;
}


template<class T, bool (*tgt)(const T &a, const T &b)>
int HeapPriorityQueue<T, tgt>::left_child(int i) const {
    return (2 * i) + 1;
}

template<class T, bool (*tgt)(const T &a, const T &b)>
int HeapPriorityQueue<T, tgt>::right_child(int i) const {
    return (2 * i) + 2;
}

template<class T, bool (*tgt)(const T &a, const T &b)>
int HeapPriorityQueue<T, tgt>::parent(int i) const {
    return (i - 1) / 2;
}

template<class T, bool (*tgt)(const T &a, const T &b)>
bool HeapPriorityQueue<T, tgt>::is_root(int i) const {
    return i == 0;
}

template<class T, bool (*tgt)(const T &a, const T &b)>
bool HeapPriorityQueue<T, tgt>::in_heap(int i) const {
    return i < (used - 1);
}


template<class T, bool (*tgt)(const T &a, const T &b)>
void HeapPriorityQueue<T, tgt>::percolate_up(int i) {
    for (; gt(pq[i], pq[parent(i)]); i = parent(i)) { // if i = 0 then parent(i) will return 0
        std::swap(pq[parent(i)], pq[i]);
    }
}


template<class T, bool (*tgt)(const T &a, const T &b)>
void HeapPriorityQueue<T, tgt>::percolate_down(int i) {
    if (left_child(i) > used - 1) { // if no children
        return;
    } else if (right_child(i) > used - 1) {
        if (gt(pq[i], pq[left_child(i)]))
            return;
        else {
            std::swap(pq[left_child(i)], pq[i]);
            percolate_down(left_child(i));
        }
    }
    else if (gt(pq[i], pq[left_child(i)]) && gt(pq[i], pq[right_child(i)])) {
        return;
    } else if (gt(pq[left_child(i)], pq[right_child(i)])) {
        std::swap(pq[left_child(i)], pq[i]);
        percolate_down(left_child(i));
    } else {
        std::swap(pq[right_child(i)], pq[i]);
        percolate_down(right_child(i));
    }
//    while (in_heap(i)) {
//        if (in_heap(left_child(i)) && !in_heap(right_child(i))) { // one child
//            if (gt(pq[left_child(i)], pq[i])) {
//                std::swap(pq[left_child(i)], pq[i]);
//                i = left_child(i);
//            } else
//                return;
//        } else if (!in_heap(left_child(i)) && !in_heap(right_child(i))) { // no children
//            return;
//        } else { // two children
//            if (gt(pq[i], pq[left_child(i)]) && gt(pq[i], pq[right_child(i)])) // if parent greater than both children
//                return;
//            else {
//                if (gt(pq[left_child(i)], pq[right_child(i)])) {
//                    std::swap(pq[left_child(i)], pq[i]);
//                    i = left_child(i);
//                } else {
//                    std::swap(pq[right_child(i)], pq[i]);
//                    i = right_child(i);
//                }
//            }
//        }
//    }
}


template<class T, bool (*tgt)(const T &a, const T &b)>
void HeapPriorityQueue<T, tgt>::heapify() {
    for (int i = used - 1; i >= 0; --i)
        percolate_down(i);
}


////////////////////////////////////////////////////////////////////////////////
//
//Iterator class definitions

template<class T, bool (*tgt)(const T &a, const T &b)>
HeapPriorityQueue<T, tgt>::Iterator::Iterator(HeapPriorityQueue<T, tgt> *iterate_over, bool tgt_nullptr)
{
    ref_pq = iterate_over;
    it = *ref_pq;
    expected_mod_count = ref_pq->mod_count;
}


template<class T, bool (*tgt)(const T &a, const T &b)>
HeapPriorityQueue<T, tgt>::Iterator::Iterator(HeapPriorityQueue<T, tgt> *iterate_over) {
    ref_pq = iterate_over;
    expected_mod_count = ref_pq->mod_count;
}


template<class T, bool (*tgt)(const T &a, const T &b)>
HeapPriorityQueue<T, tgt>::Iterator::~Iterator() {}


template<class T, bool (*tgt)(const T &a, const T &b)>
T HeapPriorityQueue<T, tgt>::Iterator::erase() {
    if (expected_mod_count != ref_pq->mod_count)
        throw ConcurrentModificationError("HeapPriorityQueue::Iterator::erase");
    if (!can_erase)
        throw CannotEraseError("HeapPriorityQueue::Iterator::erase Iterator cursor already erased");
    if (it.size() == 0)
        throw CannotEraseError("HeapPriorityQueue::Iterator::erase Iterator cursor beyond data structure");

    can_erase = false;

    T to_return = it.dequeue();
    for (int i = 0; i < ref_pq->size() - 1; i++) {
        if (ref_pq->pq[i] == to_return) {
            ref_pq->pq[i] = ref_pq->pq[ref_pq->size() - 1];
            if (ref_pq->gt(ref_pq->pq[i], ref_pq->pq[ref_pq->parent(i)]))
                ref_pq->percolate_up(i);
            else
                ref_pq->percolate_down(i);
        }
    }
    ref_pq->used--;
    return to_return;
}


template<class T, bool (*tgt)(const T &a, const T &b)>
std::string HeapPriorityQueue<T, tgt>::Iterator::str() const {
    std::ostringstream answer;
    answer << ref_pq->str() << "/expected_mod_count=" << expected_mod_count << "/can_erase=" << can_erase;
    return answer.str();
}


template<class T, bool (*tgt)(const T &a, const T &b)>
auto HeapPriorityQueue<T, tgt>::Iterator::operator++() -> HeapPriorityQueue<T, tgt>::Iterator & {
    if (expected_mod_count != ref_pq->mod_count)
        throw ConcurrentModificationError("HeapPriorityQueue::Iterator::operator ++");
    //If can_erase is false, the value has been removed from "it" (++ does nothing)
    if (it.empty())
        return *this;

    if (can_erase)
        it.dequeue();
    else
        can_erase = true;  //current already indexes "one beyond" deleted value

    return *this;
}


template<class T, bool (*tgt)(const T &a, const T &b)>
auto HeapPriorityQueue<T, tgt>::Iterator::operator++(int) -> HeapPriorityQueue<T, tgt>::Iterator {
    if (expected_mod_count != ref_pq->mod_count)
        throw ConcurrentModificationError("HeapPriorityQueue::Iterator::operator ++");

    if (it.empty())
        return *this;

    Iterator to_return(*this);
    if (can_erase)
        it.dequeue();
    else
        can_erase = true;  //current already indexes "one beyond" deleted value

    return to_return;
}


template<class T, bool (*tgt)(const T &a, const T &b)>
bool HeapPriorityQueue<T, tgt>::Iterator::operator==(const HeapPriorityQueue<T, tgt>::Iterator &rhs) const {
    const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
    if (rhsASI == 0)
        throw IteratorTypeError("HeapPriorityQueue::Iterator::operator ==");
    if (expected_mod_count != ref_pq->mod_count)
        throw ConcurrentModificationError("HeapPriorityQueue::Iterator::operator ==");
    if (ref_pq != rhsASI->ref_pq)
        throw ComparingDifferentIteratorsError("HeapPriorityQueue::Iterator::operator ==");

    if (it.size() != rhsASI->it.size())
        return false;
    if (it.size() == 0 || rhsASI->it.size() == 0)
        return it.size() == rhsASI->it.size();

    return it.peek() == rhsASI->it.peek();
}


template<class T, bool (*tgt)(const T &a, const T &b)>
bool HeapPriorityQueue<T, tgt>::Iterator::operator!=(const HeapPriorityQueue<T, tgt>::Iterator &rhs) const {
    const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
    if (rhsASI == 0)
        throw IteratorTypeError("HeapPriorityQueue::Iterator::operator !=");
    if (expected_mod_count != ref_pq->mod_count)
        throw ConcurrentModificationError("HeapPriorityQueue::Iterator::operator !=");
    if (ref_pq != rhsASI->ref_pq)
        throw ComparingDifferentIteratorsError("HeapPriorityQueue::Iterator::operator !=");

    if (it.size() != rhsASI->it.size())
        return true;
    if (it.size() == 0 || rhsASI->it.size() == 0)
        return it.size() != rhsASI->it.size();

    return it.peek() != rhsASI->it.peek();
}


template<class T, bool (*tgt)(const T &a, const T &b)>
T &HeapPriorityQueue<T, tgt>::Iterator::operator*() const {
    if (expected_mod_count != ref_pq->mod_count)
        throw ConcurrentModificationError("HepatitisBPriorityQueue::Iterator::operator *");
    if (!can_erase || it.size() < 1) {
        std::ostringstream where;
        where << " when size = " << ref_pq->size();
        throw IteratorPositionIllegal("HeapPriorityQueue::Iterator::operator * Iterator illegal: " + where.str());
    }

    return it.peek();
}


template<class T, bool (*tgt)(const T &a, const T &b)>
T *HeapPriorityQueue<T, tgt>::Iterator::operator->() const {
    if (expected_mod_count != ref_pq->mod_count)
        throw ConcurrentModificationError("HepatitisAPriorityQueue::Iterator::operator *");
    if (!can_erase || it.size() < 0) {
        std::ostringstream where;
        where << " when size = " << ref_pq->size();
        throw IteratorPositionIllegal("HeapPriorityQueue::Iterator::operator * Iterator illegal: " + where.str());
    }

    return &it.peek();
}

}

#endif /* HEAP_PRIORITY_QUEUE_HPP_ */
