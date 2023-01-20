#To export as application use pyinstaller
# C:\Users\sean1\PycharmProjects\newstuff\ArduinoStuff\Controller\Controller\Custom Minecraft Controller.py
#in command prompt
import os
rootFolder = os.path.dirname(os.path.abspath(__file__))
dataTxt = rootFolder + r"\data.txt"
NicedataTxt = rootFolder + r"\nicedata.txt"

cooltermsettings = rootFolder + r"\Connection Settings.stc"
cppcontroller = rootFolder + r"\CPPController.exe"

try:
    os.startfile(cooltermsettings)
except:
    print("Must run the app coolterm application for Connection Settings.stc to initialize!")
    while True:
        pass
with open(dataTxt, "w") as f:
    f.write("")

print("WELCOME TO THE CONTROLLER")
print("Please Setup your connection settings using the Connection Settings tab!")
print("In Connection -> Option, check if the port is valid and the Baudrate is 2000000")
print("In Connection -> Capture to Text/Binary File -> Start Select the data.txt folder in the controller application")
print("Press the connect button!")
print("")
print("CONTROLS")
print("Left joystick press for selecting next hotbar slot")
print("Left joystick motion controls player movement")
print("Left under button will left click (only use this when in menu's)!")
print("Right joystick press will either left or right mouse click")
print("Shake the right joystick for a left click and only press for a right click")
print("You might need to constantly shake when breaking blocks")
print("Press both joysticks to toggle inventory")
print("No need to shake in inventory for left click, just press the left under button")
print("Use right joystick motion to move cursor in inventory and menu's")
print("Right joystick motion controls the player view")
print("Crouch or jump to crouch or jump")
print("")
print("Press the connect button in settings and wait for the continue prompt!")

while True:
    with open(dataTxt, "r") as f:
        if len(f.read()) > 0:
            input("CoolTerm has connected! Press enter to continue")
            break

os.startfile(cppcontroller)
targetByteAmount = 8
while True:
    lastline = ""
    Bytes = None
    with open(dataTxt, "r") as f:
        lines = f.readlines()
        for i in range(len(lines) - 1, -1, -1):
            Bytes = bytes(lines[i].strip("\n"), "utf-8")
            if len(Bytes) == targetByteAmount:
                break #We found the first valid byte array now we break

    with open(NicedataTxt, "w") as f:
        formattedLine = ""
        for i in range(targetByteAmount):
            byte = (0 if i < 3 else 1) #Shake, leftButton and joystick are zero by default, else 1
            if i < len(Bytes) and Bytes[i] > 0:
                byte = Bytes[i] - 1
                if abs(byte - 62) <= 3:
                    byte = 62 #For small joystick inconsistencies we can set it to 62
            formattedLine += str(byte) + ","
        f.write(formattedLine)
        print(formattedLine)
        