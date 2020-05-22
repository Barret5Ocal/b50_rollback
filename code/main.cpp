#include <stdio.h>
#include <conio.h>
#include "WS2tcpip.h"


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
#include "stb_truetype.h"

#endif

#include "network.cpp"


#include "b50_timing.h"

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
            b50_Running = 0;
        }break;
        case WM_CLOSE:
        {
            b50_Running = 0;
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

//int main(int argc, char *argv[])
int WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR     CmdLine, int       ShowCmd)
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
        
        imguisetup();
        
        network_data Data = {};
        
        
        
        
        
        ui_data UIData = {};
        //char *port = (char *)malloc(sizeof(char) * 4);
        //char *RemotePort = (char *)malloc(sizeof(char) * 4);
        
        char port[5] = {};
        char RemotePort[5] = {};
        char Message[51] = {}; 
        
        
        UIData.Sport = port;
        UIData.Rport = RemotePort;
        UIData.Message = Message; 
        
        int Sindex = 0;
        char Sbuff[255] = {};
        
        char Rbuff[1024];
        
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
            
            
            
            if(UIData.SetupPressed)
            {
                UIData.SetupPressed = false;
                
                Setup(&Data, atoi(UIData.Sport), atoi(UIData.Rport), &UIData);
                
                
                //NData.SendSocket = CreateSocket(atoi(UIData.port), &UIData.Console);
                //NData.ReceiveSocket = CreateSocket(atoi(UIData.port), &UIData.Console);
                //CreateSocket(&NData.Socket, NData.port, &UIData.Console);
            }
            
            
            if(UIData.SendPressed)
            {
                UIData.SendPressed = false;
                
                Send(UIData.Message, &Data, &UIData);
                //SendMessage(UIData.Message, NData.SendSocket, &NData.Remote);
                
                char PrintOut[100] = {};
                sprintf(PrintOut, "Sending Message: %s\n", UIData.Message);
                UIData.Console.AddLog(PrintOut);
            }
            
            //int yes = Recieve(NData.ReceiveSocket, PrintOut);
            if(int bytesIn = Recieve(Rbuff, &Data))
            {
                if(bytesIn == SOCKET_ERROR)
                {
                    UIData.Console.AddLog("Error receiving from client %d\n", WSAGetLastError());
                    //printf("Error receiving from client %d\n", WSAGetLastError());
                    continue;
                }
                
                char clientIP[256];
                ZeroMemory(clientIP, 256);
                
                inet_ntop(AF_INET, &Data.Client.sin_addr, clientIP, 256);
                
                UIData.Console.AddLog("Message recv from %s : %s\n", clientIP, Rbuff);
                //printf("Message recv from %s : %s\n", clientIP, Rbuff);
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
            
            
#if 0
            char s = 0;
            if ( _kbhit() )
            {
                s = _getch();
                _putch(s);
                Sbuff[Sindex++] = s; 
            }
            
            if(s == '\r')
            {
                _putch('\n');
                Sbuff[Sindex] = '\0';
                Sindex = 0;
                
                Send(Sindex, Sbuff, &Data);
            }
            
            if(int bytesIn = Recieve(Rbuff, &Data))
            {
                if(bytesIn == SOCKET_ERROR)
                {
                    printf("Error receiving from client %d\n", WSAGetLastError());
                    continue;
                }
                
                char clientIP[256];
                ZeroMemory(clientIP, 256);
                
                inet_ntop(AF_INET, &Data.Client.sin_addr, clientIP, 256);
                
                printf("Message recv from %s : %s\n", clientIP, Rbuff);
            }
#endif 
        }
        
        
        closesocket(Data.out);
        closesocket(Data.in);
        
    }
    
    WSACleanup();
    
    return 0;
}
