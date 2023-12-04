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
const re2::RE2 kNumberRe("^Card +([0-9]+):([0-9 ]+)\\|([0-9 ]*)$");

} // namespace

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  string line, winningPart, numberPart;
  // count how many of each cards that we have
  unordered_map<size_t, size_t> cardCount{};
  size_t maxCardNumber =
      0; // Record the highest card that we found in the input
  size_t answer = 0;
  while (getline(cin, line)) {
    string winningPart, numberPart;
    size_t cardNumber;
    if (!re2::RE2::FullMatch(line, kNumberRe, &cardNumber, &winningPart,
                             &numberPart)) {
      throw runtime_error("Invalid line " + line);
    }
    maxCardNumber = max(maxCardNumber, cardNumber);
    // count that we have found this card
    cardCount[cardNumber] += 1;
    // Parse all the winning number
    size_t num;
    unordered_set<size_t> winningNumbers;
    {
      istringstream numberNow(winningPart);
      while (numberNow >> num) {
        winningNumbers.insert(num);
      }
    }
    // count how many number that we win
    size_t winningCount = 0;
    {
      istringstream numberNow(numberPart);
      while (numberNow >> num) {
        if (winningNumbers.count(num) > 0) {
          // This is a winning number
          ++winningCount;
        }
      }
    }
    if (FLAGS_part2) {
      // Solving part 2
      for (size_t i = 1; i <= winningCount; ++i) {
        // Record the frequency of the next cards
        cardCount[cardNumber + i] += cardCount[cardNumber];
      }
    } else {
      // Solving part 1
      if (winningCount > 0) {
        answer += 1 << (winningCount - 1);
      }
    }
  }

  if (FLAGS_part2) {
    // Tally the result for part 2
    for (const auto k : cardCount) {
      if (k.first <= maxCardNumber) {
        // this is actually a card number that we found
        answer += k.second;
      }
    }
  }
  cout << (FLAGS_part2 ? "Part 2: " : "Part 1: ") << answer << endl;

  if (!FLAGS_part2) {
    cout << "Solution above is for part 1, if you need part 2 solution, add "
            "the flag --part2 to the command invocation"
         << endl;
  }
}
