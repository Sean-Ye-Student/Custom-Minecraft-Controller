//https://devblogs.microsoft.com/cppblog/building-your-c-application-with-visual-studio-code/
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
(0=no leftButtonPress, 1=leftButtonPress)
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

/*
void mouseScroll(){
     INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.time = 0;
    input.mi.mouseData = 1;
    input.mi.dwFlags = MOUSEEVENTF_WHEEL;
    SendInput(1, &input, sizeof(INPUT));
}
*/


const int codes[6] = {forwardKey, leftKey, backwardKey, rightKey, crouchKey, jumpKey}; //Also used in move()
const int codesSize = 6;
void releaseKeysAfterSleep(){
    std::this_thread::sleep_for(std::chrono::milliseconds(42)); //Wait for a frame before disabling keys for new invokes
    //From move function
    key(0x07, true); //Must always press a key for the mouse to work idk man

    for (int i=0; i < codesSize; i++){
        key(codes[i], true); //disabling keys for new invokes
    }


    //From action function
    for(int i=0; i<9; i++){
        key(hotbarSlotKeys[i], true);
    }
}

float hotbarValue = 0;
int slotIndex;
bool inventoryIsOpen = false;
bool cursorMode = false; //Used to disable secondary right click for easier inventory management with only clicks no shakes
void action(int shaking, int leftButton, int buttonState){
   
    //RIGHT Shakey thing will trigger a right or left click
   
    if (buttonState == 1){
        hotbarValue += 0.3;
        slotIndex = int(hotbarValue);
        if(slotIndex > 8){
            slotIndex = 0;
            hotbarValue = 0.0;
        }

        key(hotbarSlotKeys[slotIndex], false);

        // mouseScroll();  //LeftButton will switch inventory slots 
    }else if (buttonState == 3){ //To open inventory you must also shake
        inventoryIsOpen = !inventoryIsOpen;
        key(inventoryKey, false);  //Both buttons toggle inventory
        std::this_thread::sleep_for(std::chrono::milliseconds(42)); //Wait for a frame before disabling keys for new invokes
        key(inventoryKey, true);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //Inventory takes time to open anyways
    }else{
        if (leftButton == 1){
            mouseButton(true, true);                                       
            mouseButton(false, false);
        }else if (buttonState == 2){
            primaryAction = (shaking == 1);
            mouseButton(primaryAction, true);
            mouseButton(!primaryAction, false);
        }else{
            mouseButton(true, false);
            mouseButton(false, false);
        }
    }
}

void move(int leftStateX, int leftStateY, int rightStateX, int rightStateY, int stance){ //int leftX=500, int leftY=500, int rightX=500, int rightY=500){
    //Logic for Tank controls
    const bool conditions[6] = {leftStateY == 2, leftStateX == 0, leftStateY == 0, leftStateX == 2, stance == 0, stance == 2};//leftY > 1000, leftX < 23, leftY < 23, leftX > 1000};
    
    for (int i=0; i < codesSize; i++){
        if (conditions[i] == true){ 
            key(codes[i], false);
        }
    }
    
    //Logic for camera movement
    int sensitivity = 100;
    float p = sensitivity/62;
    int dx = int((rightStateX - 62) * p);
    int dy = int((62 - rightStateY) * p);
    
    key(0x07, false); //Must always press a key for the mouse to work idk man
    mouse(dx, dy);
    
    // if (rightStateX == 0){
    //     dx = -sensitivity;
    // }else if(rightStateX == 2){
    //     dx = sensitivity;
    // }

    // if (rightStateY == 0){
    //     dy = sensitivity;
    // }else if(rightStateY == 2){
    //     dy = -sensitivity;
    // }

   
   

}

//string pline; //Incase the infomation is imcomplete we will reference the previous result
const int numBytes = 8;
static int pdata[numBytes] = {0,0,0,1,1,62,62,1};
int* getData(){
    string line;
    ifstream MyReadFile("./nicedata.txt");
    getline(MyReadFile, line);
    static int data[numBytes] = {0,0,0,0,0,0,0,0};
    for(int i; i < numBytes; i++){
        data[i] = pdata[i];
    }
    int x = 0;
    string t = "";
    for(int i=0; i < (int)line.size(); i++){
        if(line[i] != ','){
            t += line[i];
        }else{
            data[x] = stoi(t);
            t = "";
            x++;
        }
    }

    MyReadFile.close();
    if(x != numBytes){
        return pdata; //Not enough data was copied over and we cannot use it
    }

    return data;

    // if ((int)line.size() == dataSize){
    //     pline = line;
    // }

    // static int data[] = {0,0,0,1,1,1,1,1};
    // if((int)pline.size() == dataSize){ //Will check if pline is actually valid, otherwise we just use the default data
    //     string t;
    //     for(int i=0; i<dataSize; i++){
    //         t = pline[i];
    //         data[i] = stoi(t);
    //     }
    // }
   
    
    
}

/*
(0=no shake, 1=shake), 
(0=no leftButtonPress, 1=leftButtonPress)
(0=no buttons, 1=left button, 2=right button, 3=both buttons), 
(0=LowLeftX, 1=NeutralLeftX, 2=HighLeftX),  
(0=LowLeftY, 1=NeutralLeftY, 2=HighLeftY), 
(0=LowRightX, 1=NeutralRightX, 2=HighRightY), 
(0=LowRightY, 1=NeutralRightY, 2=HighRightY)
(0=crouch, 1=idle, 2=jump),
*/

int main(){
    Sleep(3);
    
    while(1==1){
        int* data = getData();
        for (int i=0; i<numBytes; i++){
            cout << data[i];
        }
        cout << "\n";
        move(data[3], data[4], data[5], data[6], data[7]);
        action(data[0], data[1], data[2]);
        releaseKeysAfterSleep();

    }

    return 0;
}