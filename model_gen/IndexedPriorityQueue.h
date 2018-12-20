#ifndef INDEXED_PRIORITY_QUEUE_H
#define INDEXED_PRIORITY_QUEUE_H

//---------------------------------------------------------------------------
//
// Copyright (c) Miami University, Oxford, OHIO.
// All rights reserved.
//
// Miami University (MU) makes no representations or warranties about
// the suitability of the software, either express or implied,
// including but not limited to the implied warranties of
// merchantability, fitness for a particular purpose, or
// non-infringement.  MU shall not be liable for any damages suffered
// by licensee as a result of using, result of using, modifying or
// distributing this software or its derivatives.
//
// By using or copying this Software, Licensee agrees to abide by the
// intellectual property laws, and all other applicable laws of the
// U.S., and the terms of this license.
//
// Authors: Dhananjai M. Rao       raodm@miamiOH.edu
//
//---------------------------------------------------------------------------

#include <queue>
#include <unordered_map>
#include <algorithm>

/** A convenience functor to use the value also as the key.

    This is a default functor that enables using the value also as the
    key to look-up values in the IndexedPriorityQueue.
*/
template<typename ValueType, typename KeyType>
struct KeyFunctor {
    KeyType operator()(const ValueType& value) const {
	return value();
    }
};

/** A priority queue that enables rapid look-ups and changing priority.

    This class provides a custom priority-queue that provides the
    following key features:

    <ul>

    <li>Maintains items as a heap, with the highest priority item at
    the top of the heap.  Inserts and removal of an item is
    accomplished in O(log n) time-complexity</li>

    <li>It enables updating/changing the priority of items in the heap
    in O(long n) time-complexity</li>
    
    <li>In addition, it maintains keys in a hash map to enable
    constant-time look-up of items based on key values.</li>

    </ul>
*/
template< typename ValueType, typename KeyType,
	  typename GetKey  = KeyFunctor<ValueType, KeyType>,
          typename Compare = std::less<ValueType>,
          typename Hash    = std::hash<KeyType> >
class IndexedPriorityQueue {
    /** Internal data type used to hold values as a heap */
    using QueueType = std::vector<ValueType>;
    /** The internal hash map used to enable key-based look-up */
    using IndexType = std::unordered_map<KeyType, size_t, Hash>;
public:
    /** The constructor this data structure.

	\param[in] keyGetter The function to be used to obtain the key
	value asssociated with a given value.

	\param[in] comparator The priority-comparison function to be
	used to organize values in priority order in this queue.
    */
    IndexedPriorityQueue(const GetKey&  keyGetter  = GetKey(),
			 const Compare& comparator = Compare()) :
	getKey(keyGetter), compare(comparator) {
	// Nothing else to be done in the constructor.
    }

    /** Interface method to add (or update) a new element to the heap.

	This method is the primary interface method that must be used
	to add a new value, with a new key.  Note that if the key
	already exists, this method calls the update() method in this
	class and does not add a new entry for the value.

	\param[in] value The value to be added to the priority queue.
	The heap is updated to preserve the priority.
    */
    void push(const ValueType& value) {
	// Ensure that the key for this value does not exist in our
	// index. Duplicate values cause issues.
	if (index.find(getKey(value)) != index.end()) {
	    update(value);
	    return;
	}
	// Add new entry to the heap with index seutp.
	const size_t pos     = queue.size();
	index[getKey(value)] = pos;
	queue.push_back(value);
	// Update the position
	fixHeap(pos, value);
    }

    /** Update an existing entry and fix-up the priority queue.

	This method must be used to update an existing value (and
	consequently its priority).  If the key for the given entry
	does not exist, then this method will throw an exception.  The
	priority queue is updated after change in the entry value.

	\param[in] value The entry whose information and priority is
	to be updated.
    */
    void update(const ValueType& value) {
	const size_t pos = index.at(getKey(value));
	queue[pos] = value;
	fixHeap(pos, value);
    }

    /** Determine if the queue is empty.

        \return This method returns true if the queue is empty
     */
    bool empty() const {
        return queue.empty();
    }
    
    /** Get a copy of the element from the heap, given its key.

	\param[in] key The key associated with the value to be
	returned from the heap.  If the key does not exist, then this
	method throws an exception.

	\return The value (push'd onto the heap earlier) associated
	with the given key value.  Note that a copy is returned so
	that the actual value in the pirority queue is not modified
	externally (thereby breaking the priority queue)

	\see contains
    */    
    ValueType at(const KeyType& key) const {
	return queue[index.at(key)];
    }

    /** Determine if a given key exists in the internal index.

	\param[in] key The key whose existence is to be checked.

	\return This method returns true if the key exists (the value
	for the key can be retrieved via call to the at method in this
	class).  Otherwise it returns false.

	\see at
     */
    bool contains(const KeyType& key) const {
	return (index.find(key) != index.end());
    }

    /** Obtain the highest priority element in this queue.

        \note Calling this method with an empty priority queue results
        in an error.
        
        \return The highest piority element in this queue.
    */
    inline ValueType top() const {
        return queue.front();
    }

    /** Remove the highest priority element in this queue.

        Removes the highest prirority element from this priority
        queue.
    */
    void pop() {
        if (queue.size() > 1) {
            // Remove the index entry for the top-element
            index.erase(getKey(queue[0]));
            // Swap the first and last values to make fixing heaps
            // and removing the last entry
            std::swap(queue[0], queue[queue.size() - 1]);
            // Update the index position for the last element
            index[getKey(queue[0])] = 0;
            // Remove the last element in the queue
            queue.pop_back();
            // Move the first element (that we swapped) to its correct
            // location on the heap.
            ValueType v(queue[0]);
            fixHeap(0, v);
        } else if (queue.size() == 1) {
            // Only one entry. Clear out the queue
            index.clear();
            queue.clear();
        }
    }
    
    /** Obtain constant iterator to iterate over the backing container
	for the priroity queue.

	\note Iteration will not be in priority order
	 
	\return An iterator to iterate over the elements in the
	priority queue.
    */
    typename QueueType::const_iterator begin() const {
	return queue.begin();
    }

    /** The ending iterator to iterate over the backing container for
	the priority queue.

	\return The logical end iterator for this priority queue.
    */
    typename QueueType::const_iterator end() const {
	return queue.end();
    }

    /** Obtain the number of entries in this priority queue.

        \return The number of elements in this priority queue.
     */
    inline size_t size() const {
        return queue.size();
    }
    
protected:

    /** Helper method to fix-up the location of a value in the heap
        after its priority has been updated.

        This method can be used to update the position of a value in
        the heap after its priority has been updated.  This causes the
        objects to be re-positioned in the heap (if necessary). The
        logic consists of 2 loops -- First 

        \note The implementation for this method has been heavily
        borrowed from libstdc++'s code base to ensure that heap
        updates are consistent with std::make_heap API.
        Unfortunately, this does imply that there is a chance this
        method may be incompatible with future versions.

        \param[in] currPos The current position of the agent in the
        heap whose position is to be updated.  This value is the index
        position of the agent in the agentList vector.
        
        \return This method returns the new position of the agent in
        the agentList vector.
    */
    size_t fixHeap(size_t currPos, const ValueType& value) {
        // ASSERT(currPos < queue.size());
        // Size of half-the-heap that we need to fix-up
        const size_t len   = (queue.size() - 1) / 2;
        size_t secondChild = currPos;
        // This code was borrowed from libstdc++ implementation to ensure
        // that the fix-ups are consistent with std::make_heap API.
        while (secondChild < len) {
            secondChild = 2 * (secondChild + 1);
            if (compare(queue[secondChild], queue[secondChild - 1])) {
                secondChild--;
            }
            // Move value to correct position
            queue[currPos] = std::move(queue[secondChild]);
	    // Update index for the value with correct position
            index[getKey(queue[currPos])] = currPos;
	    // Update position of element in the next iteration.
            currPos = secondChild;
        }
        if (((queue.size() & 1) == 0) &&
	    (secondChild == (queue.size() - 2) / 2)) {
            secondChild = 2 * (secondChild + 1);
            queue[currPos] = std::move(queue[secondChild - 1]);
	    index[getKey(queue[currPos])] = currPos;
            currPos = secondChild - 1;
        }
        // Use libstdc++'s internal method to fix-up the vector from the
        // given location.
        // std::__push_heap(queue.begin(), currPos, 0, value,
        //                 __gnu_cxx::__ops::__iter_comp_val(compare));
        size_t parent = (currPos - 1) / 2;
        while ((currPos > 0) && (compare(queue[parent], value))) {
            queue[currPos] = std::move(queue[parent]);
	    index[getKey(queue[currPos])] = currPos;
            currPos = parent;
            parent = (currPos - 1) / 2;
        }
        queue[currPos] = value;
	index[getKey(queue[currPos])] = currPos;
        // Return the final index position for the agent
        return currPos;
    }
    
private:
    /** The backing vector used to store entries in the form of a
        heap.  The entries are stored consistent with the storage
        format used by standard libraries -- the parent of each
        element is stored at i / 2 (where i is an index in this
        vector).
    */
    QueueType queue;

    /** The hash map used to enable O(1) look-up of values in the
        queue.

        The values stored in this hash map give the index into the
        queue.  The index is used to return the value given the key.
    */
    IndexType index;

    /** Functor to get the key for a given value. */
    GetKey getKey;

    /** The comparator to determine priority of elements */
    Compare compare;
};

#endif
