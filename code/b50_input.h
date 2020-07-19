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
};

void Capture(b50_input_buffer *Buffer, int Player)
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


int IsButtonDown(b50_input_buffer *Input, short Button)
{
    int decrement_address_one = (Input->Index + BUFFER_MAX - 1) % BUFFER_MAX;
    int decrement_address_two = (Input->Index + BUFFER_MAX - 2) % BUFFER_MAX;
    
    short Current = Input->InputBuffer[decrement_address_one] & Button;
    short Previous = Input->InputBuffer[decrement_address_two] & Button;
    
    return (Current && !Previous);
}


int IsButton(b50_input_buffer *Input, short Button)
{
    int decrement_address_one = (Input->Index + BUFFER_MAX - 1) % BUFFER_MAX;
    int decrement_address_two = (Input->Index + BUFFER_MAX - 2) % BUFFER_MAX;
    
    short Current = Input->InputBuffer[decrement_address_one] & Button;
    short Previous = Input->InputBuffer[decrement_address_two] & Button;
    
    return (Current && Previous);
}


int IsButtonUp(b50_input_buffer *Input, short Button)
{
    int decrement_address_one = (Input->Index + BUFFER_MAX - 1) % BUFFER_MAX;
    int decrement_address_two = (Input->Index + BUFFER_MAX - 2) % BUFFER_MAX;
    
    short Current = Input->InputBuffer[decrement_address_one] & Button;
    short Previous = Input->InputBuffer[decrement_address_two] & Button;
    
    return (!Current && Previous);
}


#endif //B50_INPUT_H
