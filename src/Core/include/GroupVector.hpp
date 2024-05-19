#ifndef SIMCAN_EX_GROUP_VECTOR
#define SIMCAN_EX_GROUP_VECTOR

#include <vector>
#include <iterator>
#include <limits>
#include <algorithm>

template <class S, class E>
class group_vector
{
public:
    /**
     * @brief Construct a new group vector
     * @param rep The representative of the first collection
     */
    group_vector(const S rep = S()) { boundraries.push_back(collection(this, 0, 0, rep)); }

    /**
     * @brief Inserts a object into the current active collection
     * @param x Element to be inserted
     */
    void push_back(const E &x) { elements.push_back(x); }

    /**
     * @brief Constructs in-place the element with the forwarded arguments
     * @tparam Args Argument variadic template
     * @param args Arguments to be forwarded
     * @return E& Reference to the newly constructed object
     */
    template <class... Args>
    E &emplace_back(Args &&...args)
    {
        // If we switch to c++17 then we could just return after emplace_black()
        elements.emplace_back(args...);
        return elements.back();
    }

    /**
     * @brief Marks the start of a new collection
     * @param rep Representative of the collection/group
     */
    void new_collection(const S &rep)
    {
        boundraries.push_back(collection(this, boundraries.size(), elements.size(), rep));
    }

    /**
     * @brief Gets the collection size
     * @param index Index of the collection
     * @return size_t Size of given collection or the max number a size_t can hold
     */
    size_t get_collection_size(size_t index) const
    {
        size_t last_pos = boundraries.size() - 1;

        // Boundrary check
        if (index < 0 || index > last_pos)
            return std::numeric_limits<size_t>::max();

        // Special case
        if (index == last_pos)
            return elements.size() - boundraries[last_pos].collection_index;

        return boundraries[index + 1].collection_index - boundraries[index].collection_index;
    }

    void reserve(size_t n) { elements.reserve(n); }
    size_t size() const { return elements.size(); }
    const S &operator[](size_t index) const { return boundraries[index].element; }
    const S &at(size_t index) const { return boundraries.at(index).element; }

    S &operator[](size_t index) { return boundraries[index].element; }
    S &at(size_t index) { return boundraries.at(index).element; }

    /**
     * @brief Reference to the internal element vector for a "flatenned" access.
     * Do not modify or dire things may happen
     * @return std::vector<E>& The reference to the vector containing the elements.
     */
    std::vector<E> &flattened() { return elements; }

    class collection
    {
    private:
        group_vector *parent;

    protected:
        size_t group_index;
        size_t collection_index;
        collection(group_vector *parent,
                   size_t g_index,
                   size_t c_index,
                   const S &element) : parent(parent),
                                       group_index(g_index),
                                       collection_index(c_index),
                                       element(element) {}

    public:
        S element;

        const E &operator[](size_t index) const { return this->operator[](index); }
        const E &at(size_t index) const { return this->at(index); }

        E &operator[](size_t index) { return parent->elements[collection_index + index]; }
        E &at(size_t index)
        {
            // As this is only positive, the check should only really go for not invading the next collection
            if (index >= parent->get_collection_size(group_index))
                throw std::out_of_range("out of range");

            return this[index];
        }

        typedef typename std::vector<E>::iterator iterator;
        typedef typename std::vector<E>::const_iterator const_iterator;

        size_t size() const { return parent->get_collection_size(group_index); }

        iterator begin() { return parent->elements.begin() + collection_index; }
        iterator end() { return parent->elements.begin() + (collection_index + parent->get_collection_size(group_index)); }

        const_iterator begin() const { return parent->elements.begin() + collection_index; }
        const_iterator end() const { return parent->elements.begin() + (collection_index + parent->get_collection_size(group_index)); }

        bool operator==(const S &element) const { return this->element == element; }
        bool operator!=(const S &element) const { return this->element != element; }

        friend class group_vector;
    };

    // typename is used because collection<S> is a dependent name
    typedef typename std::vector<collection>::iterator iterator;
    typedef typename std::vector<collection>::const_iterator const_iterator;

    iterator begin() { return boundraries.begin(); }
    iterator end() { return boundraries.end(); }

    const_iterator begin() const { return boundraries.begin(); }
    const_iterator end() const { return boundraries.end(); }

    iterator find(const S &key)
    {
        return std::find_if(boundraries.begin(), boundraries.end(), [key](const collection &c)
                            { return c.element == key; });
    }

private:
    std::vector<E> elements;             // Flat vector to store all elements of type E
    std::vector<collection> boundraries; // Vector to store indices where collections start
};

#endif
