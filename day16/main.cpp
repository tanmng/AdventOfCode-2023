#include <iostream>
#include <queue>
#include <set>
#include <utility>
#include <vector>

using namespace std;
namespace {
// Possible field values
const char kEmpty = '.', kNS = '|', kWE = '-', kWN = '/', kNE = '\\';
// Possible direction of travel
enum TravelDirection {
  kEast = 0,
  kSouth,
  kWest,
  kNorth,
};
typedef pair<size_t, size_t> Coordinate;
typedef pair<Coordinate, TravelDirection> TravelState;

/**
 * Construct a new travel state given the old coordinate and the direction to
 * switch to
 *
 * Throw if the travel will end up outside of the map (given by map width and
 * height)
 */
TravelState newState(const Coordinate oldCoordinate,
                     const TravelDirection newDirection, const size_t mapWidth,
                     const size_t mapHeight) {
  switch (newDirection) {
  case kEast:
    if (oldCoordinate.first == mapWidth - 1) {
      // We are already at the right edge -> no more travel
      throw runtime_error("Trying to travel pass right edge");
    }
    return {{oldCoordinate.first + 1, oldCoordinate.second}, newDirection};
  case kWest:
    if (oldCoordinate.first == 0) {
      // We are already at the left edge -> no more travel
      throw runtime_error("Trying to travel pass left edge");
    }
    return {{oldCoordinate.first - 1, oldCoordinate.second}, newDirection};
  case kNorth:
    if (oldCoordinate.second == 0) {
      // We are already at the top edge -> no more travel
      throw runtime_error("Trying to travel pass top edge");
    }
    return {{oldCoordinate.first, oldCoordinate.second - 1}, newDirection};
  case kSouth:
    if (oldCoordinate.second == mapHeight - 1) {
      // We are already at the bottom edge -> no more travel
      throw runtime_error("Trying to travel pass bottom edge");
    }
    return {{oldCoordinate.first, oldCoordinate.second + 1}, newDirection};
  default:
    throw logic_error("Invalid direction");
  }
  throw logic_error("This should never happen");
}

/**
 * Try to energize the map
 */
size_t energizing(const vector<string> &map, const Coordinate &startingPoint,
                  const TravelDirection startingDirection) {
  set<TravelState> visited;
  queue<TravelState> traveling;
  auto mapWidth = map.at(0).size(), mapHeight = map.size();
  traveling.push({startingPoint, startingDirection});
  while (!traveling.empty()) {
    auto cur = traveling.front();
    traveling.pop();
    if (visited.count(cur) == 1) {
      // We already visited this -> skip
      continue;
    }
    auto coordinate = cur.first;
    auto direction = cur.second;
    // We can have a ray split into 2, so we need a vector here
    vector<TravelDirection> newDirections;
    const auto mapVal = map.at(coordinate.second).at(coordinate.first);
    switch (mapVal) {
    case kEmpty:
      // Empty - continue the travel
      newDirections.push_back(direction);
      break;
    case kNE:
      // \ character
      switch (direction) {
      case TravelDirection::kEast:
        // Traveling east -> switch to south
        newDirections.push_back(TravelDirection::kSouth);
        break;
      case TravelDirection::kSouth:
        // Traveling south -> switch to east
        newDirections.push_back(TravelDirection::kEast);
        break;
      case TravelDirection::kWest:
        // Traveling west -> switch to north
        newDirections.push_back(TravelDirection::kNorth);
        break;
      case TravelDirection::kNorth:
        // Traveling north -> switch to west
        newDirections.push_back(TravelDirection::kWest);
        break;
      }
      break;
    case kWN:
      // / character
      switch (direction) {
      case TravelDirection::kEast:
        // Traveling east -> switch to north
        newDirections.push_back(TravelDirection::kNorth);
        break;
      case TravelDirection::kSouth:
        // Traveling south -> switch to west
        newDirections.push_back(TravelDirection::kWest);
        break;
      case TravelDirection::kWest:
        // Traveling west -> switch to south
        newDirections.push_back(TravelDirection::kSouth);
        break;
      case TravelDirection::kNorth:
        // Traveling north -> switch to east
        newDirections.push_back(TravelDirection::kEast);
        break;
      }
      break;
    case kWE:
      // - character
      switch (direction) {
      case TravelDirection::kEast:
      case TravelDirection::kWest:
        // Traveling west/east -> pass right through
        newDirections.push_back(direction);
        break;
      case TravelDirection::kSouth:
      case TravelDirection::kNorth:
        // Traveling north/south -> Split into 2
        newDirections.push_back(TravelDirection::kEast);
        newDirections.push_back(TravelDirection::kWest);
        break;
      }
      break;
    case kNS:
      // | character
      switch (direction) {
      case TravelDirection::kEast:
      case TravelDirection::kWest:
        // Traveling west/east -> split into 2
        newDirections.push_back(TravelDirection::kNorth);
        newDirections.push_back(TravelDirection::kSouth);
        break;
      case TravelDirection::kSouth:
      case TravelDirection::kNorth:
        // Traveling north/south -> Pass right through
        newDirections.push_back(direction);
        break;
      }
      break;
    }

    // Mark new ones for process
    for (const auto &dir : newDirections) {
      try {
        traveling.push(newState(coordinate, dir, mapWidth, mapHeight));
      } catch (runtime_error &e) {
        // Outside of map - just skip
      }
    }

    // Record that we already recorded this
    visited.insert(cur);
  }

  // Count all the coordinate that we have energized
  set<Coordinate> visitedCoordinate;
  for (const auto &state : visited) {
    auto coordinate = state.first;
    visitedCoordinate.insert(coordinate);
  }
  return visitedCoordinate.size();
}

} // namespace

int main(int argc, char **argv) {
  size_t answerPart1 = 0, answerPart2 = 0;
  string line;
  vector<string> lines;
  while (getline(cin, line)) {
    lines.push_back(std::move(line));
  }

  // Part 1
  answerPart1 = energizing(lines, {0, 0}, TravelDirection::kEast);

  cout << "Part 1: " << answerPart1 << endl;

  // Part 2
  // Try all the line/row
  for (size_t i = 0; i < lines.size(); ++i) {
    // From the left edge
    answerPart2 =
        max(answerPart2, energizing(lines, {0, i}, TravelDirection::kEast));
    // From the right edge
    answerPart2 =
        max(answerPart2, energizing(lines, {lines.at(0).size() - 1, i},
                                    TravelDirection::kWest));
  }

  // Try all the column
  for (size_t i = 0; i < lines.at(0).size(); ++i) {
    // From the top edge
    answerPart2 =
        max(answerPart2, energizing(lines, {i,0}, TravelDirection::kSouth));
    // From the right edge
    answerPart2 =
        max(answerPart2, energizing(lines, {i, lines.size() - 1},
                                    TravelDirection::kNorth));
  }

  cout << "Part 2: " << answerPart2 << endl;
}
