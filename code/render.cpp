void LoadCardVAO(GLuint *OutVBO, GLuint *OutVAO, GLuint *OutEBO, v3 Scale)
{
    
    float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    
    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 
    
    *OutVBO = VBO;
    *OutVAO = VAO;
    *OutEBO = EBO;
    
} 

void UnLoadCardVAO(GLuint VBO, GLuint VAO, GLuint EBO)
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
}

void RenderCard(render_entry *Entry, GLuint  ShaderProgram)
{
    unsigned int VBO, VAO, EBO;
    LoadCardVAO(&VBO, &VAO, &EBO, Entry->Scale);
    
    gbMat4 Ortho; 
    gb_mat4_ortho2d(&Ortho, -ScreenWidth, ScreenWidth, ScreenHeight, -ScreenHeight);
    
    gbMat4 Scale; 
    gb_mat4_scale(&Scale, {10 * Entry->Scale.x, 10 * Entry->Scale.y, 10});
    
    
    gbMat4 Translate = {}; 
    gb_mat4_translate(&Translate, {Entry->Position.x, Entry->Position.y, 0});
    
    // NOTE(Barret5Ocal): you need to order the multiplies correctly. matices are not "commutative"
    gbMat4 Transform = Ortho * Translate * Scale;
    
    glUseProgram(ShaderProgram);
    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do 
    
    unsigned int transformLoc = glGetUniformLocation(ShaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, Transform.e);
    
    int vertexColorLocation = glGetUniformLocation(ShaderProgram, "ourColor");
    glUniform4f(vertexColorLocation, 0.0f, 1.0f, 0.0f, 1.0f);
    
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    // glBindVertexArray(0); // no need to unbind it every time 
    
    
    UnLoadCardVAO(VBO, VAO, EBO);
}

void RenderClear(render_entry *Entry)
{
    v4 Color = Entry->Color;
    glClearColor(Color.r, Color.g, Color.b, Color.a);
    glClear(GL_COLOR_BUFFER_BIT);
    
}


void EndFrame(HWND Window, int Width, int Height)
{
    
    HDC WindowDC = GetDC(Window);
    
    glViewport(0, 0, Width, Height);
    SwapBuffers(WindowDC);
    
    ReleaseDC(Window, WindowDC);
    
}