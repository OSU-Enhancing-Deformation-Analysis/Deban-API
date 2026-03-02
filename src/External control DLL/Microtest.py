#You need to have Python running AsAdministrator.

#Navigate to the python.exe file (windows start, type python, right click, Open File Location) and set it to run AsAdministrator in both compatibilty and shortcut/advanced tabs.

#You should now be able to run the script and connect to Microtest.exe
import time
from ctypes import *
deben = CDLL("C:/Program Files (x86)/Microtest/DebenMT64.dll")
print("Loaded library: ",deben)
result = deben.MT_Connect()    
print("Connect result = ", str(result))


print("Goto Extension...")
#deben.MT_GotoExtension(c_double(1.5)) 
PosTarget = 1.5
deben.MT_GotoExtension.argtypes = [c_double]
deben.MT_GotoExtension(PosTarget)

deben.MT_GetForce.restype = c_double
currentforce = deben.MT_GetForce()
print("Current force = ",str(currentforce), "N")

deben.MT_GetPosition.restype = c_double
currentposition = deben.MT_GetPosition()
print("Current position = ",str(currentposition), "mm")

while(currentposition<PosTarget):
	currentposition = deben.MT_GetPosition()
	currentforce = deben.MT_GetForce()
	print(str(currentforce), "N ",str(currentposition), "mm")
	time.sleep(0.5)
	
print("Complete.")
deben.MT_Disconnect()
input("Press <CR> to end")
