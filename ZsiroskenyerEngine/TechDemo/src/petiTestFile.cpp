#include <iostream>
#include <conio.h>
#include "../../Core/src/math/math_all.h"

using namespace std;




int petiMain() {
	Matrix44 mat;
	Matrix44 inv;

	mat.RotationAxisAngle(Vec3(0.5f, 0.5f, 0.5f).Normalize(), 0.45582f);
	inv = Matrix44::Inverse(mat);

	Matrix44 res = inv*mat;

	return 0;
}