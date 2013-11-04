#include <iostream>
#include <conio.h>
#include "../../Core/src/shared_ptr.h"

using namespace std;



class cZsirosKenyer {
public:
	cZsirosKenyer() : calories(150) { cout << "megkenve!\n"; }
	~cZsirosKenyer() { cout << "megpeneszedett! " << calories << " kcal" << endl; }
	void Release() { cout << "release: "; delete this; }
	int calories;
};

template <class T>
void del_array(T* ptr) {
	delete[] ptr;
}

int petiMain() {
	cout << "program started...\n";

	cout << "sizeof(zs_shared_ptr) = " << sizeof(zs_shared_ptr<int>) << endl;
	cout << "sizeof(std::function) = " << sizeof(std::function<void(int*)>) << endl;
	cout << endl;

	cZsirosKenyer* p1 = new cZsirosKenyer();
	cZsirosKenyer* p2 = new cZsirosKenyer();
	cZsirosKenyer* p3 = new cZsirosKenyer();

	{
		zs_shared_ptr<cZsirosKenyer> ptr1 = p1;			cout << ptr1.use_count() << endl;
		zs_shared_ptr<cZsirosKenyer> ptr2 = p1;			cout << ptr1.use_count() << endl;
		zs_shared_ptr<cZsirosKenyer> ptr3 = ptr2;		cout << ptr1.use_count() << endl;
		zs_shared_ptr<cZsirosKenyer> ptr4 = nullptr;	cout << ptr1.use_count() << endl;
		zs_shared_ptr<cZsirosKenyer> ptr5 = p2;			cout << ptr1.use_count() << endl;
		zs_shared_ptr<cZsirosKenyer> ptr6(p3, [](cZsirosKenyer* ptr){ ptr->Release(); });
	}
	cout << endl;

	cout << "array test:\n";
	cZsirosKenyer* katonasagiElelmezes = new cZsirosKenyer[10];
	for (int i = 0; i < 10; i++) {
		katonasagiElelmezes[i].calories = 150 + i;
	}
	{
		zs_shared_ptr<cZsirosKenyer> ptr1(katonasagiElelmezes, [](cZsirosKenyer* ptr){ delete[] ptr; });
	}


	cout << "program ended\n";
	_getch();
	return 0;
}