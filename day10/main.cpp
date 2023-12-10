#include "gflags/gflags.h"
#include "re2/re2.h"
#include <iostream>
#include <set>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

DEFINE_bool(part2, false, "Run the binary to solve part 2");
DEFINE_bool(debug, false, "Print more debug info");

using namespace std;
namespace {
typedef pair<int, int> Coordinate;
// Expression
const char kGround = '.', kWE = '-', kNS = '|', kWN = 'J', kNE = 'L', kES = 'F',
           kSW = '7', kStart = 'S';
const char kN = 'N', kS = 'S', kW = 'W', kE = 'E';
const re2::RE2 kNumberRe("^Card +([0-9]+):([0-9 ]+)\\|([0-9 ]*)$");

/**
 * Follow the pipe at the given coordinate, using the direction as noted
 *
 * Update currentPoint an the direction of movement according to what we have in
 * the map
 */
void followPipe(Coordinate &currentPoint, char &headingDir,
                const vector<string> &lines) {
  auto val = lines.at(currentPoint.second).at(currentPoint.first);
  switch (headingDir) {
  case kN:
    // Heading North
    switch (val) {
    case kNS:
      --currentPoint.second;
      return;
    case kSW:
      headingDir = kW;
      --currentPoint.first;
      return;
    case kES:
      headingDir = kE;
      ++currentPoint.first;
      return;
    default:
      throw runtime_error("Invalid pipe for the given direction");
    }
    break;
  case kS:
    // Heading South
    switch (val) {
    case kNS:
      ++currentPoint.second;
      return;
    case kWN:
      headingDir = kW;
      --currentPoint.first;
      return;
    case kNE:
      headingDir = kE;
      ++currentPoint.first;
      return;
    default:
      cout << "dir: " << headingDir << " char: " << val << endl;
      throw runtime_error("Invalid pipe for the given direction");
    }
    break;
  case kE:
    // Heading East
    switch (val) {
    case kWE:
      ++currentPoint.first;
      return;
    case kWN:
      headingDir = kN;
      --currentPoint.second;
      return;
    case kSW:
      headingDir = kS;
      ++currentPoint.second;
      return;
    default:
      throw runtime_error("Invalid pipe for the given direction");
    }
    break;
  case kW:
    // Heading West
    switch (val) {
    case kWE:
      --currentPoint.first;
      return;
    case kNE:
      headingDir = kN;
      --currentPoint.second;
      return;
    case kES:
      headingDir = kS;
      ++currentPoint.second;
      return;
    default:
      throw runtime_error("Invalid pipe for the given direction");
    }
    break;
  default:
    throw runtime_error("Invalid direction");
  }
}

} // namespace

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  size_t answer = 0, answerPart1 = 0, answerPart2 = 0;
  string line;
  vector<string> lines;
  Coordinate start;
  while (getline(cin, line)) {
    auto pos = line.find(kStart);
    if (pos != string::npos) {
      start.second = lines.size();
      start.first = pos;
    }
    cout << line << endl;
    lines.emplace_back(std::move(line));
  }

  cout << "Start: " << start.first << " " << start.second << endl;

  // Our data and sample we have the south of start connected to start
  Coordinate current{start.first, start.second + 1};
  char curHeading = kS;
  set<Coordinate> loop{start, current};

  while (current != start) {
    followPipe(current, curHeading, lines);
    loop.insert(current);
  }

  cout << "Part 1: " << loop.size() / 2 << endl;

  // Part 2 - check every row
  // Row index = Y coordinate
  for (size_t y = 0; y < lines.size(); ++y) {
    // Col index = X coordinate
    size_t loopPipeFound = 0;
    size_t thisRowPotential = 0;
    for (size_t x = 0; x < lines[0].size(); ++x) {
      if (loop.count({x, y}) == 1) {
        const auto val = lines.at(y).at(x);
        // This is part of the loop itself that can cross vertically
        if (val == kNS || val == kWN || val == kNE) {
          ++loopPipeFound;
        }
        continue;
      } else {
        if (loopPipeFound % 2 == 1) {
          // We have encountered an od number of pipe so far
          // And this is not part of the loop
          if (FLAGS_debug) {
            cout << " part 2: " << x << " " << y << endl;
          }
          ++thisRowPotential;
        }
      }
    }
    // Finished with the row
    if (loopPipeFound % 2 == 0) {
      // We really found valid range
      if (FLAGS_debug) {
        cout << " above " << thisRowPotential << " are fine " << endl;
      }
      answerPart2 += thisRowPotential;
    }
  }

  cout << "Part 2: " << answerPart2 << endl;
}
