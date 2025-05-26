#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <vector>
#include <cmath>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

using namespace ImGui;
using namespace std;
using namespace chrono;

GLFWwindow* window = NULL;
steady_clock::time_point lastInputTime = steady_clock::now();
vector<vector<ImVec2>> drawnPoints;

bool showCarte = false;
bool open = true;
bool showLayang = true;
bool showPentagon = false;
bool showPrismaSegienam = false;
bool showBalok = false;
bool isRotate = false;
bool isObjectRotate = false;
bool isFreeCam = false;
bool isDragging = false;
bool isTranslateObject = false;
bool hasInput = false;
bool isIdle = false;
bool isFreeDraw = false;
bool penCol = false;

int width, height;
int scaleType = 0;
int colType = 0;
float camX=0.0f,camY=0.0f;
float lastMouseX = 0.0f, lastMouseY = 0.0f;
float xAxis = 0.0f, yAxis = 0.0f;
float depth = -3.0f;
float angle = 0.0f, currentAngle=0.0f;
float scaleX = 1.0f, scaleY = 1.0f, scaleZ = 1.0f;
float rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f;
float objX = 0.0f, objY = 0.0f, objZ = 0.0f;
float drawX=0.0f,drawY=0.0f;
float lineThickness = 1.0f;
float borderThickness = 1.0f;
float controlAngle = 0.0f;

ImVec4 defaultObjColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
ImVec4 defaultBorColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
ImVec4 defaultPencilColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

//INIT
bool Init() {
    if (!glfwInit()) return false;
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    int windowWidth = mode->width;
    int windowHeight = mode->height;

    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_MAXIMIZED,GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    window = glfwCreateWindow(windowWidth, windowHeight, "Project", NULL, NULL);
    if (!window) {
        return false;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return false;
    glEnable(GL_DEPTH_TEST);

    IMGUI_CHECKVERSION();
    CreateContext();
    ImGuiIO& io = GetIO();(void) io;
    io.Fonts->AddFontDefault();
    ImFont* mainfont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\ARLRDBD.TTF", 17.0f);
    IM_ASSERT(mainfont);
    io.FontDefault = mainfont;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    StyleColorsDark();
    
    return true;
}

//IDLE
void idleanim(){
    auto now = steady_clock::now();
    duration<float> elapsed = now - lastInputTime;
    if (elapsed.count() >= 10.0f) {
        isIdle = true;
    }
    if (isIdle)
    {
        angle += 0.1f;
        if (angle >= 360.0f){
            angle = 0.0f;
        }
    }
}

//MID TEXT
void middletext(const char* text){
    SetCursorPosX(GetWindowWidth()/2 - CalcTextSize(text).x/2);
    Text(text);
}

//INPUT LISTENER
void inputlistener(){
    hasInput=false;
    ImGuiIO& io = GetIO();
    ImVec2 mousePos = io.MousePos;

    //Q & E
    if (!isTranslateObject && !isRotate && !isFreeCam)
    {
            if (IsKeyDown(ImGuiKey_Q))
            {
                if (depth >= -20.0f)
                {
                    depth -= 0.1f;
                }
                hasInput = true;
            }
            if (IsKeyDown(ImGuiKey_E))
            {
                if (depth < -2.1f)
                {
                    depth += 0.1f;
                }
                hasInput = true;
            }
    }

    //T
    if (IsKeyPressed(ImGuiKey_T, false))
    {
        isFreeCam = false;
        isRotate = false;
       if (!isTranslateObject)
        {
            isTranslateObject=true;
        }
        else{
            isTranslateObject=false;
        }
        hasInput=true;
    }

    //F
    if (IsKeyPressed(ImGuiKey_F, false))
    {
        isTranslateObject = false;
        isRotate = false;
       if (!isFreeCam)
        {
            isFreeCam=true;
        }
        else{
            isFreeCam=false;
        }
        hasInput=true;
    }
    
    //R
    if (IsKeyPressed(ImGuiKey_R, false))
    {
        isTranslateObject = false;
        isFreeCam = false;
       if (!isRotate)
        {
            isRotate=true;
        }
        else{
            isRotate=false;
        }
        hasInput=true;
    }

    //C
    if (IsKeyPressed(ImGuiKey_C, false))
    {
        if (!showCarte)
        {
            showCarte=true;
        }
        else{
            showCarte=false;
        }
        hasInput=true;
    }

    //FREE CAM
    if (isFreeCam)
    {
        if (IsKeyDown(ImGuiKey_A))
        {
            camX+=0.1;
            hasInput = true;
        }
        if (IsKeyDown(ImGuiKey_D))
        {
            camX-=0.1;
            hasInput = true;
        }
        if (IsKeyDown(ImGuiKey_Q))
        {
            camY+=0.1;
            hasInput = true;
        }
        if (IsKeyDown(ImGuiKey_E))
        {
            camY-=0.1;
            hasInput = true;
        }
        if (IsKeyDown(ImGuiKey_S))
        {
            depth-=0.1;
            hasInput = true;
        }
        if (IsKeyDown(ImGuiKey_W))
        {
            depth+=0.1;
            hasInput = true;
        }
        if (IsMouseClicked(0))
        {
            isDragging = true;
            lastMouseX = mousePos.x;
            lastMouseY = mousePos.y;
            hasInput = true;
        }

        if (IsMouseReleased(0))
        {
            isDragging = false;
        }

        if (isDragging) {
            float deltaX = mousePos.x - lastMouseX;
            float deltaY = mousePos.y - lastMouseY;

            yAxis += deltaX * 0.25f;
            xAxis += deltaY * 0.25f;
            lastMouseX = mousePos.x;
            lastMouseY = mousePos.y;
            hasInput = true;
        }
    }

    //TRANLATE OBJ
    if (isTranslateObject)
    {
        if (IsKeyDown(ImGuiKey_A) || IsKeyDown(ImGuiKey_UpArrow))
        {
            objX-=0.1;
            hasInput = true;
        }
        if (IsKeyDown(ImGuiKey_D) || IsKeyDown(ImGuiKey_RightArrow))
        {
            objX+=0.1;
            hasInput = true;
        }
        if (IsKeyDown(ImGuiKey_Q))
        {
            objZ-=0.1;
            hasInput = true;
        }
        if (IsKeyDown(ImGuiKey_E))
        {
            objZ+=0.1;
            hasInput = true;
        }
        if (IsKeyDown(ImGuiKey_S) || IsKeyDown(ImGuiKey_DownArrow))
        {
            objY-=0.1;
            hasInput = true;
        }
        if (IsKeyDown(ImGuiKey_W) || IsKeyDown(ImGuiKey_UpArrow))
        {
            objY+=0.1;
            hasInput = true;
        }
    }

    //ROTATE OBJ
    if (isRotate)
    {
        if (IsKeyDown(ImGuiKey_A) || IsKeyDown(ImGuiKey_UpArrow))
        {
            rotY-=1.0f;
            hasInput = true;
        }
        if (IsKeyDown(ImGuiKey_D) || IsKeyDown(ImGuiKey_RightArrow))
        {
            rotY+=1.0f;
            hasInput = true;
        }
        if (IsKeyDown(ImGuiKey_Q))
        {
            rotZ-=1.0f;
            hasInput = true;
        }
        if (IsKeyDown(ImGuiKey_E))
        {
            rotZ+=1.0f;
            hasInput = true;
        }
        if (IsKeyDown(ImGuiKey_S) || IsKeyDown(ImGuiKey_DownArrow))
        {
            rotX+=1.0f;
            hasInput = true;
        }
        if (IsKeyDown(ImGuiKey_W) || IsKeyDown(ImGuiKey_UpArrow))
        {
            rotX-=1.0f;
            hasInput = true;
        }
    }

    //FREE DRAW
    if(isFreeDraw && !io.WantCaptureMouse)
    {
        if (IsMouseClicked(0))
        {
            isDragging = true;
            lastMouseX = mousePos.x;
            lastMouseY = mousePos.y;
            drawnPoints.push_back({});
            hasInput = true;
        }

        if (IsMouseReleased(0))
        {
            isDragging = false;
        }
        if (isDragging) {
            ImVec2 currentPos = io.MousePos;
            vector<ImVec2>& currentGroup = drawnPoints.back();

            if (!currentGroup.empty()) {
                ImVec2 lastPos = currentGroup.back();
                float dx = currentPos.x - lastPos.x;
                float dy = currentPos.y - lastPos.y;
                float distance = sqrtf(dx * dx + dy * dy);
                int steps = (int)(distance / 1.0f);

                for (int i = 1; i <= steps; ++i) {
                    float t = (float)i / (float)steps;
                    ImVec2 interpolated = ImVec2(lastPos.x + t * dx, lastPos.y + t * dy);
                    currentGroup.push_back(interpolated);
                }
            }

            currentGroup.push_back(currentPos);
            hasInput = true;
        }
        if (IsKeyPressed(ImGuiKey_Backspace))
        {
            drawnPoints.clear();
            hasInput = true;
        }
        
    }

    //IDLE CHECK
    if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
    hasInput = true;
    }
    if (hasInput)
    {
        lastInputTime = steady_clock::now();
        isIdle = false;
    }
}

//CARTESIUS
void carte() {
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-20.0f, 0.0f,0.0f);
    glVertex3f(20.0f, 0.0f,0.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 20.0f,0.0f);
    glVertex3f(0.0f, -20.0f,0.0f);
    glColor3f(0.0f,0.0f,1.0f);
    glVertex3f(0.0f, 0.0f,20.0f);
    glVertex3f(0.0f, 0.0f,-20.0f);
    glEnd();
}


//KITE
void kite(ImVec4 color,ImVec4 border) {
    glColor3f(color.x, color.y, color.z);
    glBegin(GL_QUADS);
    glVertex3f(0.0f, 0.85f, 0.0f);
    glVertex3f(-0.4f, 0.3f, 0.0f);
    glVertex3f(0.0f, -0.85f, 0.0f);
    glVertex3f(0.4f, 0.3f, 0.0f);
    glEnd();

    //BORDER
    glLineWidth(borderThickness);
    glColor3f(border.x, border.y, border.z);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.85f, 0.0f);
    glVertex3f(-0.4f, 0.3f, 0.0f);
    glVertex3f(-0.4f, 0.3f, 0.0f);
    glVertex3f(0.0f, -0.85f, 0.0f);
    glVertex3f(0.0f, -0.85f, 0.0f);
    glVertex3f(0.4f, 0.3f, 0.0f);
    glVertex3f(0.4f, 0.3f, 0.0f);
    glVertex3f(0.0f, 0.85f, 0.0f);
    glEnd();
    glLineWidth(1.0f);
}

//PENTAGON
void pentagon(ImVec4 color, ImVec4 border) {
    glColor3f(color.x, color.y, color.z);
    glBegin(GL_POLYGON);
    glVertex3f(0.0f, 0.85f, 0.0f);
    glVertex3f(-0.45f, 0.2f, 0.0f);
    glVertex3f(-0.225f, -0.7f, 0.0f);
    glVertex3f(0.225f, -0.7f, 0.0f);
    glVertex3f(0.45f, 0.2f, 0.0f);
    glEnd();

    //BORDER
    glLineWidth(borderThickness);
    glColor3f(border.x, border.y, border.z);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.85f, 0.0f);
    glVertex3f(-0.45f, 0.2f, 0.0f);
    glVertex3f(-0.45f, 0.2f, 0.0f);
    glVertex3f(-0.225f, -0.7f, 0.0f);
    glVertex3f(-0.225f, -0.7f, 0.0f);
    glVertex3f(0.225f, -0.7f, 0.0f);
    glVertex3f(0.225f, -0.7f, 0.0f);
    glVertex3f(0.45f, 0.2f, 0.0f);
    glVertex3f(0.45f, 0.2f, 0.0f);
    glVertex3f(0.0f, 0.85f, 0.0f);
    glEnd();
    glLineWidth(1.0f);
}

//BLOCK
void balok(ImVec4 color, ImVec4 border){
    glColor3f(color.x, color.y, color.z);
    glBegin(GL_QUADS);
    //BACK
  	glVertex3f(-1.0f, 0.5f, 0.5f);
  	glVertex3f(1.0f, 0.5f, 0.5f);
  	glVertex3f(1.0f, -0.5f, 0.5f);
  	glVertex3f(-1.0f, -0.5f, 0.5f);
  	
  	//RIGHT
  	glVertex3f(1.0f, 0.5f, 0.5f);
  	glVertex3f(1.0f, 0.5f, -0.5f);
  	glVertex3f(1.0f, -0.5f, -0.5f);
  	glVertex3f(1.0f, -0.5f, 0.5f);
  	
  	//LEFT
  	glVertex3f(-1.0f, 0.5f, 0.5f);
  	glVertex3f(-1.0f, 0.5f, -0.5f);
  	glVertex3f(-1.0f, -0.5f, -0.5f);
  	glVertex3f(-1.0f, -0.5f, 0.5f);
  	
  	//FRONT
  	glVertex3f(1.0f, 0.5f, -0.5f);
  	glVertex3f(-1.0f, 0.5f, -0.5f);
  	glVertex3f(-1.0f, -0.5f, -0.5f);
  	glVertex3f(1.0f, -0.5f, -0.5f);
  	
  	//TOP
  	glVertex3f(-1.0f, 0.5f, 0.5f);
  	glVertex3f(1.0f, 0.5f, 0.5f);
  	glVertex3f(1.0f, 0.5f, -0.5f);
  	glVertex3f(-1.0f, 0.5f, -0.5f);
  	
  	//BOT
  	glVertex3f(-1.0f, -0.5f, 0.5f);
  	glVertex3f(1.0f, -0.5f, 0.5f);
  	glVertex3f(1.0f, -0.5f, -0.5f);
  	glVertex3f(-1.0f, -0.5f, -0.5f);
    glEnd();

    //BORDER
    glLineWidth(borderThickness);
    glColor3f(border.x, border.y, border.z);
    glBegin(GL_LINES);
    //BACK
  	glVertex3f(-1.0f, 0.5f, 0.5f);
  	glVertex3f(1.0f, 0.5f, 0.5f);
  	glVertex3f(1.0f, -0.5f, 0.5f);
  	glVertex3f(-1.0f, -0.5f, 0.5f);
  	
  	//RIGHT
  	glVertex3f(1.0f, 0.5f, 0.5f);
  	glVertex3f(1.0f, 0.5f, -0.5f);
  	glVertex3f(1.0f, -0.5f, -0.5f);
  	glVertex3f(1.0f, -0.5f, 0.5f);
    glVertex3f(1.0f, 0.5f, 0.5f);
    glVertex3f(1.0f, -0.5f, 0.5f);
    glVertex3f(1.0f, 0.5f, -0.5f);
    glVertex3f(1.0f, -0.5f, -0.5f);
  	
  	//LEFT
  	glVertex3f(-1.0f, 0.5f, 0.5f);
  	glVertex3f(-1.0f, 0.5f, -0.5f);
  	glVertex3f(-1.0f, -0.5f, -0.5f);
  	glVertex3f(-1.0f, -0.5f, 0.5f);
    glVertex3f(-1.0f, 0.5f, 0.5f);
    glVertex3f(-1.0f, -0.5f, 0.5f);
    glVertex3f(-1.0f, 0.5f, -0.5f);
    glVertex3f(-1.0f, -0.5f, -0.5f);
  	
  	//FRONT
  	glVertex3f(1.0f, 0.5f, -0.5f);
  	glVertex3f(-1.0f, 0.5f, -0.5f);
  	glVertex3f(-1.0f, -0.5f, -0.5f);
  	glVertex3f(1.0f, -0.5f, -0.5f);
  	
  	//TOP
  	glVertex3f(-1.0f, 0.5f, 0.5f);
  	glVertex3f(1.0f, 0.5f, 0.5f);
  	glVertex3f(1.0f, 0.5f, -0.5f);
  	glVertex3f(-1.0f, 0.5f, -0.5f);
  	
  	//BOT
  	glVertex3f(-1.0f, -0.5f, 0.5f);
  	glVertex3f(1.0f, -0.5f, 0.5f);
  	glVertex3f(1.0f, -0.5f, -0.5f);
  	glVertex3f(-1.0f, -0.5f, -0.5f);
    glEnd();
    glLineWidth(1.0f);
}

//HEXA PRISM
void hexaprism(ImVec4 color, ImVec4 border) {
    glColor3f(color.x, color.y, color.z);

    //BOT
    glBegin(GL_POLYGON);
    glVertex3f(-1.0f, -0.5f,  0.0f);
    glVertex3f(-0.5f, -0.5f,  0.5f);
    glVertex3f( 0.5f, -0.5f,  0.5f);
    glVertex3f( 1.0f, -0.5f,  0.0f);
    glVertex3f( 0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glEnd();

    glBegin(GL_QUADS);
    //FRONT RIGHT
    glVertex3f(-1.0f, -0.5f,  0.0f);
    glVertex3f(-0.5f, -0.5f,  0.5f);
    glVertex3f(-0.5f,  0.5f,  0.5f);
    glVertex3f(-1.0f,  0.5f,  0.0f);

    //FRONT
    glVertex3f(-0.5f, -0.5f,  0.5f);
    glVertex3f( 0.5f, -0.5f,  0.5f);
    glVertex3f( 0.5f,  0.5f,  0.5f);
    glVertex3f(-0.5f,  0.5f,  0.5f);

    //FRONT LEFT
    glVertex3f( 0.5f, -0.5f,  0.5f);
    glVertex3f( 1.0f, -0.5f,  0.0f);
    glVertex3f( 1.0f,  0.5f,  0.0f);
    glVertex3f( 0.5f,  0.5f,  0.5f);

    //BACK LEFT
    glVertex3f( 1.0f, -0.5f,  0.0f);
    glVertex3f( 0.5f, -0.5f, -0.5f);
    glVertex3f( 0.5f,  0.5f, -0.5f);
    glVertex3f( 1.0f,  0.5f,  0.0f);

    //BACK
    glVertex3f( 0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f,  0.5f, -0.5f);
    glVertex3f( 0.5f,  0.5f, -0.5f);

    //BACK RIGHT
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-1.0f, -0.5f,  0.0f);
    glVertex3f(-1.0f,  0.5f,  0.0f);
    glVertex3f(-0.5f,  0.5f, -0.5f);
    glEnd();

    //TOP
    glBegin(GL_POLYGON);
    glVertex3f(-1.0f,  0.5f,  0.0f);
    glVertex3f(-0.5f,  0.5f,  0.5f);
    glVertex3f( 0.5f,  0.5f,  0.5f);
    glVertex3f( 1.0f,  0.5f,  0.0f);
    glVertex3f( 0.5f,  0.5f, -0.5f);
    glVertex3f(-0.5f,  0.5f, -0.5f);
    glEnd();

    //BORDER
    glLineWidth(borderThickness);
    glColor3f(border.x, border.y, border.z);
    glBegin(GL_LINES);
    //BOT
    glVertex3f(-1.0f, -0.5f,  0.0f);
    glVertex3f(-0.5f, -0.5f,  0.5f);
    glVertex3f(-0.5f, -0.5f,  0.5f);
    glVertex3f( 0.5f, -0.5f,  0.5f);
    glVertex3f( 0.5f, -0.5f,  0.5f);
    glVertex3f( 1.0f, -0.5f,  0.0f);
    glVertex3f( 1.0f, -0.5f,  0.0f);
    glVertex3f( 0.5f, -0.5f, -0.5f);
    glVertex3f( 0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-1.0f, -0.5f,  0.0f);

    //FRONT RIGHT
    glVertex3f(-1.0f, -0.5f,  0.0f);
    glVertex3f(-0.5f, -0.5f,  0.5f);
    glVertex3f(-0.5f, -0.5f,  0.5f);
    glVertex3f(-0.5f,  0.5f,  0.5f);
    glVertex3f(-0.5f,  0.5f,  0.5f);
    glVertex3f(-1.0f,  0.5f,  0.0f);
    glVertex3f(-1.0f,  0.5f,  0.0f);
    glVertex3f(-1.0f, -0.5f,  0.0f);

    //FRONT
    glVertex3f(-0.5f, -0.5f,  0.5f);
    glVertex3f( 0.5f, -0.5f,  0.5f);
    glVertex3f( 0.5f, -0.5f,  0.5f);
    glVertex3f( 0.5f,  0.5f,  0.5f);
    glVertex3f( 0.5f,  0.5f,  0.5f);
    glVertex3f(-0.5f,  0.5f,  0.5f);
    glVertex3f(-0.5f,  0.5f,  0.5f);
    glVertex3f(-0.5f, -0.5f,  0.5f);

    //FRONT LEFT
    glVertex3f( 0.5f, -0.5f,  0.5f);
    glVertex3f( 1.0f, -0.5f,  0.0f);
    glVertex3f( 1.0f, -0.5f,  0.0f);
    glVertex3f( 1.0f,  0.5f,  0.0f);
    glVertex3f( 1.0f,  0.5f,  0.0f);
    glVertex3f( 0.5f,  0.5f,  0.5f);
    glVertex3f( 0.5f,  0.5f,  0.5f);
    glVertex3f( 0.5f, -0.5f,  0.5f);

    //BACK LEFT
    glVertex3f( 1.0f, -0.5f,  0.0f);
    glVertex3f( 0.5f, -0.5f, -0.5f);
    glVertex3f( 0.5f, -0.5f, -0.5f);
    glVertex3f( 0.5f,  0.5f, -0.5f);
    glVertex3f( 0.5f,  0.5f, -0.5f);
    glVertex3f( 1.0f,  0.5f,  0.0f);
    glVertex3f( 1.0f,  0.5f,  0.0f);
    glVertex3f( 1.0f, -0.5f,  0.0f);
    
    //BACK
    glVertex3f( 0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f,  0.5f, -0.5f);
    glVertex3f(-0.5f,  0.5f, -0.5f);
    glVertex3f( 0.5f,  0.5f, -0.5f);
    glVertex3f( 0.5f,  0.5f, -0.5f);
    glVertex3f( 0.5f, -0.5f, -0.5f);

    //BACK RIGHT
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-1.0f, -0.5f,  0.0f);
    glVertex3f(-1.0f, -0.5f,  0.0f);
    glVertex3f(-1.0f,  0.5f,  0.0f);
    glVertex3f(-1.0f,  0.5f,  0.0f);
    glVertex3f(-0.5f,  0.5f, -0.5f);
    glVertex3f(-0.5f,  0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, -0.5f);

    //TOP
    glVertex3f(-1.0f,  0.5f,  0.0f);
    glVertex3f(-0.5f,  0.5f,  0.5f);
    glVertex3f(-0.5f,  0.5f,  0.5f);
    glVertex3f( 0.5f,  0.5f,  0.5f);
    glVertex3f( 0.5f,  0.5f,  0.5f);
    glVertex3f( 1.0f,  0.5f,  0.0f);
    glVertex3f( 1.0f,  0.5f,  0.0f);
    glVertex3f( 0.5f,  0.5f, -0.5f);
    glVertex3f( 0.5f,  0.5f, -0.5f);
    glVertex3f(-0.5f,  0.5f, -0.5f);
    glVertex3f(-0.5f,  0.5f, -0.5f);
    glVertex3f(-1.0f,  0.5f,  0.0f);
    glEnd();
    glLineWidth(1.0f);
}

//UI
void RenderUI() {
    ImGuiIO& io = GetIO();
    NewFrame();
    ImGuiStyle& style = GetStyle();
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f,0.0f,0.0f,1.0f);
    ImVec4 activeColor = ImVec4(0.008f, 0.031f, 0.231f, 1.0f);
    style.Colors[ImGuiCol_TitleBg]         = activeColor;
    style.Colors[ImGuiCol_TitleBgActive]   = activeColor;
    style.Colors[ImGuiCol_TitleBgCollapsed] = activeColor;
    PushFont(io.FontDefault);

    //OBJEK
    SetNextWindowSize(ImVec2(350,180));
    SetNextWindowPos(ImVec2(1570,0));
    if (Begin("Objek", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
    {
        SetCursorPos(ImVec2(35,45));
        if (Button("Layang - layang",ImVec2(130,50))) {
            if (!showLayang)
            {
                showLayang=true;
                showPentagon = false;
                showPrismaSegienam = false;
                showBalok = false;
                isFreeDraw = false;
            }
            else{
                showLayang = false;
                showPentagon = false;
                showPrismaSegienam = false;
                showBalok = false;
            }
        }
        SetCursorPos(ImVec2(180,45));
        if (Button("Segilima",ImVec2(130,50))) {
            if (!showPentagon)
            {
                showPentagon=true;
                showLayang = false;
                showPrismaSegienam = false;
                showBalok = false;
                isFreeDraw = false;
            }
            else{
                showLayang = false;
                showPentagon = false;
                showPrismaSegienam = false;
                showBalok = false;
            }
        }
        SetCursorPos(ImVec2(35,110));
        if (Button("Prisma Segienam",(ImVec2(130,50)))){
            if (!showPrismaSegienam)
            {
                showPrismaSegienam=true;
                showPentagon = false;
                showLayang = false;
                showBalok = false;
                isFreeDraw = false;
            }
            else{
                showLayang = false;
                showPentagon = false;
                showPrismaSegienam = false;
                showBalok = false;
            }
        }
        SetCursorPos(ImVec2(180,110));
        if (Button("Balok",ImVec2(130,50))) {
            if (!showBalok)
            {
                showBalok=true;
                showPentagon = false;
                showPrismaSegienam = false;
                showLayang = false;
                isFreeDraw = false;
            }
            else{
                showLayang = false;
                showPentagon = false;
                showPrismaSegienam = false;
                showBalok = false;
            }
        }
    }
    End();

    //WARNA
    SetNextWindowSize(ImVec2(350,260));
    SetNextWindowPos(ImVec2(1570,180));
    if (Begin("Warna", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
    {
        if (penCol)
        {
            SetCursorPos(ImVec2(250,130));
            RadioButton("Pensil", true);
            SetCursorPos(ImVec2(10,40));
            ColorPicker3("Warna Pensil", (float*)&defaultPencilColor, ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs);
        }
        else
        {
            SetCursorPos(ImVec2(250,130));
            RadioButton("Objek", &colType, 0);
            SetCursorPos(ImVec2(250,170));
            RadioButton("Border", &colType, 1);
            if (colType == 0)
            {
                SetCursorPos(ImVec2(10,40));
                ColorPicker3("Warna Objek", (float*)&defaultObjColor, ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs);
            }
            else if (colType == 1)
            {
                SetCursorPos(ImVec2(10,40));
                ColorPicker3("Warna Border", (float*)&defaultBorColor, ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs);
            }      
        }
    }
    End();

    //ROTASI
    SetNextWindowSize(ImVec2(350,115));
    SetNextWindowPos(ImVec2(1570,440));
    if (Begin("Rotasi", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
    {
        SetCursorPos(ImVec2(35,35));
        Checkbox("Rotasi", &isRotate);
        if (isRotate)
        {
            isTranslateObject=false;
            isFreeCam=false;
            isFreeDraw=false;
        }
        SetCursorPos(ImVec2(25,70));
        if (Button("Reset Rotasi",ImVec2(300,30))) {
            rotX=0;
            rotY=0;
            rotZ=0;
        }
    }
    End();

    //SKALA
    SetNextWindowSize(ImVec2(350,200));
    SetNextWindowPos(ImVec2(1570,555));
    if (Begin("Skala", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
    {
        SetCursorPos(ImVec2(35,35));
        RadioButton("Perbesar / Perkecil",&scaleType,0);
        SetCursorPos(ImVec2(220,35));
        RadioButton("Bebas",&scaleType,1);
        if(scaleType==0){
            SetCursorPos(ImVec2(35,65));
            SliderFloat("Skala",&scaleX,0.01f,7.0f);
            SetCursorPos(ImVec2(25,150));
            if (Button("Reset Skala",ImVec2(300,30)))
            {
                scaleX=1.0f;
                scaleY=1.0f;
                scaleZ=1.0f;
            }
        }
        else if (scaleType == 1)
        {
            SetCursorPos(ImVec2(35,65));
            SliderFloat("Skala X",&scaleX,0.01f,7.0f);
            SetCursorPos(ImVec2(35,90));
            SliderFloat("Skala Y",&scaleY,0.01f,7.0f);
            SetCursorPos(ImVec2(35,115));
            SliderFloat("Skala Z",&scaleZ,0.01f,7.0f);
            SetCursorPos(ImVec2(25,150));
            if (Button("Reset Skala",ImVec2(300,30)))
            {
                scaleX=1.0f;
                scaleY=1.0f;
                scaleZ=1.0f;
            }
        }
    }
    End();

    //TRANSLASI
    SetNextWindowSize(ImVec2(350,115));
    SetNextWindowPos(ImVec2(1570,755));
    if (Begin("Translasi", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
    {
        SetCursorPos(ImVec2(35,35));
        Checkbox("Translasi", &isTranslateObject);
        if (isTranslateObject)
        {
            isRotate=false;
            isFreeCam=false;
            isFreeDraw=false;
        }
        SetCursorPos(ImVec2(25,70));
        if (Button("Reset Translasi",ImVec2(300,30)))
        {
            objX=0.0f;
            objY=0.0f;
            objZ=0.0f;
        }
    }
    End();

    //MISC
    SetNextWindowSize(ImVec2(350,210));
    SetNextWindowPos(ImVec2(1570,870));
    if (Begin("Misc", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
    {
        SetCursorPos(ImVec2(35,35));
        Checkbox("Kartesius", &showCarte);
        SetCursorPos(ImVec2(135,35));
        Checkbox("Free Cam", &isFreeCam);
        if (isFreeCam)
        {
            isTranslateObject=false;
            isRotate=false;
            isFreeDraw=false;
        }
        SetCursorPos(ImVec2(235,35));
        Checkbox("Free Draw", &isFreeDraw);
        if (isFreeDraw)
        {
            showLayang = false;
            showPentagon = false;
            showPrismaSegienam = false;
            showBalok = false;
            isTranslateObject = false;
            isFreeCam = false;
            isRotate=false;
            penCol = true;
            SetCursorPosX(30);
            SliderFloat("Thickness", &lineThickness,1.0f,8.0f);
            ImDrawList* drawList = GetBackgroundDrawList();
            for (const auto& group : drawnPoints) {
                for (size_t i = 1; i < group.size(); ++i) {
                    drawList->AddLine(
                        group[i - 1], group[i],
                        IM_COL32(
                            (int)(defaultPencilColor.x * 255),
                            (int)(defaultPencilColor.y * 255),
                            (int)(defaultPencilColor.z * 255),
                            (int)(defaultPencilColor.w * 255)),
                        lineThickness
                    );
                }
            }
            SetCursorPos(ImVec2(25,87));
            if (Button("Reset Gambar",ImVec2(300,30)))
            {
                drawnPoints.clear();
            }
        }else{
            penCol = false;
            drawnPoints.clear();
            if(!showLayang && !showBalok && !showPentagon && !showPrismaSegienam){
                showLayang=true;
            }
            SetCursorPos(ImVec2(25,87));
            if (Button("Reset Kamera",ImVec2(300,30)))
            {
                camX=0;
                camY=0;
                xAxis=0;
                yAxis=0;
                depth=-3;
            }
        }
    }
    End();

    //ZOOM
    SetNextWindowSize(ImVec2(70,180));
    SetNextWindowPos(ImVec2(1500,0));
    if (Begin("Zoom", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration))
    {
        middletext("ZOOM");
        SetCursorPos(ImVec2(20,35));
        VSliderFloat("##Depth",ImVec2(30,130),&depth,-20.0f,-2.0f, "");
    }
    End();
    
    //BORDER THICKNESS
    if (showLayang || showPentagon || showBalok || showPrismaSegienam)
    {
        SetNextWindowSize(ImVec2(70,180));
        SetNextWindowPos(ImVec2(1500,180));
        if (Begin("Border", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration))
        {
            middletext("BORDER");
            SetCursorPos(ImVec2(20,35));
            VSliderFloat("##Thick",ImVec2(30,130),&borderThickness, 0.0f, 6.0f, "");
        }
        End();
    }

    //NOTE
    SetNextWindowSize(ImVec2(500,100));
    SetNextWindowPos(ImVec2(0,0));
    if (Begin("NOTE", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
    {
        if (isRotate)
        {
            Text("KUNCI : ROTASI");
            Text("W : -X, S : +X, A : -Y, D : +Y, Q : -Z, E : +Z");
        }
        else if (isTranslateObject){
            Text("KUNCI : TRANSLASI");
            Text("W : +Y, S : -Y, A : -X, D : +X, Q : -Z, E : +Z");
        }
        else if (isFreeCam)
        {
            Text("KUNCI : PERGERAKAN");
            Text("W : MAJU, S : MUNDUR, A : KIRI, D : KANAN, Q : ATAS, E : BAWAH");
            Text("KLIK + DRAG : ROTASI KAMERA");
        }
        else if (isFreeDraw){
            Text("KUNCI : AKSI");
            Text("KLIK KIRI + DRAG : GAMBAR, BACKSPACE : RESET GAMBAR");
        }
        else
        {
            Text("KARTESIUS : X = MERAH, Y = HIJAU, Z = BIRU");
            Text("Q : ZOOM OUT, E : ZOOM IN");
            Text("R : ROTASI, T : TRANSLASI, C : TOGGLE CARTE, F : FREE CAM");
        }
    }
    End();
    
    PopFont();
    Render();
}


//RENDER
void RenderScene() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glfwGetFramebufferSize(window, &width, &height);
    float aspect = (float)width / (float)height;
    glFrustum(-aspect, aspect, -1.0, 1.0, 2.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //CAM
    glTranslatef(camX, camY, depth);
    glRotatef(xAxis, 1.0f, 0.0f, 0.0f);
    glRotatef(yAxis, 0.0f, 1.0f, 0.0f);
    if(showCarte) carte();

    //OBJ
    glPushMatrix();
    glTranslatef(objX,objY,objZ);
    glRotatef(rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(rotZ, 0.0f, 0.0f, 1.0f);
    if (isIdle)
    {
        glRotatef(angle, 1.0f, 1.0f, 0.0f);
    }
    else{
        angle=0;
    }
    if (scaleType == 0)
    {
        glScalef(scaleX, scaleX, scaleX);
    }
    else if (scaleType == 1)
    {
        glScalef(scaleX, scaleY, scaleZ);
    }
    if (showLayang) kite(defaultObjColor, defaultBorColor);
    if (showPentagon) pentagon(defaultObjColor, defaultBorColor);
    if (showPrismaSegienam) hexaprism(defaultObjColor, defaultBorColor);
    if (showBalok) balok(defaultObjColor, defaultBorColor);
    glPopMatrix();
}

//DESTROY
void Cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

int main() {
    if (!Init()) return -1;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        inputlistener();
        idleanim();
        RenderUI();
        RenderScene();
        ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
        glfwSwapBuffers(window);
    }

    Cleanup();
    return 0;
}