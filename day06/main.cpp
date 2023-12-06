#include "gflags/gflags.h"
#include "re2/re2.h"
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

DEFINE_bool(part2, false, "Run the binary to solve part 2");

using namespace std;
namespace {

enum BoundType {
  kUpper,
  kLower,
};

/**
 * Find upper/lower bound (inclusive) as to how long to hold the button and can
 * still win the race
 */
size_t findBound(const size_t distance, const size_t time, BoundType bound) {
  size_t lower = 1, upper = time - 1;
  while (upper > lower + 1) {
    size_t middle = (upper + lower) / 2;
    if (distance < middle * (time - middle)) {
      // Middle win the race
      if (bound == BoundType::kLower) {
        upper = middle;
      } else {
        lower = middle;
      }
      continue;
    } else {
      // Middle lose the race
      if (bound == BoundType::kLower) {
        lower = middle;
      } else {
        upper = middle;
      }
      continue;
    }
  }

  if (bound == BoundType::kLower) {
    return upper;
  } else {
    return lower;
  }
}
} // namespace

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  size_t answer = 1;
  string timeLine, distanceLine;
  string temp;
  getline(cin, timeLine);
  getline(cin, distanceLine);
  istringstream timeLineSS(timeLine);
  istringstream distanceLineSS(distanceLine);

  timeLineSS >> temp;
  distanceLineSS >> temp;

  size_t distance, time;
  if (!FLAGS_part2) {
    // Part 1 - we parse multiple races
    while (timeLineSS >> time) {
      distanceLineSS >> distance;

      auto lower = findBound(distance, time, BoundType::kLower);
      auto upper = findBound(distance, time, BoundType::kUpper);

      answer *= upper - lower + 1;
    }
  } else {
    // Part 2 - only 1 race
    string timeStr, distanceStr;
    while (timeLineSS >> temp) {
      timeStr += temp;
    }
    while (distanceLineSS >> temp) {
      distanceStr += temp;
    }
    time = stoul(timeStr);
    distance = stoul(distanceStr);
    auto lower = findBound(distance, time, BoundType::kLower);
    auto upper = findBound(distance, time, BoundType::kUpper);
    answer *= upper - lower + 1;
  }

  cout << (FLAGS_part2 ? "Part 2: " : "Part 1: ") << answer << endl;

  if (!FLAGS_part2) {
    cout << "Solution above is for part 1, if you need part 2 solution, add "
            "the flag --part2 to the command invocation"
         << endl;
  }
}
