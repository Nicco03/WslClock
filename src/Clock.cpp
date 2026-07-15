#include "Clock.hpp"

#include <format>
#include <regex>
#include <stdexcept>
#include <string>

// ---------------------------------------------------------------------------
//  Starting point: every method throws "not implemented" so the project
//  compiles and links but every test fails. Replace each TODO with a real
//  implementation. You are free to change the private representation in the
//  header; the seconds-since-midnight field is only a suggestion.
// ---------------------------------------------------------------------------
namespace {
[[noreturn]] void notImplemented() {
    throw std::logic_error("not implemented");
}
}  // namespace

Clock::Clock() {
}

Clock::Clock(int h, int m, int s) {
    if (h < 0 || h > 23) {
        throw std::invalid_argument("Invalid hour");
    }
    if (m < 0 || m > 59) {
        throw std::invalid_argument("Invalid minute");
    }
    if (s < 0 || s > 59) {
        throw std::invalid_argument("Invalid second");
    }

    secondsSinceMidnight_ = s + m * 60 + h * 60 * 60;
}

Clock Clock::fromSecondsSinceMidnight(long long s) {
    if (s < 0) {
        s = 86400 + (s % 86400);
    }

    auto result = Clock();
    result.secondsSinceMidnight_ = s % 86400;
    return result;
}

int Clock::hours() const {
    return static_cast<int>(secondsSinceMidnight_ / (60 * 60));
}

int Clock::minutes() const {
    return static_cast<int>(secondsSinceMidnight_ / 60 % 60);
}

int Clock::seconds() const {
    return static_cast<int>(secondsSinceMidnight_ % 60);
}

long long Clock::secondsSinceMidnight() const {
    return secondsSinceMidnight_;
}

Clock Clock::addSeconds(long long delta) const {
    if (delta < 0) {
        delta = 86400 + (delta % 86400);
    }

    auto result = *this;
    result.secondsSinceMidnight_ = (secondsSinceMidnight_ + delta) % 86400;
    return result;
}

long long Clock::secondsUntil(const Clock& other) const {
    if (other.secondsSinceMidnight_ < this->secondsSinceMidnight_) {
        return other.secondsSinceMidnight_ + 86400 - this->secondsSinceMidnight_;
    } else {
        return other.secondsSinceMidnight_ - this->secondsSinceMidnight_;
    }
}

std::string Clock::toString() const {
    return std::format("{:02}:{:02}:{:02}", hours(), minutes(), seconds());
}

Clock Clock::parse(const std::string& text) {
    auto re = std::regex("(\\d{2}):(\\d{2}):(\\d{2})");
    std::smatch match;

    if (std::regex_match(text, match, re)) {
        int h = std::stoi(match[1]);
        int m = std::stoi(match[2]);
        int s = std::stoi(match[3]);
        return Clock(h, m, s);
    } else {
        throw std::invalid_argument("Invalid string");
    }
}

bool Clock::operator==(const Clock& other) const {
    return this->secondsSinceMidnight_ == other.secondsSinceMidnight_;
}

bool Clock::operator<(const Clock& other) const {
    return this->secondsSinceMidnight_ < other.secondsSinceMidnight_;
}
