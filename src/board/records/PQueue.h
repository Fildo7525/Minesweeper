#pragma once

#include <algorithm>
#include <vector>
#include <functional>

template <typename T>
class PQueue : public std::vector<T>
{
public:
	PQueue() : m_sorter(std::greater<T>()) {}
	PQueue(const PQueue &other) = default;
	PQueue(PQueue &&other) = default;
	PQueue &operator=(const PQueue &other) = default;
	PQueue &operator=(PQueue &&other) = default;

	~PQueue() = default;

	PQueue &setSorter(std::function<bool(const T &, const T &)> sorter)
	{
		m_sorter = sorter;
		return *this;
	}

	void sort()
	{
		std::sort(this->begin(), this->end(), m_sorter);
	}

	void push(const T &value)
	{
		this->push_back(value);
		sort();
	}

	void pop()
	{
		this->pop_back();
	}

	T &top()
	{
		return this->front();
	}

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
