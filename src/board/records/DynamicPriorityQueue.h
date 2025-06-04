#pragma once

#include <algorithm>
#include <execution>
#include <vector>
#include <functional>

/**
 * @brief Priority queue with dynamic sorting.
 *
 * The DynamicPriorityQueue class is a priority queue based on std::vector<T> that sorts the elements
 * in the container using the given sorter. The main difference from std::priority_queue<T>
 * is that this class's sorter can be changed at runtime. The default sorter is std::greater<T>
 * which sorts the elements in descending order.
 *
 * @tparam T Type of the elements in the container.
 */
template <typename T>
class DynamicPriorityQueue : public std::vector<T>
{
public:
	DynamicPriorityQueue() : m_sorter(std::greater<T>()) {}
	DynamicPriorityQueue(const DynamicPriorityQueue &other) = default;
	DynamicPriorityQueue(DynamicPriorityQueue &&other) = default;
	DynamicPriorityQueue &operator=(const DynamicPriorityQueue &other) = default;
	DynamicPriorityQueue &operator=(DynamicPriorityQueue &&other) = default;

	~DynamicPriorityQueue() = default;

	/**
	 * @brief Set the dynamic sorter for the priority queue.
	 *
	 * @param sorter Sorter function that takes two elements of type T and returns true if the first.
	 * @param invoke Flag to invoke the sorter immediately after setting it.
	 * @return PQueue& Reference to the priority queue.
	 */
	DynamicPriorityQueue &setSorter(std::function<bool(const T &, const T &)> sorter, bool invoke = true)
	{
		m_sorter = sorter;

		if (invoke)
			sort();

		return *this;
	}

	/**
	 * @brief Sort the elements in the container.
	 */
	void sort()
	{
		std::sort(std::execution::par_unseq, this->begin(), this->end(), m_sorter);
	}

	/**
	 * @brief Push an element to the priority queue.
	 *
	 * @param value Element to push to the priority queue.
	 */
	void push(const T &value)
	{
		this->push_back(value);
		sort();
	}

	/**
	 * @brief Pop the top element from the priority queue.
	 */
	void pop()
	{
		this->pop_back();
	}

	/**
	 * @brief Get the top element from the priority queue.
	 *
	 * @return T& Reference to the top element.
	 */
	T &top()
	{
		return this->front();
	}

	/**
	 * @brief Get the top element from the priority queue.
	 *
	 * @return const T& Reference to the top element.
	 */
	const T &top() const
	{
		return this->front();
	}

private:
	void push_back(const T &__x)
	{
		this->std::vector<T>::push_back(__x);
	}

private:
	std::function<bool(const T &, const T &)> m_sorter;
};
