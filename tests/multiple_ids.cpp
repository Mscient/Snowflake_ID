#include <iostream>
#include <unordered_set>
#include <chrono>

#include <snowflake/snowflake.hpp>

int main()
{
    Snowflake snowflake(1);

    const int ID_COUNT = 10000;

    std::unordered_set<uint64_t> ids;

    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < ID_COUNT; i++)
    {
        uint64_t id = snowflake.generate();
        ids.insert(id);
    }

    auto end = std::chrono::steady_clock::now();

    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(
            end - start
        ).count();

    std::cout << "Total IDs: " << ID_COUNT << '\n';
    std::cout << "Unique IDs: " << ids.size() << '\n';
    std::cout << "Duplicates: "
              << static_cast<std::size_t>(ID_COUNT) - ids.size() << '\n';

    std::cout << "Generation time: "
              << duration << " us\n";

    if (ids.size() == static_cast<std::size_t>(ID_COUNT))
    {
        std::cout << "[PASS] Multiple ID generation\n";
        return 0;
    }

    std::cout << "[FAIL] Duplicate IDs detected\n";
    return 1;
}