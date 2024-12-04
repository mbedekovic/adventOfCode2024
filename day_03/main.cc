#include <regex>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <numeric>
#include <tuple>
#include <iostream>
#include <algorithm>
#include <map>

using Program = std::vector<std::string>;
Program parseInput()
{
    Program prog;
    std::fstream                    input("input.txt");
    std::string                     line;
    while (std::getline(input, line))
    {
        if (!line.empty())
        {
            prog.push_back(line);
        }
    }
    return prog;
}

using Args = std::vector<std::pair<long, long>>;

Args demangleAndExtractArgs(Program const& prog)
{
    // Input string
    Args output;
    for (auto const& line : prog)
    {
        // Regular expression to capture numbers from the pattern mul(X,Y)
        std::regex pattern(R"(mul\((\d{1,3}),(\d{1,3})\))");
        // Iterator to find all matches
        auto begin = std::sregex_iterator(line.begin(), line.end(), pattern);
        auto end = std::sregex_iterator();

        // Output the matches
        for (auto it = begin; it != end; ++it) {
            std::smatch match = *it;
            output.push_back({std::stol(match[1].str()), std::stol(match[2].str())});
        }
    }
    return output;
}

Args demangleAndExtractActiveArgs(Program const& prog)
{
    // Input string
    Args output;
    bool enabled = true;

    // Regular expression to capture numbers from the pattern mul(X,Y) along with "do()" and "don't()" substrings
    // Thanks ChatGPT for creating this regex, that is not my strong side
    std::regex pattern(R"(mul\((\d{1,3}),(\d{1,3})\)|do\(\)|don't\(\))");

    for (auto const& line : prog)
    {
        // Iterator to find all matches
        auto begin = std::sregex_iterator(line.begin(), line.end(), pattern);
        auto end = std::sregex_iterator();

        // Output the matches
        for (auto it = begin; it != end; ++it) {
            std::smatch match = *it;

            if (match[1].matched && match[2].matched && enabled)
            {
                output.push_back({std::stol(match[1].str()), std::stol(match[2].str())});
            }
            else if (match.str() == "do()")
            {
                enabled = true;
            }
            else if (match.str() == "don't()")
            {
                enabled = false;
            }
        }
    }
    return output;
}

int main()
{
    auto mangledInput = parseInput();
    {
        Args demangled = demangleAndExtractArgs(mangledInput);

        long totalSumPart1 = std::accumulate(demangled.begin(), demangled.end(), 0, [](long sum, std::pair<long, long> args)
        {
            return (args.first * args.second) + sum;
        });

        std::cout << "First part " << totalSumPart1 << std::endl;
    }

    {
       Args demangled = demangleAndExtractActiveArgs(mangledInput);

        long totalSumPart2 = std::accumulate(demangled.begin(), demangled.end(), 0, [](long sum, std::pair<long, long> args)
        {
            return (args.first * args.second) + sum;
        });

        std::cout << "Second part " << totalSumPart2 << std::endl;
    }
    return 0;
}
