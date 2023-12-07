#include "gflags/gflags.h"
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>

DEFINE_bool(part2, false, "Run the binary to solve part 2");
DEFINE_bool(debug, false, "Print the sorted list of hands");

using namespace std;
namespace {
enum HandType {
  kHigh = 0,
  kOnePair,
  kTwoPair,
  kThreeOfAKind,
  kFullHouse,
  kFourOfAKind,
  kFiveOfAKind,
};
struct Hand {
  Hand(const string raw, const size_t rank);

  string raw_;
  string rawStrength_;
  size_t rank_;
  HandType type_;
};

Hand::Hand(const string raw, const size_t rank) : raw_(raw), rank_(rank) {
  // Because we have to compare strenght of the cards later, we have to replace
  // the TJQKA with better thing that we can actually compare
  rawStrength_ = raw_;
  replace(rawStrength_.begin(), rawStrength_.end(), 'T', (char)('9' + 1));
  if (FLAGS_part2) {
    // Joker
    replace(rawStrength_.begin(), rawStrength_.end(), 'J', '1');
  } else {
    replace(rawStrength_.begin(), rawStrength_.end(), 'J', (char)('9' + 2));
  }
  replace(rawStrength_.begin(), rawStrength_.end(), 'Q', (char)('9' + 3));
  replace(rawStrength_.begin(), rawStrength_.end(), 'K', (char)('9' + 4));
  replace(rawStrength_.begin(), rawStrength_.end(), 'A', (char)('9' + 5));

  // Determine the type of the hand
  unordered_map<char, size_t> frequencyMap;
  for (const auto &card : raw_) {
    frequencyMap[card] += 1;
  }

  // Go through the frequency list, luckily we score only 1 way
  bool hasOnePair = false, hasTwoPair = false, hasThree = false,
       hasFour = false;
  for (const auto cardRecord : frequencyMap) {
    if (cardRecord.second == 5) {
      // Five of a kind - Joker doesn't help much here
      type_ = HandType::kFiveOfAKind;
      return;
    } else if (cardRecord.second == 4) {
      hasFour = true;
    } else if (cardRecord.second == 3) {
      hasThree = true;
    } else if (cardRecord.second == 2) {
      if (hasOnePair) {
        hasTwoPair = true;
      } else {
        hasOnePair = true;
      }
    }
  }
  if (hasFour) {
    if (FLAGS_part2 && (frequencyMap['J'] == 1 || frequencyMap['J'] == 4)) {
      // The other one of the 4 itself are jokers
      type_ = HandType::kFiveOfAKind;
    } else {
      // Strict four of a kind
      type_ = HandType::kFourOfAKind;
    }
  } else if (hasOnePair && hasThree) {
    if (FLAGS_part2 && (frequencyMap['J'] == 2 || frequencyMap['J'] == 3)) {
      // Activate joker and have 5 of a kind
      type_ = HandType::kFiveOfAKind;
    } else {
      // Strict Full house
      type_ = HandType::kFullHouse;
    }
  } else if (hasThree) {
    // The other 2 are different
    if (FLAGS_part2 && (frequencyMap['J'] == 1 || frequencyMap['J'] == 3)) {
      // One of the other 2 is joker or the 3 are jokers
      type_ = HandType::kFourOfAKind;
    } else {
      // Strict Three of a kind
      type_ = HandType::kThreeOfAKind;
    }
  } else if (hasTwoPair) {
    // The other 3 are different
    if (FLAGS_part2 && frequencyMap['J'] == 1) {
      // Remaining card is a joker
      type_ = HandType::kFullHouse;
    } else if (FLAGS_part2 && frequencyMap['J'] == 2) {
      // One of the pair is joker
      type_ = HandType::kFourOfAKind;
    } else {
      // Strict Two pair
      type_ = HandType::kTwoPair;
    }
  } else if (hasOnePair) {
    // The other 3 are different
    if (FLAGS_part2 && (frequencyMap['J'] == 1 || frequencyMap['J'] == 2)) {
      // One of the other 3 or the pair itself is joker
      type_ = HandType::kThreeOfAKind;
    } else {
      // Strict One pair
      type_ = HandType::kOnePair;
    }
  } else {
    // All 5 cards are different
    if (FLAGS_part2 && frequencyMap['J'] == 1) {
      type_ = HandType::kOnePair;
    } else {
      // Strict high
      type_ = HandType::kHigh;
    }
  }
  return;
}

} // namespace

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  size_t answer = 0;
  string hand;
  size_t rank;
  vector<Hand> hands;
  while (cin >> hand >> rank) {
    hands.emplace_back(hand, rank);
  }

  // Sort
  sort(hands.begin(), hands.end(), [](const Hand &a, const Hand &b) {
    if (a.type_ != b.type_) {
      return a.type_ < b.type_;
    } else {
      return a.rawStrength_ < b.rawStrength_;
    }
  });
  size_t counter = 1;
  for (const auto &hand : hands) {
    if (FLAGS_debug) {
      cout << hand.raw_ << " " << hand.rank_ << " " << hand.rawStrength_ << " "
           << hand.type_ << endl;
    }
    answer += counter++ * hand.rank_;
  }

  if (FLAGS_part2) {
    // Tally the result for part 2
  }
  cout << (FLAGS_part2 ? "Part 2: " : "Part 1: ") << answer << endl;

  if (!FLAGS_part2) {
    cout << "Solution above is for part 1, if you need part 2 solution, add "
            "the flag --part2 to the command invocation"
         << endl;
  }
  if (!FLAGS_debug) {
    cout << "If the solution is incorrect, use --debug to print out the sorted "
            "list of hands"
         << endl;
  }
}
