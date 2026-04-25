/*
* timer.h
 *
 *  Created on: Dec 8, 2025
 *      Author: bill_booth
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <chrono>
#include <iostream>

class Timer {
public:
    Timer() : start_(Clock::now()) {}

    void reset() {
        start_ = Clock::now();
    }

    // Returns elapsed time in seconds (double precision)
    double elapsed() const {
        return std::chrono::duration<double>(Clock::now() - start_).count();
    }

    // Returns elapsed nanoseconds as a 64-bit integer
    long long elapsedNanoseconds() const {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            Clock::now() - start_).count();
    }

    // Returns elapsed microseconds
    long long elapsedMicroseconds() const {
        return std::chrono::duration_cast<std::chrono::microseconds>(
            Clock::now() - start_).count();
    }

    // Returns elapsed milliseconds
    long long elapsedMilliseconds() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            Clock::now() - start_).count();
    }

private:
    using Clock = std::chrono::steady_clock;
    std::chrono::time_point<Clock> start_;
};

/*
int main() {
    Timer t;

    // Example workload
    volatile long long sum = 0;
    for (long long i = 0; i < 1000000; ++i)
        sum += i;

    std::cout << "Elapsed time:\n";
    std::cout << "  ns: " << t.elapsedNanoseconds() << "\n";
    std::cout << "  µs: " << t.elapsedMicroseconds() << "\n";
    std::cout << "  ms: " << t.elapsedMilliseconds() << "\n";
    std::cout << "  s : " << t.elapsed() << "\n";

    return 0;
}

*/

#endif /* TIMER_H_ */