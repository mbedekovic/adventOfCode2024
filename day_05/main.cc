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

using Words = std::vector<std::string>;

struct Ordering
{
    std::vector<long> before;
    std::vector<long> after;
};

using OrderingRules = std::map<long, Ordering>;
using PageUpdate = std::vector<long>;
using PageUpdates = std::vector<PageUpdate>;


std::pair<OrderingRules, PageUpdates> parseInput()
{
    OrderingRules rules;
    PageUpdates updates;
    std::fstream                    input("input.txt");
    std::string                     line;
    bool parsingRules = true;
    while (std::getline(input, line))
    {
        if (!line.empty())
        {
            if (parsingRules)
            {
                std::stringstream ss(line);
                std::string token;
                std::getline(ss, token, '|');
                long before = std::stol(token);
                std::getline(ss, token, '|');
                long after = std::stol(token);

                // store ordering in map
                rules[before].after.push_back(after);
                rules[after].before.push_back(before);
            }
            else
            {
                updates.emplace_back(); // new updates
                // store updates
                std::stringstream ss(line);
                std::string token;
                while (std::getline(ss, token, ','))
                {
                    updates.back().push_back(std::stol(token));
                }
            }

        }
        else
        {
            parsingRules = false;
        }
    }
    return {rules,updates};
}


// Checks if the elements of the sequence before current position are not found in the list of "after" rules for a given element
bool verifyBefore(PageUpdate const& update, PageUpdate::const_iterator current, std::vector<long> const& after)
{
    // Each element before must not break the rules set by the before list
    return std::all_of(update.begin(), current, [&after](long u) {
        return std::none_of(after.begin(), after.end(), [u](long cur){ return cur == u; });
    });
}

// Checks if the elements of the sequence after current position are not found in the list of "before" rules for a given element
bool verifyAfter(PageUpdate const& update, PageUpdate::const_iterator current, std::vector<long> const& before)
{
    return std::all_of(current + 1,  update.end(), [&before](long u) {
        return std::none_of(before.begin(), before.end(), [u](long cur){ return cur == u; });
    });
}

bool verify(PageUpdate const& update, PageUpdate::const_iterator current, Ordering const& rules)
{
    return verifyBefore(update, current, rules.after) && verifyAfter(update, current, rules.before);
}

std::pair<PageUpdates, PageUpdates> partitionUpdates(OrderingRules const& rules, PageUpdates const& updates)
{
    PageUpdates validUpdates;
    PageUpdates invalidUpdates;
    // For each element in update, check if it satisfies the ordering by its rules
    for (auto const& update : updates)
    {
        bool valid = true;
        for (auto itr = update.begin(); itr != update.end(); itr++)
        {
            valid = valid && verify(update, itr, rules.at(*itr));
        }

        if (valid)
        {
            validUpdates.push_back(update);
        }
        else
        {
            invalidUpdates.push_back(update);
        }
    }
    return {validUpdates, invalidUpdates};
}

// Simpler solution to first part that came to mind once I solved the seconds part
// Using std::is_sorted to extract the already sorted update lists with the comparison functor based on the rules of the sorting.
std::pair<PageUpdates, PageUpdates> partitionSorted(OrderingRules const& rules, PageUpdates const& updates)
{
    // comparison lambda
    auto comp = [&rules](long before, long after)
    {

       return (std::any_of(rules.at(before).after.begin(), rules.at(before).after.end(), [after](long cur) { return cur == after; }))
            && (std::any_of(rules.at(after).before.begin(), rules.at(after).before.end(), [before](long cur) { return cur == before; }));
    };

    PageUpdates sorted, unsorted;
    for (auto const& update : updates)
    {
        if (std::is_sorted(update.begin(), update.end(), comp))
        {
            sorted.push_back(update);
        }
        else
        {
            unsorted.push_back(update);
        }
    }
    return {sorted, unsorted};
}


void sortInvalid(OrderingRules const& rules, PageUpdates& updates)
{
    // comparison lambda
    auto comp = [&rules](long before, long after)
    {

       return (std::any_of(rules.at(before).after.begin(), rules.at(before).after.end(), [after](long cur) { return cur == after; }))
            && (std::any_of(rules.at(after).before.begin(), rules.at(after).before.end(), [before](long cur) { return cur == before; }));
    };

    for (auto& update : updates)
    {
        // for each character, permutate the sequence
        std::sort(update.begin(), update.end(), comp);
    }
}

int main()
{

    PageUpdates updates;
    OrderingRules rules;
    std::tie(rules, updates) = parseInput();

    PageUpdates valid, invalid;
    // std::tie(valid, invalid) = partitionUpdates(rules, updates);
    std::tie(valid, invalid) = partitionSorted(rules, updates);

    size_t firstPart = std::accumulate(valid.begin(), valid.end(), 0,  [](size_t sum, PageUpdate const& update){
        size_t middle = update.size() / 2;
        return sum + update.at(middle);
    });

    std::cout << "First part: " << firstPart << std::endl;


    sortInvalid(rules, invalid);

    size_t secondPart = std::accumulate(invalid.begin(), invalid.end(), 0,  [](size_t sum, PageUpdate const& update){
        size_t middle = update.size() / 2;
        return sum + update.at(middle);
    });
    std::cout << "Second part: " << secondPart << std::endl;

    return 0;
}