#include <iostream>
#include <conio.h>
#include <random>
#include <deque>
#include "../../Core/src/multithreading/WorkerThread.h"
#include "../../Core/src/multithreading/ThreadPool.h"
#include <condition_variable>

#include <Windows.h>

using namespace std;

/*
class cMalloc {
public:
	static bool display;
	static void* alloc(size_t size) {
		void* ptr = malloc(size);
		if (display)
			cout << "malloc: " << ptr << ", " << size << endl;
		return ptr;
	}
	static void dealloc(void* ptr) {
		if (display)
			cout << "free: " << ptr << endl;
		free(ptr);
	}
};
bool cMalloc::display = false;
void* operator new(size_t size){
	return cMalloc::alloc(size);
}
void operator delete(void* ptr){
	cMalloc::dealloc(ptr);
}
*/

////////////////////////////////////////////////////////////////////////////////
// Main
int petiMain() {
	/*
	LARGE_INTEGER st, en, freq;
	unsigned long long cycle;
	QueryPerformanceFrequency(&freq);


	// cout the number of cores
	cout << "cores: " << cThreadPool::GetNumLogicalCores() << endl;
	cThreadPool pool;

	// launch some int() jobs
	int id = 0;
	std::mutex lk;
	auto fut = pool.EnqueueForeach(
	[&lk, &id](){
		lk.lock();
		id++;
		cout << id << endl;
		lk.unlock();
		return id; 
	});

	// wait for results
	fut.wait();
	auto res = fut.get();
	for (auto& v : res) {
		cout << v << " ";
	}
	cout << endl << endl;
	
	// launch some void() jobs
	id = 0;
	auto fut2 = pool.EnqueueForeach(
		[&lk, &id](){
		lk.lock();
		id++;
		cout << id << endl;
		lk.unlock();
	});
	fut2.wait();
	cout << endl << endl;
	fut2.get();

	// copy test
	struct CopyTest {
		CopyTest() { cout << "consturct\n"; }
		CopyTest(const CopyTest& other) { cout << "copy\n"; }
		CopyTest(CopyTest&& other) { cout << "move\n"; }
		~CopyTest() { cout << "destroy\n"; }
	} copyTest;

	// std::thread overhead
	QueryPerformanceCounter(&st);
	cycle = __rdtsc();
	thread t([](){return 42; });
	cycle = __rdtsc() - cycle;
	QueryPerformanceCounter(&en);
	cout << "Launching an std::thread took\n    "
		<< double(en.QuadPart - st.QuadPart) / double(freq.QuadPart) * 1e6 << " microsecs\n"
		<< "   " << cycle << "   cycles\n";

	t.join();

	// worker thread overhead
	cWorkerThread w;
	QueryPerformanceCounter(&st);

	cycle = __rdtsc();
	auto fut4 = w.Enqueue([](){return 42; });
	cycle = __rdtsc()-cycle;

	QueryPerformanceCounter(&en);
	fut4.wait();
	cout << "Launching a task on worker thread took\n   "
		<< double(en.QuadPart - st.QuadPart) / double(freq.QuadPart) * 1e6 << " microsecs\n"
		<< "   " << cycle << "   cycles:\n";

	// threadpool overhead
	/*
	const int nPooledTasks = 10000;
	cout << "Launching " << nPooledTasks << " async stuff... \n";
	auto start = chrono::high_resolution_clock::now();
	for (int i = 0; i < nPooledTasks; i++) {
		pool.EnqueueForeach([](){return 42; });
	}
	QueryPerformanceCounter(&st);
	auto fut3 = pool.EnqueueForeach([](){return 42; });
	QueryPerformanceCounter(&en);
	fut3.wait();
	auto timeMs = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count();
	cout << "aaaaaaand it took " << timeMs << " milliseconds.\n";
	cout << "Launching last task took "
		<< double(en.QuadPart - st.QuadPart) / double(freq.QuadPart) * 1e6 << " microsecs\n";
	*/

	spinlock queueLock;
	queue<int*> execQueue;
	std::mutex waitMutex1;
	spinlock waitMutex2;
	atomic<bool> notified;
	atomic<bool> locked;

	// worker thread function
	thread 	worker(
	[&]() {
		bool isContinue = true;
		do {
			// try to lock mutex
			if (!notified) {
				while (!locked){}
				locked = false;
				waitMutex1.lock();
				waitMutex1.unlock();
			}
			bool b = notified.exchange(false);
			if (b) {
				notified = b;
			}

			while (queueLock.lock(), execQueue.size() > 0) {
				auto data = execQueue.front();
				execQueue.pop();
				queueLock.unlock();

				if (data != nullptr) {
					//cout << *data;
					delete data;
				}
				else {
					isContinue = false;
				}
			}
			queueLock.unlock();

		} while (isContinue);
	});
	
	// some var
	int n = 0;

	// enqueue rutin
	waitMutex1.lock();
	locked = true;
	auto Enqueue = [&](int* p) {
		queueLock.lock();
		execQueue.push(p);
		queueLock.unlock();

		waitMutex1.unlock();
		notified = true;
		waitMutex2.lock();
		waitMutex2.unlock();
		waitMutex1.lock();
		locked = true;
	};

	// timing
	auto start = __rdtsc();

	// enqueue
	for (int i = 0; i < 10000; i++) {
		Enqueue(new int(++n));
	}
	Enqueue(nullptr);

	// timing
	auto time = __rdtsc() - start;
	cout << double(time)/3.3e9*1e6;
	
	// join thread
	worker.join();

	waitMutex1.unlock();

	_getch();
	return 0;
}