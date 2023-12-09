#include "gflags/gflags.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>

DEFINE_bool(part2, false, "Run the binary to solve part 2");

using namespace std;
namespace {
/**
 * Calculate the diff of numbers, and return if the diffs are all zeroes
 */
bool createDiff(const vector<long long> &numbers, vector<long long> &diffs) {
  auto result = true;

  for (long long i = 0; i < numbers.size() - 1; ++i) {
    // Because in part 2 we already reverse the order, this difference is
    // actually correct
    auto diffVal = numbers[i + 1] - numbers[i];
    diffs.push_back(diffVal);
    if (result && diffVal != 0) {
      result = false;
    }
  }

  return result;
}

} // namespace

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  long long answer = 0;
  string line;
  while (getline(cin, line)) {
    istringstream lineSS(line);
    // Record the numbers
    vector<vector<long long>> allLayers{2};
    size_t num;
    while (lineSS >> num) {
      allLayers[0].push_back(num);
    }

    if (FLAGS_part2) {
      // Reverse for part 2 (so that we don't have to deal with negative index
      // or anything like that)
      reverse(allLayers[0].begin(), allLayers[0].end());
    }
    // Calculate all the layers of diffs
    while (!createDiff(allLayers[allLayers.size() - 2],
                       allLayers[allLayers.size() - 1])) {
      // The last layer of diff is not zero - add another layer
      allLayers.push_back({});
    }

    // Reverse the stack to get the result
    for (auto i = allLayers.size() - 1; i > 0; --i) {
      allLayers[i - 1].push_back(allLayers[i].back() + allLayers[i - 1].back());
    }

    answer += allLayers[0].back();
  }

  cout << (FLAGS_part2 ? "Part 2: " : "Part 1: ") << answer << endl;

  if (!FLAGS_part2) {
    cout << "Solution above is for part 1, if you need part 2 solution, add "
            "the flag --part2 to the command invocation"
         << endl;
  }
}
