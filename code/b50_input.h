/* date = June 10th 2020 9:52 am */

#ifndef B50_INPUT_H
#define B50_INPUT_H

#include <xinput.h>

// TODO(Barret5Ocal): need to learn how to save inputs to file 

#define BUFFER_MAX 200

struct b50_input_buffer
{
    short InputBuffer[BUFFER_MAX];
    int Index = 0;
    
    // NOTE(Barret5Ocal): set this to make IsButton functions get inputs from a few frame back 
    int InputLag = 0;
};

void CaptureXboxController(b50_input_buffer *Buffer, int Player)
{
    
    XINPUT_STATE state;
    ZeroMemory( &state, sizeof(XINPUT_STATE) );
    
    // Simply get the state of the controller from XInput.
    int dwResult = XInputGetState( Player, &state );
    
    if( dwResult == ERROR_SUCCESS )
    {
        // Controller is connected 
        
        Buffer->InputBuffer[Buffer->Index] = 0;
        Buffer->InputBuffer[Buffer->Index++] = state.Gamepad.wButtons;
        
        if(Buffer->Index > BUFFER_MAX - 1)
            Buffer->Index = 0;
        
    }
    else
    {
        // Controller is not connected 
    }
}

enum BUTTONS
{
    DPAD_UP = 0x0001,
    DPAD_DOWN = 0x0002,
    DPAD_LEFT = 0x0004,
    DPAD_RIGHT = 0x0008,
    
    START = 0x0010,
    BACK = 0x0020,
    LEFT_THUMB = 0x0040,
    RIGHT_THUMB = 0x0080,
    
    LEFT_SHOULDER = 0x0100,
    RIGHT_SHOULDER = 0x0200,
    LEFT_TRIGGER = 0x0400,
    RIGHT_TRIGGER = 0x0800,
    
    A = 0x1000,
    B = 0x2000,
    X = 0x4000,
    Y = 0x8000
};

void CaptureKeyboard(b50_input_buffer *Buffer)
{
    
    short Final = 0;
    
    if(GetKeyState(0x57) >> 15)
        Final |= DPAD_UP;
    
    if(GetKeyState(0x53) >> 15)
        Final |= DPAD_DOWN;
    
    if(GetKeyState(0x41) >> 15)
        Final |= DPAD_LEFT;
    
    if(GetKeyState(0x44) >> 15)
        Final |= DPAD_RIGHT;
    
    if(GetKeyState(VK_RETURN) >> 15)
        Final |= START;
    
    if(GetKeyState(VK_BACK) >> 15)
        Final |= BACK;
    
    if(GetKeyState(VK_TAB) >> 15)
        Final |= LEFT_THUMB;
    
    if(GetKeyState(VK_CLEAR) >> 15)
        Final |= RIGHT_THUMB;
    
    if(GetKeyState(0x56) >> 15)
        Final |= LEFT_SHOULDER;
    
    if(GetKeyState(0x42) >> 15)
        Final |= RIGHT_SHOULDER;
    
    if(GetKeyState(0x4E) >> 15)
        Final |= LEFT_TRIGGER;
    
    if(GetKeyState(0x4D) >> 15)
        Final |= RIGHT_TRIGGER;
    
    if(GetKeyState(0x55) >> 15)
        Final |= A;
    
    if(GetKeyState(0x49) >> 15)
        Final |= B;
    
    if(GetKeyState(0x4F) >> 15)
        Final |= X;
    
    if(GetKeyState(0x50) >> 15)
        Final |= Y;
    
    
    Buffer->InputBuffer[Buffer->Index] = 0;
    Buffer->InputBuffer[Buffer->Index++] = Final;
    
    if(Buffer->Index > BUFFER_MAX - 1)
        Buffer->Index = 0;
    
}

int IsButtonDown(b50_input_buffer *Input, short Button)
{
    if(!Input)
        return 0;
    
    int IndexWithLag = ((Input->Index - Input->InputLag) + BUFFER_MAX - 1) % BUFFER_MAX;
    
    int decrement_address_one = (IndexWithLag + BUFFER_MAX - 1) % BUFFER_MAX;
    int decrement_address_two = (IndexWithLag + BUFFER_MAX - 2) % BUFFER_MAX;
    
    short Current = Input->InputBuffer[decrement_address_one] & Button;
    short Previous = Input->InputBuffer[decrement_address_two] & Button;
    
    
    return (Current && !Previous);
}


int IsButton(b50_input_buffer *Input, short Button)
{
    if(!Input)
        return 0;
    
    int IndexWithLag = ((Input->Index - Input->InputLag) + BUFFER_MAX - 1) % BUFFER_MAX;
    
    if(IndexWithLag < 0)
        InvalidCodePath; 
    
    int decrement_address_one = (IndexWithLag + BUFFER_MAX - 1) % BUFFER_MAX;
    int decrement_address_two = (IndexWithLag + BUFFER_MAX - 2) % BUFFER_MAX;
    
    short Current = Input->InputBuffer[decrement_address_one] & Button;
    short Previous = Input->InputBuffer[decrement_address_two] & Button;
    
    return (Current && Previous);
}


int IsButtonUp(b50_input_buffer *Input, short Button)
{
    if(!Input)
        return 0;
    
    int IndexWithLag = ((Input->Index - Input->InputLag) + BUFFER_MAX - 1) % BUFFER_MAX;
    
    int decrement_address_one = (IndexWithLag + BUFFER_MAX - 1) % BUFFER_MAX;
    int decrement_address_two = (IndexWithLag + BUFFER_MAX - 2) % BUFFER_MAX;
    
    short Current = Input->InputBuffer[decrement_address_one] & Button;
    short Previous = Input->InputBuffer[decrement_address_two] & Button;
    
    return (!Current && Previous);
}


#endif //B50_INPUT_H
