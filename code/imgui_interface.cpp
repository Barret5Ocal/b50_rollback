static char* Strdup(const char *str)
{
    size_t len = strlen(str) + 1; void* buff = malloc(len); return (char*)memcpy(buff, (const void*)str, len);
}

struct ui_console
{
    ImVector<char*>       Items;
    bool                  ScrollToBottom;
    ImVector<char*>       History;
    int                   HistoryPos;
    
    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        // FIXME-OPT
        char buf[1024];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
        buf[IM_ARRAYSIZE(buf)-1] = 0;
        va_end(args);
        Items.push_back(Strdup(buf));
        ScrollToBottom = true;
    }
    
    void    ClearLog()
    {
        for (int i = 0; i < Items.Size; i++)
            free(Items[i]);
        Items.clear();
        ScrollToBottom = true;
    }
    
    
    void    Draw(const char* title, bool* p_open)
    {
        ImGui::SetNextWindowSize(ImVec2(520,600), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin(title, p_open))
        {
            ImGui::End();
            return;
        }
        
        // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar. So e.g. IsItemHovered() will return true when hovering the title bar.
        // Here we create a context menu only available from the title bar.
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Close"))
                *p_open = false;
            ImGui::EndPopup();
        }
        
        
        // TODO: display items starting from the bottom
        
        
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
        ImGui::PopStyleVar();
        ImGui::Separator();
        
        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::Selectable("Clear")) ClearLog();
            ImGui::EndPopup();
        }
        
        // Display every line as a separate entry so we can change their color or add custom widgets. If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
        // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping to only process visible items.
        // You can seek and display only the lines that are visible using the ImGuiListClipper helper, if your elements are evenly spaced and you have cheap random access to the elements.
        // To use the clipper we could replace the 'for (int i = 0; i < Items.Size; i++)' loop with:
        //     ImGuiListClipper clipper(Items.Size);
        //     while (clipper.Step())
        //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        // However, note that you can not use this code as is if a filter is active because it breaks the 'cheap random-access' property. We would need random-access on the post-filtered list.
        // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices that passed the filtering test, recomputing this array when user changes the filter,
        // and appending newly elements as they are inserted. This is left as a task to the user until we can manage to improve this example code!
        // If your items are of variable size you may want to implement code similar to what ImGuiListClipper does. Or split your data into fixed height items to allow random-seeking into your list.
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4,1)); // Tighten spacing
        ImVec4 col_default_text = ImGui::GetStyleColorVec4(ImGuiCol_Text);
        for (int i = 0; i < Items.Size; i++)
        {
            const char* item = Items[i];
            ImVec4 col = col_default_text;
            if (strstr(item, "[error]")) col = ImColor(1.0f,0.4f,0.4f,1.0f);
            else if (strncmp(item, "# ", 2) == 0) col = ImColor(1.0f,0.78f,0.58f,1.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, col);
            ImGui::TextUnformatted(item);
            ImGui::PopStyleColor();
        }
        if (ScrollToBottom)
            ImGui::SetScrollHere(1.0f);
        ScrollToBottom = false;
        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::Separator();
        
        // Command-line
        bool reclaim_focus = false;
        
        // Demonstrate keeping focus on the input box
        ImGui::SetItemDefaultFocus();
        if (reclaim_focus)
            ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
        
        ImGui::End();
    }
    
};

struct ui_data
{
    
    bool ConsoleOpen;
    ui_console Console;
    
    
    bool SetupWindow = true;
    int SetupPressed = false;
    
    bool SendWindow = true;
    int SendPressed = false;
    
    char *Sport;
    char *Rport;
    
    char *Message;
};





void imguisetup()
{
    
#if IMGUI
    ImGui::CreateContext();
    
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    io.DisplaySize = {(float)ScreenWidth, (float)ScreenHeight};
    float FrameRate = 60;
    float dt = 1 / FrameRate; 
    io.DeltaTime = dt;
    //io.WantCaptureKeyboard = true;
    
    
    
    ImGui_ImplOpenGL3_Init(0);
    ImGui::StyleColorsDark();
    bool show_demo_window = true;
#endif 
    
}

void imguistuff(win32_windowdim Dim, ui_data *Data, int LeftMouse)
{
#if IMGUI
    POINT Point = {}; 
    GetCursorPos(&Point);
    ImGuiIO& io = ImGui::GetIO();
    
    io.MousePos = {(float)Point.x - Dim.x - 5, (float)Point.y - Dim.y + 5};
    
    //io.MouseDown[0] = 0;
    io.MouseDown[0] = LeftMouse;
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    
    //ImGui::PushAllowKeyboardFocus(true);
    io.WantCaptureKeyboard = true;
    io.WantTextInput = true;
    
    
    
    if(Data->SetupWindow)
    {
        ImGui::Begin("Socket", &Data->SetupWindow,  ImGuiWindowFlags_None);
        ImGui::InputText("Sport", Data->Sport, sizeof(char) * 5);
        ImGui::InputText("Rport", Data->Rport, sizeof(char) * 5);
        
        if (ImGui::Button("Get Socket"))
        {
            Data->SetupPressed = true;
            
        }
        ImGui::End();
    }
    
    if(Data->SendWindow)
    {
        ImGui::Begin("Send Box", &Data->SendWindow, ImGuiWindowFlags_None);
        ImGui::InputText("message input##text3", Data->Message, sizeof(char) * 50);
        
        if (ImGui::Button("Send Message"))
        {
            Data->SendPressed = true;
        }
        
        ImGui::End();
    }
    
    Data->Console.Draw("Console", &Data->ConsoleOpen);
    
    ImGui::Render();
#endif 
    
}


void imguirender()
{
#if IMGUI
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif 
}