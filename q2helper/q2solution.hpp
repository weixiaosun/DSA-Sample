#ifndef SOLUTION_HPP_
#define SOLUTION_HPP_

#include <string>
#include <iostream>
#include <fstream>

using std::cout;
using std::endl;

template<class T>
class LN {
  public:
    LN ()                        : next(nullptr){}
    LN (const LN<T>& ln)         : value(ln.value), next(ln.next){}
    LN (T v, LN<T>* n = nullptr) : value(v), next(n){}
    T      value;
    LN<T>* next;
};


//Simple way to print linked lists (useful for driver and debugging)
template<class T>
std::ostream& operator << (std::ostream& outs, LN<T>* l) {
  for (LN<T>* p = l; p!=nullptr; p=p->next)
    outs << p->value << "->";
  outs << "nullptr";
  return outs;
}


//Simple way to print linked lists given first node
//  (useful for driver and debugging)
template<class T>
std::ostream& operator << (std::ostream& outs, LN<T> l) {
  outs << l.value << "->";
  for (LN<T>* p = l.next; p!=nullptr; p=p->next)
    outs << p->value << "->";
  outs << "nullptr";
  return outs;
}


char relation (const std::string& s1, const std::string& s2) {
  //write code here
    if (s1[0] < s2[0]) {
        return '<';
    } else if (s1[0] == s2[0] && s1[1] == '\0' && s2[1] == '\0') {
        return '=';
    } else if (s1[0] > s2[0]){
        return '>';
    } else {
        return relation(s1.substr(1), s2.substr(1));
    }
}


template<class T>
void remove_ascending_i (LN<T>*& l) {
  //write code here
    if (l == nullptr) {
        return;
    }
    for (LN<T>* p = l; p->next != nullptr; ) {
        if (p->next->value > p->value) {
            LN<T> *temp = p->next;
            p->next = p->next->next;
            p->value = temp->value;
            delete temp;
        } else {
            p = p->next;
        }
        cout << "p: " << p << endl;
        cout << "l: " << l << endl;
    }
}

template<class T>
void remove_ascending_r (LN<T>*& l) {
    //write code here
    if (l == nullptr || l->next == nullptr) {
        return;
    } else if (l->next->value > l->value) {
        l->value = l->next->value;
        l->next = l->next->next;
        remove_ascending_r(l);
    } else {
        remove_ascending_r(l->next);
    }
}

template<class T>
void rec_duplicate(LN<T>*& l, int v) {
    if (l == nullptr) {
        return;
    } else if (l->value == v) {
        rec_duplicate(l->next, v);
        l->next = new LN<T>(v, l->next);
    } else {
        rec_duplicate(l->next, v);
    }
}



#endif /* SOLUTION_HPP_ */
