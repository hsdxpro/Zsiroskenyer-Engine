//! @file	btMatrix3x3Euler.h
//! @brief	Construccion de matrices a parlir de los angulos de euler
//!
//! @author Pedro Barrio


#ifndef  _CABECERA_BTMATRIX3X3EULER_H_
#define  _CABECERA_BTMATRIX3X3EULER_H_

class btMatrix3x3;

btMatrix3x3 MatrixEulerXYZ ( double, double, double );
btMatrix3x3 MatrixEulerZXY ( double, double, double );
btMatrix3x3 MatrixEulerYXZ ( double, double, double );


int getEulerXYZ ( const btMatrix3x3& mat ,btScalar& yaw, btScalar& pitch, btScalar& roll, unsigned int solution_number ) 
{
    struct Euler { btScalar yaw, pitch, roll; };
    Euler euler_out;
    Euler euler_out2; //second solution
    //get the pointer to the raw data
    int nSolutions = 2 ;

    // Check that pitch is not at a singularity
    if ( btFabs ( mat.getRow ( 2 ).x( ) ) >=0.999 )
    {
        nSolutions      = -1;
        euler_out.roll  =  0.;
        euler_out2.roll =  0.;
	
        // From difference of angles formula
        
    
        if ( mat.getRow ( 2 ).x() < 0. )  //gimbal locked up
        {
            btScalar delta = btAtan2( mat.getRow ( 0 ).y(), mat.getRow ( 0 ).z() );
            euler_out.pitch  = SIMD_PI / btScalar ( 2.0 );
            euler_out2.pitch = SIMD_PI / btScalar ( 2.0 );
            euler_out.yaw    = delta + euler_out.roll;
            euler_out2.yaw   = delta + euler_out2.roll;
        }
        else // gimbal locked down
        {
            btScalar delta = btAtan2( -mat.getRow ( 0 ).y(), -mat.getRow ( 0 ).z() );
            euler_out.pitch  = -SIMD_PI / btScalar ( 2.0 );
            euler_out2.pitch = -SIMD_PI / btScalar ( 2.0 );
            euler_out.yaw    = -delta + euler_out.roll;
            euler_out2.yaw   = -delta + euler_out2.roll;
        }        
    }
    else
    {
        euler_out.pitch  = - btAsin( mat.getRow ( 2 ).x() );
        euler_out2.pitch = SIMD_PI - euler_out.pitch;
	
        euler_out.yaw   = btAtan2 ( mat.getRow ( 2 ).y() / btCos ( euler_out.pitch ), 
                                    mat.getRow ( 2 ).z() / btCos ( euler_out.pitch ) );
                         
        euler_out2.yaw  = btAtan2 ( mat.getRow ( 2 ).y() / btCos ( euler_out2.pitch ), 
			                        mat.getRow ( 2 ).z() / btCos ( euler_out2.pitch ));
            
        euler_out.roll  = btAtan2 ( mat.getRow ( 1 ).x() / btCos ( euler_out.pitch ), 
			                        mat.getRow ( 0 ).x() / btCos ( euler_out.pitch ) );
                          
        euler_out2.roll = btAtan2 ( mat.getRow ( 1 ).x() / btCos ( euler_out2.pitch ), 
                                    mat.getRow ( 0 ).x() / btCos ( euler_out2.pitch ) );
    }
    
    if ( solution_number == 1)
    { 
		yaw   = euler_out.yaw; 
		pitch = euler_out.pitch;
		roll  = euler_out.roll;
    }
    else
    { 
		yaw   = euler_out2.yaw; 
		pitch = euler_out2.pitch;
		roll  = euler_out2.roll;
    }
    return nSolutions ;
}
#endif