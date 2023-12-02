#include "gflags/gflags.h"
#include "re2/re2.h"

#include <iostream>
#include <string>

DEFINE_bool(part2, false, "Run the binary to solve part 2");

using namespace std;

namespace {
const re2::RE2
    kFirstDigitRe("^.*?([0-9]|one|two|three|four|five|six|seven|eight|nine).*");
const re2::RE2
    kLastDigitRe("^.*([0-9]|one|two|three|four|five|six|seven|eight|nine).*?$");
/**
 * Return the numeric value of a "digit"
 */
size_t digitValue(const std::string &digit) {
  if (digit == "0" || digit == "zero") {
    return 0;
  } else if (digit == "1" || digit == "one") {
    return 1;
  } else if (digit == "2" || digit == "two") {
    return 2;
  } else if (digit == "3" || digit == "three") {
    return 3;
  } else if (digit == "4" || digit == "four") {
    return 4;
  } else if (digit == "5" || digit == "five") {
    return 5;
  } else if (digit == "6" || digit == "six") {
    return 6;
  } else if (digit == "7" || digit == "seven") {
    return 7;
  } else if (digit == "8" || digit == "eight") {
    return 8;
  } else if (digit == "9" || digit == "nine") {
    return 9;
  }

  throw std::runtime_error("Invalid digit: " + digit);
}
} // namespace

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  string line;
  size_t answer = 0;

  if (!FLAGS_part2) {
    // Part 1 solution
    while (cin >> line) {
      // Find the first and last digit
      char firstDigit, lastDigit;
      for (auto i = line.begin(); i < line.end(); ++i) {
        if (*i >= '0' && *i <= '9') {
          // digit
          firstDigit = *i;
          break;
        }
      }

      for (auto i = line.rbegin(); i < line.rend(); ++i) {
        if (*i >= '0' && *i <= '9') {
          // digit
          lastDigit = *i;
          break;
        }
      }
      size_t num = (firstDigit - '0') * 10 + (lastDigit - '0');
      answer += num;
    }
  } else {
    // Part 2 solution
    while (cin >> line) {
      string firstDigit, lastDigit;
      re2::RE2::FullMatch(line, kFirstDigitRe, &firstDigit);
      re2::RE2::FullMatch(line, kLastDigitRe, &lastDigit);
      size_t num = digitValue(firstDigit) * 10 + digitValue(lastDigit);
      answer += num;
    }
  }

  cout << (FLAGS_part2 ? "Part 2: " : "Part 1: ") << answer << endl;

  if (!FLAGS_part2) {
    cout << "Solution above is for part 1, if you need part 2 solution, add "
            "the flag --part2 to the command invocation"
         << endl;
  }
}