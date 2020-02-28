//#include <windows.h>
#include <winsock2.h>
#include <Ws2ipdef.h>
#include <Ws2tcpip.h>
#include <thread>
#include <stdio.h>


static int Running = 1; 

#include "b50_timing.h"

#include <float.h>

#define GB_MATH_IMPLEMENTATION
#include "gb_math.h"
typedef gbVec2 v2;
typedef gbVec3 v3;
typedef gbVec4 v4;
typedef gbMat4 m4;
typedef gbQuat quaternion;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#include "stb_image_write.h"

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

#include <stdint.h>
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float r32;
typedef double r64;

#define global_variable static 
#define local_persist static 
#define internal static 

#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}
#define InvalidCodePath Assert(false)
#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define Kilobyte(Value) 1024 * Value
#define Megabyte(Value) 1024 * Kilobyte(Value)
#define Gigabyte(Value) 1024 * Megabyte(Value)
#define Terabyte(Value) 1024 * Gigabyte(Value)


#define ScreenWidth 1280
#define ScreenHeight 720

#include "win32_opengl.h"

#define STB_RECT_PACK_IMPLEMENTATION
#define STBRP_LARGE_RECTS
#include "stb_rect_pack.h"


struct win32_windowdim 
{
    int Width, Height; 
    int x, y;
    //int DisplayWidth, DisplayHeight; 
};

#if IMGUI
#include "imgui.cpp"
#include "imgui.h"
#include "imgui_demo.cpp"
#include "imgui_draw.cpp"
#include "imgui_internal.h"
#include "imconfig.h"
#include "imgui_impl_opengl3.cpp"
#include "imgui_interface.cpp"
#else 
#define STB_TRUETYPE_IMPLEMENTATION
#include "include\stb_truetype.h"

#endif

#include "network.cpp"

int spacebar = 0;


win32_windowdim Win32GetWindowDim(HWND Window)
{
    win32_windowdim Dim = {};
    
    RECT Rect = {};
    //GetClientRect(Window, &Rect);
    GetWindowRect(Window, &Rect);
    Dim.x = Rect.left;
    Dim.y = Rect.top;
    Dim.Width = Rect.right - Rect.left;
    Dim.Height = Rect.bottom - Rect.top;
    return Dim; 
}

GLuint CreateShaderProgram(char *VertCode, int VertSize, char *FragCode, int FragSize)
{
    GLuint ShaderProgram = glCreateProgram();
    GLuint VertexShaderObj = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
    
    glShaderSource(VertexShaderObj, 1, &VertCode, &VertSize);
    glShaderSource(FragShaderObj, 1, &FragCode, &FragSize);
    
    glCompileShader(VertexShaderObj);
    glCompileShader(FragShaderObj);
    
    GLint success;
    glGetShaderiv(VertexShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        char Buffer[1024];
        GLchar InfoLog[1024];
        glGetShaderInfoLog(VertexShaderObj, sizeof(InfoLog), NULL, InfoLog);
        stbsp_sprintf(Buffer , "Error compiling shader type %d: '%s'\n", GL_VERTEX_SHADER, InfoLog);
        OutputDebugStringA(Buffer);
        InvalidCodePath; 
    }
    
    glGetShaderiv(FragShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        char Buffer[1024];
        GLchar InfoLog[1024];
        glGetShaderInfoLog(FragShaderObj, sizeof(InfoLog), NULL, InfoLog);
        stbsp_sprintf(Buffer , "Error compiling shader type %d: '%s'\n", GL_FRAGMENT_SHADER, InfoLog);
        OutputDebugStringA(Buffer);
        InvalidCodePath; 
    }
    
    glAttachShader(ShaderProgram, VertexShaderObj);
    glAttachShader(ShaderProgram, FragShaderObj);
    
    glLinkProgram(ShaderProgram);
    
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
    if (success == 0) {
        char Buffer[1024];
        GLchar ErrorLog[1024];
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        stbsp_sprintf(Buffer, "Error linking shader program: '%s'\n", ErrorLog);
        OutputDebugStringA(Buffer);
        InvalidCodePath; 
    }
    
    return ShaderProgram; 
}

struct read_results
{
    char *Memory; 
    int32 Size; 
};

read_results Win32GetFileContents(char *Filename)
{
    read_results Result = {}; 
    
    HANDLE FileHandle = CreateFileA(Filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(FileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER FileSize;
        if(GetFileSizeEx(FileHandle, &FileSize))
        {
            uint32 FileSize32 = (uint32)FileSize.QuadPart;
            Result.Memory = (char *)VirtualAlloc(0, FileSize32, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
            if(Result.Memory)
            {
                DWORD BytesRead;
                if(ReadFile(FileHandle, Result.Memory, FileSize32, &BytesRead, 0) &&
                   (FileSize32 == BytesRead))
                {
                    Result.Size = FileSize32;
                }
                else 
                {
                    VirtualFree(Result.Memory, 0, MEM_RELEASE);
                    
                }
            }
        }
        CloseHandle(FileHandle);
    }
    return Result; 
}

int LeftMouse = 0;

LRESULT CALLBACK
MainWindowProc(HWND Window,
               UINT Message,
               WPARAM WParam,
               LPARAM LParam)
{
    LRESULT Result = {};
    
    switch(Message)
    {
        case WM_DESTROY:
        {
            Running = 0;
        }break;
        case WM_CLOSE:
        {
            Running = 0;
        }break;
        case WM_KEYDOWN:
        {
#if 1
            unsigned short c = MapVirtualKeyW(WParam, MAPVK_VK_TO_CHAR);
            ImGuiIO& io = ImGui::GetIO();
            io.AddInputCharacter(c);
#endif
            
        }break;
        
        case WM_LBUTTONDOWN: 
        {
            LeftMouse = 1;
        }break;
        case WM_LBUTTONUP: 
        {
            LeftMouse = 0;
        }break;
        default:
        {
            Result = DefWindowProc(Window, Message, WParam, LParam);
        }break;
    }
    
    return Result; 
}

DWORD WINAPI 
ThreadProc(LPVOID lpParameter)
{
    char *Param = (char *)lpParameter;
    OutputDebugStringA(Param);
    return 0;
}

int WinMain(HINSTANCE Instance, 
            HINSTANCE PrevInstance,
            LPSTR CmdLine,
            int ShowCode)
{
    
    WNDCLASS WindowClass = {};
    WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    WindowClass.lpfnWndProc = MainWindowProc;
    WindowClass.hInstance = Instance;
    WindowClass.lpszClassName = "OpenGlPratice";
    WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    
    if(RegisterClass(&WindowClass))
    {
        HWND Window = CreateWindowEx(0, 
                                     WindowClass.lpszClassName,
                                     "OpenGL Pratice",
                                     WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     ScreenWidth,
                                     ScreenHeight,
                                     0,
                                     0,
                                     Instance,
                                     0);
        
        
        Win32InitOpenGL(Window);
        
        win32_windowdim Dim = Win32GetWindowDim(Window);
        
        StartUpNetwork();
        
        
        //if(connect(sockfd, res->ai_addr, res->ai_addrlen) == -1)
        //return 0;
        imguisetup();
        
        
        ui_data UIData = {};
        //char *port = (char *)malloc(sizeof(char) * 4);
        //char *RemotePort = (char *)malloc(sizeof(char) * 4);
        
        char port[5] = {};
        char RemotePort[5] = {};
        char Message[51] = {}; 
        
        
        UIData.port = port;
        UIData.RemotePort = RemotePort;
        UIData.Message = Message; 
        
        //UIData.ConnectPressed = false;
        //UIData.GetSocketPressed = false;
        
        network_data NData = {};
        
        char *Param = "Thread";
        DWORD ThreadID;
        HANDLE ThreadHandle = CreateThread(0, 0, ThreadProc, Param, 0, &ThreadID);
        
        time_info TimeInfo = {};
        while(RunLoop(&TimeInfo, 60))
        {
            MSG Message;
            while(PeekMessage(&Message, Window, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&Message);
                DispatchMessage(&Message);
            }
            
            PBYTE Keyboard[256] = {};
            if(GetKeyboardState(Keyboard[0]))
                InvalidCodePath;
            
            ImGuiIO& io = ImGui::GetIO();
            for(int i = 0;
                i < 256;
                i++)
                io.KeysDown[i] = Keyboard[i];
            
            
            
            if(UIData.GetSocketPressed)
            {
                UIData.GetSocketPressed = false;
                
                char print1[50] = {};
                char print2[50] = {};
                
                char *PrintOut[2] = {print1, print2};
                NData.SendSocket = CreateSocket(atoi(UIData.port), &UIData.Console);
                NData.ReceiveSocket = CreateSocket(atoi(UIData.port), &UIData.Console);
                //CreateSocket(&NData.Socket, NData.port, &UIData.Console);
            }
            
            if(UIData.GetRemotePressed)
            {
                UIData.GetRemotePressed = false;
                
                FindConnection(&NData.Remote, UIData.RemotePort);
                
                char PrintOut[25] = {};
                sprintf(PrintOut, "remote port #: %s\n", UIData.RemotePort);
                UIData.Console.AddLog(PrintOut);
                
            }
            
            
            if(UIData.SendPressed)
            {
                UIData.SendPressed = false;
                
                SendMessage(UIData.Message, NData.SendSocket, &NData.Remote);
                
                char PrintOut[100] = {};
                sprintf(PrintOut, "Sending Message: %s\n", UIData.Message);
                UIData.Console.AddLog(PrintOut);
            }
            
            char print1[25] = {};
            char print2[25] = {};
            char print3[100] = {};
            
            char *PrintOut[3] = {print1, print2, print3};
            int yes = Recieve(NData.ReceiveSocket, PrintOut);
            
            if(yes)
            {
                for(int i = 0;
                    i < ArrayCount(PrintOut);
                    i++)
                    UIData.Console.AddLog(PrintOut[i]);
            }
            
            
            // TODO(Barret5Ocal): might need to switch back to windows to get inputs without the need to stop the program
            Dim = Win32GetWindowDim(Window);
            
            glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            
            
            imguistuff(Dim, &UIData, LeftMouse);
            
            
            
            imguirender();
            
            HDC WindowDC = GetDC(Window);
            
            glViewport(0, 0, ScreenWidth, ScreenHeight);
            SwapBuffers(WindowDC);
            
            ReleaseDC(Window, WindowDC);
            
        }
        
        
        
        EndDownNetwork();
        
    }
    
    return 0; 
}