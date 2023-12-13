#include "gflags/gflags.h"
#include "re2/re2.h"
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

DEFINE_bool(part2, false, "Run the binary to solve part 2");
DEFINE_bool(debug, false, "Print more debug");

using namespace std;
namespace {
/**
 * Find how many columns to the left of a reflection line
 */
size_t findVerticalReflectionLine(const vector<string> &lines) {
  if (FLAGS_debug) {
    for (const auto &line : lines) {
      cout << line << endl;
    }
  }
  const size_t colCount = lines[0].size();
  // Answer - how many columns to the left of the reflection line
  for (size_t answer = 1; answer < colCount; ++answer) {
    //  The case when we use the right next column
    bool skip = false;
    size_t mismatchCount = 0;
    for (const auto &line : lines) {
      for (int i = answer - 1; i >= 0; --i) {
        auto reflectionIndex = answer + (answer - i) - 1;
        if (reflectionIndex < colCount && line[i] != line[reflectionIndex]) {
          // Mismatch
          if (FLAGS_debug) {
            cout << answer << " mismatch when compare " << i << " with "
                 << reflectionIndex << " on line " << line << endl;
          }
          if (!FLAGS_part2) {
            // Part 1 - fail early
            skip = true;
            break;
          } else {
            // Part 2 - count how many mismatch we have
            ++mismatchCount;
            if (mismatchCount > 1) {
              // No need to go further
              skip = true;
              break;
            }
          }
        }
      }
      if (skip) {
        // Do not waste time checking all the lines
        break;
      }
    }
    if (FLAGS_part2) {
      // Part 2
      if (mismatchCount == 1) {
        return answer;
      }
    } else {
      // Part 1
      if (!skip) {
        // Did not have to skip anything - this is a valid answer
        return answer;
      }
    }
  }
  return 0;
}

/**
 * Find how many rows are on top of reflection line
 */
size_t findHorizontalReflectionLine(const vector<string> &lines) {
  if (FLAGS_debug) {
    for (const auto &line : lines) {
      cout << line << endl;
    }
  }
  // Answer - how many rows are on top of reflection line
  for (size_t answer = 1; answer < lines.size(); ++answer) {
    //  The case when we use the right next row
    bool skip = false;
    size_t mismatchCount = 0;
    for (int i = answer - 1; i >= 0; --i) {
      auto reflectionIndex = answer + (answer - i) - 1;
      if (reflectionIndex < lines.size() &&
          lines[i] != lines[reflectionIndex]) {
        // Mismatch
        if (FLAGS_debug) {
          cout << answer << " mismatch when compare row " << i << " with row "
               << reflectionIndex << endl;
        }
        if (!FLAGS_part2) {
          // Part 1 - fail early
          skip = true;
          break;
        } else {
          // Part 2 - count how many mismatch we have (in term of characters)
          for (size_t j = 0; j < lines[i].size(); ++j) {
            if (lines[i][j] != lines[reflectionIndex][j]) {
              ++mismatchCount;
            }
          }
          if (mismatchCount > 1) {
            // No need to go further
            skip = true;
            break;
          }
        }
      }
    }
    if (FLAGS_part2) {
      // Part 2
      if (mismatchCount == 1) {
        return answer;
      }
    } else {
      // Part 1
      if (!skip) {
        // Did not have to skip anything - this is a valid answer
        return answer;
      }
    }
  }
  return 0;
}

} // namespace

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  size_t answer = 0;
  vector<string> lines;
  string line;
  while (getline(cin, line)) {
    if (line.size() == 0) {
      // empty line - process the previous section
      answer += 100 * findHorizontalReflectionLine(lines) +
                findVerticalReflectionLine(lines);
      lines.clear();
    } else {
      lines.emplace_back(std::move(line));
    }
  }

  // Last section
  answer += 100 * findHorizontalReflectionLine(lines) +
            findVerticalReflectionLine(lines);

  if (FLAGS_part2) {
    // Tally the result for part 2
  }
  cout << (FLAGS_part2 ? "Part 2: " : "Part 1: ") << answer << endl;

  if (!FLAGS_part2) {
    cout << "Solution above is for part 1, if you need part 2 solution, add "
            "the flag --part2 to the command invocation"
         << endl;
  }
}
