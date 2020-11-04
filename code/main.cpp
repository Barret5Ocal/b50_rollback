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

typedef gbRect2 rect2;

#define global_variable static 
#define local_persist static 
#define internal static 

#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}
#define InvalidCodePath Assert(false)
#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#include "b50_memory.h"

#define ScreenWidth 1280
#define ScreenHeight 720

#include "win32_opengl.h"

#define STB_RECT_PACK_IMPLEMENTATION
#define STBRP_LARGE_RECTS
#include "stb_rect_pack.h"

#include "b50_input.h"

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

#include "render_buffer.h"

#include "network.cpp"

#include "b50_timing.h"

#include "game.cpp"

#include "render.cpp"

#include "render_buffer.cpp"

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
        case WM_SYSKEYDOWN:
        {
            ImGuiIO& io = ImGui::GetIO();
            if (WParam < 256)
                io.KeysDown[WParam] = 1;
            
        }break;
        
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            ImGuiIO& io = ImGui::GetIO();
            if (WParam < 256)
                io.KeysDown[WParam] = 0;
        }break;
        
        case WM_CHAR:
        {
            ImGuiIO& io = ImGui::GetIO();
            if (WParam > 0 && WParam < 0x10000)
                io.AddInputCharacter((unsigned short)WParam);
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
        
        char *VertexShaderSource = "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "uniform mat4 transform;\n"
            "void main()\n"
            "{\n"
            "   gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
            "}\0";
        
        int VertexShaderSourceLength = strlen(VertexShaderSource);
        char *FragmentShaderSource = "#version 330 core\n"
            "out vec4 FragColor;\n"
            "uniform vec4 ourColor;\n"
            "void main()\n"
            "{\n"
            "   FragColor = ourColor;\n"
            "}\n\0";
        int FragmentShaderSourceLength = strlen(FragmentShaderSource); 
        
        GLuint ShaderProgram = CreateShaderProgram(VertexShaderSource, VertexShaderSourceLength, FragmentShaderSource, FragmentShaderSourceLength);
        
        
        
        // TODO(Barret5Ocal): should left and top be negative or 0
        
        win32_windowdim Dim = Win32GetWindowDim(Window);
        
        imguisetup();
        
        network_data Data = {};
        
        
        char port[5] = {"22"};
        char RemotePort[5] = {"5200"};
        char NetworkMessage[51] = {}; 
        char LocalIP[56] = {"68.98.76.230"};
        //char RemoteIP[56] = .{"68.98.76.230"};
        char RemoteIP[56] = {"127.0.0.1"};
        //char RemoteIP[56] = {"192.168.1.3"};
        
        //int Sindex = 0;
        //char Sbuff[255] = {};
        
        char Rbuff[1024];
        
        b50_memory_space MainSpace = {};
        B50InitMemorySpace(&MainSpace, Megabyte(500));
        
        render_buffer *RenderBuffer = (render_buffer *)B50AllocateMemory(&MainSpace, sizeof(render_buffer));
        RenderBuffer->Entries = (render_entry *)B50AllocateMemory(&MainSpace, sizeof(render_entry) * 100);
        
        //LPOVERLAPPED *Overlapped = (LPOVERLAPPED *)malloc(sizeof(LPOVERLAPPED));
        b50_input_buffer Input1 = {};
        b50_input_buffer Input2 = {};
        
        
        v3 Position = {0, 0, 0};
        
        gamestate *GameState = (gamestate *)B50AllocateMemory(&MainSpace, sizeof(gamestate));
        
        StartUp(GameState);
        
        
        time_info TimeInfo = {};
        while(RunLoop(&TimeInfo, 60))
        {
            MSG Message;
            while(PeekMessage(&Message, Window, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&Message);
                DispatchMessage(&Message);
            }
            
            
            Dim = Win32GetWindowDim(Window);
            
            CaptureXboxController(&Input1, 0);
            CaptureKeyboard(&Input2);
            
            ImGuiIO& io = ImGui::GetIO();
            
            
            POINT Point = {}; 
            GetCursorPos(&Point);
            
            io.MousePos = {(float)Point.x - Dim.x - 5, (float)Point.y - Dim.y + 5};
            
            //io.MouseDown[0] = 0;
            io.MouseDown[0] = LeftMouse;
            
            ImGui_ImplOpenGL3_NewFrame();
            ImGui::NewFrame();
            
            //ImGui::PushAllowKeyboardFocus(true);
            io.WantCaptureKeyboard = true;
            io.WantTextInput = true;
            
            
            
            ClearRenderEntry(RenderBuffer, {0.1f, 0.1f, 0.3f, 1.0f});
            
            RunGame(GameState, RenderBuffer, &Input1, &Input2);
            
            
            
            local_persist bool Setup = true;
            {
                if(ImGui::Begin("Socket", &Setup,  ImGuiWindowFlags_None))
                    StartUpNetwork();
                
                ImGui::Button("StartUp");
                ImGui::InputText("Sport", port, sizeof(char) * 5, ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank);
                ImGui::InputText("Rport", RemotePort, sizeof(char) * 5, ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank);
                
                
                //ImGui::InputText("Local IP", LocalIP, ArrayCount(LocalIP), ImGuiInputTextFlags_CharsNoBlank);
                
                ImGui::InputText("Remote IP", RemoteIP, ArrayCount(RemoteIP), ImGuiInputTextFlags_CharsNoBlank);
                
                
                if (ImGui::Button("Get Socket"))
                {
                    //SetSocket(&Data, LocalIP, RemoteIP);
                    NetworkSetup(&Data, RemoteIP, atoi(port), atoi(RemotePort) );
                    
                }
                
                if(ImGui::Button("Get Address"))
                    GetAddress(&Data, RemoteIP);
                ImGui::End();
            }
            
            
            local_persist bool SendWindow = true;
            {
                ImGui::Begin("Send Box", &SendWindow, ImGuiWindowFlags_None);
                ImGui::InputText("message input##text3", NetworkMessage, sizeof(char) * 50);
                
                if (ImGui::Button("Send Message"))
                {
                    
                    Send(NetworkMessage, &Data);
                    //SendMessage(UIData.Message, NData.SendSocket, &NData.Remote);
                    
                    char PrintOut[100] = {};
                    sprintf(PrintOut, "Sending Message: %s\n", NetworkMessage);
                    Console.AddLog(PrintOut);
                    
                }
                
                ImGui::End();
            }
            
#if IOCP
            if(Data.Master)
            {
#else
#endif
                if(int bytesIn = Recieve(Rbuff, &Data))
                {
                    if(bytesIn == SOCKET_ERROR)
                    {
                        Console.AddLog("Error receiving from client %d\n", WSAGetLastError());
                        //printf("Error receiving from client %d\n", WSAGetLastError());
                        continue;
                    }
                    
                    char clientIP[256];
                    ZeroMemory(clientIP, 256);
                    
                    inet_ntop(AF_INET, &Data.Client.sin_addr, clientIP, 256);
                    
                    Console.AddLog("Message recv from %s : %s\n", clientIP, Rbuff);
                    //printf("Message recv from %s : %s\n", clientIP, Rbuff);
                    
                }
#if IOCP
            }
#else 
#endif
            
            
            // NOTE(Barret5Ocal): The stutter in the animation came from the display settings on my computer being set to 59hz the problem was fixed when I set the display setting to 154hz
            RunRenderBuffer(RenderBuffer, ShaderProgram);
            
            local_persist bool ConsoleOpen = true;
            Console.Draw("Console", &ConsoleOpen);
            
            ImGui::Render();
            
            imguirender();
            
            EndFrame(Window, Dim.Width, Dim.Height);
            
        }
        
        glDeleteProgram(ShaderProgram);
        
        closesocket(Data.out);
        closesocket(Data.in);
    }
    
    WSACleanup();
    
    return 0;
}
