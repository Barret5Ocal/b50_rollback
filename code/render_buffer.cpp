
void CardRenderEntry(render_buffer *Buffer, v3 Position, quaternion Rotation, v3 Scale, v4 Color)
{
    render_entry *Entry = Buffer->Entries + Buffer->Count++;
    
    Entry->Type = CARD;
    Entry->Position = Position;
    Entry->Rotation = Rotation;
    Entry->Scale = Scale;
    Entry->Color = Color;
}

void ClearRenderEntry(render_buffer *Buffer, v4 Color)
{
    render_entry *Entry = Buffer->Entries + Buffer->Count++;
    
    Entry->Type = CLEAR;
    Entry->Color = Color;
}

void RunRenderBuffer(render_buffer *Buffer, uint32 ShaderProgram)
{
    for(int i = 0;
        i < Buffer->Count;
        i++)
    {
        render_entry *Entry = Buffer->Entries + i; 
        switch(Entry->Type)
        {
            case CLEAR:
            {
                RenderClear(Entry);
            }break;
            case CARD:
            {
                RenderCard(Entry, ShaderProgram);
            }break; 
        }
    }
    
    Buffer->Count = 0;
}