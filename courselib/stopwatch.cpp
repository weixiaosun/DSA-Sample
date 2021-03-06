#include <string>
#include <iostream>
#include <fstream>
#include "stopwatch.hpp"

namespace ics {

 Stopwatch::Stopwatch(bool running_now, bool running_forward, long elapsed_prior, std::clock_t last_start_time) {
    this->running_now     = running_now;
    this->running_forward = running_forward;
    this->elapsed_prior   = elapsed_prior;
    this->last_start_time = last_start_time;
  }

  void Stopwatch::reset() {
    running_now     = false;
    running_forward = true;
    elapsed_prior   = 0;;
    last_start_time = std::clock();
  }

  void Stopwatch::start() {
    if (running_now) {
        if (running_forward)
            return;
        else                         // running backward
            update();                // update, then start running forward
    }
    last_start_time = std::clock();
    running_now     = true;
    running_forward = true;
  }

  void Stopwatch::start_backwards() {
    if (running_now) {
        if (!running_forward)
            return;
        else                         // running forward
            update();                // update, then start running backward
    }
    last_start_time = std::clock();
    running_now     = true;
    running_forward = false;
  }

  void Stopwatch::stop() {
    if (!running_now)
        return;
    running_now = false;
    update();
  }

  double Stopwatch::read() {
    if (running_now)
        update();
    return elapsed_prior / (double) CLOCKS_PER_SEC;
  }


std::ostream& operator << (std::ostream& outs, const Stopwatch& s) {
  outs << "Stopwatch[running_now="     << s.running_now <<
                   ",running_forward=" << s.running_forward <<
                   ",elapsed_prior="   << s.elapsed_prior <<
                   ",last_start_time=" << s.last_start_time;
  return outs;
}

}
