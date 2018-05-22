#ifndef Q6SOLUTION_HPP_
#define Q6SOLUTION_HPP_


#include <iostream>
#include <exception>
#include <fstream>
#include <sstream>
#include <algorithm>                 // std::swap
#include "ics46goody.hpp"
#include "array_queue.hpp"
#include "q6utility.hpp"

typedef ics::ArrayQueue<int> ArrayQueue;

using std::cout;
using std::endl;
////////////////////////////////////////////////////////////////////////////////

//Problem 1

//Write this function
template<class T>
void selection_sort(LN<T>* l) {
  for (LN<T>* current = l; current != nullptr; current = current->next) {
    LN<T>* min = current;
    for (LN<T>* finder = current; finder != nullptr; finder = finder->next) {
      if (finder->value < min->value)
        min = finder;
    }
    T temp = current->value;
    current->value = min->value;
    min->value = temp;
  }
}


////////////////////////////////////////////////////////////////////////////////

//Problem 2

//Write this function
template<class T>
void merge(T a[], int left_low,  int left_high,
                    int right_low, int right_high) {
  T temp[right_high+1];
  for (int i = 0; i < right_high + 1; i++)
    temp[i] = 0;
  int current = left_low;
  int left_index = left_low;
  int right_index = right_low;

  while (temp[right_high] == 0) {
    if (a[left_high] == 0) {
      temp[current++] = a[right_index];
      a[right_index++] = 0;
    } else if (a[right_high] == 0) {
      temp[current++] = a[left_index];
      a[left_index++] = 0;
    } else {
      if (a[left_index] < a[right_index]) {
        temp[current++] = a[left_index];
        a[left_index++] = 0;
      } else {
        temp[current++] = a[right_index];
        a[right_index++] = 0;
      }
    }
  }
  for (int i = left_low; i < right_high + 1; i++)
      a[i] = temp[i];
}


////////////////////////////////////////////////////////////////////////////////

//Problem 3

int select_digit (int number, int place)
{return number/place % 10;}


//Write this function
void radix_sort(int a[], int length) {
  ArrayQueue* buckets[10];
  for (int i = 0; i < 10; i++)
    buckets[i] = new ArrayQueue();

  for (int j = 1; j <= 100000; j *= 10) {
    for (int k = 0; k < length; k++)
      buckets[select_digit(a[k],j)]->enqueue(a[k]);
    int index = 0;
    for (int b = 0; b < 10; b++)
      while (!buckets[b]->empty())
        a[index++] = buckets[b]->dequeue();
  }
}


////////////////////////////////////////////////////////////////////////////////

//Problem 4

//Write this function

//Test how well a partition function (choose_pivot_index: see last and median3) works.
//Test is on an array of the values 0..length-1, randomly shuffled num_tests times
//Returns the average size of the larger of the left and right partitions.
//At best the array is cut in half, yielding the biggest side as 0.5 (50%)
double test_partition(int length, int num_tests, int (*choose_pivot_index) (int a[], int low, int high)) {
  int test_array[length];
  double total_longest = 0;
  for (int i = 0; i < length; i++)
    test_array[i] = i;
  for (int i = 0; i < num_tests; i++) {
    shuffle(test_array, length);
    int p_index = choose_pivot_index(test_array, 0, length - 1);
    int pivot = partition(test_array, 0, length - 1, p_index);
    total_longest += pivot > length / 2 ? pivot + 1 : length - pivot;
  }
  return (total_longest / num_tests) / length;
}


#endif /* Q6SOLUTION_HPP_ */
