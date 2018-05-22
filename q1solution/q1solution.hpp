#ifndef SOLUTION_HPP_
#define SOLUTION_HPP_

#include <string>
#include <iostream>
#include <fstream>
#include <math.h>            /* for atan2 and sqrt */
#include "ics46goody.hpp"
#include "ics_exceptions.hpp"
#include "array_queue.hpp"
#include "array_priority_queue.hpp"
#include "array_map.hpp"


class Point {
public:
  Point() : x(0), y(0) {} // Needed for pair
  Point(int x_val, int y_val) : x(x_val), y(y_val) {}
  friend bool operator == (const Point& p1, const Point& p2) {
    return p1.x == p2.x && p1.y == p2.y;
  }
  friend std::ostream& operator << (std::ostream& outs, const Point& p) {
    outs << "(" << p.x << "," << p.y << ")";
    return outs;
  }

  int x;
  int y;
};


//Helper Functions (you decide what is useful)
//Hint: I used helpers for sort_yo, sort_angle, points, and first_quad
bool gt_yo(const ics::pair<int,Point>& a, const ics::pair<int,Point>& b) {
  return a.second.y == b.second.y ? a.first > b.first :
                                    a.second.y < b.second.y;
}

bool gt_distance(const ics::pair<int,Point>& a, const ics::pair<int,Point>& b) {
  return sqrt(a.second.x*a.second.x + a.second.y*a.second.y) > sqrt(b.second.x*b.second.x + b.second.y*b.second.y);
}

bool gt_ordinal(const ics::pair<int,Point>& a, const ics::pair<int,Point>& b) {
  return a.first < b.first;
}

bool gt_angle(const ics::pair<int,Point>& a, const ics::pair<int,Point>& b) {
  return atan2(a.second.y,a.second.x) < atan2(b.second.y,b.second.x);
}



//Problem #1a and #1b
template<class KEY,class T>
void swap (ics::ArrayMap<KEY,T>& m, KEY key1, KEY key2) {
  m[key2] = m.put(key1,m[key2]);
  // m.put(key2,  m.put(key1, m[key2])); //Alternative (all put) solution
}


template<class KEY,class T>
void values_set_to_queue (const ics::ArrayMap<KEY,ics::ArraySet<T>>& m1,
                          ics::ArrayMap<KEY,ics::ArrayQueue<T>>&     m2) {
  for (auto e : m1)
    m2[e.first] = ics::ArrayQueue<T>(e.second);
    //or m2[e.first].enqueue_all(e.second);
}


//Problem #2a, #2b, #2c, and #2d
ics::ArrayQueue<ics::pair<int,Point>> sort_yo (const ics::ArrayMap<int,Point>& m) {
 ics::ArrayPriorityQueue<ics::pair<int,Point>,gt_yo> sorted(m);
 ics::ArrayQueue<ics::pair<int,Point>> answer(sorted);
 return answer;
// return ics::ArrayQueue<ics::pair<int,Point>> (ics::ArrayPriorityQueue<ics::pair<int,Point>,gt_yo>(m));
}


ics::ArrayQueue<Point> sort_distance (const ics::ArrayMap<int,Point>& m) {
  ics::ArrayPriorityQueue<ics::pair<int,Point>,gt_distance> sorted(m);
  ics::ArrayQueue<Point> answer;
  for (auto op : sorted)
    answer.enqueue(op.second);
  return answer;
}


ics::ArrayQueue<Point> points (const ics::ArrayMap<int,Point>& m) {
  ics::ArrayPriorityQueue<ics::pair<int,Point>,gt_ordinal> sorted(m);
  ics::ArrayQueue<Point> answer;
  for (auto op : sorted)
    answer.enqueue(op.second);
  return answer;
}


ics::ArrayQueue<ics::pair<int,double>> angles (const ics::ArrayMap<int,Point>& m) {
  ics::ArrayPriorityQueue<ics::pair<int,Point>,gt_angle> sorted(m);
  ics::ArrayQueue<ics::pair<int,double>> answer;
  for (auto op : sorted)
    answer.enqueue(ics::pair<int,double>(op.first,atan2(op.second.y,op.second.x)));
  return answer;
}


//Problem #3
ics::ArrayMap<char,ics::ArraySet<char>> follows(const std::string word) {
  ics::ArrayMap<char,ics::ArraySet<char>> answer;
  for (int i = 0; i < int(word.size())-1; ++i)
    answer[word[i]].insert(word[i+1]);
  return answer;
}


//Problem #4a and #4b
ics::ArrayMap<std::string,int> got_called(const  ics::ArrayMap<std::string,ics::ArrayMap<std::string,int>>& calls) {
  ics::ArrayMap<std::string,int> answer;
  for (auto rcm : calls)
    for (auto e : rcm.second)
      answer[e.first] += e.second;

  return answer;
}


  ics::ArrayMap<std::string,ics::ArrayMap<std::string,int>> invert (const ics::ArrayMap<std::string,ics::ArrayMap<std::string,int>>& calls) {
    ics::ArrayMap<std::string,ics::ArrayMap<std::string,int>> answer;
    for (auto r_ei : calls)
      for (auto e_i : r_ei.second)
        answer[e_i.first][r_ei.first] += e_i.second;

    return answer;
}
#endif /* SOLUTION_HPP_ */


