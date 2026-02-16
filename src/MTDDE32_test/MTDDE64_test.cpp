/* =======================================================================
Description - 64bit unmanaged DLL interface to control Microtest (via DDE)
Copyright (c) 2012 Deben UK Ltd.

Notes:
Based on Ed's "MTExternal" VB.Net program.
Coding by Austin Day
This version dated 4 July 2012

Requirements:
1) "DebenMT64.dll" must be in the same directory as this program
2) 64 bit Windows
3) If "VCP100D.DLL" is flagged as missing, install VCREDIST_x64.exe from
   http://www.microsoft.com/en-us/download/details.aspx?id=14632

======================================================================= */

#include "stdafx.h"
#include "MTDDE64.h"
#include <windows.h>
#include <iostream>
//#include <stdio.h>
#include <conio.h>
using namespace std;

 #define errordouble -9999;  // A return value that indicates an error, e.g. "value not available" or "error in conversion".
 
 void showhint () {
				cout << "\nKeys:\n";
				cout << "  q - quit\n";
				cout << "  m - start motor\n";
				cout << "  s - stop motor\n";
				cout << "  u - move apart\n";
				cout << "  t - move together\n";
				cout << "  c - clear\n";
				cout << "  f - Goto Force 5N\n";
				cout << "  x - Goto Extension 2mm\n";
				cout << "  a - temperature A 50'C\n";
				cout << "  b - temperature B 40'C\n";
				cout << "  o - temperature control off\n";
				cout << "  [space] - measure\n";
				cout << "  i - information";
				cout << "  0..4 - motor speed 0..4\n";
				cout << "  5..9 - set gain 0..4\n\n";  }

 // If the motor is running, then display the force, extension and time
 void showinfo () {
	double d;
	bool b;
	int i;

	b = MT_IsMotorRunning(); 
	if (b) cout << "Running; ";
		else cout << "Motor Stopped; ";
	d = MT_GetForce(); cout << "Force " << d << "N;\t";
	//d = MT_GetPosition(); cout << "Pos " << d << "mm;\t";
	d = MT_GetExtension(); cout << "Ext " << d << "mm;\t";	
	d = MT_GetTime(); cout << "Time " << d << "s\t";

	// Check for errors
	i = MT_GetErrorCount(); 
	if (i<=0)  cout << "\n";
	else cout << "Errors " << i << "\n";
 }

  void showinfo2 () {
	double d;
	int i;
	bool b;

	b = MT_IsConnected(); 
	if (b) cout << "Connected; ";
		else cout << "Not connected; ";

	d = MT_GetTemperatureA(); cout << "Temperature A " << d << "'C;\t";
	d = MT_GetTemperatureB(); cout << "Temperature B " << d << "'C;\t";
	d = MT_GetSampleTime(); cout << "Sample time " << d << "ms;\t";
	d = MT_GetMotorSpeed(); cout << "Motor Speed " << d << "mm/min;\t";

	i = MT_GetGain(); cout << "Gain " << i << ";\t";
	i = MT_GetGotoAction(); cout << "Goto Action " << i << ";\t";
	i = MT_GetGotoUnits(); cout << "Goto Units " << i << "\t\n";
 }

 // Display a list of errors
 void showerrors () {
	int i, n;
	char s[100];
	
	i = MT_GetErrorCount(); cout << "ErrorCount = " << i << "\n";
	for (n=0; n<i; n++)
	{
		MT_GetErrorStr(&s[0], 100);
		cout << "  Error = '" << s << "'\n";
	}
	cout << "\n";
 }

 void main() {
	// Load the DebenMT64.dll DLL
	HINSTANCE hLib=LoadLibrary(L"DebenMT64.dll");
	if(hLib==NULL) {
		cout << "Unable to load library!" << endl;
		_getch();
		return;
	}

	// Get the functions from the DLL
	MT_Connect               = (DLL_MT_NoParams   )GetProcAddress((HMODULE)hLib, "MT_Connect");
	MT_Disconnect            = (DLL_MT_NoParams   )GetProcAddress((HMODULE)hLib, "MT_Disconnect");
	MT_Version               = (DLL_MT_Version    )GetProcAddress((HMODULE)hLib, "MT_Version");					
	MT_IsMotorRunning        = (DLL_MT_Bool       )GetProcAddress((HMODULE)hLib, "MT_IsMotorRunning");
	MT_GetForce              = (DLL_MT_Double     )GetProcAddress((HMODULE)hLib, "MT_GetForce");
	MT_GetPosition           = (DLL_MT_Double     )GetProcAddress((HMODULE)hLib, "MT_GetPosition");
	MT_GetExtension          = (DLL_MT_Double     )GetProcAddress((HMODULE)hLib, "MT_GetExtension");
	MT_GetTime               = (DLL_MT_Double     )GetProcAddress((HMODULE)hLib, "MT_GetTime");
	MT_GetErrorCount         = (DLL_MT_Integer    )GetProcAddress((HMODULE)hLib, "MT_GetErrorCount");
	MT_GetErrorStr           = (DLL_MT_Err_Str    )GetProcAddress((HMODULE)hLib, "MT_GetErrorStr2");
	MT_GotoLoad              = (DLL_MT_SetDouble  )GetProcAddress((HMODULE)hLib, "MT_GotoLoad");
	MT_GotoAbsolutePosition  = (DLL_MT_SetDouble  )GetProcAddress((HMODULE)hLib, "MT_GotoAbsolutePosition");
	MT_GotoExtension         = (DLL_MT_SetDouble  )GetProcAddress((HMODULE)hLib, "MT_GotoExtension");
	MT_GotoSeconds           = (DLL_MT_SetDouble  )GetProcAddress((HMODULE)hLib, "MT_GotoSeconds");	
	MT_StartMotor            = (DLL_MT_NoParams   )GetProcAddress((HMODULE)hLib, "MT_StartMotor");
	MT_StopMotor             = (DLL_MT_NoParams   )GetProcAddress((HMODULE)hLib, "MT_StopMotor"); 
	MT_Clear                 = (DLL_MT_NoParams   )GetProcAddress((HMODULE)hLib, "MT_Clear"); 
	MT_MoveApart             = (DLL_MT_NoParams   )GetProcAddress((HMODULE)hLib, "MT_MoveApart"); 
	MT_MoveTogether          = (DLL_MT_NoParams   )GetProcAddress((HMODULE)hLib, "MT_MoveTogether"); 
	MT_Stop                  = (DLL_MT_NoParams   )GetProcAddress((HMODULE)hLib, "MT_Stop"); 
	MT_Measure               = (DLL_MT_NoParams   )GetProcAddress((HMODULE)hLib, "MT_Measure"); 
	MT_ConstantLoad          = (DLL_MT_NoParams   )GetProcAddress((HMODULE)hLib, "MT_ConstantLoad");
	MT_GotoValue             = (DLL_MT_SetDouble  )GetProcAddress((HMODULE)hLib, "MT_GotoValue");
	MT_SetMotorSpeed         = (DLL_MT_SetInteger )GetProcAddress((HMODULE)hLib, "MT_SetMotorSpeed");
	MT_SetSampleTime         = (DLL_MT_SetInteger )GetProcAddress((HMODULE)hLib, "MT_SetSampleTime");
	MT_SetGain               = (DLL_MT_SetInteger )GetProcAddress((HMODULE)hLib, "MT_SetGain");
	MT_SetGotoAction         = (DLL_MT_SetInteger )GetProcAddress((HMODULE)hLib, "MT_SetGotoAction");
	MT_SetGotoUnits          = (DLL_MT_SetInteger )GetProcAddress((HMODULE)hLib, "MT_SetGotoUnits");
	MT_SetTemperatureA       = (DLL_MT_SetDouble  )GetProcAddress((HMODULE)hLib, "MT_SetTemperatureA");
	MT_SetTemperatureB       = (DLL_MT_SetDouble  )GetProcAddress((HMODULE)hLib, "MT_SetTemperatureB");
	MT_TemperatureControLOff = (DLL_MT_NoParams   )GetProcAddress((HMODULE)hLib, "MT_TemperatureControLOff");

	// *********** New functions added 2012-08-28 ***********
	MT_GetTemperatureA       = (DLL_MT_Double     )GetProcAddress((HMODULE)hLib, "MT_GetTemperatureA");
	MT_GetTemperatureB       = (DLL_MT_Double     )GetProcAddress((HMODULE)hLib, "MT_GetTemperatureB");
	MT_GetSampleTime         = (DLL_MT_Double     )GetProcAddress((HMODULE)hLib, "MT_GetSampleTime");
	MT_GetGain               = (DLL_MT_Integer    )GetProcAddress((HMODULE)hLib, "MT_GetGain");
	MT_GetMotorSpeed         = (DLL_MT_Double     )GetProcAddress((HMODULE)hLib, "MT_GetMotorSpeed");
	MT_GetGotoAction         = (DLL_MT_Integer    )GetProcAddress((HMODULE)hLib, "MT_GetGotoAction");
	MT_GetGotoUnits          = (DLL_MT_Integer    )GetProcAddress((HMODULE)hLib, "MT_GetGotoUnits");
	MT_IsConnected           = (DLL_MT_Bool       )GetProcAddress((HMODULE)hLib, "MT_IsConnected");



	// Check the functions are not null, i.e we have something to call
	if (MT_Version==NULL) {
		cout << "Unable to load function(s)." << endl;
		FreeLibrary((HMODULE)hLib);
		return;
	}	

	// Connect to the Microtest (via the 64bit DLL and thus DDE)
	MT_Connect();

	// Get the version information for the DLL
	int vermaj, vermin;         
	MT_Version(&vermaj, &vermin);
	cout << "Microtest DDE 64bit DLL test";
	cout << "  Version " << vermaj << "." << vermin << "\n\n";

	// Show hints (keyboard shortcuts) and current information, e.g. Force.
	showhint();
	showinfo();
	
	// Loop 
	char ch=0;
	do 
	{
		// If the motor is running, then show current information, force, extension, time...
		if (MT_IsMotorRunning())
			showinfo();

		// Check if a key has been pressed
		if (_kbhit()!=0) 
		{
			ch = _getch();
			ch = tolower(ch);

			// Respond to the key press
			switch (ch)
			{
				case 'm': MT_StartMotor(); break;							// Start the motor
				case 's': MT_StopMotor(); break;							// Stop the motor
				case 'u': MT_MoveApart(); break;							// Move jaws apart
				case 't': MT_MoveTogether(); break;							// Move jaws togehter	
				case '0': MT_SetMotorSpeed(0); break;						// Set motor speed 0
				case '1': MT_SetMotorSpeed(1); break;						// Set motor speed 1
				case '2': MT_SetMotorSpeed(2); break;						// Set motor speed 2
				case '3': MT_SetMotorSpeed(3); break;						// Set motor speed 3
				case '4': MT_SetMotorSpeed(4); break;						// Set motor speed 4
				case '5': MT_SetGain(0); break;								// Set Gain 0
				case '6': MT_SetGain(1); break;								// Set Gain 1
				case '7': MT_SetGain(2); break;								// Set Gain 2
				case '8': MT_SetGain(3); break;								// Set Gain 3
				case '9': MT_SetGain(4); break;								// Set Gain 4
				case 'f': MT_GotoLoad(5/*Load 5N*/); break;					// Goto Load 5N
				case 'x': MT_GotoExtension(2/*Extension 2mm*/); break;		// Goto Extension 2mm
				case 'c': MT_Clear(); break;								// Clear data
				case 'a': MT_SetTemperatureA(50); break;					// Set temperature A 50'C
				case 'b': MT_SetTemperatureB(40); break;					// Set temperature B 40'C
				case 'o': MT_TemperatureControLOff(); break;				// Switch off temperature control
				case ' ': MT_Measure(); break;								// Switch on measurement
				case 'e': showerrors(); break;								// List errors
				case 'i': showinfo2(); break;								// Show information - temperature, etc.
				default: showhint(); break;
			}
		}

		// Wait a little
		Sleep(200);
	} while (ch!='q');

	// We've finished, so disconnect from the Microtest
	MT_Disconnect;

	// Free the DLL
	FreeLibrary((HMODULE)hLib);
 }

