#ifndef KINEMATICS_H
#define KINEMATICS_H

#include "GlobalArm.h"
#include <Arduino.h>


//////////////////////////////////////////////////////////////////////////////
// KINEMATICS CONFIG  //
//////////////////////////////////////////////////////////////////////////////

// IK Modes defined, 0-2
enum {
	IKM_IK3D_CARTESIAN, IKM_CYLINDRICAL, IKM_BACKHOE};


// status messages for IK return codes..
enum {
	IKS_SUCCESS=0, IKS_WARNING, IKS_ERROR};

#define IK_FUDGE	5     // How much a fudge between warning and error

#define ftl(x) ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))  //float to long conversion


byte     g_bIKMode = IKM_IK3D_CARTESIAN;   // Which mode of operation are we in...

/* pre-calculations */
float hum_sq = HUMERUS*HUMERUS;
float uln_sq = ULNA*ULNA;

// IK coordinate variables
// Current IK values
float	g_sIKX  =0.00;					// Current X value in mm
float	g_sIKY  =150.00;				//
float	g_sIKZ  =150.00;
float	g_sIKGA =0.00;					// IK Gripper angle..

//Next IK Values
float	sIKX  =0.00;					// Current X value in mm
float	sIKY  =0.00;					//
float	sIKZ  =0.00;
float	sIKGA =0.00;					// IK Gripper angle..

// Values for current servo values for the different joints
int	g_sBase ;					// Current Base servo value
int	g_sShoulder;				// Current shoulder target 
int	g_sElbow;					// Current
int	g_sWrist;					// Current Wrist value
int	g_sWristRot;				// Current Wrist rotation
int	g_sGrip;					// Current Grip position

int sBase, sShoulder, sElbow, sWrist, sWristRot, sGrip = 1500;



////////////////////////////////////////////////////////////////////////////// 

//===================================================================================================
// doArmIK: Floating Point Arm IK Solution for PWM Servos
//===================================================================================================
// arm positioning routine utilizing inverse kinematics
// Z is height, Y is distance from base center out, X is side to side. Y,Z can only be positive
// return SBase, sShoulder, sElbow, sWrist

boolean doArmIK(boolean fCartesian, float x, float y, float z, float grip_angle_d)
{

	float grip_angle_r = radians( grip_angle_d );    //grip angle in radians for use in calculations

	// Base angle and radial distance from x,y coordinates
	float bas_angle_r = atan2( x, y );
	float rdist = sqrt(( x * x ) + ( y * y ));

	if (fCartesian) 
	{
		// rdist is y coordinate for the arm
		y = rdist;
	}
	else 
	{
		// We are in cylindrical mode, probably simply set y` to the y we passed in...
		y = y;
	}

	// Grip offsets calculated based on grip angle
	float grip_off_z = ( sin( grip_angle_r )) * GRIPPER;
	float grip_off_y = ( cos( grip_angle_r )) * GRIPPER;

	// Wrist position
	float wrist_z = ( z - grip_off_z ) - BASE_HGT;
	float wrist_y = y - grip_off_y;

	// Shoulder to wrist distance ( AKA sw )
	float s_w = ( wrist_z * wrist_z ) + ( wrist_y * wrist_y );
	float s_w_sqrt = sqrt( s_w );

	// s_w angle to ground
	float a1 = atan2( wrist_z, wrist_y );

	// s_w angle to humerus
	float a2 = acos((( hum_sq - uln_sq ) + s_w ) / ( 2 * HUMERUS * s_w_sqrt ));

	// Shoulder angle
	float shl_angle_r = a1 + a2;
	float shl_angle_d = degrees( shl_angle_r );

	// Elbow angle
	float elb_angle_r = acos(( hum_sq + uln_sq - s_w ) / ( 2 * HUMERUS * ULNA ));
	float elb_angle_d = degrees( elb_angle_r );
	float elb_angle_dn = -( 180.0 - elb_angle_d );

	// Wrist angle
	float wri_angle_d = ( grip_angle_d - elb_angle_dn ) - shl_angle_d;

	// Servo pulses
	if (fCartesian) 
	{
		sBase = (ftl(1500.0 - (( degrees( bas_angle_r )) * 10.55 )));//only set base if we are n cartesian mode
	}

	sShoulder = (ftl(1500.0 - (( shl_angle_d - 90) * 10.55 )));
	sElbow = (ftl(1500.0 + (( elb_angle_d - 90.0 ) * 10.55 )));
	sWrist = (ftl(1500 + ( wri_angle_d  * 10.55 )));

	//return = g_bIKStatus = IKS_SUCCESS;
}

//=============================================================================
//=============================================================================



//===================================================================================================
// MoveArmTo
//===================================================================================================
void MoveArmTo(int sBase, int sShoulder, int sElbow, int sWrist, int sWristRot, int sGrip) {

	// Save away the current positions...
	g_sBase = sBase;
	g_sShoulder = sShoulder;
	g_sElbow = sElbow;
	g_sWrist = sWrist;
	g_sWristRot = sWristRot;
	g_sGrip = sGrip;



	Base = sBase;
	Shoulder = sShoulder;
	Elbow = sElbow;
	Wrist = sWrist;

	Gripper = sGrip;



}

//===================================================================================================
// MoveArmToHome
//===================================================================================================
//void MoveArmToHome(void) {
//
//	sBase = 1500;
//	sShoulder = 1500;
//	sElbow = 1500;
//	sWrist = 1500;
//
//	MoveArmTo(sBase, sShoulder, sElbow, sWrist, 512, 256, 2000, true);
//	g_fArmActive = false;
//	
//}




#endif




