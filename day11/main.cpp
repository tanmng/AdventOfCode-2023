#include "gflags/gflags.h"
#include "re2/re2.h"
#include <cstdlib>
#include <iostream>
#include <unordered_set>
#include <vector>

DEFINE_bool(part2, false, "Run the binary to solve part 2");

using namespace std;
namespace {
const size_t expansionRatePart1 = 2, expansionRatePart2 = 1000000;
typedef pair<size_t, size_t> Coordinate;
const char kGalaxy = '#';
const re2::RE2 kEmptyLineRe("^\\.+$");

/**
 * Find the distance between 2 value on a given axis
 */
size_t distanceOnAxis(const size_t smallerVal, size_t largerVal,
                      const unordered_set<size_t> &expansionIndices) {
  size_t result = largerVal - smallerVal;
  for (size_t i = smallerVal + 1; i < largerVal; ++i) {
    if (expansionIndices.count(i) == 1) {
      // Need expansion on this axis
      if (FLAGS_part2) {
        // Part 2
        result += expansionRatePart2 - 1;
      } else {
        // Part 1
        result += expansionRatePart1 - 1;
      }
    }
  }
  return result;
}

/**
 * Find the distance bertween 2 galaxies (taking into account the expanding
 * universe)
 */
size_t distanceGalaxies(const Coordinate &galaxy1, const Coordinate &galaxy2,
                        const unordered_set<size_t> &expandingColumns,
                        const unordered_set<size_t> &expandingRows) {
  if (galaxy1 == galaxy2) {
    return 0;
  }

  size_t result = 0;

  // X Axis
  result += distanceOnAxis(min(galaxy1.first, galaxy2.first),
                           max(galaxy1.first, galaxy2.first), expandingColumns);
  // Y Axis
  result += distanceOnAxis(min(galaxy1.second, galaxy2.second),
                           max(galaxy1.second, galaxy2.second), expandingRows);
  return result;
}

} // namespace

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  size_t answer = 0;
  string line;
  // All the galaxies
  vector<Coordinate> galaxies;
  // Index of the rows/lines that need indexing
  unordered_set<size_t> expansionColumns, expansionRows;
  size_t rowIndex = 0;
  while (getline(cin, line)) {
    if (rowIndex == 0) {
      // First row - insert all the number to expansionColumns
      for (size_t i = 0; i < line.size(); ++i) {
        expansionColumns.insert(i);
      }
    }
    if (re2::RE2::FullMatch(line, kEmptyLineRe)) {
      // This is an empty line
      expansionRows.insert(rowIndex);
    } else {
      // Find all the galaxies in here
      for (size_t i = 0; i < line.size(); ++i) {
        auto ch = line.at(i);
        if (ch == kGalaxy) {
          // Galaxy
          // This cannot be an expansion column anymove
          expansionColumns.erase(i);

          // Mark the galaxy
          galaxies.push_back({i, rowIndex});
        }
      }
    }

    // Save the line
    ++rowIndex;
  }

  for (size_t i = 0; i < galaxies.size() - 1; ++i) {
    for (auto j = i + 1; j < galaxies.size(); ++j) {
      answer += distanceGalaxies(galaxies.at(i), galaxies.at(j),
                                 expansionColumns, expansionRows);
    }
  }

  cout << (FLAGS_part2 ? "Part 2: " : "Part 1: ") << answer << endl;

  if (!FLAGS_part2) {
    cout << "Solution above is for part 1, if you need part 2 solution, add "
            "the flag --part2 to the command invocation"
         << endl;
  }
}
