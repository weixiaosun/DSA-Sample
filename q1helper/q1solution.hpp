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
#include "array_set.hpp"

using std::cout;

typedef ics::ArrayMap<std::string,std::string>      ArrayMap;


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
typedef ics::pair<int, Point> NumberedPoint;

bool gt_PointEntry (const NumberedPoint& a, const NumberedPoint& b)
{return a.second.y<b.second.y;}

//2b
double calc_distance_to_origin(Point p) {
    return sqrt(pow(p.x, 2) + pow(p.y, 2));
    //cout << "distance for " << p.x << " and " << p.y << " = " << d << std::endl;
}
bool lt_distance_PointEntry (const Point& a, const Point& b)
{ return calc_distance_to_origin(a) > calc_distance_to_origin(b); }

//2c
bool gt_PointEntry_ordinal (const NumberedPoint& a, const NumberedPoint& b)
{return a.first<b.first;}

//2d
bool gt_PointEntry_radians (const NumberedPoint& a, const NumberedPoint& b)
{ return atan2(a.second.y, a.second.x) < atan2(b.second.y, b.second.x);}

//Problem #1a and #1b
template<class KEY,class T>
void swap (ics::ArrayMap<KEY,T>& m, KEY key1, KEY key2) {
    m[key1] = m.put(key2, m[key1]);
}


template<class KEY,class T>
void values_set_to_queue (const ics::ArrayMap<KEY,ics::ArraySet<T>>& m1,
                          ics::ArrayMap<KEY,ics::ArrayQueue<T>>&     m2) {
    for (auto i: m1) {
        m2.put(i.first, ics::ArrayQueue<T>(i.second));
    }
}


//Problem #2a, #2b, #2c, and #2d
ics::ArrayQueue<ics::pair<int,Point>> sort_yo (const ics::ArrayMap<int,Point>& m) {
    ics::ArrayPriorityQueue<NumberedPoint, gt_PointEntry> pointPQ;
    typedef ics::ArrayQueue<ics::pair<int, Point>> PointQueue;
    for (auto i: m) {
        NumberedPoint numberedPoint(i.first, i.second);
        pointPQ.enqueue(numberedPoint);
    }
    PointQueue pointQueue;
    pointQueue.enqueue_all(pointPQ);
    return pointQueue;
}


ics::ArrayQueue<Point> sort_distance (const ics::ArrayMap<int,Point>& m) {
//Write code here
    ics::ArrayPriorityQueue<Point, lt_distance_PointEntry> pointPQ;
    for (auto i: m) {
        Point point = i.second;
        pointPQ.enqueue(point);
    }
    ics::ArrayQueue<Point> pointQueue;
    pointQueue.enqueue_all(pointPQ);
    return pointQueue;
}


ics::ArrayQueue<Point> points (const ics::ArrayMap<int,Point>& m) {
//Write code here
    ics::ArrayPriorityQueue<NumberedPoint, gt_PointEntry_ordinal> pointPQ;
    for (auto i: m) {
        NumberedPoint numberedPoint(i.first, i.second);
        pointPQ.enqueue(numberedPoint);
    }
    ics::ArrayQueue<Point> pointQueue;
    for (auto i: pointPQ) {
        pointQueue.enqueue(i.second);
    }
    return pointQueue;
}


ics::ArrayQueue<ics::pair<int,double>> angles (const ics::ArrayMap<int,Point>& m) {
//Write code here
    ics::ArrayPriorityQueue<NumberedPoint, gt_PointEntry_radians> pointPQ;
    for (auto i: m) {
        NumberedPoint numberedPoint(i.first, i.second);
        pointPQ.enqueue(numberedPoint);
    }
    ics::ArrayQueue<ics::pair<int, double>> pointQueue;
    for (auto i: pointPQ) {
        double tan = atan2(i.second.y, i.second.x);
        pointQueue.enqueue(ics::make_pair(i.first, tan));
    }
    return pointQueue;
}


//Problem #3
ics::ArrayMap<char,ics::ArraySet<char>> follows(const std::string word) {
//Write code here
    ics::ArrayMap<char, ics::ArraySet<char>> letterMap;
    for (int i = 0; i < word.size(); i++) {
        if (i != word.size() - 1) {
            letterMap[word.at(i)].insert(word.at(i + 1));
        }
    }
    return letterMap;
}


//Problem #4a and #4b
ics::ArrayMap<std::string,int> got_called(const  ics::ArrayMap<std::string,ics::ArrayMap<std::string,int>>& calls) {
//Write code here
    ics::ArrayMap<std::string, int> calledMap;
    for (auto caller: calls) {
        for (auto calleeMap: caller.second) {
            calledMap[calleeMap.first] += calleeMap.second;
        }
    }
    return calledMap;
}


ics::ArrayMap<std::string,ics::ArrayMap<std::string,int>> invert (const ics::ArrayMap<std::string,ics::ArrayMap<std::string,int>>& calls) {
//Write code here
    ics::ArrayMap<std::string,ics::ArrayMap<std::string,int>> invertedCalls;
    for (auto caller: calls) {
        ics::ArrayMap<std::string, int> invertedMap;
        for (auto calleeMapKV: caller.second) {
            invertedMap[caller.first] = calleeMapKV.second;
            invertedCalls[calleeMapKV.first].put_all(invertedMap);
        }
    }
    return invertedCalls;
}
#endif /* SOLUTION_HPP_ */


