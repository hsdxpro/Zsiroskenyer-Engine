#include <iostream>
#include <conio.h>
#include "../../Core/src/math/math_all.h"

using namespace std;




int petiMain() {
	Matrix44 rotMat = Matrix44::RotationEuler(5, 10, 13.4);
	Matrix44 rotMatInv = Matrix44::Inverse(rotMat);
	Matrix44 checKmat = rotMatInv * rotMat;

	return 0;
}