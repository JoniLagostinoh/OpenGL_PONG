#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdlib>

 float WORLD_W = 800.0f;
 float WORLD_H = 600.0f;
 float Half_Weight = WORLD_W * 0.5f;   
 float Half_Height = WORLD_H * 0.5f;   

static const char* VertexShader = R"glsl(
#version 330 core

layout(location = 0) in vec2 aPos;
uniform vec2 uPos, uSize, uProjScale;


void main() {
    vec2 world = aPos * uSize + uPos;
    gl_Position = vec4(world * uProjScale, 0.0, 1.0);
}
)glsl";

static const char* FragmentShader = R"glsl(
#version 330 core

uniform vec3 uColor;
out vec4 FragColor;


void main() { FragColor = vec4(uColor, 1.0); }
)glsl";

struct Rectangle { float x = 0, y = 0, w = 0, h = 0; };
struct Paddle { Rectangle rectangle; float Velocity_y = 0; float speed = 520.0f; };
struct Ball   { Rectangle rectangle; float Velocity_x = 0, Velocity_y = 0; };



bool overlaps(const Rectangle& rectangle_A, const Rectangle& rectangle_B) {
    
    
    
    
    return std::abs(rectangle_A.x - rectangle_B.x) < (rectangle_A.w + rectangle_B.w) * 0.5f &&
        std::abs(rectangle_A.y - rectangle_B.y) < (rectangle_A.h + rectangle_B.h) * 0.5f;
       
    
    
    /*Old collisions
    * 
    if (rectangle_A.x > rectangle_B.x + rectangle_B.w) return false; // A's left edge is past B's right edge
    if (rectangle_A.x + rectangle_A.w < rectangle_B.x) return false; // A's right edge is behind B's left edge
    if (rectangle_A.y > rectangle_B.y + rectangle_B.h) return false; // A's top edge is past B's bottom edge
    if (rectangle_A.y + rectangle_A.h < rectangle_B.y) return false; // A's bottom edge is behind B's top edge

    return true; // If none are true, they're overlapping
     */
    
    
} //Box Collision



static GLuint compileShader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint sucess = 0; glGetShaderiv(s, GL_COMPILE_STATUS, &sucess);
    if (!sucess) { char log[1024]; glGetShaderInfoLog(s, 1024, nullptr, log);
               std::cerr << "Shader error:\n" << log << "\n"; }
    return s;
}

static GLuint makeProgram(const char* vs, const char* fs) {
    GLuint v = compileShader(GL_VERTEX_SHADER, vs);
    GLuint f = compileShader(GL_FRAGMENT_SHADER, fs);
    GLuint p = glCreateProgram();
    glAttachShader(p, v); glAttachShader(p, f); glLinkProgram(p);
    GLint success = 0; glGetProgramiv(p, GL_LINK_STATUS, &success);
    if (!success) { char log[1024]; glGetProgramInfoLog(p, 1024, nullptr, log);
               std::cerr << "Link error:\n" << log << "\n"; }
    glDeleteShader(v); glDeleteShader(f);
    return p;
}

//Ball Collision with Paddles
      
float ReverseSpeed(float& speed)
{
    return speed = -speed;
}


int main(int, char**) { //Setup stuff, DON'T TOUCH
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_Window* gameWindow = SDL_CreateWindow("Pong", static_cast<int>(WORLD_W), static_cast<int>(WORLD_H),
                                       SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext context = SDL_GL_CreateContext(gameWindow);
    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

    // Paddle Graphics data (both Paddles share the same data)
    float verts[] = { -0.5f,-0.5f,  0.5f,-0.5f,  0.5f,0.5f,  -0.5f,0.5f };
    unsigned int idx[] = { 0,1,2,  2,3,0 };

    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao); glBindVertexArray(vao);
    glGenBuffers(1, &vbo); glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glGenBuffers(1, &ebo); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

    GLuint prog = makeProgram(VertexShader, FragmentShader);
    GLint uPos   = glGetUniformLocation(prog, "uPos");
    GLint uSize  = glGetUniformLocation(prog, "uSize");
    GLint uColor = glGetUniformLocation(prog, "uColor");
    GLint uProj  = glGetUniformLocation(prog, "uProjScale");

    
    
    
    //We start here
    
    
    //Paddle Initialization
    float paddleSpawnOffset = 40.0f;
    
    float paddleSize = 100.0f;
    float paddleThickness = 20.0f;
    
    Paddle LeftPaddle { { -Half_Weight + paddleSpawnOffset, 0.0f, paddleThickness, paddleSize } };
    Paddle RightPaddle{ {  Half_Weight - paddleSpawnOffset, 0.0f, paddleThickness, paddleSize } };
    Ball ball { { 0.0f, 0.0f, 18.0f, 18.0f }, 420.0f, 180.0f };
    int scoreL = 0, scoreR = 0;

    
    
    auto resetBall = [&](float dirX) {
        ball.rectangle.x = 0.0f; ball.rectangle.y = 0.0f;
        ball.Velocity_x = 420.0f * dirX;
        ball.Velocity_y = ((std::rand() % 2) ? 1.0f : -1.0f) * 180.0f;
    };
    
    

    
    
    
    // Get prev frame for eltaTime calculations
    Uint64 freq = SDL_GetPerformanceFrequency();
    Uint64 prev_Frame = SDL_GetPerformanceCounter();
    
    
    
    bool running = true;
   //On Update
    while (running) {
        
        
        
        Uint64 current_Frame = SDL_GetPerformanceCounter();
        float dt = static_cast<float>(current_Frame - prev_Frame) / static_cast<float>(freq);
        prev_Frame = current_Frame;
        
        
       

        
        
        
        // Exit game via Esc key (SDL is much easier than openGL)
        SDL_Event exitGame;
        while (SDL_PollEvent(&exitGame))
        {
            if (exitGame.type == SDL_EVENT_QUIT) running = false;
            if (exitGame.type == SDL_EVENT_KEY_DOWN && exitGame.key.key == SDLK_ESCAPE) running = false;
        }

        //Input handling
        const bool* Key = SDL_GetKeyboardState(nullptr);
        LeftPaddle.Velocity_y  = ((Key[SDL_SCANCODE_W] ? 1.0f : 0.0f) - (Key[SDL_SCANCODE_S] ? 1.0f : 0.0f)) * LeftPaddle.speed;
        RightPaddle.Velocity_y = ((Key[SDL_SCANCODE_UP] ? 1.0f : 0.0f) - (Key[SDL_SCANCODE_DOWN] ? 1.0f : 0.0f)) * RightPaddle.speed;

        
        
        
        
        //Player Movement
        LeftPaddle.rectangle.y  += LeftPaddle.Velocity_y  * dt;
        RightPaddle.rectangle.y += RightPaddle.Velocity_y * dt;
        
        
        float L_MovementLimit = Half_Height - LeftPaddle.rectangle.h  * 0.5f;
        float R_MovementLimit = Half_Height - RightPaddle.rectangle.h * 0.5f;
        LeftPaddle.rectangle.y  = std::clamp(LeftPaddle.rectangle.y,  -L_MovementLimit, L_MovementLimit);
        RightPaddle.rectangle.y = std::clamp(RightPaddle.rectangle.y, -R_MovementLimit, R_MovementLimit);

        
        
        
        
        
        //Ball Movement
        ball.rectangle.x += ball.Velocity_x * dt;
        ball.rectangle.y += ball.Velocity_y * dt;

        
        //Fakign Ball collision with Screen
        if (ball.rectangle.y + ball.rectangle.h * 0.5f >  Half_Height)
        {
            ball.rectangle.y =  Half_Height - ball.rectangle.h * 0.5f;
            ReverseSpeed(ball.Velocity_y);
        }
        
        
        if (ball.rectangle.y - ball.rectangle.h * 0.5f < -Half_Height)
        {
            ball.rectangle.y = -Half_Height + ball.rectangle.h * 0.5f;
            ReverseSpeed(ball.Velocity_y);
        }

       
        
        //Handle Ball Collision with Players
        if (ball.Velocity_x < 0 && overlaps(ball.rectangle, LeftPaddle.rectangle)) //Left Paddle
        {
            ball.rectangle.x = LeftPaddle.rectangle.x + (LeftPaddle.rectangle.w + ball.rectangle.w) * 0.5f;
            ReverseSpeed(ball.Velocity_x);
            
        }
        
        
        if (ball.Velocity_x > 0 && overlaps(ball.rectangle, RightPaddle.rectangle)) //right Paddle
        {
            ball.rectangle.x = RightPaddle.rectangle.x - (RightPaddle.rectangle.w + ball.rectangle.w) * 0.5f;
            ReverseSpeed(ball.Velocity_x);
        }

        
        //Handle scoring
        //TODO: DISPLAY SCORE
        if (ball.rectangle.x - ball.rectangle.w * 0.5f >  Half_Weight)
        {
            scoreL++;
            resetBall(-1.0f);
        }
        if (ball.rectangle.x + ball.rectangle.w * 0.5f < -Half_Weight)
        {
            scoreR++;
            resetBall( 1.0f);
        }

        
        
        /*
        //Normalize coordinates
        int pixel_Weight, pixel_Height;
        SDL_GetWindowSizeInPixels(gameWindow, &pixel_Weight, &pixel_Height);
        float target = WORLD_W / WORLD_H;
        float actual = (pixel_Height > 0) ? (float)pixel_Weight / (float)pixel_Height : target;
        
        
        int windowWeight, windowHeight;
        if (actual > target)
        {
            windowHeight = pixel_Height;
            windowWeight = (int)(pixel_Height * target);
        }
        else
        {
            windowWeight = pixel_Weight;
            windowHeight = (int)(pixel_Weight / target);
        }
        glViewport((pixel_Weight - windowWeight) / 2, (pixel_Height - windowHeight) / 2, windowWeight, windowHeight);*/

        
        
        
        glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        
        
        glUseProgram(prog);
        glUniform2f(uProj, 2.0f / WORLD_W, 2.0f / WORLD_H);
        glBindVertexArray(vao);

       auto draw = [&](const Rectangle& rectangle, float red, float green, float blue)
        {
            glUniform2f(uPos,  rectangle.x, rectangle.y);
            glUniform2f(uSize, rectangle.w, rectangle.h);
            glUniform3f(uColor, red, green, blue);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        };

        draw(LeftPaddle.rectangle,  0.00f, 0.00f, 0.90f);
        draw(RightPaddle.rectangle, 0.90f, 0.00f, 0.00f);
        draw(ball.rectangle,  1.00f, 1.00f, 1.00f);

        SDL_GL_SwapWindow(gameWindow);
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(prog);
    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(gameWindow);
    SDL_Quit();
    return 0;
}