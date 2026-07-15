#include <rapidcheck.h>

#include <cstddef>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>

#include "Clock.hpp"

// ---------------------------------------------------------------------
//  Mini-harness: groups the tests and computes an indicative grade.
// ---------------------------------------------------------------------
namespace {
struct Group {
    std::string name;
    int passed = 0;
    int total = 0;
};

template <typename Prop>
void runProp(Group& g, const std::string& descr, Prop&& prop) {
    ++g.total;
    if (rc::check(descr, std::forward<Prop>(prop))) ++g.passed;
}

// A uniformly chosen valid clock, exercising the component constructor.
Clock anyClock() {
    const int h = *rc::gen::inRange(0, 24);
    const int m = *rc::gen::inRange(0, 60);
    const int s = *rc::gen::inRange(0, 60);
    return Clock(h, m, s);
}

// A clock from a uniformly chosen seconds-since-midnight value.
Clock clockFromSecs(long long t) { return Clock::fromSecondsSinceMidnight(t); }

constexpr long long kDay = 86400;
}  // namespace

int main() {
    Group base{"Base group (pass, 12 tests)"};
    Group b1{"Group B1 - wrap-around on addSeconds (2 tests)"};
    Group b2{"Group B2 - toString/parse round-trip (2 tests)"};
    Group b3{"Group B3 - arithmetic consistency (2 tests)"};
    Group b4{"Group B4 - total order (2 tests)"};

    // ===================== BASE GROUP (12) =====================
    runProp(base, "01. a default-constructed clock is midnight 00:00:00", [] {
        Clock c;
        RC_ASSERT(c.hours() == 0);
        RC_ASSERT(c.minutes() == 0);
        RC_ASSERT(c.seconds() == 0);
        RC_ASSERT(c.secondsSinceMidnight() == 0);
    });

    runProp(base, "02. the component constructor stores the components", [] {
        const int h = *rc::gen::inRange(0, 24);
        const int m = *rc::gen::inRange(0, 60);
        const int s = *rc::gen::inRange(0, 60);
        Clock c(h, m, s);
        RC_ASSERT(c.hours() == h);
        RC_ASSERT(c.minutes() == m);
        RC_ASSERT(c.seconds() == s);
    });

    runProp(base, "03. secondsSinceMidnight() matches the components", [] {
        const int h = *rc::gen::inRange(0, 24);
        const int m = *rc::gen::inRange(0, 60);
        const int s = *rc::gen::inRange(0, 60);
        Clock c(h, m, s);
        RC_ASSERT(c.secondsSinceMidnight() ==
                  static_cast<long long>(h) * 3600 + m * 60 + s);
    });

    runProp(base, "04. components recompose secondsSinceMidnight()", [] {
        const long long t = *rc::gen::inRange<long long>(0, kDay);
        Clock c = clockFromSecs(t);
        const long long recomposed = static_cast<long long>(c.hours()) * 3600 +
                                     c.minutes() * 60 + c.seconds();
        RC_ASSERT(recomposed == t);
        RC_ASSERT(c.secondsSinceMidnight() == t);
    });

    runProp(base, "05. out-of-range components throw std::invalid_argument", [] {
        RC_ASSERT_THROWS_AS(Clock(24, 0, 0), std::invalid_argument);
        RC_ASSERT_THROWS_AS(Clock(-1, 0, 0), std::invalid_argument);
        RC_ASSERT_THROWS_AS(Clock(0, 60, 0), std::invalid_argument);
        RC_ASSERT_THROWS_AS(Clock(0, -1, 0), std::invalid_argument);
        RC_ASSERT_THROWS_AS(Clock(0, 0, 60), std::invalid_argument);
        RC_ASSERT_THROWS_AS(Clock(0, 0, -1), std::invalid_argument);
    });

    runProp(base, "06. valid components are accepted and stay in range", [] {
        Clock c = anyClock();  // must not throw
        RC_ASSERT(c.secondsSinceMidnight() >= 0);
        RC_ASSERT(c.secondsSinceMidnight() < kDay);
    });

    runProp(base, "07. toString() matches known times", [] {
        RC_ASSERT(Clock(0, 0, 0).toString() == "00:00:00");
        RC_ASSERT(Clock(9, 5, 7).toString() == "09:05:07");
        RC_ASSERT(Clock(13, 0, 0).toString() == "13:00:00");
        RC_ASSERT(Clock(23, 59, 59).toString() == "23:59:59");
    });

    runProp(base, "08. parse() reads well-formed strings", [] {
        RC_ASSERT(Clock::parse("00:00:00") == Clock(0, 0, 0));
        RC_ASSERT(Clock::parse("09:05:07") == Clock(9, 5, 7));
        RC_ASSERT(Clock::parse("23:59:59") == Clock(23, 59, 59));
    });

    runProp(base, "09. parse() throws std::invalid_argument on bad input", [] {
        RC_ASSERT_THROWS_AS(Clock::parse(""), std::invalid_argument);
        RC_ASSERT_THROWS_AS(Clock::parse("1:2:3"), std::invalid_argument);
        RC_ASSERT_THROWS_AS(Clock::parse("24:00:00"), std::invalid_argument);
        RC_ASSERT_THROWS_AS(Clock::parse("12:60:00"), std::invalid_argument);
        RC_ASSERT_THROWS_AS(Clock::parse("12:00:60"), std::invalid_argument);
        RC_ASSERT_THROWS_AS(Clock::parse("12:00:00x"), std::invalid_argument);
        RC_ASSERT_THROWS_AS(Clock::parse("aa:bb:cc"), std::invalid_argument);
    });

    runProp(base, "10. addSeconds within the same day adds correctly", [] {
        const long long start = *rc::gen::inRange<long long>(0, kDay);
        const long long delta = *rc::gen::inRange<long long>(0, kDay - start);
        Clock c = clockFromSecs(start);
        RC_ASSERT(c.addSeconds(delta).secondsSinceMidnight() == start + delta);
    });

    runProp(base, "11. secondsUntil() measures the forward gap", [] {
        const long long a = *rc::gen::inRange<long long>(0, kDay);
        const long long b = *rc::gen::inRange<long long>(0, kDay);
        const long long expected = ((b - a) % kDay + kDay) % kDay;
        RC_ASSERT(clockFromSecs(a).secondsUntil(clockFromSecs(b)) == expected);
    });

    runProp(base, "12. == and < agree with secondsSinceMidnight on known pairs",
            [] {
                const long long a = *rc::gen::inRange<long long>(0, kDay);
                const long long b = *rc::gen::inRange<long long>(0, kDay);
                Clock ca = clockFromSecs(a);
                Clock cb = clockFromSecs(b);
                RC_ASSERT((ca == cb) == (a == b));
                RC_ASSERT((ca < cb) == (a < b));
            });

    // ===================== GROUP B1 (2) =====================
    runProp(b1, "13. addSeconds wraps modulo a day for any delta", [] {
        const long long start = *rc::gen::inRange<long long>(0, kDay);
        const long long delta = *rc::gen::inRange<long long>(-1000000, 1000000);
        Clock c = clockFromSecs(start);
        const long long expected = ((start + delta) % kDay + kDay) % kDay;
        RC_ASSERT(c.addSeconds(delta).secondsSinceMidnight() == expected);
    });

    runProp(b1, "14. midnight boundary and whole-day period", [] {
        RC_ASSERT(Clock(23, 59, 59).addSeconds(1) == Clock(0, 0, 0));
        RC_ASSERT(Clock(0, 0, 0).addSeconds(-1) == Clock(23, 59, 59));
        const long long start = *rc::gen::inRange<long long>(0, kDay);
        const long long days = *rc::gen::inRange<long long>(-5, 6);
        Clock c = clockFromSecs(start);
        RC_ASSERT(c.addSeconds(days * kDay) == c);
    });

    // ===================== GROUP B2 (2) =====================
    runProp(b2, "15. parse(toString(c)) == c", [] {
        Clock c = clockFromSecs(*rc::gen::inRange<long long>(0, kDay));
        RC_ASSERT(Clock::parse(c.toString()) == c);
    });

    runProp(b2, "16. toString() has the HH:MM:SS shape", [] {
        Clock c = clockFromSecs(*rc::gen::inRange<long long>(0, kDay));
        const std::string s = c.toString();
        RC_ASSERT(s.size() == 8u);
        RC_ASSERT(s[2] == ':');
        RC_ASSERT(s[5] == ':');
        for (int i : {0, 1, 3, 4, 6, 7}) {
            const char ch = s[static_cast<std::size_t>(i)];
            const bool isDigit = ch >= '0' && ch <= '9';
            RC_ASSERT(isDigit);
        }
    });

    // ===================== GROUP B3 (2) =====================
    runProp(b3, "17. addSeconds composes and has identity/inverse", [] {
        Clock c = clockFromSecs(*rc::gen::inRange<long long>(0, kDay));
        const long long a = *rc::gen::inRange<long long>(-100000, 100000);
        const long long b = *rc::gen::inRange<long long>(-100000, 100000);
        RC_ASSERT(c.addSeconds(0) == c);
        RC_ASSERT(c.addSeconds(a).addSeconds(b) == c.addSeconds(a + b));
        RC_ASSERT(c.addSeconds(a).addSeconds(-a) == c);
    });

    runProp(b3, "18. addSeconds(secondsUntil(o)) reaches o", [] {
        Clock a = clockFromSecs(*rc::gen::inRange<long long>(0, kDay));
        Clock o = clockFromSecs(*rc::gen::inRange<long long>(0, kDay));
        RC_ASSERT(a.addSeconds(a.secondsUntil(o)) == o);
        const long long fwd = a.secondsUntil(o);
        const long long bwd = o.secondsUntil(a);
        const bool inRange = fwd >= 0 && fwd < kDay;
        RC_ASSERT(inRange);
        const bool consistent =
            (a == o) ? (fwd == 0 && bwd == 0) : (fwd + bwd == kDay);
        RC_ASSERT(consistent);
    });

    // ===================== GROUP B4 (2) =====================
    runProp(b4, "19. operator< is a strict order consistent with the time", [] {
        const long long ta = *rc::gen::inRange<long long>(0, kDay);
        const long long tb = *rc::gen::inRange<long long>(0, kDay);
        Clock a = clockFromSecs(ta);
        Clock b = clockFromSecs(tb);
        RC_ASSERT(!(a < a));  // irreflexive
        const bool lt = a < b;
        const bool gt = b < a;
        const bool eq = a == b;
        RC_ASSERT((lt + gt + eq) == 1);  // exactly one of <, ==, >
        RC_ASSERT(lt == (ta < tb));
        RC_ASSERT(eq == (ta == tb));
    });

    runProp(b4, "20. operator< is transitive", [] {
        Clock a = clockFromSecs(*rc::gen::inRange<long long>(0, kDay));
        Clock b = clockFromSecs(*rc::gen::inRange<long long>(0, kDay));
        Clock c = clockFromSecs(*rc::gen::inRange<long long>(0, kDay));
        if ((a < b) && (b < c)) RC_ASSERT(a < c);
        if ((a == b) && (b == c)) RC_ASSERT(a == c);
    });

    // ===================== SUMMARY =====================
    auto print = [](const Group& g) {
        std::cout << "  " << g.name << ": " << g.passed << "/" << g.total
                  << (g.passed == g.total ? "  OK" : "  --") << "\n";
    };
    std::cout << "\n================ SUMMARY ================\n";
    print(base);
    print(b1);
    print(b2);
    print(b3);
    print(b4);
    std::cout << "-------------------------------------------\n";

    if (base.passed == base.total) {
        int extra = 0;
        if (b1.passed == b1.total) ++extra;
        if (b2.passed == b2.total) ++extra;
        if (b3.passed == b3.total) ++extra;
        if (b4.passed == b4.total) ++extra;

        const char* label[] = {"Pass", "Fair", "Good", "Very good", "Excellent"};
        std::printf("# Indicative grade: %s (%d/30)\n", label[extra], 18 + 3 * extra);
    } else {
        std::printf("# Indicative grade: Fail (base group not fully passed)\n");
    }
    std::fflush(stdout);

    const int totalPassed =
        base.passed + b1.passed + b2.passed + b3.passed + b4.passed;
    return (totalPassed == 20) ? 0 : 1;
}
