#include "Vector/Vector.h"
#include <iostream>
#include <utility>

int passedCount = 0;
int failedCount = 0;
int testsCount = 0;

#define TEST(name)   (std::cout << "TESTING: " << name << std::endl, testsCount++)
#define PASSED(name) (std::cout << "PASSED: " << name << "\n\n", passedCount++)

void FAILED(const std::string& name)
{
	std::cout << "FAILED: " << name << "\n\n", failedCount++;
}

// ============================================================
// Helpers
// ============================================================

void Print(Vector<int>& vec)
{
	for (size_t i = 0; i < vec.size(); i++)
		std::cout << vec[i] << "\n";

	std::cout << "Capacity: " << vec.capacity() << "\n";
	std::cout << "Elements: " << vec.size() << "\n";
}


// Used to test object lifetime/destruction.
struct LifetimeTracker
{
	static int alive;

	int value;

	LifetimeTracker(int value = 0)
		: value(value)
	{
		alive++;
	}

	LifetimeTracker(const LifetimeTracker& other)
		: value(other.value)
	{
		alive++;
	}

	LifetimeTracker(LifetimeTracker&& other) noexcept
		: value(other.value)
	{
		alive++;
	}

	LifetimeTracker& operator=(const LifetimeTracker& other)
	{
		value = other.value;
		return *this;
	}

	LifetimeTracker& operator=(LifetimeTracker&& other) noexcept
	{
		value = other.value;
		return *this;
	}

	~LifetimeTracker()
	{
		alive--;
	}
};

int LifetimeTracker::alive = 0;


// Used to test resize() construction.
struct ResizeObject
{
	int value;

	ResizeObject()
		: value(1234)
	{
	}
};


// ============================================================
// PUSH_BACK
// ============================================================

void Test_PushBack()
{
	TEST("PUSH_BACK");

	Vector<int> vector;

	for (size_t i = 0; i < 5000; i++)
		vector.push_back(i);

	if (vector.size() != 5000)
	{
		FAILED("PUSH_BACK");
		return;
	}

	for (size_t i = 0; i < vector.size(); i++)
	{
		if (vector[i] != i)
		{
			FAILED("PUSH_BACK");
			Print(vector);
			return;
		}
	}

	PASSED("PUSH_BACK");
}


// ============================================================
// POP_BACK
// ============================================================

void Test_PopBack()
{
	TEST("POP_BACK");

	Vector<int> vector;

	for (size_t i = 0; i < 5000; i++)
		vector.push_back(i);

	for (size_t i = 0; i < 100; i++)
		vector.pop_back();

	if (vector.size() != 4900)
	{
		FAILED("POP_BACK");
		return;
	}

	if (*vector.back() != 4899)
	{
		FAILED("POP_BACK");
		return;
	}

	PASSED("POP_BACK");
}


// ============================================================
// SWAP
// ============================================================

void Test_Swap()
{
	TEST("SWAP");

	Vector<int> vector;
	Vector<int> vector2;

	for (size_t i = 0; i < 5000; i++)
		vector.push_back(i);

	for (size_t i = 0; i < 500; i++)
		vector2.push_back(i);

	size_t size1 = vector.size();
	size_t capacity1 = vector.capacity();

	size_t size2 = vector2.size();
	size_t capacity2 = vector2.capacity();

	vector.swap(vector2);

	if (*vector.front() != 0 ||
		*vector.back() != 499 ||
		vector.size() != size2 ||
		vector.capacity() != capacity2)
	{
		FAILED("SWAP");
		return;
	}

	if (*vector2.front() != 0 ||
		*vector2.back() != 4999 ||
		vector2.size() != size1 ||
		vector2.capacity() != capacity1)
	{
		FAILED("SWAP");
		return;
	}

	PASSED("SWAP");
}


// ============================================================
// SWAP_POP
// ============================================================

void Test_SwapPop()
{
	TEST("SWAP_POP");

	Vector<int> vector;

	for (size_t i = 0; i < 5000; i++)
		vector.push_back(i);

	int originalBack = *vector.back();

	for (size_t i = 0; i < 50; i++)
		vector.swap_pop(i);

	if (vector.size() != 4950)
	{
		FAILED("SWAP_POP");
		return;
	}

	if (*vector.front() != originalBack)
	{
		FAILED("SWAP_POP");
		return;
	}

	PASSED("SWAP_POP");
}


// ============================================================
// RESIZE
// ============================================================

void Test_Resize()
{
	TEST("RESIZE");

	Vector<int> vector;

	vector.resize(10);

	if (vector.size() != 10)
	{
		FAILED("RESIZE");
		Print(vector);
		return;
	}

	for (size_t i = 0; i < vector.size(); i++)
	{
		if (vector[i] != 0)
		{
			FAILED("RESIZE");
			Print(vector);
			return;
		}
	}

	for (size_t i = 0; i < 50; i++)
		vector.push_back(i);

	if (vector.size() != 60)
	{
		FAILED("RESIZE");
		Print(vector);
		return;
	}

	vector.resize(1000);

	if (vector.size() != 1000)
	{
		FAILED("RESIZE");
		Print(vector);
		return;
	}

	for (size_t i = 0; i < 10; i++)
	{
		if (vector[i] != 0)
		{
			FAILED("RESIZE");
			Print(vector);
			return;
		}
	}

	for (size_t i = 10; i < 60; i++)
	{
		if (vector[i] != i - 10)
		{
			FAILED("RESIZE");
			Print(vector);
			return;
		}
	}

	for (size_t i = 60; i < 1000; i++)
	{
		if (vector[i] != 0)
		{
			FAILED("RESIZE");
			Print(vector);
			return;
		}
	}

	vector.resize(100);

	if (vector.size() != 100)
	{
		FAILED("RESIZE");
		Print(vector);
		return;
	}

	PASSED("RESIZE");
}


// ============================================================
// RESERVE
// ============================================================

void Test_Reserve()
{
	TEST("RESERVE");

	Vector<int> vector;

	vector.reserve(1000);

	if (vector.size() != 0 || vector.capacity() != 1000)
	{
		FAILED("RESERVE");
		Print(vector);
		return;
	}

	for (size_t i = 0; i < 900; i++)
		vector.push_back(i);

	if (vector.size() != 900)
	{
		FAILED("RESERVE");
		Print(vector);
		return;
	}

	if (vector.capacity() != 1000)
	{
		FAILED("RESERVE");
		Print(vector);
		return;
	}

	if (*vector.back() != 899)
	{
		FAILED("RESERVE");
		Print(vector);
		return;
	}

	PASSED("RESERVE");
}


// ============================================================
// CLEAR
// ============================================================

void Test_Clear()
{
	TEST("CLEAR");

	Vector<int> vector;

	for (size_t i = 0; i < 5000; i++)
		vector.push_back(i);

	size_t originalCapacity = vector.capacity();

	vector.clear();

	if (vector.size() != 0)
	{
		FAILED("CLEAR");
		Print(vector);
		return;
	}

	if (vector.capacity() != originalCapacity)
	{
		FAILED("CLEAR");
		Print(vector);
		return;
	}

	for (size_t i = 0; i < 2500; i++)
		vector.push_back(i);

	if (vector.size() != 2500)
	{
		FAILED("CLEAR");
		Print(vector);
		return;
	}

	if (vector.capacity() != originalCapacity)
	{
		FAILED("CLEAR");
		Print(vector);
		return;
	}

	if (*vector.back() != 2499)
	{
		FAILED("CLEAR");
		Print(vector);
		return;
	}

	PASSED("CLEAR");
}


// ============================================================
// RESERVE MUST KEEP DATA
// ============================================================

void Test_ReserveKeepsData()
{
	TEST("RESERVE_KEEPS_DATA");

	Vector<int> vector;

	for (int i = 0; i < 100; i++)
		vector.push_back(i);

	size_t oldSize = vector.size();

	vector.reserve(1000);

	if (vector.size() != oldSize)
	{
		FAILED("RESERVE_KEEPS_DATA");
		return;
	}

	for (int i = 0; i < 100; i++)
	{
		if (vector[i] != i)
		{
			FAILED("RESERVE_KEEPS_DATA");
			Print(vector);
			return;
		}
	}

	if (vector.capacity() < 1000)
	{
		FAILED("RESERVE_KEEPS_DATA");
		return;
	}

	PASSED("RESERVE_KEEPS_DATA");
}


// ============================================================
// COPY CONSTRUCTOR
// ============================================================

void Test_CopyConstructor()
{
	TEST("COPY_CONSTRUCTOR");

	Vector<int> original;

	for (int i = 0; i < 100; i++)
		original.push_back(i);

	Vector<int> copy(original);

	// Same size
	if (copy.size() != original.size())
	{
		FAILED("COPY_CONSTRUCTOR");
		return;
	}

	// Same capacity
	if (copy.capacity() != original.capacity())
	{
		FAILED("COPY_CONSTRUCTOR");
		return;
	}

	// Same data
	for (size_t i = 0; i < original.size(); i++)
	{
		if (copy[i] != original[i])
		{
			FAILED("COPY_CONSTRUCTOR");
			return;
		}
	}

	// They must have separate allocations.
	if (copy.data() == original.data())
	{
		FAILED("COPY_CONSTRUCTOR");
		return;
	}

	// Modifying one must not affect the other.
	copy[0] = 9999;

	if (original[0] != 0)
	{
		FAILED("COPY_CONSTRUCTOR");
		return;
	}

	PASSED("COPY_CONSTRUCTOR");
}


// ============================================================
// MOVE CONSTRUCTOR
// ============================================================

void Test_MoveConstructor()
{
	TEST("MOVE_CONSTRUCTOR");

	Vector<int> original;

	for (int i = 0; i < 100; i++)
		original.push_back(i);

	size_t originalSize = original.size();
	size_t originalCapacity = original.capacity();

	int* originalData = original.data();

	Vector<int> moved(std::move(original));

	// Destination received the original data.
	if (moved.size() != originalSize)
	{
		FAILED("MOVE_CONSTRUCTOR");
		return;
	}

	if (moved.capacity() != originalCapacity)
	{
		FAILED("MOVE_CONSTRUCTOR");
		return;
	}

	if (moved.data() != originalData)
	{
		FAILED("MOVE_CONSTRUCTOR");
		return;
	}

	// Verify contents.
	for (int i = 0; i < 100; i++)
	{
		if (moved[i] != i)
		{
			FAILED("MOVE_CONSTRUCTOR");
			return;
		}
	}

	// Moved-from object should be empty and valid.
	if (original.data() != nullptr ||
		original.size() != 0 ||
		original.capacity() != 0)
	{
		FAILED("MOVE_CONSTRUCTOR");
		return;
	}

	// Moved-from object should still be usable.
	original.push_back(12345);

	if (original.size() != 1 ||
		original[0] != 12345)
	{
		FAILED("MOVE_CONSTRUCTOR");
		return;
	}

	PASSED("MOVE_CONSTRUCTOR");
}


// ============================================================
// COPY ASSIGNMENT
// ============================================================

void Test_CopyAssignment()
{
	TEST("COPY_ASSIGNMENT");

	Vector<int> original;

	for (int i = 0; i < 100; i++)
		original.push_back(i);

	Vector<int> copy;

	for (int i = 0; i < 20; i++)
		copy.push_back(999);

	copy = original;

	// Same size.
	if (copy.size() != original.size())
	{
		FAILED("COPY_ASSIGNMENT");
		return;
	}

	// Same capacity.
	if (copy.capacity() != original.capacity())
	{
		FAILED("COPY_ASSIGNMENT");
		return;
	}

	// Same data.
	for (size_t i = 0; i < original.size(); i++)
	{
		if (copy[i] != original[i])
		{
			FAILED("COPY_ASSIGNMENT");
			return;
		}
	}

	// Must not share memory.
	if (copy.data() == original.data())
	{
		FAILED("COPY_ASSIGNMENT");
		return;
	}

	// Modifying copy must not modify original.
	copy[0] = 9999;

	if (original[0] != 0)
	{
		FAILED("COPY_ASSIGNMENT");
		return; 
	}

	// Test self-assignment.

	copy[0] = 0;
	copy = copy;

	if (copy.size() != 100)
	{
		FAILED("COPY_ASSIGNMENT");
		return;
	}

	for (int i = 0; i < 100; i++)
	{
		if (copy[i] != i)
		{
			FAILED("COPY_ASSIGNMENT");
			return;
		}
	}

	PASSED("COPY_ASSIGNMENT");
}


// ============================================================
// MOVE ASSIGNMENT
// ============================================================

void Test_MoveAssignment()
{
	TEST("MOVE_ASSIGNMENT");

	Vector<int> original;

	for (int i = 0; i < 100; i++)
		original.push_back(i);

	int* originalData = original.data();
	size_t originalSize = original.size();
	size_t originalCapacity = original.capacity();

	// Give destination existing data first.
	Vector<int> destination;

	for (int i = 0; i < 50; i++)
		destination.push_back(999);

	destination = std::move(original);

	// Destination should now own original's allocation.
	if (destination.data() != originalData)
	{
		FAILED("MOVE_ASSIGNMENT");
		return;
	}

	if (destination.size() != originalSize)
	{
		FAILED("MOVE_ASSIGNMENT");
		return;
	}

	if (destination.capacity() != originalCapacity)
	{
		FAILED("MOVE_ASSIGNMENT");
		return;
	}

	// Verify data.
	for (int i = 0; i < 100; i++)
	{
		if (destination[i] != i)
		{
			FAILED("MOVE_ASSIGNMENT");
			return;
		}
	}

	// Source should be empty and valid.
	if (original.data() != nullptr ||
		original.size() != 0 ||
		original.capacity() != 0)
	{
		FAILED("MOVE_ASSIGNMENT");
		return;
	}

	// Source should still be reusable.
	original.push_back(12345);

	if (original.size() != 1 ||
		original[0] != 12345)
	{
		FAILED("MOVE_ASSIGNMENT");
		return;
	}

	PASSED("MOVE_ASSIGNMENT");
}


// ============================================================
// ERASE MUST DESTROY THE REMOVED OBJECT
// ============================================================

void Test_EraseDestroysElement()
{
	TEST("ERASE_DESTROYS_ELEMENT");

	LifetimeTracker::alive = 0;

	{
		Vector<LifetimeTracker> vector;

		for (int i = 0; i < 10; i++)
			vector.push_back(LifetimeTracker(i));

		int aliveBefore = LifetimeTracker::alive;

		vector.erase(5);

		// One object should have been removed.
		if (LifetimeTracker::alive != aliveBefore - 1)
		{
			FAILED("ERASE_DESTROYS_ELEMENT");
			return;
		}

		if (vector.size() != 9)
		{
			FAILED("ERASE_DESTROYS_ELEMENT");
			return;
		}
	}

	// Everything should have been destroyed.
	if (LifetimeTracker::alive != 0)
	{
		FAILED("ERASE_DESTROYS_ELEMENT");
		return;
	}

	PASSED("ERASE_DESTROYS_ELEMENT");
}


// ============================================================
// RESIZE MUST CONSTRUCT NEW OBJECTS
// ============================================================

void Test_ResizeConstructsElements()
{
	TEST("RESIZE_CONSTRUCTS_ELEMENTS");

	Vector<ResizeObject> vector;

	if (vector.size() != 0)
	{
		FAILED("RESIZE_CONSTRUCTS_ELEMENTS");
		return;
	}

	vector.resize(100);

	if (vector.size() != 100)
	{
		FAILED("RESIZE_CONSTRUCTS_ELEMENTS");
		return;
	}

	for (size_t i = 0; i < 100; i++)
	{
		if (vector[i].value != 1234)
		{
			FAILED("RESIZE_CONSTRUCTS_ELEMENTS");
			return;
		}
	}

	PASSED("RESIZE_CONSTRUCTS_ELEMENTS");
}


// ============================================================
// MAIN
// ============================================================

int main()
{
	Test_PushBack();
	Test_PopBack();
	Test_Swap();
	Test_SwapPop();
	Test_Resize();
	Test_Reserve();
	Test_Clear();

	Test_ReserveKeepsData();

	Test_CopyConstructor();
	Test_MoveConstructor();
	Test_CopyAssignment();
	Test_MoveAssignment();

	Test_EraseDestroysElement();
	Test_ResizeConstructsElements();

	std::cout << "============================\n";
	std::cout << "Tests:  " << testsCount << "\n";
	std::cout << "Passed: " << passedCount << "\n";
	std::cout << "Failed: " << failedCount << "\n";
	std::cout << "============================\n";

	return failedCount == 0 ? 0 : 1;
}