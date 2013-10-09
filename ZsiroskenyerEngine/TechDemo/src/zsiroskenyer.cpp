////////////////////////////////////////////////////////////////////////////////
//	TechDemo/src/zsiroskenyer.cpp
//	Ezt a fájlt isten teremtette disznóvágás után.
////////////////////////////////////////////////////////////////////////////////
//	Ebben úgyis szemét lesz szóval gecimindegy mit írok ide.
////////////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <thread>

using namespace std;


int main() {
	auto st = chrono::steady_clock::now();

	while (true) {
		this_thread::sleep_for(chrono::seconds(1));

		cout << "[" << chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now()-st).count() << "\t] Zsiros kenyer woohoooo!!!\n";

	}

	return 0;
}