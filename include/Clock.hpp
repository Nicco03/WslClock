#ifndef CLOCK_HPP
#define CLOCK_HPP

#include <string>

// ---------------------------------------------------------------------------
//  Clock - a time of day on a 24-hour cycle, at one-second resolution.
//
//  Times run from 00:00:00 to 23:59:59 and arithmetic wraps around midnight.
//  See the assignment text for the full contract. Only the *declarations* live
//  here; implement the bodies in src/Clock.cpp.
// ---------------------------------------------------------------------------
class Clock {
public:
    // Midnight, i.e. 00:00:00.
    Clock();

    // From components. Throws std::invalid_argument if
    //   h not in [0, 23], m not in [0, 59], or s not in [0, 59].
    Clock(int h, int m, int s);

    // From a (possibly negative or large) seconds count, wrapped modulo 86400
    // into [0, 86399].
    static Clock fromSecondsSinceMidnight(long long s);

    // Components.
    int hours() const;
    int minutes() const;
    int seconds() const;

    // The time as a value in [0, 86399].
    long long secondsSinceMidnight() const;

    // A new clock advanced by 'delta' seconds, wrapping at midnight.
    // 'delta' may be negative.
    Clock addSeconds(long long delta) const;

    // Forward seconds needed to go from *this to 'other', in [0, 86399].
    long long secondsUntil(const Clock& other) const;

    // The zero-padded form "HH:MM:SS".
    std::string toString() const;

    // The inverse of toString(). Throws std::invalid_argument on any malformed
    // string or out-of-range component.
    static Clock parse(const std::string& text);

    // Ordered by time of day.
    bool operator==(const Clock& other) const;
    bool operator<(const Clock& other) const;

private:
    // TODO: choose your representation. The natural choice is a single integer
    // holding the seconds since midnight, kept in [0, 86399]. You may replace
    // this field with whatever you prefer.
    long long secondsSinceMidnight_ = 0;
};

#endif  // CLOCK_HPP
