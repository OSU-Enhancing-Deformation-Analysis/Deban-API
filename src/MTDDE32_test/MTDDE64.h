/* =======================================================================
Description - 64bit unmanaged DLL interface to control Microtest (via DDE)
Copyright (c) 2012 Deben UK Ltd.
======================================================================= */

#ifndef MRDDE64_H
#define MRDDE64_H

//#ifdef __cplusplus
//extern "C" {
//#endif

	typedef void   (__stdcall* DLL_MT_NoParams) ();				// Prototype for a function with no parameters that returns nothing, e.g. MT_Connect
	typedef double (__stdcall* DLL_MT_Double) ();				// Prototype for a function with no parameters that returns a double, e.g. MT_GetForce
	typedef int    (__stdcall* DLL_MT_Integer) ();				// Prototype for a function with no parameters that returns an integer, e.g. MT_GetGain
	typedef bool   (__stdcall* DLL_MT_Bool) ();					// Prototype for a function with no parameters that returns a Boolean, e.g. MT_IsMotorRunning
	typedef void   (__stdcall* DLL_MT_SetDouble) (double);		// Prototype for a function the passes a double as a parameter but returns nothing, e.g. MT_GotoLoad
	typedef void   (__stdcall* DLL_MT_SetInteger) (int);		// Prototype for a function the passes an integer as a parameter but returns nothing, e.g. MT_SetGain
	typedef void   (__stdcall* DLL_MT_Version) (int*, int*);	// Prototype for MT_Version that writes data to two integer parameters, e.g. MT_Version
	typedef void   (__stdcall* DLL_MT_Err_Str) (char*, int);	// Prototype for MT_Version that writes data to a string (using a pointer to string buffer and its length), e.g. MT_GetErrorString2

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// List of Microtest Functions retrieved from DebenMT64.dll - type first, function name second
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// MT_Connect connects to the Microtest (via DDE). This should be the first command called
	DLL_MT_NoParams   MT_Connect;					

	// MT_Disconnect disconnects from the Microtest (via DDE). This should be the last command called
	DLL_MT_NoParams   MT_Disconnect;				

	// MT_Version(Major, Minor) returns the Major and Minor version numbers for the DLL, e.g. "1.0"
	DLL_MT_Version    MT_Version;					

	// MT_IsMotorRunning returns true (255) if the motor is running, false (0) if not
	DLL_MT_Bool       MT_IsMotorRunning;			

	// MT_GetForce returns the current Force, in Newtons; otherwise -9999 to indicate an error
	DLL_MT_Double     MT_GetForce;	 				

	// MT_GetPosition returns the Start Position for the specimen, in mm; otherwise -9999 to indicate an error
	DLL_MT_Double     MT_GetPosition;				

	// MT_GetExtension returns the current Extension, in mm; otherwise -9999 to indicate an error 
	DLL_MT_Double 	  MT_GetExtension; 				

	// MT_GetTime returns the duration of the current test, in seconds; otherwise -9999 to indicate an error 
	DLL_MT_Double     MT_GetTime;					

	// MT_GetErrorStr returns the last error message and removes it from the queue (cyclical buffer)
	DLL_MT_Err_Str    MT_GetErrorStr;

	// MT_GetErrorCount returns the number of error messages waiting to be read
	DLL_MT_Integer    MT_GetErrorCount;				

	// MT_GotoLoad(Load) requests the Microtest to go to specified Load in Newtons (passed as a double)
	DLL_MT_SetDouble  MT_GotoLoad;					

	// MT_GotoAbsolutePosition(AbsPos) requests the Microtest to go to specified Absolute Position in mm (passed as a double)
	DLL_MT_SetDouble  MT_GotoAbsolutePosition;		

	// MT_GotoExtension(Ext) requests the Microtest to go to specified Extension in mm (passed as a double)
	DLL_MT_SetDouble  MT_GotoExtension;				

	// MT_GotoSeconds(Secs) requests the Microtest to go to specified Extension in mm (passed as a double)
	DLL_MT_SetDouble  MT_GotoSeconds;				

	// MT_StartMotor asks the Microtest to start the motor
	DLL_MT_NoParams   MT_StartMotor;				

	// MT_StopMotor asks the Microtest to stop the motor 
	DLL_MT_NoParams   MT_StopMotor;					

	// MT_Clear asks the Microtest to stop the current data, extension... and start a new experiment
	DLL_MT_NoParams   MT_Clear;						

	// MT_MoveApart asks the Microtest to move the stage jaws apart (at full speed)
	DLL_MT_NoParams   MT_MoveApart;					

	// MT_MoveTogether asks the Microtest to move the stage jaws together (at full speed)
	DLL_MT_NoParams   MT_MoveTogether; 				

	// MT_Stop asks the Microtest to stop the motors
	DLL_MT_NoParams   MT_Stop;						

	// MT_Measure asks the Microtest to start measuring
	DLL_MT_NoParams   MT_Measure;					

	// MT_ConstantLoad asks the Microtest to switch to constant load mode
	DLL_MT_NoParams   MT_ConstantLoad;				

	// MT_GotoValue(Value) tells Microtest to go to a specific value (specified by the MT_SetGotoUnits command), e.g. 23.45N 	
	DLL_MT_SetDouble  MT_GotoValue;					
	
	// MT_SetMotorSpeed(Speed 0..4) asks the Microtest to select Motor Speed 0 to 4. The actual speed will depend on your stage and gearbox
	DLL_MT_SetInteger MT_SetMotorSpeed;

	// MT_SetSampleTime(Secs) sets the sample time for a Goto command (MT_GotoValue)
	DLL_MT_SetInteger MT_SetSampleTime;

	// MT_SetGain(Gain) sets the Gain 
	DLL_MT_SetInteger MT_SetGain;

	// MT_SetGotoAction(Action 0..2) tells Microtest what Goto action to perform once it has reached the specified value (MT_GotoValue) - 0=Stop, 1=Measure, 2=Constant load
	DLL_MT_SetInteger MT_SetGotoAction;

	// MT_SetGotoUnits(Units 0..3) tells Microtest what units the MT_GotoValue command should use - 0=Force (N); 1=Absolute Pos (mm); 2=Extension (mm); 3=Seconds
	DLL_MT_SetInteger MT_SetGotoUnits;				

	// MT_SetTemperatureA(Temp_C) asks Microtest to set temperature A to the value, in °C
	DLL_MT_SetDouble  MT_SetTemperatureA;

	// MT_SetTemperatureB(Temp_C) asks Microtest to set temperature B to the value, in °C
	DLL_MT_SetDouble  MT_SetTemperatureB;

	// MT_TemperatureControLOff asks Microtest to switch off temperature control
	DLL_MT_NoParams   MT_TemperatureControLOff;



	// *********** New functions added 2012-08-28 ***********

	// MT_GetTemperatureB asks Microtest for the current temperature of the upper (A) jaw, in °C
	DLL_MT_Double  MT_GetTemperatureA;

	// MT_GetTemperatureB asks Microtest for the current temperature of the lower (B) jaw, in °C
	DLL_MT_Double  MT_GetTemperatureB;

	// MT_GetSampleTime gets the sample time, in seconds
	DLL_MT_Double MT_GetSampleTime;

	// MT_GetGain gets the Gain 
	DLL_MT_Integer MT_GetGain;

	// MT_GetMotorSpeed asks the Microtest for the Motor Speed (0 to 4). The actual speed will depend on your stage and gearbox
	DLL_MT_Double MT_GetMotorSpeed;

	// MT_GetGotoAction asks Microtest what the Goto action is - 0=Stop, 1=Measure, 2=Constant load, [3=Step & 4=Cyclic]
	DLL_MT_Integer MT_GetGotoAction;

	// MT_GetGotoUnits asks Microtest what units the MT_GotoValue command is using - 0=N (Force); 1=mm (Extension); 2=mm (Absolute Pos); 3=Seconds
	DLL_MT_Integer MT_GetGotoUnits;				

	// MT_IsConnected returns true if connected to the Microtest (via the DebenMicrotest DLL & thus DDE) 
	DLL_MT_Bool MT_IsConnected;

//#ifdef __cplusplus
//}
//#endif
#endif  /* MRDDE64_H */