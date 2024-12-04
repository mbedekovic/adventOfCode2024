#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <numeric>
#include <iostream>
#include <algorithm>
#include <utility>
#include <functional>

using Words = std::vector<std::string>;
Words parseInput()
{
    Words                           words;
    std::fstream                    input("input.txt");
    std::string                     line;
    while (std::getline(input, line))
    {
        if (!line.empty())
        {
            words.push_back(line);
        }
    }
    return words;
}


// Recursive function to match all characters of a string starting from the point and moving direction determined by functor "nextPoint"
bool find(Words const& words, std::string match, std::pair<long,long> point, std::function<std::pair<long,long>(long, long)> nextPoint)
{
    // matched fully, success
    if (match.empty())
    {
        return true;
    }

    long row, col;
    std::tie(row, col) = point;

    // out of bounds
    if ((row < 0) || (col < 0) || (row >= words.size()) || (col >= words.front().size()))
    {
        return false;
    }

    // Character matches, search for next character in a given direction
    if (words.at(row).at(col) == match.front())
    {
        std::string remaining(match.begin()+1, match.end());
        return find(words, remaining, nextPoint(row,col), nextPoint);
    }
    // character doesn't match, word not found
    return false;

}

// For each character 'X' found, search in all directions from this point for the phrase "XMAS"
size_t findXmas(Words const& words)
{
    std::string key = "XMAS";
    long i = 0;
    long j = 0;
    size_t xmasFoundCnt = 0;
    for (auto const& line : words)
    {
        auto itr = line.begin();
        itr = std::find(itr, line.end(), 'X');
        while (itr != line.end())
        {
            j = std::distance(line.begin(), itr);
            xmasFoundCnt += (find(words, key, {i,j}, [](long row, long col) -> std::pair<long,long> {return {row, col + 1};}) ? 1 : 0); // right
            xmasFoundCnt += (find(words, key, {i,j}, [](long row, long col) -> std::pair<long,long> {return {row, col - 1};}) ? 1 : 0); // left
            xmasFoundCnt += (find(words, key, {i,j}, [](long row, long col) -> std::pair<long,long> {return {row + 1, col};}) ? 1 : 0); // down
            xmasFoundCnt += (find(words, key, {i,j}, [](long row, long col) -> std::pair<long,long> {return {row - 1, col};}) ? 1 : 0); // up

            xmasFoundCnt += (find(words, key, {i,j}, [](long row, long col) -> std::pair<long,long> {return {row + 1, col + 1};}) ? 1 : 0); // right down
            xmasFoundCnt += (find(words, key, {i,j}, [](long row, long col) -> std::pair<long,long> {return {row + 1, col - 1};}) ? 1 : 0); // left down
            xmasFoundCnt += (find(words, key, {i,j}, [](long row, long col) -> std::pair<long,long> {return {row - 1, col + 1};}) ? 1 : 0); // right up
            xmasFoundCnt += (find(words, key, {i,j}, [](long row, long col) -> std::pair<long,long> {return {row - 1, col - 1};}) ? 1 : 0); // left up

            itr = std::find(itr + 1, line.end(), 'X');
        }
        i++;
    }
    return xmasFoundCnt;
}

// In this case the character 'A' is the center of the pattern.
// For each 'A' character found look if diagonals form a phrase "MAS" in any direction
// Two directions must match for the pattern to match "X-MAS"
size_t findX_mas(Words const& words)
{
    std::string key{"MAS"};
    long i = 0;
    long j = 0;
    size_t xmasFoundCnt = 0;
    // Find all each 'A' char and check if it's surrounded by M and S on diagonals
    for (auto const& line : words)
    {
        auto itr = line.begin();
        itr = std::find(itr, line.end(), 'A');
        while (itr != line.end())
        {
            size_t masFound = 0;
            j = std::distance(line.begin(), itr);
            // MAS from up left to bottom right
            if (find(words, "AM", {i,j}, [](long row, long col) -> std::pair<long,long> {return {row - 1, col - 1};}) &&
                find(words, "AS", {i,j}, [](long row, long col) -> std::pair<long,long> {return {row + 1, col + 1};}) )
            {
                masFound++;
            }

            // MAS from bottom right to top left
            if (find(words, "AM", {i,j}, [](long row, long col) -> std::pair<long,long> {return {row + 1, col + 1};}) &&
                find(words, "AS", {i,j}, [](long row, long col) -> std::pair<long,long> {return {row - 1, col - 1};}) )
            {
                masFound++;
            }

            // MAS from bottom left to top right
            if (find(words, "AM", {i,j}, [](long row, long col) -> std::pair<long,long> {return {row + 1, col - 1};}) &&
                find(words, "AS", {i,j}, [](long row, long col) -> std::pair<long,long> {return {row - 1, col + 1};}) )
            {
                masFound++;
            }

            // MAS from top right to bottom left
            if (find(words, "AM", {i,j}, [](long row, long col) -> std::pair<long,long> {return {row - 1, col + 1};}) &&
                find(words, "AS", {i,j}, [](long row, long col) -> std::pair<long,long> {return {row + 1, col - 1};}) )
            {
                masFound++;
            }

            if (masFound == 2)
            {
                xmasFoundCnt++;
            }

            itr = std::find(itr + 1, line.end(), 'A');
        }
        i++;
    }
    return xmasFoundCnt;
}

int main()
{
    Words words = parseInput();

    std::cout << "First part: " << findXmas(words) << std::endl;
    std::cout << "Second part: " << findX_mas(words) << std::endl;

    return 0;
}