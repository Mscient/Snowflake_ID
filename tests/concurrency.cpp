#include <snowflake/snowflake.hpp>

#include <iostream>
#include <thread>
#include <vector>
#include <unordered_set>
#include <mutex>

void generate_ids(
    Snowflake& snowflake,
    std::vector<uint64_t>& ids,
    int count)
{
    for (int i = 0; i < count; i++)
    {
        ids.push_back(snowflake.generate());
    }
}

int main()
{
    Snowflake snowflake(1);

    const int THREAD_COUNT = 8;
    const int IDS_PER_THREAD = 10000;

    std::vector<std::thread> threads;

    std::vector<std::vector<uint64_t>> thread_ids(
        THREAD_COUNT
    );

    for (int i = 0; i < THREAD_COUNT; i++)
    {
        threads.emplace_back(
            generate_ids,
            std::ref(snowflake),
            std::ref(thread_ids[i]),
            IDS_PER_THREAD
        );
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    std::unordered_set<uint64_t> unique_ids;

    for (const auto& ids : thread_ids)
    {
        for (uint64_t id : ids)
        {
            unique_ids.insert(id);
        }
    }

    const int total_ids =
        THREAD_COUNT * IDS_PER_THREAD;

    std::cout << "Total IDs: "
              << total_ids << '\n';

    std::cout << "Unique IDs: "
              << unique_ids.size() << '\n';

    if (unique_ids.size() == total_ids)
    {
        std::cout << "[PASS] Concurrent generation\n";
        return 0;
    }

    std::cout << "[FAIL] Duplicate IDs detected\n";
    return 1;
}