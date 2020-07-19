/* date = July 18th 2020 9:53 pm */

#ifndef RENDER_BUFFER_H
#define RENDER_BUFFER_H

enum RENDER_TYPE
{
    CLEAR,
    CARD
};

struct render_entry
{
    int Type;
    v3 Position;
    quaternion Rotation;
    v3 Scale;
    v4 Color;
};

struct render_buffer
{
    int Count;
    render_entry *Entries; 
};

void CardRenderEntry(render_buffer *Buffer, v3 Position, quaternion Rotation, v3 Scale, v4 Color);

void RunRenderBuffer(render_buffer *Buffer, uint32 ShaderProgram);

void ClearRenderEntry(render_buffer *Buffer, v4 Color);
#endif //RENDER_BUFFER_H
