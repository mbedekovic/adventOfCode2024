#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <numeric>
#include <iostream>
#include <algorithm>
#include <utility>
#include <functional>
#include <map>
#include <future>

using Map = std::vector<std::string>;

Map parseInput()
{
    Map                             map;
    std::fstream                    input("input.txt");
    std::string                     line;
    bool parsingRules = true;
    while (std::getline(input, line))
    {
        if (!line.empty())
        {
            map.push_back(line);
        }
    }
    return map;
}

struct Direction
{
    long row;
    long col;
};

struct Point
{
    long row;
    long col;
};
bool operator<(Point const& l, Point const& r)
{
    if (l.row == r.row)
    {
        return l.col < r.col;
    }
    return l.row < r.row;
}

std::pair<size_t, size_t> operator+(std::pair<size_t, size_t> const& a, std::pair<size_t, size_t> const& b)
{
    return {a.first + b.first, a.second + b.second};
}

size_t countDistinctPositions(Map const& map, Point point, Direction direction, std::vector<Point>& visited)
{
    // exit condition when we leave the map
    if ((point.row < 0) || (point.col < 0) || (point.row >= map.size()) || (point.col >= map.front().size()))
    {
        return 0;
    }

    Point next = point;
    Direction newDirection = direction;
    if (map[point.row][point.col] == '#')
    {
        // step back and change direction
        next.row -= direction.row;
        next.col -= direction.col;

        // clockwise rotation of 2D vector
        newDirection.row = direction.col;
        newDirection.col = -1 * direction.row;

        return countDistinctPositions(map, next, newDirection, visited);
    }

    next.row += newDirection.row;
    next.col += newDirection.col;

    if (std::any_of(visited.begin(), visited.end(), [&point](Point const& p){ return p.row == point.row && p.col == point.col;}))
    {
        // Back on the same path. Guard should turn here to end up in the loop
        return countDistinctPositions(map, next, newDirection, visited);
    }
    else
    {
        visited.push_back(point);
        return  1 + countDistinctPositions(map, next, newDirection, visited);
    }
}

Point findStart(Map const& map)
{
    Point start;
    start.col = 0;
    start.row = 0;
    for (auto row : map)
    {
        start.col = row.find('^');
        if (start.col != std::string::npos)
        {
            return start;
        }

        start.row++;
    }
    return start;
}

// modify the map with one obstruction, run the path until either we exit the map or if we visited some obstacles multiple times
bool loopDetector(Map const& map, Point point, Direction direction, std::map<Point, size_t>& visited)
{
    // exit condition when we leave the map
    if ((point.row < 0) || (point.col < 0) || (point.row >= map.size()) || (point.col >= map.front().size()))
    {
        return false;
    }

    Point next = point;
    Direction newDirection = direction;
    if (map[point.row][point.col] == '#')
    {
        // Store the ocurred obstacles
        visited[point] += 1;

        // step back and change direction
        next.row -= direction.row;
        next.col -= direction.col;

        // clockwise rotation of 2D vector
        newDirection.row = direction.col;
        newDirection.col = -1 * direction.row;

        return loopDetector(map, next, newDirection, visited);
    }

    next.row += newDirection.row;
    next.col += newDirection.col;

    // Infinite loop might only be a part of all the visited points.
    // Detect if some of the obstacle points were visited more than 4 times (arbitrary large limit)
    if (!visited.empty() && std::any_of(visited.begin(), visited.end(), [](std::pair<Point, size_t> const& elm){ return elm.second >= 4; }))
    {
        // loop detected
        return true;
    }
    else
    {
        return loopDetector(map, next, newDirection, visited);
    }

}

size_t countInfiniteLoops(Map map, Point start, Direction direction)
{
    std::vector<std::future<size_t>> tasks;
    size_t cnt = 0;
    size_t i = 0;
    for (auto& row : map)
    {
        // launch parallel tasks for each line of the map
        tasks.push_back(std::async(std::launch::async,
        [start, direction](Map map, size_t i) -> size_t {
            auto& row = map.at(i);
            size_t count = 0;
            for (auto& field  : row)
            {
                if (field != '#' && field != '^')
                {
                    std::map<Point, size_t> visited;
                    // place obstacle and try looping
                    field = '#';
                    count += loopDetector(map, start, direction, visited);
                    // reset
                    field = '.';

                }
            }
            return count;
        }, map, i));
        i++;
    }

    // Sum the returning values of all tasks once they are done
    cnt = std::accumulate(tasks.begin(), tasks.end(), 0, [](size_t sum, std::future<size_t>& f) { return sum + f.get();});
    return cnt;
}

int main()
{
    auto map = parseInput();

    Point start = findStart(map);
    Direction initial{-1, 0};
    std::vector<Point> visited;

    size_t count = countDistinctPositions(map, start, initial, visited);

    std::cout << "First part: " << count << std::endl;

    size_t countLoops = countInfiniteLoops(map, start, initial);

    std::cout << "Second part: " << countLoops << std::endl;
}
