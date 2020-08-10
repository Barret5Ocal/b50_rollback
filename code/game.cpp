// NOTE(Barret5Ocal): Game Design

/*
- real time 
- 2 player
 - deterministic 
- no graphics (only shapes)

- Hitbox Fighter
- characters are only hit boxes 
- instant kills 
- 2 or 2 moves
- only one character
*/

struct gamestate
{
    
    b50_input_buffer *Input1;
    v2 Player1Position;
    rect2 Player1Hitbox[4];
    int Player1LeftSide;
    
    b50_input_buffer *Input2;
    v2 Player2Position;
    rect2 Player2Hitbox[4];
    
    
    int GameStart;
};

void StartUp(gamestate *GameState)
{
    rect2 Model[4];
    Model[0] = {{0,0}, {15, 25}}; // body
    Model[1] = {{0,-200}, {8, 10}}; // head
    Model[2] = {{25,200}, {3, 10}}; // right leg
    Model[3] = {{-25,200}, {3, 10}}; // left leg
    
    GameState->Player1Position = {-640, 350};
    for(int32 i =0;
        i < 4;
        i++)
    {
        GameState->Player1Hitbox[i] = Model[i];
    }
    
    GameState->Player2Position = {640, 350};
    for(int32 i =0;
        i < 4;
        i++)
    {
        GameState->Player2Hitbox[i] = Model[i];
    }
    
    
    GameState->GameStart = false;
    
}

// TODO(Barret5Ocal): I need a way to associate contollers to players
/*
- have the player store a reference to the input buffer
- when a player has a remote to send inputs to, will assign them an input buffer and assign the other to the remote
- the first player will send a starting message to the remote an have them assign the controllers correctly
- after that we start sending the inputs
- network code should not have in the game layer 
- only responds to inputs


*/
void RunGame(gamestate *GameState, render_buffer *RenderBuffer, b50_input_buffer *DontUseExceptStart_Input1, b50_input_buffer *DontUseExceptStart_Input2)
{
    
    
    // NOTE(Barret5Ocal): start game message
    if(IsButtonDown(DontUseExceptStart_Input1, START) && GameState->GameStart == false)
    {
        GameState->Input1 = DontUseExceptStart_Input1;
        GameState->Input2 = DontUseExceptStart_Input2;
        
        GameState->Player1LeftSide = true;
        GameState->GameStart = true;
    }
    
    
    if (IsButtonDown(DontUseExceptStart_Input2, START) && GameState->GameStart == false)
    {
        GameState->Input1 = DontUseExceptStart_Input2;
        GameState->Input2 = DontUseExceptStart_Input1;
        
        GameState->Player1LeftSide = false;
        GameState->GameStart = true;
    }
    
    if(IsButton(GameState->Input1, DPAD_RIGHT))
        GameState->Player1Position.x += 10.0f;
    if(IsButton(GameState->Input1, DPAD_LEFT))
        GameState->Player1Position.x -= 10.0f;
    
    if(IsButton(GameState->Input2, DPAD_RIGHT))
        GameState->Player2Position.x += 10.0f;
    if(IsButton(GameState->Input2, DPAD_LEFT))
        GameState->Player2Position.x -= 10.0f;
    
    for(int32 i = 0;
        i < 4; 
        i++)
    {
        v2 Pos = GameState->Player1Position;
        rect2 Rect = GameState->Player1Hitbox[i];
        CardRenderEntry(RenderBuffer, {Pos.x + Rect.pos.x, Pos.y + Rect.pos.y, 0} , gb_quat_identity(), {Rect.dim.x, Rect.dim.y, 1}, {0.0f, 1.0f, 0.0f, 1.0f});
    }
    
    for(int32 i = 0;
        i < 4; 
        i++)
    {
        v2 Pos = GameState->Player2Position;
        rect2 Rect = GameState->Player2Hitbox[i];
        CardRenderEntry(RenderBuffer, {Pos.x + Rect.pos.x, Pos.y + Rect.pos.y, 0} , gb_quat_identity(), {Rect.dim.x, Rect.dim.y, 1}, {0.0f, 1.0f, 0.0f, 1.0f});
    }
    
}