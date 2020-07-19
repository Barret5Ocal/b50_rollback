/* date = July 12th 2020 8:21 am */

#ifndef B50_MEMORY_H
#define B50_MEMORY_H

#define Kilobyte(Value) 1024 * Value
#define Megabyte(Value) 1024 * Kilobyte(Value)
#define Gigabyte(Value) 1024 * Megabyte(Value)
#define Terabyte(Value) 1024 * Gigabyte(Value)

struct b50_memory_space
{
    unsigned char *Space;
    size_t Size;
    size_t Used;
};

void B50InitMemorySpace(b50_memory_space *Space, size_t MemorySize)
{
    Space->Space = (unsigned char *)VirtualAlloc(0,
                                                 MemorySize,
                                                 MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    
    Space->Size = MemorySize;
    Space->Used = 0;
}

b50_memory_space B50CreateSubSpace(b50_memory_space *Space, size_t MemorySize)
{
    b50_memory_space NewSpace = {};
    
    NewSpace.Size = MemorySize;
    NewSpace.Used = 0;
    
    NewSpace.Space = Space->Space + Space->Used; 
    
    Space->Used += MemorySize; 
    
    return NewSpace;
}

void *B50AllocateMemory(b50_memory_space *Space, size_t MemorySize)
{
    void *Result;
    
    Result = Space->Space + Space->Used; 
    
    Space->Used += MemorySize; 
    
    return Result; 
}

#endif //B50_MEMORY_H
