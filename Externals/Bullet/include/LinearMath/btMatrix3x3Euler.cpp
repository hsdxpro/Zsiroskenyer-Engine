//! @file	btMatrix3x3Euler.cpp
//! @brief	Construccion de matrices a partir de los angulos de euler
//!
//! @author Pedro Barrio

#include <iostream>
#include "../utils/auxiliar.h"
#include "btMatrix3x3Euler.h"
#include "../include/LinearMath.h"

using namespace std;

/** @brief Set the matrix from euler angles YPR around XYZ axes
 *  @param mat    matrix to calculate values from
 *  @param Yaw    around X axis
 *  @param Pitch  around Y axis
 *  @param Roll   around Z axis
 * 
 *  These angles are used to produce a rotation matrix. The euler
 *  angles are applied in XYZ order. I.e a vector is first rotated 
 *  about X then Y and then Z
 **/
 
btMatrix3x3 MatrixEulerXYZ ( double yaw, double pitch, double roll )
{
    btScalar ci ( btCos ( yaw   ) ); 
    btScalar cj ( btCos ( pitch ) ); 
	btScalar ch ( btCos ( roll  ) ); 
	btScalar si ( btSin ( yaw   ) ); 
	btScalar sj ( btSin ( pitch ) ); 
	btScalar sh ( btSin ( roll  ) ); 
	btScalar cc = ci * ch; 
	btScalar cs = ci * sh; 
	btScalar sc = si * ch; 
	btScalar ss = si * sh;
		
	return btMatrix3x3 ( cj * ch, sj * sc - cs, sj * cc + ss,
				         cj * sh, sj * ss + cc, sj * cs - sc, 
                            -sj,      cj * si,      cj * ci);
}

/** @brief Set the matrix from euler angles YPR around ZXY axes
 *  @param Yaw   about Z axis
 *  @param Pitch about X axis
 *  @param Roll  about Y axis
 * 
 *  These angles are used to produce a rotation matrix. The euler
 *  angles are applied in ZXY order.
 *  
 *  Esta la usamos para la orientacion del cepillo.
 **/

btMatrix3x3 MatrixEulerZXY ( double yaw, double pitch, double roll )
{
    // we use a basis change x original vector y rotated one
    // x |xyz = A·x |zxy
    // y |xyz = E·x |xyz = E·A·x|zxy 
    // y |zxy = 1/A ·E·A·x |zxy 
    
    btMatrix3x3 E = MatrixEulerXYZ( yaw, pitch, roll );
    btMatrix3x3 A = btMatrix3x3 ( 0, 0 ,1 , // x -> Z
                                  1, 0 ,0 , // y -> X
                                  0 ,1, 0 );// z -> Y
    
    return A.transpose() * E * A;
}

/** @brief Set the matrix from euler angles YPR around YXZ axes
 *  @param Yaw   about Y axis
 *  @param Pitch about X axis
 *  @param Roll  about Z axis
 * 
 *  These angles are used to produce a rotation matrix. The euler
 *  angles are applied in YXZ order.
 *  
 *  Esta la usamos para la orientacion del cepillo.
 **/

btMatrix3x3 MatrixEulerYXZ ( double yaw, double pitch, double roll )
{
    // we use a basis change x original vector y rotated one
    // x |xyz = A·x |zxy
    // y |xyz = E·x |xyz = E·A·x|zxy 
    // y |zxy = 1/A ·E·A·x |zxy 
    
    btMatrix3x3 E = MatrixEulerXYZ( yaw, pitch, roll );
    btMatrix3x3 A = btMatrix3x3 ( 0, 1 ,0 , // x -> Y
                                  1, 0 ,0 , // y -> X
                                  0 ,0, 1 );// z -> Z
    
    return A.transpose() * E * A;
}

/**@brief get euler angles from a matrix around XYZ
 * @param mat   Matrix from which calculate YPR
 * @param yaw   Yaw   around X axis
 * @param pitch Pitch around Y axis
 * @param roll  Roll  around Z axis 
 * @param solution_number Which solution of two possible solutions ( 1 or 2) are possible values
 * @return number of solutions or -1 if gimbal lock */	
 
