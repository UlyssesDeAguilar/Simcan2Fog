#ifndef SIMCAN_EX_GROUP_VECTOR
#define SIMCAN_EX_GROUP_VECTOR

#include <vector>

template <class E>
class group_vector
{
private:
    std::vector<E> elements;         // Flat vector to store all elements of type E
    std::vector<size_t> boundraries; // Vector to store indices where collections start

public:
    class group_iterator
    {
    private:
        const group_vector &parent; // Reference to the parent group_vector
        size_t collection_index;    // The collection where iterator resides

    public:
        // Constructor
        group_iterator(const group_vector &parent, size_t collection_index)
            : parent(parent), collection_index(collection_index) {}

        // Dereference operator to get the current element
        size_t &operator*()
        {
            return parent.boundraries[collection_index];
        }

        // Increment operator to move to the next element
        group_iterator &operator++()
        {
            ++collection_index;
            return *this;
        }

        // Equality operator to check for group_iterator equality
        bool operator==(const group_iterator &other) const
        {
            return &parent == &other.parent && collection_index == other.collection_index;
        }

        // Inequality operator to check for group_iterator inequality
        bool operator!=(const group_iterator &other) const
        {
            return !(*this == other);
        }

        class iterator
        {
        private:
            const group_vector &parent; // Reference to the parent group_vector
            size_t current_index;        // Current index in the flat vector

        public:
            // Constructor
            iterator(const group_vector &parent, size_t current_index)
                : parent(parent), current_index(current_index) {}

            // Dereference operator to get the current element
            E &operator*()
            {
                return parent.elements[current_index];
            }

            // Increment operator to move to the next element
            iterator &operator++()
            {
                ++current_index;
                return *this;
            }

            // Equality operator to check for flat_iterator equality
            bool operator==(const iterator &other) const
            {
                return &parent == &other.parent && current_index == other.current_index;
            }

            // Inequality operator to check for flat_iterator inequality
            bool operator!=(const iterator &other) const
            {
                return !(*this == other);
            }
        };

        iterator begin() const
        {
            return iterator(parent, parent.boundraries[collection_index]);
        }

        iterator end() const
        {
            size_t size = parent.get_collection_size(collection_index);
            return iterator(parent, collection_index + size);
        }
    };

    // Constructor
    group_vector()
    {
        boundraries.push_back(0); // Initially, there is one collection starting at index 0
    }

    // Function to add an object of type E to the current collection
    void push_back(const E &x)
    {
        elements.push_back(x);
    }

    // Function to start a new collection
    void new_collection()
    {
        boundraries.push_back(elements.size());
    }

    ssize_t get_collection_size(size_t index) const
    {
        size_t last_pos = boundraries.size() - 1;

        // Boundrary check
        if (index < 0 || index > last_pos)
            return -1;

        // Special case
        if (index == last_pos)
            return elements.size() - boundraries[last_pos];

        return boundraries[index + 1] - boundraries[index];
    }

    // Iterator functions
    group_iterator begin() const
    {
        return group_iterator(*this, 0);
    }

    group_iterator end() const
    {
        return group_iterator(*this, boundraries.size());
    }

    std::vector<E> &flattened()
    {
        return elements;
    }
};

#endif