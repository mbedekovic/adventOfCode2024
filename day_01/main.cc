#include <vector>
#include <fstream>
#include <sstream>
#include <numeric>
#include <tuple>
#include <iostream>
#include <algorithm>
#include <map>

using IdList = std::vector<long long>;

std::pair<IdList, IdList> parseInput()
{
    std::pair<IdList, IdList>       lists;
    std::fstream                    input("input.txt");
    std::string                     line;
    while (std::getline(input, line))
    {
        if (!line.empty())
        {
            // split
            std::stringstream ss{line};
            std::string token;
            ss >> token;
            lists.first.push_back(std::stoull(token));
            ss >> token;
            lists.second.push_back(std::stoull(token));
        }
    }
    return lists;
}

unsigned long long part1(IdList const& first, IdList const& second)
{
    unsigned long long sum = 0U;
    for (auto idx = 0U; idx < first.size(); idx++)
    {
        sum += std::abs(first.at(idx) - second.at(idx));
    }

    return sum;
}


unsigned long long part2(IdList const& first, IdList const& second)
{
    std::map<unsigned long long, size_t> occurrence;
    // Prefill the map of unique values
    for (auto idx = 0U; idx < first.size(); idx++)
    {
        occurrence[first.at(idx)] = 0;
    }

    for (auto idx = 0U; idx < second.size(); idx++)
    {
        auto mapItr = occurrence.find(second.at(idx));
        if (mapItr != occurrence.end())
        {
            mapItr->second += 1;
        }
    }

    return std::accumulate(occurrence.begin(), occurrence.end(), 0, [](size_t sum, auto const& mapItr)
    {
        return sum + (mapItr.first*mapItr.second);
    });
}

int main()
{
    IdList firstList;
    IdList secondList;
    std::tie(firstList, secondList) = parseInput();
    std::sort(firstList.begin(), firstList.end());
    std::sort(secondList.begin(), secondList.end());
    std::cout << "First part: " << part1(firstList, secondList) << std::endl;
    std::cout << "Second part: " << part2(firstList, secondList) << std::endl;
}