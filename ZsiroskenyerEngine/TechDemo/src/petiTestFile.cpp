#include <iostream>
#include <conio.h>
#include <random>

#include <vector>
#include <unordered_map>
#include <map>
#include <deque>
#include <list>
#include <iomanip>

#include "../../Core/src/VertexFormat.h"
#include "../../Core/src/math/math_all.h"

#include <Windows.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Main
int petiMain() {
	Vec3 lightDirs[] = {
		Vec3(1, 0, 1),
		Vec3(1, 1, 1),
		Vec3(0, 1, 1),
		Vec3(0, 0, 1)
	};

	cout << fixed;
	cout << setprecision(3);

	Vec3 v(0, 0, 1);
	float angle = 90.f / 180.f * 3.1415926f;
	Vec3 axis(.883, 1.892, .784); axis.Normalize();
	Quat q(axis, angle);

	cout << "axis = " << axis << endl;
	cout << "angle = " << angle * 180.f / 3.1415926f << endl;
	cout << "vector " << v << " rotates to " << v*Matrix44(q) << endl;

	Quat q_c = ~q;
	cout << "vector " << v << " rotates to " << Quat::RotateVec3_2(v,q) << endl;
	/*
	for (auto lightDir : lightDirs) {
		Vec3 targetLigthDir(0, 0, 1);
		Vec3 ld = Normalize(lightDir);
		auto cp = Cross(ld, targetLigthDir);
		auto dp = Dot(ld, targetLigthDir);
		Quat rot(cp.x, cp.y, cp.z, 1.0f+dp);
		rot.Normalize();
		Matrix44 m = rot;

		cout << ld << "\t" << (ld*m) << endl;
	}
	*/
	_getch();

	return 0;
}