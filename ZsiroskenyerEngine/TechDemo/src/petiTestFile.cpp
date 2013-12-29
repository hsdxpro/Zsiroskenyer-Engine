#include <iostream>
#include <conio.h>
#include "../../Core/src/math/math_all.h"
#include "../../Core/src/memory/tlsf.h"
#include <random>
#include <deque>

using namespace std;

#define ALLOC_COUNT 100000
#define PATTERN_SIZE 100000

int petiMain() {
	unsigned char pattern[PATTERN_SIZE];
	mt19937 randomEngine;
	uniform_int_distribution<unsigned> randomGen;
	uniform_int_distribution<size_t> randomSize(16, 20 * 1048576);
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

	return 0;
}