#include <iostream>
#include <conio.h>
#include "../../Core/src/math/math_all.h"
#include "../../Core/src/memory/tlsf.h"
#include <random>
#include <deque>
#include "../../Core/src/zsString.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Test TLSF
#define ALLOC_COUNT 100000
#define PATTERN_SIZE 100000

void TestTLSF() {
	unsigned char pattern[PATTERN_SIZE];
	mt19937 randomEngine;
	uniform_int_distribution<unsigned> randomGen;
	uniform_int_distribution<size_t> randomSize(16, 1 * 1048576);
	uniform_int_distribution<size_t> randomIndex;
	randomEngine.seed(5489);
	TLSF pool;

	// generate pattern
	for (int i = 0; i < PATTERN_SIZE; i++) {
		pattern[i] = (unsigned char)randomGen(randomEngine);
	}

	// database for remebering allocations
	struct tAlloced {
		unsigned char* ptr;
		size_t size;
	};
	deque<tAlloced> allocs;

	// allocate some stuff at start
	for (size_t i = 0; i < 100; i++) {
		// generate random allocation size
		size_t size = randomSize(randomEngine);
		// allocate and check result
		unsigned char* p = (unsigned char*)pool.allocate(size);
		if (p == NULL) {
			cout << "failed to alloc: " << i << endl;
			break;
		}
		// copy pattern
		for (size_t j = 0; j < size; j++) {
			p[j] = pattern[j%PATTERN_SIZE];
		}
		// add stuff to database
		tAlloced allocation = {p, size};
		allocs.push_back(allocation);
	}
	// allocate over and over again to shuffle things
	for (size_t i = 0; i < ALLOC_COUNT; i++) {
		// print status message
		if (i % (ALLOC_COUNT / 100) == 99) {
			cout << i / (ALLOC_COUNT / 100) + 1 << "%" << endl;
		}

		// generate a random index
		size_t index = randomIndex(randomEngine) % allocs.size();
		auto it = allocs.begin() + index;
		// check pattern
		for (size_t j = 0; j < it->size; j++) {
			if (pattern[j%PATTERN_SIZE] != it->ptr[j]) {
				cout << "patterns don't match: " << j << " " << it->ptr;
				break;
			}
		}
		// free memory
		pool.deallocate(it->ptr);
		allocs.erase(it);

		// generate random allocation size
		size_t size = randomSize(randomEngine);
		// allocate and check result
		unsigned char* p = (unsigned char*)pool.allocate(size);
		if (p == NULL) {
			cout << "failed to alloc: " << i << endl;
			break;
		}
		// copy pattern
		for (size_t j = 0; j < size; j++) {
			p[j] = pattern[j%PATTERN_SIZE];
		}
		// add stuff to database
		tAlloced allocation = {p, size};
		allocs.push_back(allocation);
	}

}



////////////////////////////////////////////////////////////////////////////////
// Test serializer
#define NUM_STRINGS_WRITTEN 500
#define NUM_INTEGERS_WRITTEN 500
#define NUM_STRINGS 10000
#define NUM_INTEGERS 10000
void TestSerializer() {
	/*
	cSerializable s;
	mt19937 randomEngine;
	uniform_int_distribution<unsigned> randomNumber(1, 1000000);



	// generate 'some' random strings & integers
	vector<zsString> strings;
	for (int i = 0; i < NUM_STRINGS; i++) {
		char str[256] = {'\0'};
		auto n = randomNumber(randomEngine);
		itoa(n, str, 10);
		strings.push_back(str);
	}
	vector<size_t> integers;
	for (int i = 0; i < NUM_INTEGERS; i++) {
		integers.push_back(randomNumber(randomEngine));
	}
	cout << "strings & integers generated!" << endl;



	// add like 500 random strings
	for (int i = 0; i < NUM_STRINGS_WRITTEN; i++) {
		s << strings[i];
	}
	cout << "strings added!" << endl;
	// add like 500 random numbers
	for (int i = 0; i < NUM_INTEGERS_WRITTEN; i++) {
		s << integers[i];
	}
	// add like 500 random strings
	for (int i = 0; i < NUM_STRINGS_WRITTEN; i++) {
		s << strings[i + NUM_STRINGS_WRITTEN];
	}
	cout << "strings added!" << endl;
	// add like 500 random numbers
	for (int i = 0; i < NUM_INTEGERS_WRITTEN; i++) {
		s << integers[i + NUM_INTEGERS_WRITTEN];
	}



	// write them to file
	s.WriteToFile("TEST_FILE.dat");
	cout << "written to file" << endl;

	// nahh, we don't want this, we want a new one
	s.~cSerializable();
	new (&s) cSerializable();
	cout << "new instance created" << endl;
	
	// read it back
	s.ReadFromFile("TEST_FILE.dat");
	cout << "data read back from file" << endl;



	// read back all added strings
	bool isCorrect = true;

	for (int i = 0; i < NUM_STRINGS_WRITTEN; i++) {
		zsString str;
		s >> str;
		if (str != strings[i]) {
			cout << "(s) NOT EQUAL\n";
			isCorrect = false;
		}
	}
	// read back all added numbers
	for (int i = 0; i < NUM_INTEGERS_WRITTEN; i++) {
		size_t val;
		s >> val;
		if (val != integers[i]) {
			cout << "(i) NOT EQUAL\n";
			isCorrect = false;
		}
	}
	for (int i = 0; i < NUM_STRINGS_WRITTEN; i++) {
		zsString str;
		s >> str;
		if (str != strings[i + NUM_STRINGS_WRITTEN]) {
			cout << "(s) NOT EQUAL\n";
			isCorrect = false;
		}
	}
	// read back all added numbers
	for (int i = 0; i < NUM_INTEGERS_WRITTEN; i++) {
		size_t val;
		s >> val;
		if (val != integers[i + NUM_INTEGERS_WRITTEN]) {
			cout << "(i) NOT EQUAL\n";
			isCorrect = false;
		}
	}



	// display feedback
	if (isCorrect)
		cout << "all string & numbers succesfully read back!" << endl;
	else
		cout << "all string & numbers read back, but there were errors" << endl;
	*/
}


////////////////////////////////////////////////////////////////////////////////
// Test strings
void TestString(const zsString str) {
	auto c = str.c_str();
	wcout << c;
}


////////////////////////////////////////////////////////////////////////////////
// Main
int petiMain() {	
	//TestSerializer();
	//TestTLSF();

	TestString(L"Just go fuck yourself!");

	_getch();
	return 0;
}