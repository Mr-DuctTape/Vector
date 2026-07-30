#include "Vector/Vector.h"
#include <iostream>
#include <utility>

int passedCount = 0;
int failedCount = 0;
int testsCount = 0;

#define TEST(name)   (std::cout << "TESTING: " << name << std::endl, testsCount++)
#define PASSED(name) (std::cout << "PASSED: " << name << "\n\n", passedCount++)
#define FAILED(name) (std::cout << "FAILED: " << name << "\n\n", failedCount++)


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


// Used to test that resize() actually constructs objects.
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

	// vector should now contain vector2's old data.
	if (*vector.front() != 0 ||
		*vector.back() != 499 ||
		vector.size() != size2 ||
		vector.capacity() != capacity2)
	{
		FAILED("SWAP");
		return;
	}

	// vector2 should now contain vector's old data.
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

	// Resize from 0 -> 10
	vector.resize(10);

	if (vector.size() != 10)
	{
		FAILED("RESIZE");
		Print(vector);
		return;
	}

	// New int elements should be value-initialized to 0
	for (size_t i = 0; i < vector.size(); i++)
	{
		if (vector[i] != 0)
		{
			FAILED("RESIZE");
			Print(vector);
			return;
		}
	}

	// Add some values
	for (size_t i = 0; i < 50; i++)
		vector.push_back(i);

	if (vector.size() != 60)
	{
		FAILED("RESIZE");
		Print(vector);
		return;
	}

	// Resize larger
	vector.resize(1000);

	if (vector.size() != 1000)
	{
		FAILED("RESIZE");
		Print(vector);
		return;
	}

	// Existing elements should remain intact
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

	// Newly created elements should be 0
	for (size_t i = 60; i < 1000; i++)
	{
		if (vector[i] != 0)
		{
			FAILED("RESIZE");
			Print(vector);
			return;
		}
	}

	// Resize smaller
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

	// Fill less than capacity
	for (size_t i = 0; i < 900; i++)
		vector.push_back(i);

	if (vector.size() != 900)
	{
		FAILED("RESERVE");
		Print(vector);
		return;
	}

	// Capacity should not change because we haven't exceeded it
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

	// clear() should destroy elements but keep allocated storage
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

	// Make sure the vector can still be reused
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
// RESERVE MUST KEEP EXISTING DATA
// ============================================================

void Test_ReserveKeepsData()
{
	TEST("RESERVE_KEEPS_DATA");

	Vector<int> vector;

	for (int i = 0; i < 100; i++)
		vector.push_back(i);

	size_t oldSize = vector.size();

	vector.reserve(1000);

	// reserve() should not change size.
	if (vector.size() != oldSize)
	{
		FAILED("RESERVE_KEEPS_DATA");
		return;
	}

	// Existing elements must still be there.
	for (int i = 0; i < 100; i++)
	{
		if (vector[i] != i)
		{
			FAILED("RESERVE_KEEPS_DATA");
			Print(vector);
			return;
		}
	}

	// Capacity should have increased.
	if (vector.capacity() < 1000)
	{
		FAILED("RESERVE_KEEPS_DATA");
		return;
	}

	PASSED("RESERVE_KEEPS_DATA");
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

	Vector<int> moved(std::move(original));

	// Destination should have received everything.
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

	// Verify data.
	for (int i = 0; i < 100; i++)
	{
		if (moved[i] != i)
		{
			FAILED("MOVE_CONSTRUCTOR");
			return;
		}
	}

	// For our Vector, moved-from state should be empty.
	if (original.data() != nullptr)
	{
		FAILED("MOVE_CONSTRUCTOR");
		return;
	}

	if (original.size() != 0)
	{
		FAILED("MOVE_CONSTRUCTOR");
		return;
	}

	if (original.capacity() != 0)
	{
		FAILED("MOVE_CONSTRUCTOR");
		return;
	}

	// The moved-from vector must still be usable.
	original.push_back(12345);

	if (original.size() != 1)
	{
		FAILED("MOVE_CONSTRUCTOR");
		return;
	}

	if (original[0] != 12345)
	{
		FAILED("MOVE_CONSTRUCTOR");
		return;
	}

	PASSED("MOVE_CONSTRUCTOR");
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

		// Exactly one object should have been removed.
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

	// This must actually construct 100 ResizeObjects.
	vector.resize(100);

	if (vector.size() != 100)
	{
		FAILED("RESIZE_CONSTRUCTS_ELEMENTS");
		return;
	}

	// Every object should have been default constructed.
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
	Test_MoveConstructor();
	Test_EraseDestroysElement();
	Test_ResizeConstructsElements();


	for (size_t i = 0; i < 10000; i++)
	{
		Test_PushBack();
		Test_PopBack();
		Test_Swap();
		Test_SwapPop();
		Test_Resize();
		Test_Reserve();
		Test_Clear();

		Test_ReserveKeepsData();
		Test_MoveConstructor();
		Test_EraseDestroysElement();
		Test_ResizeConstructsElements();
	}

	std::cout << "============================\n";
	std::cout << "Tests:  " << testsCount << "\n";
	std::cout << "Passed: " << passedCount << "\n";
	std::cout << "Failed: " << failedCount << "\n";
	std::cout << "============================\n";

	return failedCount == 0 ? 0 : 1;
}