#ifndef SIMCAN_EX_CONTROL_TABLE
#define SIMCAN_EX_CONTROL_TABLE

#include <vector>
#include <algorithm>

template <class T>
class ControlTable
{
    std::vector<std::pair<bool, T>> elements;
    uint32_t lastId;
    uint32_t allocatedIds;

protected:
    /**
     * @brief Scans for a free entry
     * @details It assumes that there's at least one entry
     * @return uint32_t The first index/id that it's free
     */
    uint32_t scanFreeId()
    {
        // Filter
        const auto filter = [](std::pair<bool, T> &e) -> bool
        { return e.first; };

        // Search for a free slot
        auto beginning = elements.begin();
        beginning += lastId;

        auto iter = std::find_if(beginning, elements.end(), filter);

        // Found going forwards
        if (iter != elements.end())
        {
            lastId = iter - beginning;
            return lastId;
        }

        // Found going backwards
        iter = std::find_if(elements.begin(), beginning, filter);
        lastId = iter - beginning;
        return lastId;
    }

public:
    typedef void (T::*InitFunc)(uint32_t);

    /**
     * @brief Initializes the table
     * @param size      Number of entries
     * @param init_f    Initializer function which takes the given id
     */
    void init(uint32_t size, InitFunc init_f)
    {
        // Reserve memory
        elements.reserve(size);

        // Construct and give the VM id
        for (uint32_t i = 0; i < size; i++)
        {
            // Default initialize elements
            elements.emplace_back(true, T());
            auto inserted = elements.back();

            // Call init function
            (inserted.second.*init_f)(i);
        }

        // Save memory
        elements.shrink_to_fit();
    }

    uint32_t takeId()
    {
        uint32_t id = scanFreeId();
        elements[id].first = false;
        allocatedIds++;
        return id;
    }

    void releaseId(uint32_t id)
    {
        allocatedIds--;
        elements[id].first = true;
    }

    uint32_t getAllocatedIds() { return allocatedIds; }
    uint32_t getFreeIds() { return elements.size() - allocatedIds; }

    T &operator[](uint32_t id) { return elements[id].second; }
    T &at(uint32_t id) { return elements.at(id).second; }

    typedef typename std::vector<std::pair<bool, T>>::iterator iterator;
    iterator begin() { return elements.begin(); }
    iterator end() { return elements.end(); }
};

#endif /* SIMCAN_EX_CONTROL_TABLE */