#pragma once
#include <stdexcept>

template<typename T>
class Vector
{
private:
	T* m_data = nullptr;
	size_t m_size = 0;
	size_t m_capacity = 0;

	static void Destroy(T* block, size_t size) // Call destructor manually
	{
		for (size_t i = 0; i < size; i++) {
			std::destroy_at(&block[i]);
		}
	}

	static void DefaultConstruct(T* block, size_t start, size_t end) // Construcs objects from start -> end inside memory block
	{
		for (size_t i = start; i < end; i++)
			std::construct_at(&block[i]);
	}

	static void Deallocate(T* block, size_t size) // Calls destructors and frees memory
	{
		Destroy(block, size);
		free(block);
	}

	static void RawAllocate(T** block, size_t size) // Allocates bytes no objects constructed
	{
		*block = (T*)malloc(sizeof(T) * size);
	}

	static void DefaultAllocate(T** block, size_t size, size_t capacity) // Allocates raw bytes to capacity, then defaults constructs up to size
	{
		RawAllocate(block, capacity);
		DefaultConstruct(*block, 0, size);
	}

	void Reallocate(T** block, size_t size, size_t capacity) // Allocates new memory block, keeping old data. If nullptr is passed default construction till size
	{
		if (*block == nullptr)
		{
			DefaultAllocate(block, size, capacity);
			return;
		}

		T* oldData = *block;

		RawAllocate(block, capacity);

		size_t max = (m_size > size) ? size : m_size;

		for (size_t i = 0; i < max; i++)
		{
			std::construct_at(&(*block)[i], oldData[i]);
		}

		size_t oldSize = m_size;
		Deallocate(oldData, oldSize);
	}

public:

	void push_back(const T& val)
	{
		if (m_size == 0 && !m_data)
		{
			m_size++;
			RawAllocate(&m_data, m_size);
			std::construct_at(&m_data[0], val);
			return;
		}

		m_size++;
		if (m_capacity <= m_size)
		{
			m_capacity = m_size * 2;
			Reallocate(&m_data, m_size, m_capacity);
		}

		std::construct_at(&m_data[back_index()], val);
	}

	void pop_back()
	{
		if (empty())
		{
			std::destroy_at(back());
			return;
		}
		std::destroy_at(back());
		m_size--;
	}

	void swap_pop(size_t index)
	{
		m_data[index] = m_data[back_index()];
		pop_back();
	}

	void erase(size_t index) // O(N) worst case
	{
		if (&m_data[index] == back())
		{
			pop_back();
			return;
		}
		for (size_t i = index; i < m_size; i++)
		{
			m_data[i] = m_data[i + 1];
		}
		pop_back();
	}

	void resize(size_t size)
	{
		m_capacity = size * 2;

		Reallocate(&m_data, size, m_capacity);

		if(size > m_size)
			DefaultConstruct(m_data, m_size, size);

		this->m_size = size;
		return;
	}

	T* data()
	{
		return m_data;
	}

	void clear()
	{
		Destroy(m_data, m_size);
		m_size = 0;
	}

	void reserve(size_t size)
	{
		if (size < m_capacity)
			return;

		m_capacity = size;
		if (m_data)
		{
			Reallocate(&m_data, m_size, m_capacity);
		}
		else
		{
			RawAllocate(&m_data, m_capacity);
		}
	}

	void swap(Vector<T>& other)
	{
		// Could maybe use std::swap here instead 

		T* tempData = this->m_data;
		this->m_data = other.m_data;
		other.m_data = tempData;

		size_t tempCap, tempSize;
		tempCap = this->m_capacity;
		tempSize = this->m_size;

		this->m_capacity = other.m_capacity;
		this->m_size = other.m_size;

		other.m_capacity = tempCap;
		other.m_size = tempSize;
	}

	size_t size() const
	{
		return m_size;
	}

	size_t capacity() const 
	{
		return m_capacity;
	}

	bool empty() const
	{
		return (m_size <= 0);
	}

	T* front() const
	{
		if (empty())
			return nullptr;

		return &m_data[0];
	}

	T* back() const
	{
		if (empty())
			return nullptr;

		return &m_data[back_index()];
	}

	size_t back_index() const
	{
		if (empty())
			return 0;

		return m_size - 1;
	}

	Vector() = default;

	Vector(Vector<T>&& other) noexcept // Move constructor
	{
		m_capacity = other.m_capacity;
		m_size = other.m_size;

		other.m_capacity = 0;
		other.m_size = 0;

		m_data = other.m_data;
		other.m_data = nullptr;
	}

	Vector(const Vector<T>& other) // Copy constructor
	{
		m_capacity = other.m_capacity;
		m_size = other.m_size;
		DefaultAllocate(&m_data, m_size, m_capacity);

		for (size_t i = 0; i < m_size; i++)
		{
			m_data[i] = other.m_data[i];
		}
	}

	Vector<T>& operator= (Vector&& other) noexcept // Move assigment operator
	{
		Deallocate(m_data, m_size);
		m_capacity = other.m_capacity;
		m_size = other.m_size;

		other.m_capacity = 0;
		other.m_size = 0;

		m_data = other.m_data;
		other.m_data = nullptr;

		return *this;
	}

	Vector<T>& operator= (const Vector<T>& other) // Copy assigment operator
	{
		if (this == &other)
			return *this;

		Deallocate(m_data, m_size);

		m_size = other.m_size;
		m_capacity = other.m_capacity;

		DefaultAllocate(&m_data, m_size, m_capacity);
		for (size_t i = 0; i < m_size; i++)
		{
			m_data[i] = other.m_data[i];
		}

		return *this;
	}

	~Vector()
	{
		Deallocate(m_data, m_size);
	}

	T& operator[] (size_t index)
	{
		if (index > m_size)
			throw std::runtime_error("Invalid memory location");
		return m_data[index];
	}
}; 