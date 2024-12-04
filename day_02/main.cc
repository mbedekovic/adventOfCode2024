#include <vector>
#include <fstream>
#include <sstream>
#include <numeric>
#include <tuple>
#include <iostream>
#include <algorithm>
#include <map>

using Report = std::vector<long>;
using Reports = std::vector<Report>;

Reports parseInput()
{
    Reports reports;
    std::fstream                    input("input.txt");
    std::string                     line;
    while (std::getline(input, line))
    {
        if (!line.empty())
        {
            // split
            std::stringstream ss{line};
            std::string token;
            reports.emplace_back();
            while (!ss.eof())
            {
                ss >> token;
                reports.back().push_back(std::stoul(token));
            }
        }
    }
    return reports;
}

std::ostream& operator<<(std::ostream& os, Report const& v)
{
    os << "[";
    for (auto i : v)
    {
        os << i << " ";
    }
    os << " ]";
    return os;
}

Report::const_iterator findFirstOffender(Report const& report)
{
    if (report.size() < 2)
    {
        return report.end();
    }
    bool grows = report.at(1) > report.at(0);
    auto itr = std::adjacent_find(report.begin(), report.end(), [grows](long left, long right)
    {
        long diff = right - left;
        // Diff not in range
        if ((std::abs(diff) < 1) || std::abs(diff) > 3)
        {
            return true;
        }

        // direction change
        if (grows && (diff < 0))
        {
            return true;
        }
        if (!grows && (diff > 0))
        {
            return true;
        }

        // all good
        return false;
    });
    return itr;
}

bool isSafe(Report const& report)
{
    return findFirstOffender(report) == report.end();
}


long part1(Reports const& reports)
{
    return std::count_if(reports.begin(), reports.end(), [](Report const& report)
    {
        return isSafe(report);
    });
}

long part2(Reports const& reports)
{
    return std::count_if(reports.begin(), reports.end(), [](Report const& report) {
        for (auto itr = report.begin(); itr != report.end(); itr++)
        {
            Report modifiedReport;
            modifiedReport.insert(modifiedReport.end(), report.begin(), itr);
            modifiedReport.insert(modifiedReport.end(), itr+1, report.end());
            if (findFirstOffender(modifiedReport) == modifiedReport.end())
            {
                return true;
            }
        }
        return false;
    });
}


int main()
{
    auto reports = parseInput();

    std::cout << "First part: " << part1(reports) << std::endl;
    std::cout << "Second part: " << part2(reports) << std::endl;

    return 0;
}