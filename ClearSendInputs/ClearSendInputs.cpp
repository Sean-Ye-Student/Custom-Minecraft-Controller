#include <iostream>
#include <windows.h> //https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-input
#include <conio.h>

#include <fstream> //For file reading


using namespace std;

//https://stackoverflow.com/questions/4184468/sleep-for-millisecondsor 
#include <chrono>
#include <thread> 

/*Mapping (all int) 
(0=no shake, 1=shake), 
(0=no buttons, 1=left button, 2=right button, 3=both buttons), 
(0=LowLeftX, 1=NeutralLeftX, 2=HighLeftX),  
(0=LowLeftY, 1=NeutralLeftY, 2=HighLeftY), 
(0=LowRightX, 1=NeutralRightX, 2=HighRightY), 
(0=LowRightY, 1=NeutralRightY, 2=HighRightY)
(0=crouch, 1=idle, 2=jump),
LeftButton, LeftX, LeftY, RightButton, RightX, RightY

*/

const int forwardKey = 0x57;
const int leftKey = 0x41;
const int backwardKey = 0x53;
const int rightKey = 0x44;

const int inventoryKey = 0x45;
const int crouchKey = 0x10;
const int jumpKey = 0x20;

const int hotbarSlotKeys[9] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};

bool primaryAction = false;


//https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-keybdinput
void key(int code, bool released){
    INPUT inputs[4] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = code;
    if(released){
        inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
    }
   
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = 0x07;

    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = 0x07;
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

    UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    if (uSent != ARRAYSIZE(inputs))
    {
        cout << (L"SendInput failed: 0x%x\n", HRESULT_FROM_WIN32(GetLastError()));
    } 
}

//https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-mouseinput
void mouse(int x=0, int y=0){
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.dx = x;
    input.mi.dy = y;
    input.mi.time = 0;
    input.mi.dwFlags = MOUSEEVENTF_MOVE;
    SendInput(1, &input, sizeof(INPUT));
}

void mouseButton(bool left, bool down){
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.time = 0;
    if(left){
        if(down){
            input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        }else{
            input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        }
    }else{
        if(down){
            input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
        }else{
            input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
        }
    }
    
    
    SendInput(1, &input, sizeof(INPUT));
}

const int codes[6] = {forwardKey, leftKey, backwardKey, rightKey, crouchKey, jumpKey}; //Also used in move()
const int codesSize = 6;
int main(){
    key(0x07, false); //Must always press a key for the mouse to work idk man
    key(inventoryKey, true);
    for (int i=0; i < codesSize; i++){
        key(codes[i], true);
    }
    for(int i=0; i<9; i++){
        key(hotbarSlotKeys[i], true);
    }
    mouse(0, 0);
    mouseButton(true, false);
    mouseButton(false, false);
    return 0;
}