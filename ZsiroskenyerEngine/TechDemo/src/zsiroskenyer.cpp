////////////////////////////////////////////////////////////////////////////////
//	TechDemo/src/zsiroskenyer.cpp
//	Ezt a f�jlt isten teremtette diszn�v�g�s ut�n.
////////////////////////////////////////////////////////////////////////////////
//	Ebben �gyis szem�t lesz sz�val gecimindegy mit �rok ide.
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