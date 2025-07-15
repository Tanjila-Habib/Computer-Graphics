#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <cmath>
#include <math.h>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <stdio.h>
using namespace std;
bool timerStarted = false;
const float PI = 3.1416f;
float wheelAngle = 0.0f;
bool windowState = true; // Toggle window lights
bool startBlinking = false;
bool showSkeleton = true;   // Whether the skeleton is currently visible
bool skeletonBlinking = true; // Control for automatic toggle (optional)
int skeletonBlinkTimer = 10000;  // milliseconds countdown
bool lightning = false;
int lightningTimer = 0;
bool showRain = false;
std::vector<std::pair<float, float>> raindrop;
const int MAX_RAINDROPS = 300;
int rainState = 0;
int currentRaindrops = 0;
bool gravestoneCracked = false;
float posX = 10.0f, posY = 10.0f; // Bat's position
float dx = 0.02, dy = 0.015f;   // Bat speed
float batWidth = 12.0f, batHeight = 4.5f;
int windowWidth = 800;
 int windowHeight = 600;
const int ObjectWidth = 800;
const int ObjectHeight = 600;
bool mouseControl = false;
float mouseWorldX = 0.0f, mouseWorldY = 0.0f;
float defaultDx = dx;  // Store original speeds (set these after you initialize dx, dy)
float defaultDy = dy;
bool wasHovering = false;
bool isFlapSoundPlaying = false;
bool batPaused = false;
int moving = 0;
float wingAngle = 0.0f;         // Angle for wing flapping
float wingSpeed = 2.0f;         // Speed of flapping
bool flapUp = 1;
float swayAngle = 0.8f;
float cloudSpeed=0.01f;
const float cloudWidth = 1.0f;
const float windowidth = 10.0f;
float cloudOffset = 0.0f;
bool cloudRunning = true;  // Cloud is moving by default
bool headlightsOn = true;  // Headlights initially off
bool autoToggleHeadlights = true;  // Control auto toggle timer
int headlightToggleTimer = 500;  // milliseconds
float carPosX = 0.0f;        // Current horizontal offset of the car
bool moveCar = false;        // Whether the car is currently moving
float carSpeed = 0.08f;       // Movement speed per timer tick
float carTargetX = 30.0f;    // Target X position to move to (adjust as needed)
bool showBell=false;
float rotationAngle = 0.0f;
float rotationSpeed = 0.1f;
bool rotateArrowActive = true; // Initially rotating
bool showStains = false;  // Controls blood stains on danger sign
int stainCount = 0;         // How many stains to draw
const int maxStains = 20;
int currentScene = 1;  // Default to Scene 1
bool rotateClockwise = true;

inline void playFlap()   { PlaySound(TEXT("flap.wav"),   NULL,  SND_FILENAME | SND_ASYNC); }
inline void playRain() {PlaySound(TEXT("rain.wav"),   NULL,  SND_FILENAME | SND_ASYNC| SND_LOOP);}
inline void LightBlink() {PlaySound(TEXT("lightblink.wav"),   NULL,  SND_FILENAME | SND_ASYNC| SND_LOOP);}
inline void BellSound() {PlaySound(TEXT("bell.wav"),   NULL,  SND_FILENAME | SND_ASYNC);}
inline void CarSound() {PlaySound(TEXT("car.wav"),   NULL,  SND_FILENAME | SND_ASYNC);}
inline void ghost2Sound() {PlaySound(TEXT("ghost3.wav"),   NULL,  SND_FILENAME | SND_ASYNC);}
inline void BirdSound() {PlaySound(TEXT("bird.wav"),   NULL,  SND_FILENAME | SND_ASYNC| SND_LOOP);}


////////////////////////////////////////////////////////////////////////////////////////////////////
//Scene-1:
void drawSemiCircle(float x1, float y1, float x2, float y2) {
    const int segments = 50;

    float cx = (x1 + x2) / 2.0f;
    float cy = (y1 + y2) / 2.0f;
    float dx = x2 - x1;
    float dy = y2 - y1;
    float r = sqrt(dx * dx + dy * dy) / 2.0f;
    float angle = atan2(dy, dx);

    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy); // Center point
        for (int i = 0; i <= segments; i++) {
            float theta = M_PI * i / segments;  // Angle from 0 to π (half circle)
            float x = cos(theta) * r;
            float y = sin(theta) * r;
            // Rotate the point by 'angle' and translate back to center
            float xr = cx + (x * cos(angle) - y * sin(angle));
            float yr = cy + (x * sin(angle) + y * cos(angle));
            glVertex2f(xr, yr);
        }
    glEnd();
}
void drawCircle(float cx, float cy, float r, int num_segments = 100) {
    glBegin(GL_POLYGON);  // Use GL_POLYGON to fill the circle
    for (int i = 0; i < num_segments; ++i) {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}
//for background
void drawGradientBackground() {
    // Top narrow band (y: 9 to 10)
    glBegin(GL_QUADS);
        glColor3f(0.498f, 0.702f, 0.835f);
        glVertex2f(-3.0f, 20.0f);
        glVertex2f(20.0f, 20.0f);
        glColor3f(0.4392f, 0.4824f, 0.4863f);
        glVertex2f(20.0f, 9.0f);
        glVertex2f(-3.0f, 9.0f);
    glEnd();

    // Middle section (y: 0 to 10)
    glBegin(GL_QUADS);
     glColor3f(0.2039f, 0.2863f, 0.3686f);
        glVertex2f(-3.0f, 9.0f);
        glVertex2f(20.0f, 9.0f);

        glColor3f(0.3216f, 0.7451f, 0.5020f);
        glVertex2f(20.0f, 0.0f);
        glVertex2f(-3.0f, 0.0f);
    glEnd();

    // Bottom section (y: -10 to 0)
    glBegin(GL_QUADS);
        glColor3f(0.27f, 0.27f, 0.27f);  // Dark gray color
        glVertex2f(-20.0f, 0.5f);
        glVertex2f(20.0f, 0.5f);
        glVertex2f(20.0f, -10.0f);
        glVertex2f(-20.0f, -10.0f);
    glEnd();
}
//Building start
// Rectangular window
void drawRectWindow(float x, float y, float w, float h) {
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    glEnd();
}
// Arch window
void drawArchWindow(float cx, float cy, float w, float h) {
    float r = w / 2.0f;
    int segments = 100;

    // Rectangular part
    drawRectWindow(cx - r, cy, w, h - r);

    // Top arch
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy + h - r);
        for (int i = 0; i <= segments; ++i) {
            float angle = 3.1416f * i / segments;
            glVertex2f(cx + cos(angle) * r, cy + h - r + sin(angle) * r);
        }
    glEnd();
}
// Building shape
void drawBuildingShape() {
    glColor3f(0.0, 0.0, 0.0);

    // Bottom block
    glBegin(GL_QUADS);
        glVertex2f(2.0f, 1.7f);
        glVertex2f(8.0f , 1.7f);
        glVertex2f(8.0f , 6.0f);
        glVertex2f(2.0f, 6.0f);
    glEnd();

    // Middle section
    glBegin(GL_QUADS);
        glVertex2f(3.6f, 6.0f);
        glVertex2f(6.4f, 6.0f);
        glVertex2f(6.4f, 8.0f);
        glVertex2f(3.6f, 8.0f);
    glEnd();

    // Tower
    glBegin(GL_QUADS);
        glVertex2f(4.3f , 8.0f);
        glVertex2f(5.5f, 8.0f);
        glVertex2f(5.5f, 9.5f);
        glVertex2f(4.3f, 9.5f);
    glEnd();
}
// Roofs
 void drawRoofs() {
    glColor3f(0.0, 0.0, 0.0);

    // Main roof
    glBegin(GL_TRIANGLES);
        glVertex2f(1.0f, 6.0f);
        glVertex2f(9.6f, 6.0f);
        glVertex2f(5.0f, 7.8f);
    glEnd();

    // Middle roof
    glBegin(GL_TRIANGLES);
        glVertex2f(3.4f, 8.0f);
        glVertex2f(6.6f, 8.0f);
        glVertex2f(5.0f, 8.8f);
    glEnd();

    // Tower roof
    glBegin(GL_TRIANGLES);
        glVertex2f(3.81f, 9.51f);
        glVertex2f(6.0f, 9.5f);
        glVertex2f(4.93f, 10.43f);
    glEnd();
}
// windows
void drawWindows() {
    if (windowState)
        glColor3f(1.0, 0.6, 0.0); // Lights on
    else
        glColor3f(0.1, 0.1, 0.1); // Lights off

    // Lower level
    drawArchWindow(2.9f, 2.3f, 1.0f, 2.0f);
    drawArchWindow(5.0f, 2.3f, 1.2f, 2.0f);
    drawArchWindow(7.0f, 2.3f, 1.0f, 2.0f);

    // Middle level
    drawArchWindow(4.2f, 6.2f, 0.8f, 1.2f);
    drawArchWindow(6.1f, 6.2f, 0.8f, 1.2f);

    // Tower
    drawArchWindow(4.9f, 8.2f, 0.6f, 1.0f);
}
void toggleWindows(int value) {
    if (startBlinking) {
        windowState = !windowState;      // Toggle lights
        glutPostRedisplay();             // Request redraw
        glutTimerFunc(500, toggleWindows, 0); // Repeat every 500ms
    }
}
void stair()
{
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_QUADS);
     glVertex2f(3.0f,2.0f);
        glVertex2f(7.0f,2.0f);
        glVertex2f(6.7f,1.5f);
        glVertex2f(2.7f,1.5f);
        glEnd();

        glBegin(GL_QUADS);
     glVertex2f(2.7f,1.5f);
        glVertex2f(2.7f,1.0f);
        glVertex2f(6.7f,1.0f);
        glVertex2f(6.7f,1.5f);
        glEnd();
        glBegin(GL_QUADS);
        glVertex2f(7.0f,2.0f);
        glVertex2f(7.0f,1.5f);
        glVertex2f(6.7f,1.0f);
        glVertex2f(6.7f,1.5f);
        glEnd();
}
void drawBellManual() {
    glColor3f(0.9686f, 0.8627f, 0.4353f);

    // Bell body (trapezoid)
    glBegin(GL_QUADS);
        glVertex2f(2.3f, 5.3f);  // cx - radius
        glVertex2f(3.1f, 5.3f);  // cx + radius
        glVertex2f(2.94f, 4.6f); // cx + radius * 0.6
        glVertex2f(2.46f, 4.6f); // cx - radius * 0.6
    glEnd();

    // Bell top (approximated semicircle with 5 points)

    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(2.7f, 5.3f);  // center
        glVertex2f(2.3f, 5.3f);  // left
        glVertex2f(2.36f, 5.42f);
        glVertex2f(2.52f, 5.48f);
        glVertex2f(2.88f, 5.48f);
        glVertex2f(3.04f, 5.42f);
        glVertex2f(3.1f, 5.3f);  // right
    glEnd();
    // Bottom triangle (clapper)
    glBegin(GL_TRIANGLES);
        glVertex2f(2.62f, 4.5f); // left
        glVertex2f(2.78f, 4.5f); // right
        glVertex2f(2.7f, 4.2f);  // bottom
    glEnd();
}
void arrow()
{
    glColor3f(0.698f, 0.729f, 0.733f);  // Color for all parts

    // Static base (shaft)
    glBegin(GL_QUADS);
        glVertex2f(8.6f, 7.5f);
        glVertex2f(8.59f, 6.39f);
        glVertex2f(8.9f, 6.27f);
        glVertex2f(8.9f, 7.5f);
    glEnd();
     glBegin(GL_QUADS);
        glVertex2f(8.72f, 7.6f);
        glVertex2f(8.72f, 7.5f);
        glVertex2f(8.8f, 7.5f);
        glVertex2f(8.8f, 7.6f);
    glEnd();


    // Rotating head (second rectangle + triangle)
    glPushMatrix();
    glTranslatef(8.6f, 7.7f, 0.0f);         // Rotate around this point
    glRotatef(rotationAngle, 0.0f, 0.0f, 1.0f);
    glTranslatef(-8.6f, -7.7f, 0.0f);       // Undo translation

    // Second rectangle
    glBegin(GL_QUADS);
        glVertex2f(8.0f, 7.89f);
        glVertex2f(8.0f, 7.5f);
        glVertex2f(9.2f, 7.5f);
        glVertex2f(9.2f, 7.88f);
    glEnd();

    // Triangle head
    glBegin(GL_TRIANGLES);
        glVertex2f(9.2f, 8.2f);
        glVertex2f(9.2f, 7.2f);
        glVertex2f(9.71f, 7.7f);
    glEnd();

    glPopMatrix();
}

//ID1
void building() {
    drawBuildingShape();
    drawRoofs();
    drawWindows();
    stair();
    arrow();

    drawBellManual();

}
//Building end

//TREE start
void tree1()
{
    glColor3f(0.0, 0.0, 0.0);

    // Trunk (no sway)
    glBegin(GL_QUADS);
        glVertex2f(0.1f, 7.6f);
        glVertex2f(0.1f, 2.4f);
        glVertex2f(0.6f, 2.4f);
        glVertex2f(0.6f, 7.6f);
    glEnd();

    // Calculate sway offset (e.g. ±0.05 units)
    float swayOffset = sinf(swayAngle) * 0.05f;

    // Branch triangles with sway on X coordinates
    glBegin(GL_TRIANGLES);
        glVertex2f(-0.76746f + swayOffset, 5.64889f);//AA
        glVertex2f(1.37f + swayOffset, 5.64f);//BB
        glVertex2f(0.38f + swayOffset, 7.2f);//CC
    glEnd();

    glBegin(GL_TRIANGLES);
        glVertex2f(-0.79f + swayOffset, 6.2f);//DD
        glVertex2f(1.4f + swayOffset, 6.2f);//EE
        glVertex2f(0.32f + swayOffset, 7.74f);//FF
    glEnd();

    // Continue similarly for other branch triangles
    glBegin(GL_TRIANGLES);
        glVertex2f(-0.7f + swayOffset, 6.97f);//GG
        glVertex2f(1.28f + swayOffset, 6.95f);//HH
        glVertex2f(0.36f + swayOffset, 8.25f);//II
    glEnd();

    glBegin(GL_TRIANGLES);
        glVertex2f(-0.6f + swayOffset, 7.7f);//JJ
        glVertex2f(1.25f + swayOffset, 7.69f);//KK
        glVertex2f(0.38f + swayOffset, 8.53f);//LL
    glEnd();

    glBegin(GL_TRIANGLES);
        glVertex2f(-0.38f + swayOffset, 8.21f);//MM
        glVertex2f(1.03f + swayOffset, 8.22f);//NN
        glVertex2f(0.37f + swayOffset, 8.84f);//OO
    glEnd();
}

void tree2()
{
    glColor3f(0.0, 0.0, 0.0);

    // Trunk (no sway)
    glBegin(GL_QUADS);
        glVertex2f(-2.0f, 2.4f);
        glVertex2f(-1.5f, 2.4f);
        glVertex2f(-1.5f, 7.0f);
        glVertex2f(-2.0f, 7.0f);
    glEnd();

    // Calculate sway offset (e.g., ±0.05 units)
    float swayOffset = sinf(swayAngle) * 0.05f;

    // Branch triangles with sway on X coordinates
    glBegin(GL_TRIANGLES);
        glVertex2f(-3.0f + swayOffset, 6.45f);
        glVertex2f(-1.62f + swayOffset, 7.66f);
        glVertex2f(-0.4f + swayOffset, 6.45f);
    glEnd();

    glBegin(GL_TRIANGLES);
        glVertex2f(-3.0f + swayOffset, 6.0f);
        glVertex2f(-1.72f + swayOffset, 7.35f);
        glVertex2f(-0.5f + swayOffset, 6.0f);
    glEnd();

    glBegin(GL_TRIANGLES);
        glVertex2f(-2.85f + swayOffset, 7.02f);
        glVertex2f(-0.7f + swayOffset, 7.01f);
        glVertex2f(-1.66f + swayOffset, 7.95f);
    glEnd();

    glBegin(GL_TRIANGLES);
        glVertex2f(-2.72f + swayOffset, 7.42f);
        glVertex2f(-0.72f + swayOffset, 7.42f);
        glVertex2f(-1.63f + swayOffset, 8.26f);
    glEnd();

    glBegin(GL_TRIANGLES);
        glVertex2f(-2.61f + swayOffset, 7.83f);
        glVertex2f(-0.81f + swayOffset, 7.83f);
        glVertex2f(-1.65f + swayOffset, 8.59f);
    glEnd();

    glBegin(GL_TRIANGLES);
        glVertex2f(-2.37f + swayOffset, 8.25f);
        glVertex2f(-0.97f + swayOffset, 8.25f);
        glVertex2f(-1.67f + swayOffset, 8.92f);
    glEnd();
}
//ID2
void Tree()
{
    tree1();
    tree2();
}
//Tree end
//Cloud start
void cloud1() {
     glColor4f(0.79f, 0.811f, 0.823f, 0.7f);
    drawSemiCircle(1.99 + cloudOffset, 17.01, 4.78 + cloudOffset, 18.05);
    drawSemiCircle(4.78 + cloudOffset, 18.05, 7.69 + cloudOffset, 16.88);
    drawSemiCircle(7.69 + cloudOffset, 16.88, 5.97 + cloudOffset, 15.48);
    drawSemiCircle(5.97 + cloudOffset, 15.48, 3.53 + cloudOffset, 15.63);
    drawSemiCircle(3.53 + cloudOffset, 15.63, 1.99 + cloudOffset, 17.01);
}

void cloud2() {
     glColor4f(0.79f, 0.811f, 0.823f, 0.7f);
    drawSemiCircle(12.43 + cloudOffset, 17.05, 15.14 + cloudOffset, 18.49);
    drawSemiCircle(15.14 + cloudOffset, 18.49, 18.28 + cloudOffset, 17.0);
    drawSemiCircle(18.28 + cloudOffset, 17.0, 16.68 + cloudOffset, 15.38);
    drawSemiCircle(16.68 + cloudOffset, 15.38, 14.18 + cloudOffset, 15.43);
    drawSemiCircle(14.18 + cloudOffset, 15.43, 12.43 + cloudOffset, 17.05);
}

void cloud3() {
    glColor4f(0.79f, 0.811f, 0.823f, 0.7f);
    drawSemiCircle(-2.0 + cloudOffset, 16.82, -1.74 + cloudOffset, 15.4);
    drawSemiCircle(-1.74 + cloudOffset, 15.4, -3.94 + cloudOffset, 15.26);
    drawSemiCircle(-3.94 + cloudOffset, 15.26, -5.51 + cloudOffset, 16.03);
    drawSemiCircle(-5.51 + cloudOffset, 16.03, -4.23 + cloudOffset, 17.36);
    drawSemiCircle(-4.23 + cloudOffset, 17.36, -2.0 + cloudOffset, 16.82);
}

void fillMiddle1() {
     glColor4f(0.79f, 0.811f, 0.823f, 0.7f);
    glBegin(GL_POLYGON);
        glVertex2f(1.99f + cloudOffset, 17.01f);
        glVertex2f(4.78f + cloudOffset, 18.05f);
        glVertex2f(7.69f + cloudOffset, 16.88f);
        glVertex2f(5.97f + cloudOffset, 15.48f);
        glVertex2f(3.53f + cloudOffset, 15.63f);
    glEnd();
}
void fillMiddle2() {
     glColor4f(0.79f, 0.811f, 0.823f, 0.7f);
    glBegin(GL_POLYGON);
        glVertex2f(12.43f + cloudOffset, 17.05f);
        glVertex2f(15.14f + cloudOffset, 18.49f);
        glVertex2f(18.28f + cloudOffset, 17.0f);
        glVertex2f(16.68f + cloudOffset, 15.38f);
        glVertex2f(14.18f + cloudOffset, 15.43f);
    glEnd();
}
void fillMiddle3() {
    glColor4f(0.79f, 0.811f, 0.823f, 0.7f);
    glBegin(GL_POLYGON);
        glVertex2f(-2.0f + cloudOffset, 16.82f);
        glVertex2f(-1.74f + cloudOffset, 15.4f);
        glVertex2f(-3.94f + cloudOffset, 15.26f);
        glVertex2f(-5.51f + cloudOffset, 16.03f);
        glVertex2f(-4.23f + cloudOffset, 17.36f);
    glEnd();
}

void moon()
{ glColor3f(0.988f, 0.953f, 0.812f);


   drawCircle(12.63,16.14,1.5);
   glEnd();
}
//ID3
void Cloud()
{    moon();
    cloud1();
    cloud2();
    cloud3();

    fillMiddle1();
    fillMiddle2();
    fillMiddle3();

}

//Cloud end
//Car start
void carupperpart()
{
    glColor3f(0.0, 0.0, 0.0);

    glBegin(GL_QUADS);
        glVertex2f(10.0f, 2.0f);
        glVertex2f(14.0f, 2.0f);
        glVertex2f(15.0f, 0.0f);
        glVertex2f(9.0f, 0.0f);
    glEnd();
}
void carlowerpart()
{
    glColor3f(0.0, 0.0, 0.0);

    glBegin(GL_QUADS);
        glVertex2f(8.0f, 0.0f);
        glVertex2f(16.0f, 0.0f);
        glVertex2f(16.0f, -2.0f);
        glVertex2f(8.0f, -2.0f);
    glEnd();
}
void drawwheel(float x, float y, float radius) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glRotatef(wheelAngle, 0.0f, 0.0f, 1.0f);

    glColor3f(0.1f, 0.1f, 0.1f);  // Color of the wheel
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float rad = i * 3.14159 / 180;
        glVertex2f(cos(rad) * radius, sin(rad) * radius);
    }
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);  // Color for wheel spokes
    glBegin(GL_LINES);
    for (int i = 0; i < 6; i++) {
        float rad = i * 60 * 3.14159 / 180;
        glVertex2f(0.0f, 0.0f);
        glVertex2f(cos(rad) * radius, sin(rad) * radius);
    }
    glEnd();
    glPopMatrix();
}
void window1()
{
     glColor3f(1.0f, 1.0f, 0.0f);

    glBegin(GL_QUADS);
        glVertex2f(10.5f, 1.4f);
        glVertex2f(11.4f, 1.4f);
        glVertex2f(11.4f, 0.4f);
        glVertex2f(10.5f, 0.4f);
    glEnd();
}
void window2()
{
     glColor3f(1.0f, 1.0f, 0.0f);

    glBegin(GL_QUADS);
        glVertex2f(13.0f, 1.4f);
        glVertex2f(14.0f, 1.4f);
        glVertex2f(14.0f, 0.4f);
        glVertex2f(13.0f, 0.4f);
    glEnd();
}
void Headlight()
{
    if (!headlightsOn) return; // Skip drawing if off

    glColor3f(0.9686f, 0.8627f, 0.4353f);
    drawSemiCircle(8, -1.2, 8, 0);
    drawSemiCircle(16, 0, 16, -1.2);
}
//ID4
void car()
{
    glPushMatrix();
    glTranslatef(carPosX, 0.0f, 0.0f);  // Move car horizontally by carPosX

    carupperpart();
    carlowerpart();
    drawwheel(10.12, -2, 0.8);
    drawwheel(13.56, -2, 0.8);
    window1();
    window2();
    Headlight();

    glPopMatrix();
}
//Car end
//Skeleton start
void drawSkull() {
    glColor3f(1, 1, 1); // White
    drawCircle(9.6f, 4.43f, 1.0f); // Top of skull (scaled)

    glBegin(GL_POLYGON); // Jaw (scaled)
        glVertex2f(9.0f, 4.4f);
        glVertex2f(9.0f, 3.0f);
        glVertex2f(10.2f, 3.0f);
        glVertex2f(10.2f, 4.4f);
    glEnd();
}
void eyes()
{
    glColor3f(0.0f, 0.0f, 0.0f); // White
    drawCircle(9.01048f, 4.60409f, 0.3f);
    drawCircle(10.17508f, 4.59387f, 0.3f);
    glEnd();
}
void nose()
{
    glColor3f(0.0f, 0.0f, 0.0f); // White
    glBegin(GL_TRIANGLES);
    glVertex2f(9.4f,4.2f);
    glVertex2f(9.6f,4.6f);
    glVertex2f(9.8f,4.2f);
    glEnd();

}
void Teeth() {
    glColor3f(0.0f, 0.0f, 0.0f); // Black color

    float startX = 8.95f;    // X position of first tooth
    float toothWidth = 0.1f; // Width of each tooth
    float toothHeight = 0.3f;
    int numTeeth = 9;

    for (int i = 0; i < numTeeth; ++i) {
        float x = startX + i * 0.15f; // Space between teeth
        glBegin(GL_QUADS);
            glVertex2f(x, 4.0f);
            glVertex2f(x, 4.0f - toothHeight);
            glVertex2f(x + toothWidth, 4.0f - toothHeight);
            glVertex2f(x + toothWidth, 4.0f);
        glEnd();
    }
}
//ID5
void skeleton()
{
  drawSkull();
  eyes();
  nose();
  Teeth();
}
//Skeleton end
//road start
//ID6
void road()
{
    glColor3f(1.0f,1.0f,1.0f);
    //1st
    glBegin(GL_QUADS);
    glVertex2f(0.5f,-1.4f);
    glVertex2f(0.5f,-2.0f);
    glVertex2f(2.0f,-2.0f);
    glVertex2f(2.0f,-1.4f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex2f(2.0f,-1.0f);
    glVertex2f(2.0f,-2.4f);
    glVertex2f(3.0f,-1.7f);
    glEnd();

    //2nd
    glBegin(GL_QUADS);
    glVertex2f(4.4f,-1.4f);
    glVertex2f(4.4f,-2.0f);
    glVertex2f(6.0f,-2.0f);
    glVertex2f(6.0f,-1.4f);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(6.0f,-1.0f);
    glVertex2f(6.0f,-2.4f);
    glVertex2f(6.8f,-1.7f);
    glEnd();
    //3rd
     glBegin(GL_QUADS);
    glVertex2f(10.7f, -1.4f);
    glVertex2f(10.7f, -2.0f);
    glVertex2f(12.3f, -2.0f);
    glVertex2f(12.3f, -1.4f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex2f(12.29f, -1.0f);
    glVertex2f(12.29f, -2.4f);
    glVertex2f(13.09f, -1.7f);
    glEnd();

    //4th
    glBegin(GL_QUADS);
    glVertex2f(17.2f,-1.4f);
    glVertex2f(17.2f,-2.0f);
    glVertex2f(18.8f,-2.0f);
    glVertex2f(18.8f,-1.4f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex2f(18.8f,-1.0f);
    glVertex2f(18.8f,-2.4f);
    glVertex2f(19.4f,-1.7f);
    glEnd();

}
//road end
//Danger Sign Board
void drawBloodStains() {
    glColor3f(0.6f, 0.0f, 0.0f); // blood red

    // Define a few fixed positions to cycle through
    float positions[][2] = {
        {11.5f, 5.2f}, {12.6f, 5.6f}, {12.0f, 4.5f},
        {11.8f, 5.8f}, {12.4f, 5.0f}, {12.2f, 4.9f},
        {11.6f, 4.7f}, {12.5f, 5.3f}, {12.1f, 5.6f},
        {12.3f, 4.6f}, {11.9f, 5.1f}, {12.4f, 5.7f},
        {11.7f, 5.4f}, {12.6f, 4.8f}, {11.6f, 5.9f},
        {12.0f, 5.3f}, {12.3f, 4.4f}, {12.1f, 5.0f},
        {11.9f, 4.6f}, {12.5f, 5.5f}
    };

    for (int i = 0; i < stainCount && i < maxStains; ++i) {
        float x = positions[i][0];
        float y = positions[i][1];
        float r = 0.05f + (i % 3) * 0.015f;  // vary radius slightly
        drawCircle(x, y, r);
    }
}

//ID7
void dangerSignBoard()
{
    glColor3f(0.0f,0.0f,0.0f);
    glBegin(GL_QUADS);
    glVertex2f(12.0f,0.0f);
    glVertex2f(12.5f,0.0f);
    glVertex2f(12.5f,4.0f);
    glVertex2f(12.0f,4.0f);
    glEnd();

   glColor3f(0.945f, 0.769f, 0.059f);
   glBegin(GL_TRIANGLES);
    glVertex2f(10.5f, 4.0f);
    glVertex2f(14.0f, 4.0f);
    glVertex2f(12.28f, 6.18f);
    glEnd();

   glColor3f(0.0f, 0.0f, 0.0f);
   glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(10.5f, 4.0f);
    glVertex2f(14.0f, 4.0f);
    glVertex2f(12.28f, 6.18f);
     glEnd();

    //exclamatory sign
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(12.1f,5.4f);
    glVertex2f(12.3f,5.4f);
    glVertex2f(12.3f,4.8f);
    glVertex2f(12.1f,4.8f);
    glEnd();
    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircle(12.2,4.64,0.1);
  if (stainCount > 0)
    drawBloodStains();


}

//Forest start
//ID8
void forest()
{ glColor3f(0.078f,0.353f,0.196f);
    drawSemiCircle(-3,9,-1.22,10);
    drawSemiCircle(-1.22,10,1.58,9.92);
    drawSemiCircle(1.58,9.92,2.51,9);
    drawSemiCircle(2.51,9,4,9);
    drawSemiCircle(5.57,9,6.24,9.19);
    drawSemiCircle(6.24,9.19,8,10);
    drawSemiCircle(8,10,10.38,10.68);
    drawSemiCircle(10.38,10.68,12.0,9);

    drawSemiCircle(12.0,9,14.32,10.28);
    drawSemiCircle(14.32,10.28,17.37,10.32);
    drawSemiCircle(17.37,10.32,20,9);

}
void fillMiddle4()
{
    glColor3f(0.078f,0.353f,0.196f);
    glBegin(GL_POLYGON);
    glVertex2f(-3.0f,9.0f);
    glVertex2f(-1.22f,10.0f);
    glVertex2f(1.58f,9.92f);
    glVertex2f(2.51f,9.0f);
    glEnd();

     glBegin(GL_POLYGON);
    glVertex2f(5.57f,9.0f);
    glVertex2f(6.24f,9.19f);
    glVertex2f(8.0f,10.0f);
    glVertex2f(10.38f,10.68f);
    glVertex2f(12.0f,9.0f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(12.0f,9.0f);
    glVertex2f(14.32f,10.28f);
    glVertex2f(17.37f,10.32f);
    glVertex2f(20.0f,9.0f);
    glEnd();
}
void Forest()
{
    forest();
    fillMiddle4();
}
//Forest end
//Rain start
void initRain() {
    raindrop.clear();
    for (int i = 0; i < MAX_RAINDROPS; ++i) {
        float x = static_cast<float>(rand() % 230) / 10.0f - 3.0f; // Range: -3.0 to 20.0
        float y = static_cast<float>(rand() % 260) / 10.0f - 6.0f; // Range: -6.0 to 20.0
        raindrop.push_back({x, y});
    }
}
//ID9
void drawrain() {
    if (!showRain) return;

    glColor3f(0.5f, 0.5f, 1.0f); // Light blue
    for (auto& drop : raindrop) {
        glRasterPos2f(drop.first, drop.second);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'l');
    }
}
//ID9
void drawLightning() {
    if (!lightning) return;

    glColor3f(1.0f, 1.0f, 1.0f);   // Bright white
    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
        glVertex2f(6.0f, 18.0f);
        glVertex2f(6.6f, 16.8f);
        glVertex2f(6.0f, 15.7f);
        glVertex2f(6.5f, 14.5f);
        glVertex2f(6.0f, 13.0f);
        glVertex2f(6.3f, 11.8f);
        glVertex2f(6.0f, 11.0f);
    glEnd();

    glLineWidth(1.0f);  // Reset
}
//Rain end
//Bat start
void drawWingSide(bool isRight)
{
    float direction = isRight ? 1.0f : -1.0f;
    glPushMatrix();
    glRotatef(direction * wingAngle, 0.0f, 0.0f, 1.0f); // rotate around z-axis
    glBegin(GL_POLYGON);
    glVertex2f(0.3f, 0.9f);
    glVertex2f(0.8f, 0.9f);
    glVertex2f(1.1f, -0.6f);
    glVertex2f(0.5f, -1.0f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.85f, -0.75f);
    glVertex2f(0.4f, -1.15f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(0.8f, 0.9f);
    glVertex2f(1.2f, 1.0f);
    glVertex2f(2.35f, -0.1f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(0.8f, 0.9f);
    glVertex2f(1.1f, -0.6f);
    glVertex2f(2.35f, -0.1f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(1.2f, 1.0f);
    glVertex2f(1.4f, 1.1f);
    glVertex2f(3.9f, 0.0f);
    glVertex2f(2.35f, -0.1f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(1.4f, 1.1f);
    glVertex2f(1.6f, 1.3f);
    glVertex2f(4.15f, 0.5f);
    glVertex2f(3.9f, 0.0f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(1.6f, 1.3f);
    glVertex2f(1.75f, 1.6f);
    glVertex2f(4.4f, 0.85f);
    glVertex2f(4.15f, 0.5f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(1.75f, 1.6f);
    glVertex2f(1.85f, 1.9f);
    glVertex2f(5.0f, 1.3f);
    glVertex2f(4.4f, 0.85f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(1.85f, 1.9f);
    glVertex2f(1.9f, 2.2f);
    glVertex2f(7.0f, 2.2f);
    glVertex2f(5.0f, 1.3f);
    glEnd();
    glPopMatrix();
}
//ID10
void drawBat()
{
    glColor3f(0, 0, 0);
    //right body
    glBegin(GL_POLYGON);
    glVertex2f(0.0f, 1.4f);
    glVertex2f(0.1f, 1.37f);
    glVertex2f(0.2f, 1.35f);
    glVertex2f(0.2f, 0.0f);
    glVertex2f(0.0f, 0.0f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(0.2f, 1.35f);
    glVertex2f(0.4f, 2.0f);
    glVertex2f(0.5f, 1.0f);
    glVertex2f(0.5f, -1.0f);
    glVertex2f(0.2f, 0.0f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.6f, 0.0f);
    glVertex2f(0.5f, -1.0f);
    glVertex2f(0.0f, -2.0f);
    glEnd();
    //right wing
    drawWingSide(true);
    //left side
    glPushMatrix();
    glScalef(-1.0f, 1.0f, 1.0f); // Reflect for left side
    // Left body- mirrored
    glBegin(GL_POLYGON);
    glVertex2f(0.0f, 1.4f);
    glVertex2f(0.1f, 1.37f);
    glVertex2f(0.2f, 1.35f);
    glVertex2f(0.2f, 0.0f);
    glVertex2f(0.0f, 0.0f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(0.2f, 1.35f);
    glVertex2f(0.4f, 2.0f);
    glVertex2f(0.5f, 1.0f);
    glVertex2f(0.5f, -1.0f);
    glVertex2f(0.2f, 0.0f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(0.6f, 0.0f);
    glVertex2f(0.5f, -1.0f);
    glVertex2f(0.0f, -2.0f);
    glEnd();
    // Left wing
    drawWingSide(true); // same function, reflected by scale

    glPopMatrix();
}
void convertMouseToWorld(int x, int y, float* wx, float* wy) {
    *wx = (float)x / ObjectWidth * 20.0f;
    *wy = (float)(ObjectHeight - y) / ObjectHeight * 20.0f;
}
void mouseHover(int x, int y) {
    convertMouseToWorld(x, y, &mouseWorldX, &mouseWorldY);

    float batLeft = posX - batWidth / 2.0f;
    float batRight = posX + batWidth / 2.0f;
    float batBottom = posY - batHeight / 2.0f;
    float batTop = posY + batHeight / 2.0f;
  bool isHoveringNow =
        mouseWorldX >= batLeft && mouseWorldX <= batRight &&
        mouseWorldY >= batBottom && mouseWorldY <= batTop;
   if (isHoveringNow && !wasHovering) {
        if (!isFlapSoundPlaying) {
            playFlap();             // Only play once when entering hover
            isFlapSoundPlaying = true;
        }
    }

    if (!isHoveringNow && wasHovering) {
        isFlapSoundPlaying = false; // Reset so it can play next time
    }
    mouseControl = isHoveringNow;
    wasHovering = isHoveringNow;

    glutPostRedisplay(); // Redraw after mouse move
}
void updateBatPosition() {
    if (batPaused) return;

    if (!isFlapSoundPlaying) {
        playFlap();
        isFlapSoundPlaying = true;
    }

    if (mouseControl) {
        // Hovering — follow mouse slowly
        float speed = 0.2f;
        posX += (mouseWorldX - posX) * speed;
        posY += (mouseWorldY - posY) * speed;
    }
     else{
        // Move in that direction
        posX += dx;
        posY += dy;

        // Bounce off edges
        if (posX < batWidth / 2 || posX > 20 - batWidth / 2) dx = -dx;
        if (posY < batHeight / 2 || posY > 20 - batHeight / 2) dy = -dy;
    }

}
//Bat end
//Gravestone start
void renderText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (int i = 0; text[i] != '\0'; ++i) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}
void drawCracksOnGraveTop(float cx, float cy, float radius) {
    glColor3f(0.1f, 0.1f, 0.1f);  // Very dark gray/black for crack
    glLineWidth(2.0f);

    glBegin(GL_LINE_STRIP);
        glVertex2f(cx, cy + radius * 0.95f);                  // top center of semi-circle
        glVertex2f(cx - radius * 0.15f, cy + radius * 0.75f);
        glVertex2f(cx + radius * 0.05f, cy + radius * 0.55f);
        glVertex2f(cx - radius * 0.1f, cy + radius * 0.35f);
        glVertex2f(cx + radius * 0.1f, cy + radius * 0.15f);
        glVertex2f(cx, cy);                                   // meets the flat body
    glEnd();

    glLineWidth(1.0f); // reset line width
}
//ID11
void GraveStone(bool cracked) {
    // Filled first polygon (rectangle base)
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_POLYGON);
        glVertex2f(15.25f, 4.5f);
        glVertex2f(17.75f, 4.5f);
        glVertex2f(17.75f, 7.25f);
        glVertex2f(15.25f, 7.25f);
    glEnd();
    // Border for first polygon
    glLineWidth(2.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(15.25f, 4.5f);
        glVertex2f(17.75f, 4.5f);
        glVertex2f(17.75f, 7.25f);
        glVertex2f(15.25f, 7.25f);
    glEnd();
    // Filled second polygon (trapezoid base)
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_POLYGON);
        glVertex2f(15.25f, 4.5f);
        glVertex2f(13.8f, 3.0f);
        glVertex2f(16.8f, 3.0f);
        glVertex2f(17.75f, 4.5f);
    glEnd();
    // Border for second polygon
    glLineWidth(2.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(15.25f, 4.5f);
        glVertex2f(13.8f, 3.0f);
        glVertex2f(16.8f, 3.0f);
        glVertex2f(17.75f, 4.5f);
    glEnd();
    // Filled semicircle top
      // Draw top semicircle
    glColor3f(0.3f, 0.3f, 0.3f);
    drawSemiCircle(15.25f, 7.25f, 17.75f, 7.25f);
    // Border of semicircle
    float cx = (15.25f + 17.75f) / 2.0f;
    float cy = 7.25f;
    float radius = (17.75f - 15.25f) / 2.0f;

    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 180; i++) {
        float rad = i * 3.14159f / 180.0f;
        float x = cx + cos(rad) * radius;
        float y = cy + sin(rad) * radius;
        glVertex2f(x, y);
    }
    glEnd();
    // RIP text
    glColor3f(0.9f, 0.9f, 0.9f);
    renderText(16.15f, 6.0f, "RIP");
       // --- Draw Cracks if cracked ---
    if (cracked) {
        // Crack on top (semi-circle)
        glColor3f(0.1f, 0.1f, 0.1f);
        glLineWidth(1.5f);
        glBegin(GL_LINE_STRIP);
            glVertex2f(cx, cy + radius);
            glVertex2f(cx - 0.2f, cy + radius - 0.3f);
            glVertex2f(cx + 0.1f, cy + radius - 0.6f);
        glEnd();

        // Crack on rectangle (first polygon)
        glColor3f(0.1f, 0.1f, 0.1f);
        glBegin(GL_LINE_STRIP);
            glVertex2f(16.1f, 7.0f);
            glVertex2f(16.0f, 6.6f);
            glVertex2f(16.2f, 6.2f);
            glVertex2f(16.0f, 5.7f);
            glVertex2f(16.3f, 5.2f);
        glEnd();
     glColor3f(0.1f, 0.1f, 0.1f);  // Dark gray crack
    glLineWidth(1.8f);
    glBegin(GL_LINE_STRIP);
        glVertex2f(15.4f, 4.4f);  // Near the top edge of trapezoid
        glVertex2f(15.1f, 3.9f);
        glVertex2f(14.7f, 3.5f);
        glVertex2f(14.3f, 3.2f);
        glVertex2f(13.9f, 3.0f);  // Ends exactly at the left base corner (border)
    glEnd();

    }

}
//Gravestone end

void drawScene1() {
        glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-3.0, 20.0, -6.0, 20.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
       // Flash effect
    if (lightning) {
        glClearColor(1.0, 1.0, 1.0, 1.0);
    } else {
        glClearColor(0.0, 0.0, 0.0, 1.0);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glDisable(GL_DEPTH_TEST);  // Disable depth testing for background
    drawGradientBackground();   // Draw 2D background
    building();
    Tree();
    Cloud();
    if (showSkeleton)
    skeleton();
    road();
    dangerSignBoard();
    car();
    Forest();
     GraveStone(gravestoneCracked);
     if (showRain) drawrain();
    drawLightning();
    glPushMatrix();            // Save current matrix
    updateBatPosition();
    glTranslatef(posX, posY, 0);  // Move bat position
    drawBat();
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);
    glFlush();
    glutSwapBuffers();
}
/////////////////////////////////////////////////////////////////////////////////////////////////
//Scene-2:
bool isNight = false;
bool isScene2 = false;
bool isRaining = false;
bool mirrorCracked = false;
float chandelierAngle = 0.0f;      // Current swing angle
bool swingRight = true;           // Direction flag
bool fireplaceOn = true;
bool fireplaceHovered = false;
bool showJumpScare = false;
int jumpScareDuration = 0;
float pendulumAngle = 0.0f;
struct Raindrop {
    float x, y;

};
vector<Raindrop> raindrops;

// Function prototypes
void display();
void handleKeypress(unsigned char key, int x, int y);
void handleMouse(int button, int state, int x, int y);
void updateRain(int value);


// ------------------ Plate Animation Code ------------------
struct Plate {
    float x, y;
    float radius;
    bool movingRight;
    bool isMoving;
};

vector<Plate> plates;

void initPlates();
void drawPlates();
void updatePlates(int value);
void drawcircle(float cx, float cy, float r, int segments = 100);

struct Candle {
    float x, height;
    bool hovered;
};

vector<Candle> candles;
bool candleFlickering = true;
float candleFlameOffset = 0.0f;


void initCandles();
void drawCandles();
void updateCandleFlames(int value);
void updateJumpScare(int value); // 👈 Add this



// --------------------------- Scene Elements ---------------------------

void room() {
    glClear(GL_COLOR_BUFFER_BIT);
    if (isNight)
    glColor3f(0.04f, 0.04f, 0.06f);  // darker for night
else
    glColor3f(0.1f, 0.1f, 0.15f); // Floor
// Similarly adjust other colors in room() for darker walls, ceiling, etc.
 // Floor
    glBegin(GL_POLYGON);
        glVertex2f(-1.1f, -0.9f); glVertex2f(1.1f, -0.9f);
        glVertex2f(0.6f, -0.2f);  glVertex2f(-0.6f, -0.2f);
    glEnd();

glColor3f(isNight ? 0.08f : 0.2f, isNight ? 0.08f : 0.2f, isNight ? 0.12f : 0.25f);

 // Back wall
    glBegin(GL_POLYGON);
        glVertex2f(-0.6f, 0.8f); glVertex2f(0.6f, 0.8f);
        glVertex2f(0.6f, -0.2f); glVertex2f(-0.6f, -0.2f);
    glEnd();

    // Left and right walls
glColor3f(isNight ? 0.06f : 0.15f, isNight ? 0.06f : 0.15f, isNight ? 0.1f : 0.2f);

 // Left wall
    glBegin(GL_POLYGON);
        glVertex2f(-1.1f, -0.9f); glVertex2f(-0.6f, -0.2f);
        glVertex2f(-0.6f, 0.8f);  glVertex2f(-1.1f, 1.0f);
    glEnd();

   // Left and right walls
glColor3f(isNight ? 0.06f : 0.15f, isNight ? 0.06f : 0.15f, isNight ? 0.1f : 0.2f);

// Right wall
    glBegin(GL_POLYGON);
        glVertex2f(0.6f, 0.8f); glVertex2f(1.1f, 1.0f);
        glVertex2f(1.1f, -0.9f); glVertex2f(0.6f, -0.2f);
    glEnd();

  // Roof
glColor3f(isNight ? 0.03f : 0.05f, isNight ? 0.03f : 0.05f, isNight ? 0.05f : 0.05f);

 // Roof
    glBegin(GL_POLYGON);
        glVertex2f(-1.1f, 1.0f); glVertex2f(1.1f, 1.0f);
        glVertex2f(0.6f, 0.8f);  glVertex2f(-0.6f, 0.8f);
    glEnd();
}



void drawRain() {
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(1.2f);
    glBegin(GL_LINES);
    for (const auto &drop : raindrops) {
        glVertex2f(drop.x, drop.y);
        glVertex2f(drop.x + 0.01f, drop.y - 0.03f);
    }
    glEnd();
}

void drawWindow() {
    glColor3f(0.4f, 0.25f, 0.1f); // Frame
    glBegin(GL_POLYGON);
        glVertex2f(-0.4f, 0.75f); glVertex2f(0.4f, 0.75f);
        glVertex2f(0.4f, -0.15f); glVertex2f(-0.4f, -0.15f);
    glEnd();

    glBegin(GL_POLYGON); // Outside view
        glColor3f(0.05f, 0.05f, 0.1f); // top
        glVertex2f(-0.36f, 0.715f); glVertex2f(0.36f, 0.715f);
       glColor3f(0.1f, 0.1f, 0.2f);  // bottom
        glVertex2f(0.362f, -0.116f); glVertex2f(-0.36f, -0.115f);
    glEnd();

    glColor3f(0.4f, 0.25f, 0.1f);
    glBegin(GL_LINES);
        glVertex2f(-0.12f, 0.75f); glVertex2f(-0.12f, -0.15f);
        glVertex2f(0.12f, 0.75f); glVertex2f(0.12f, -0.15f);
        glVertex2f(-0.4f, 0.45f); glVertex2f(0.4f, 0.45f);
        glVertex2f(-0.4f, 0.15f); glVertex2f(0.4f, 0.15f);
    glEnd();

    drawRain();
}

void drawCarpet() {
    glColor3f(0.3f, 0.05f, 0.12f); // Haunted maroon

    glBegin(GL_POLYGON);
        glVertex2f(-0.55f, -0.27f);
        glVertex2f(0.56f, -0.28f);
        glVertex2f(0.87f, -0.82f);
        glVertex2f(-0.85f, -0.82f);
    glEnd();
}

void drawtable() {
    glColor3f(0.4f, 0.25f, 0.1f);
    glBegin(GL_POLYGON);
        glVertex2f(-0.46f,-0.18f); glVertex2f(0.46f,-0.18f);
        glVertex2f(0.57f,-0.45f); glVertex2f(-0.57f,-0.45f);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex2f(-0.566f,-0.453f); glVertex2f(-0.566f,-0.5f);
        glVertex2f(0.569f,-0.5f); glVertex2f(0.569f,-0.453f);
    glEnd();

    glBegin(GL_POLYGON); glVertex2f(-0.44f,-0.5f); glVertex2f(-0.4f,-0.5f); glVertex2f(-0.4f,-0.66f); glVertex2f(-0.44f,-0.66f); glEnd();
    glBegin(GL_POLYGON); glVertex2f(-0.57f,-0.5f); glVertex2f(-0.52f,-0.5f); glVertex2f(-0.52f,-0.8f); glVertex2f(-0.57f,-0.8f); glEnd();
    glBegin(GL_POLYGON); glVertex2f(0.52f,-0.5f); glVertex2f(0.57f,-0.5f); glVertex2f(0.57f,-0.8f); glVertex2f(0.52f,-0.8f); glEnd();
    glBegin(GL_POLYGON); glVertex2f(0.4f,-0.5f); glVertex2f(0.44f,-0.5f); glVertex2f(0.44f,-0.66f); glVertex2f(0.4f,-0.66f); glEnd();
}
float flameOffset = 0.0f;

void drawFlames() {
    float baseX = 0.84f;
    float baseY = -0.2f + flameOffset;

    // Colors: red, orange, yellow
float colors[3][3] = {
    {1.0f, fireplaceHovered ? 0.6f : 0.2f, 0.0f},
    {1.0f, fireplaceHovered ? 0.7f : 0.5f, 0.0f},
    {1.0f, fireplaceHovered ? 1.0f : 0.8f, 0.0f}
};


    float angle = -30.0f * 3.14159f / 180.0f; // 🔁 25 degrees for more tilt left
 // 🔄 Tilt 10 degrees to the LEFT (negative angle)

    for (int i = 0; i < 4; i++) {
        float offsetX = ((rand() % 7) - 3) / 100.0f;  // small X variation
        int c = rand() % 3;

        float tipX = baseX + offsetX;
        float tipY = baseY;

        // Triangle base before rotation
        float leftX = -0.015f;
        float leftY = -0.06f;

        float rightX = 0.015f;
        float rightY = -0.06f;

        // Rotate base points LEFT (counterclockwise)
        float rotatedLeftX = leftX * cos(angle) - leftY * sin(angle);
        float rotatedLeftY = leftX * sin(angle) + leftY * cos(angle);

        float rotatedRightX = rightX * cos(angle) - rightY * sin(angle);
        float rotatedRightY = rightX * sin(angle) + rightY * cos(angle);

        // Translate to world position
        float finalLeftX = baseX + offsetX + rotatedLeftX;
        float finalLeftY = baseY + rotatedLeftY;

        float finalRightX = baseX + offsetX + rotatedRightX;
        float finalRightY = baseY + rotatedRightY;

        // Draw flame
        glColor3f(colors[c][0], colors[c][1], colors[c][2]);
        glBegin(GL_TRIANGLES);
            glVertex2f(tipX, tipY);               // Tip
            glVertex2f(finalLeftX, finalLeftY);   // Left base
            glVertex2f(finalRightX, finalRightY); // Right base
        glEnd();
    }
    if (!fireplaceOn) return;

}

void updateFlame(int value) {
    flameOffset = ((rand() % 6) - 3) / 500.0f;  // ±0.006

    glutPostRedisplay();
    glutTimerFunc(150, updateFlame, 0);  // slower = 150ms
}



void fireplace() {
    // Outer frame
    glColor3f(0.25f, 0.1f, 0.25f);
    glBegin(GL_POLYGON);
        glVertex2f(0.72f, 0.33f); glVertex2f(0.98f, 0.24f);
        glVertex2f(0.98f, -0.74f); glVertex2f(0.72f, -0.37f);
    glEnd();

    // Inner wall/background
    glColor3f(0.36f, 0.22f, 0.38f);
    glBegin(GL_POLYGON);
        glVertex2f(0.76f, 0.08f); glVertex2f(0.92f, -0.01f);
        glVertex2f(0.92f, -0.57f); glVertex2f(0.76f, -0.34f);
    glEnd();

    // 🔥 Only draw flames if turned on
           // 🔥 Change glow based on state
    if (fireplaceOn) {
        glColor3f(0.36f, 0.22f, 0.38f);  // Brighter wall
    } else {
        glColor3f(0.1f, 0.05f, 0.12f);   // Dim wall when off
    }

    // Inner glow rect
    glBegin(GL_POLYGON);
        glVertex2f(0.76f, 0.08f); glVertex2f(0.92f, -0.01f);
        glVertex2f(0.92f, -0.57f); glVertex2f(0.76f, -0.34f);
    glEnd();

    // ✅ Actually draw the flames only if it's on
    if (fireplaceOn) {
        drawFlames();
    }
}
void drawBasicEllipse(float radiusX, float radiusY) {
    int segments = 100;
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; i++) {
        float angle = 2.0f * 3.1416f * i / segments;
        float x = radiusX * cos(angle);
        float y = radiusY * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

void drawMirror() {
    float centerX = -0.85f;
    float centerY = 0.25f;
    float radiusX = 0.07f;
    float radiusY = 0.22f;

    glPushMatrix();
    glTranslatef(centerX, centerY, 0.0f);

    glColor3f(0.72f, 0.78f, 0.88f);
    drawBasicEllipse(radiusX, radiusY);

    glColor3f(0.3f, 0.2f, 0.1f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++) {
        float angle = 2.0f * 3.1416f * i / 100;
        float x = (radiusX + 0.01f) * cos(angle);
        float y = (radiusY + 0.01f) * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();

    if (mirrorCracked) {
        glColor3f(0.1f, 0.1f, 0.1f);
        glLineWidth(1.5f);
        glBegin(GL_LINES);
        glVertex2f(0.0f, 0.0f); glVertex2f(0.03f, 0.2f);
        glVertex2f(0.0f, 0.0f); glVertex2f(-0.05f, 0.15f);
        glVertex2f(0.0f, 0.0f); glVertex2f(0.04f, -0.2f);
        glVertex2f(0.0f, 0.0f); glVertex2f(-0.06f, -0.1f);
        glVertex2f(0.0f, 0.0f); glVertex2f(0.07f, 0.05f);
        glEnd();
    }

    glPopMatrix();
}

void drawChandelier() {
    float centerX = 0.0f;      // Top center of room
    float centerY = 0.95f;
    float rodLength = 0.4f;    // Increased from 0.25f

    // Calculate swinging angle (like pendulum)
    float angleRad = chandelierAngle * 3.1416f / 180.0f;

    // Rotate the rod and chandelier
    float endX = centerX + rodLength * sin(angleRad);
    float endY = centerY - rodLength * cos(angleRad);

    // Rod
    glColor3f(0.5f, 0.4f, 0.3f);
    glLineWidth(4.0f); // Thicker rod
    glBegin(GL_LINES);
        glVertex2f(centerX, centerY);
        glVertex2f(endX, endY);
    glEnd();

    // Chandelier body (scaled up)
    glPushMatrix();
    glTranslatef(endX, endY, 0.0f);

    glColor3f(isNight ? 0.2f : 0.6f, isNight ? 0.2f : 0.6f, isNight ? 0.1f : 0.3f);


    drawCircle(0.0f, 0.0f, 0.05f); // Bigger body (was 0.03f)

    // Arms with candle flames (wider reach)
    for (float armAngle = -1.0f; armAngle <= 1.0f; armAngle += 1.0f) {
        float armX = 0.08f * sin(armAngle); // Longer arms (was 0.05f)
        float armY = 0.0f;

        // Arm line
        glBegin(GL_LINES);
           // Arm line
glColor3f(isNight ? 0.2f : 0.6f, isNight ? 0.2f : 0.5f, isNight ? 0.1f : 0.3f);

            glVertex2f(0.0f, 0.0f);
            glVertex2f(armX, armY);
        glEnd();

        // Candle flame
       // Flame
glColor3f(isNight ? 0.5f : 1.0f, isNight ? 0.3f : 0.8f, 0.0f);

float flicker = ((rand() % 3) - 1) / 3000.0f;  // ±0.001
drawCircle(armX, armY + 0.02f + flicker, 0.012f + flicker);

    }

    glPopMatrix();
}

void updateChandelier(int value) {
    if (swingRight) chandelierAngle += 0.4f;
    else chandelierAngle -= 0.4f;

    // Swing range
    if (chandelierAngle > 15.0f) swingRight = false;
    if (chandelierAngle < -15.0f) swingRight = true;

    glutPostRedisplay();
    glutTimerFunc(30, updateChandelier, 0);
}


// ---------------- Rain Logic ----------------

void updateRain(int value) {
    if (isRaining) {
        Raindrop drop;
        drop.x = -0.36f + static_cast<float>(rand()) / RAND_MAX * 0.72f;
        drop.y = 0.715f;
        raindrops.push_back(drop);
    }

    for (auto &drop : raindrops) {
        drop.x += 0.001f;
        drop.y -= 0.01f;
    }

    raindrops.erase(remove_if(raindrops.begin(), raindrops.end(),
        [](Raindrop &d) { return d.y < -0.12f; }), raindrops.end());

    glutPostRedisplay();         // <- Keeps animating flames
    glutTimerFunc(30, updateRain, 0);
}

// ---------------- Input & Display ----------------

void handleMouse(int button, int state, int x, int y) {

        float oglX = (float)x / windowWidth * 2.0f - 1.0f;
        float oglY = 1.0f - (float)y / windowHeight * 2.0f;

        // 🔥 Fireplace Hover Check (always update, even if no click)
        bool fireplaceClicked = false;
        if (oglX >= 0.72f && oglX <= 0.98f && oglY <= 0.33f && oglY >= -0.74f) {
            fireplaceHovered = true;
            fireplaceClicked = true;
        } else {
            fireplaceHovered = false;
        }

        // 💡 If left click pressed
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

            // 🔥 Fireplace Click Toggle
            if (fireplaceClicked) {
                fireplaceOn = !fireplaceOn;
                glutPostRedisplay();
                return; // Exit early if fireplace clicked
            }

            // 🍽️ Plate click
            for (auto& plate : plates) {
                float dx = oglX - plate.x;
                float dy = oglY - plate.y;
                if (dx * dx + dy * dy <= plate.radius * plate.radius) {
                    plate.isMoving = true;
                    plate.movingRight = !plate.movingRight;
                    glutTimerFunc(0, updatePlates, 0);
                    return; // Exit after click
                }
            }

            // 🪞 Mirror click
            float dx = oglX + 0.85f;
            float dy = oglY - 0.25f;
            if ((dx * dx) / (0.07f * 0.07f) + (dy * dy) / (0.22f * 0.22f) <= 1.0f) {
    mirrorCracked = !mirrorCracked;

    if (!showJumpScare) {
        showJumpScare = true;
        jumpScareDuration = 20; // ~20 frames = 600ms
        #ifdef _WIN32
        PlaySound("jumpscare.wav", NULL, SND_FILENAME | SND_ASYNC);
        #endif
        glutTimerFunc(30, updateJumpScare, 0); // no need for lambda here

    }

    glutPostRedisplay();
}

        }

        // 🕯️ Candle hover (always check)
        for (auto& candle : candles) {
            float cx = candle.x;
            float cy = -0.06f + 0.12f + candle.height;

            float dx = oglX - cx;
            float dy = oglY - cy;

            candle.hovered = (dx * dx + dy * dy <= 0.02f * 0.02f);
        }

        glutPostRedisplay(); // 🔁 Update display

}


void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    srand(time(0));

    initPlates();
    initCandles();

}

void CandleHolder(){

    glColor3f(0.7f, 0.7f, 0.6f);
    glBegin(GL_POLYGON);
        glVertex2f(-0.13f, -0.03f);
        glVertex2f(0.13f, -0.03f);
        glVertex2f(0.13f, -0.05f);
        glVertex2f(-0.13f, -0.05f);

    glEnd();


}
void wallClock() {

    // Clock Body
    glColor3f(0.5f, 0.4f, 0.8f);
    glBegin(GL_POLYGON);
        glVertex2f(-0.55f, 0.6f);
        glVertex2f(-0.47f, 0.6f);
        glVertex2f(-0.47f, 0.32f);
        glVertex2f(-0.55f, 0.32f);
    glEnd();

        // --- Clock Dial Center ---
    float centerX = -0.51f;
    float centerY = 0.55f;

    glColor3f(0.9f, 0.9f, 0.9f); // light gray dial background
    float dialRadius = 0.035f;
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(centerX, centerY);
        for (int i = 0; i <= 100; ++i) {
            float angle = i * 2.0f * M_PI / 100;
            float x = centerX + dialRadius * cos(angle);
            float y = centerY + dialRadius * sin(angle);
            glVertex2f(x, y);
        }
    glEnd();

    // --- Clock Hands ---
    // Minute Hand (longer)
    glLineWidth(2.0f);
    glColor3f(0.0f, 0.0f, 0.0f);  // black
    glBegin(GL_LINES);
        glVertex2f(centerX, centerY);
        glVertex2f(centerX, centerY + 0.03f); // vertical up
    glEnd();

    // Hour Hand (shorter)
    glLineWidth(3.0f);
    glColor3f(0.2f, 0.2f, 0.2f);  // dark gray
    glBegin(GL_LINES);
        glVertex2f(centerX, centerY);
        glVertex2f(centerX + 0.015f, centerY); // right side
    glEnd();

    // Dial Center Point
    glPointSize(5.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POINTS);
        glVertex2f(centerX, centerY);
    glEnd();


    // --- Pendulum (rotated around pivot) ---
    glPushMatrix();

    float cx = -0.51f;  // Pivot X
    float cy = 0.48f;   // Pivot Y

    glTranslatef(cx, cy, 0.0f);  // Move to pivot point
    float swing = 15.0f * sin(pendulumAngle * M_PI / 180.0f);  // Swing angle in degrees
    glRotatef(swing, 0.0f, 0.0f, 1.0f);  // Rotate about Z-axis
    glTranslatef(-cx, -cy, 0.0f);  // Move back

    // Pendulum Stick
    glLineWidth(2.5f);
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_LINES);
        glVertex2f(cx, cy);
        glVertex2f(cx, 0.36f);
    glEnd();

    // Pendulum Bob (circle)
    float pendulumY = 0.36f;
    float pradius = 0.015f;
    glColor3f(0.8f, 0.6f, 0.2f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, pendulumY);
        for (int i = 0; i <= 100; i++) {
            float angle = i * 2.0f * M_PI / 100;
            float x = cx + pradius * cos(angle);
            float y = pendulumY + pradius * sin(angle);
            glVertex2f(x, y);
        }
    glEnd();

    glPopMatrix();
}




void drawScene2() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

     glDisable(GL_DEPTH_TEST);
    room();

    drawWindow();
    wallClock();

    drawCandles();
    CandleHolder();
    drawCarpet();
    drawtable();
    fireplace();
    drawChandelier();

    drawMirror();
    drawPlates();

    if (isNight) {
    glColor4f(0.0f, 0.0f, 0.1f, 0.3f); // dark blue overlay
    glBegin(GL_POLYGON);
        glVertex2f(-1.0f, -1.0f);
        glVertex2f(1.0f, -1.0f);
        glVertex2f(1.0f, 1.0f);
        glVertex2f(-1.0f, 1.0f);
    glEnd();
}

    if (showJumpScare) {
    glPushMatrix();

    // Dim white background to give ghostly appearance
    glColor3f(1.0f, 1.0f, 1.0f); // white
    drawCircle(0.0f, 0.0f, 0.4f); // ghost head/body

    // Eyes (black circles)
    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircle(-0.12f, 0.15f, 0.05f); // left eye
    drawCircle(0.12f, 0.15f, 0.05f);  // right eye

    // Open mouth (oval shape)
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; ++i) {
        float angle = 2.0f * 3.1416f * i / 100;
        float x = 0.05f * cos(angle);
        float y = 0.08f * sin(angle);
        glVertex2f(x, y - 0.1f);  // mouth lower
    }
    glEnd();

    // Wavy bottom (like ghost sheet)
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLE_STRIP);
    for (float x = -0.4f; x <= 0.4f; x += 0.05f) {
        float y = -0.4f + 0.03f * sin(x * 20);
        glVertex2f(x, -0.2f);
        glVertex2f(x, y);
    }
    glEnd();

    glPopMatrix();
}
 glEnable(GL_DEPTH_TEST);
    glFlush();

    glutSwapBuffers();
}

/*void display() {
    if (isScene2) {
        drawScene2();
    } else {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        glColor3f(1, 1, 1);
        glRasterPos2f(-0.4f, 0.0f);
        const char* msg = "Press '2' to enter Scene 2";
        const char* msg2 = "Use 'r'=Rain, 'n'=Night, 'c'=Candle Flicker";
const char* msg3 = "Click Mirror, Plates, Candles to Interact";

glRasterPos2f(-0.5f, -0.1f);
for (int i = 0; msg2[i] != '\0'; i++)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, msg2[i]);

glRasterPos2f(-0.5f, -0.2f);
for (int i = 0; msg3[i] != '\0'; i++)
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, msg3[i]);

        for (int i = 0; msg[i] != '\0'; i++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, msg[i]);
        glutSwapBuffers();
    }
}
*/

// ------------------ Plate Animation Code ------------------


void initPlates() {
    plates.clear();
    plates.push_back({ -0.2f, -0.3f, 0.05f, true, false }); // plate 1
    plates.push_back({ 0.2f, -0.3f, 0.05f, false, false }); // plate 2
}

void initCandles() {
    candles.clear();
    candles.push_back({ -0.12f, 0.12f, false });  // left
    candles.push_back({ 0.0f,  0.16f, false });   // middle (taller)
    candles.push_back({ 0.12f, 0.12f, false });   // right
}


/*void drawCircle(float cx, float cy, float r, int segments) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; i++) {
        float angle = 2.0f * 3.1416f * i / segments;
        float x = r * cos(angle);
        float y = r * sin(angle);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}*/

void drawCandles() {
    glPushMatrix();
    glTranslatef(0.0f, -0.18f, 0.0f); // Position on table

    // Base holder
    glColor3f(0.7f, 0.7f, 0.6f);
    glBegin(GL_POLYGON);
        glVertex2f(-0.15f, 0.0f);
        glVertex2f(0.15f, 0.0f);
        glVertex2f(0.12f, -0.05f);
        glVertex2f(-0.12f, -0.05f);

    glEnd();


    // Stem
    glBegin(GL_POLYGON);
        glVertex2f(-0.02f, 0.0f);
        glVertex2f(0.02f, 0.0f);
        glVertex2f(0.015f, 0.12f);
        glVertex2f(-0.015f, 0.12f);
    glEnd();

    // Draw candles
    for (const auto& candle : candles) {
        glColor3f(1.0f, 1.0f, 0.9f); // Candle body
        glBegin(GL_POLYGON);
            glVertex2f(candle.x - 0.01f, 0.12f);
            glVertex2f(candle.x + 0.01f, 0.12f);
            glVertex2f(candle.x + 0.01f, 0.12f + candle.height);
            glVertex2f(candle.x - 0.01f, 0.12f + candle.height);
        glEnd();

        // Flame
        float flameHeight = 0.02f + (rand() % 10) / 5000.0f;
        if (candle.hovered) flameHeight += 0.01f;

        float flameY = 0.12f + candle.height + candleFlameOffset;

        if (isNight){
    glColor3f(1.0f, 0.7f, 0.2f);  // warmer at night
}else{
    glColor3f(1.0f, 0.5f, 0.0f);
}
        glBegin(GL_TRIANGLES);
            glVertex2f(candle.x, flameY + flameHeight);
            glVertex2f(candle.x - 0.01f, flameY);
            glVertex2f(candle.x + 0.01f, flameY);
        glEnd();
    }

    glPopMatrix();
}


void drawPlates() {
    for (const auto& plate : plates) {
        glColor3f(0.9f, 0.9f, 0.9f); // light color
        drawCircle(plate.x, plate.y, plate.radius);
    }
}

void updatePlates(int value) {
    bool anyMoving = false;

    for (auto& plate : plates) {
        if (plate.isMoving) {
            float step = 0.004f;
            anyMoving = true;

            if (plate.movingRight) {
                plate.x += step;
                if (plate.x > 0.45f) plate.isMoving = false;
            } else {
                plate.x -= step;
                if (plate.x < -0.45f) plate.isMoving = false;
            }
        }
    }

    if (anyMoving) glutPostRedisplay();
    glutTimerFunc(16, updatePlates, 0); // ~60 FPS
}
void updateCandleFlames(int value) {
    if (candleFlickering) {
        candleFlameOffset = ((rand() % 6) - 3) / 2000.0f;  // subtle flicker
        glutPostRedisplay();
    }
    glutTimerFunc(100, updateCandleFlames, 0);
}

void updateJumpScare(int value) {
    if (jumpScareDuration > 0) {
        jumpScareDuration--;
        glutPostRedisplay();
        glutTimerFunc(30, updateJumpScare, 0);
    } else {
        showJumpScare = false;
        glutPostRedisplay();
    }
}

void updatePendulum(int value) {
    pendulumAngle += 2.0f;  // Increase angle (speed of swing)
    if (pendulumAngle > 360) pendulumAngle -= 360;

    glutPostRedisplay();              // Redraw the scene
    glutTimerFunc(16, updatePendulum, 0); // Call again after ~16 ms (60 FPS)
}


void handleResize(int w, int h) {

    windowWidth = w;
    windowHeight = h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}
//////////////////////////////////////////////////
//Scene-3
bool showGhost = false;
float ghostY = -1.0f;         // Starting vertical position
float ghostSpeed = 0.02f;     // Speed of upward movement
int ghostDelay = 5000;        // 5 seconds
int ghostStartTime = 0;
float rainOffset = 0.0f;
float rainSpeed = 0.05f; // speed of rain
bool rainActive = false;  // Initially rain is off
int bulbBlinkInterval = 1000;
struct Point {
    float x, y;
};

// --- Door ---
Point A = {-1.0f, 4.0f}, B = {-1.0f, 0.0f}, C = {1.0f, 0.0f}, D = {1.0f, 4.0f};

// --- Left Wall ---
Point E = {-8.0f, 8.0f}, F = {-8.0f, -4.0f}, I = {-8.0f, 6.0f}, J = {-1.0f, 2.98f};
Point K = {-1.0f, 0.87f}, L = {-8.0f, -1.73f};

// --- Right Wall ---
Point G = {8.0f, 8.0f}, H = {8.0f, -4.0f}, M = {1.0f, 3.15f}, N = {8.0f, 6.0f};
Point O = {8.0f, -1.64f}, P = {1.0f, 0.93f};

// --- Wallmate Polygon ---
Point B1 = {3.0f, 3.0f}, C1 = {5.01f, 3.58f}, D1 = {5.01f, 1.66f}, E1 = {3.01f, 1.5f};
// --- Left Wallmate (matching right wall's angle) ---
Point WM2_A = {-7.0f, 4.6f}; // Top-left
Point WM2_B = {-2.6f, 3.0f}; // Top-right
Point WM2_C = {-2.6f, 1.5f}; // Bottom-right
Point WM2_D = {-7.0f, 2.1f}; // Bottom-left


// --- Bulb Coordinates ---
Point W = {0.0f, 7.26f}, Z = {-0.92f, 6.46f}, A1 = {0.82f, 6.46f};  // Bulb 1
Point T = {0.0f, 6.0f},  U = {-0.61f, 5.39f}, V = {0.53f, 5.39f};  // Bulb 2
Point Q = {0.0f, 5.0f},  R = {0.37f, 4.61f}, S = {-0.37f, 4.61f};  // Bulb 3

// --- Ghost Points ---
Point F1 = {-1.02f, -1.4f};
Point G1 = {0.97917f, -1.48696f};
Point H1 = {-1.58f, -3.58f};
Point I1 = {1.59f, -3.59f};

// --- Blinking Bulb Control ---
bool bulbsOn = true;

// --- Door color toggle control ---
bool doorIsBrown = true;
int doorColorToggleInterval = 5000; // milliseconds (5 seconds)

// --- Utility to draw quads ---
void drawQuad(Point p1, Point p2, Point p3, Point p4, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
        glVertex2f(p1.x, p1.y);
        glVertex2f(p2.x, p2.y);
        glVertex2f(p3.x, p3.y);
        glVertex2f(p4.x, p4.y);
    glEnd();
}

void drawGhost() {
    if (!showGhost) return; //  until 5 seconds

    float offsetY = ghostY;

    glColor3f(1.0f, 1.0f, 1.0f);

    // Semicircle head
    float cx = 0.0f, cy = -1.0f + offsetY;
    float radius = 1.2f;
    int numSegments = 100;

    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for (int i = 0; i <= numSegments; ++i) {
            float angle = M_PI * i / numSegments;
            float x = radius * cos(angle) + cx;
            float y = radius * sin(angle) + cy;
            glVertex2f(x, y);
        }
    glEnd();

    // Wavy bottom
    glBegin(GL_POLYGON);
        glVertex2f(cx - radius, cy);
        for (int i = 0; i <= 4; ++i) {
            float arcCenter = cx - radius + i * (radius * 2 / 4);
            for (int j = 0; j <= numSegments / 4; ++j) {
                float angle = M_PI + (M_PI * j / (numSegments / 4));
                float x = 0.3f * cos(angle) + arcCenter;
                float y = 0.3f * sin(angle) + (cy - 1.0f);
                glVertex2f(x, y);
            }
        }
        glVertex2f(cx + radius, cy);
    glEnd();

    // Eyes
    glColor3f(0.0f, 0.0f, 0.0f);
    float eyeY = cy + 0.4f;
    float eyeOffset = 0.4f;
    float eyeRadius = 0.15f;

    for (int i = 0; i < 2; ++i) {
        float eyeX = cx + (i == 0 ? -eyeOffset : eyeOffset);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(eyeX, eyeY);
        for (int j = 0; j <= 20; ++j) {
            float angle = 2 * M_PI * j / 20;
            float x = eyeRadius * cos(angle) + eyeX;
            float y = eyeRadius * sin(angle) + eyeY;
            glVertex2f(x, y);
        }
        glEnd();
    }
}

// --- Components Drawing Functions ---
void drawDoor() {
    if (doorIsBrown) {
        // Brown color
        drawQuad(A, B, C, D, 0.4f, 0.2f, 0.1f);
    } else {
        // Black color
        drawQuad(A, B, C, D, 0.0f, 0.0f, 0.0f);
    }
}

void drawLeftWall() {
    glColor3f(0.2f, 0.25f, 0.3f);
    glBegin(GL_POLYGON);
        glVertex2f(A.x, A.y);
        glVertex2f(B.x, B.y);
        glVertex2f(L.x, L.y);
        glVertex2f(F.x, F.y);
        glVertex2f(E.x, E.y);
    glEnd();
}

void drawRightWall() {
    glColor3f(0.2f, 0.25f, 0.3f);
    glBegin(GL_POLYGON);
        glVertex2f(D.x, D.y);
        glVertex2f(C.x, C.y);
        glVertex2f(H.x, H.y);
        glVertex2f(G.x, G.y);
    glEnd();
}

void drawWallMate() {
    // Wallmate background - night sky
    glColor3f(0.0f, 0.0f, 0.3f); // Dark blue sky
    glBegin(GL_POLYGON);
        glVertex2f(B1.x, B1.y);
        glVertex2f(C1.x, C1.y);
        glVertex2f(D1.x, D1.y);
        glVertex2f(E1.x, E1.y);
    glEnd();

    // Draw moon
    glColor3f(1.0f, 1.0f, 0.8f); // Pale yellow-white moon
    float moonX = 4.5f, moonY = 3.2f, moonRadius = 0.2f;
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(moonX, moonY);
        for (int i = 0; i <= 100; ++i) {
            float angle = 2 * M_PI * i / 100;
            float x = moonRadius * cos(angle) + moonX;
            float y = moonRadius * sin(angle) + moonY;
            glVertex2f(x, y);
        }
    glEnd();

    // Draw light green hills
    glColor3f(0.3f, 0.5f, 0.2f); // Light green
    glBegin(GL_POLYGON);
        glVertex2f(3.0f, 1.7f);
        glVertex2f(3.4f, 2.1f);
        glVertex2f(3.8f, 1.9f);
        glVertex2f(4.3f, 2.3f);
        glVertex2f(4.7f, 1.85f);
        glVertex2f(5.0f, 1.7f);
        glVertex2f(5.01f, 1.66f);
        glVertex2f(3.01f, 1.5f);
    glEnd();

    // Frame border
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(B1.x, B1.y);
        glVertex2f(C1.x, C1.y);
        glVertex2f(D1.x, D1.y);
        glVertex2f(E1.x, E1.y);
    glEnd();
}

void drawLeftWallMate() {
    // Background
    glColor3f(0.05f, 0.0f, 0.2f); // Night purple-blue
    glBegin(GL_POLYGON);
        glVertex2f(WM2_A.x, WM2_A.y);
        glVertex2f(WM2_B.x, WM2_B.y);
        glVertex2f(WM2_C.x, WM2_C.y);
        glVertex2f(WM2_D.x, WM2_D.y);
    glEnd();


    // Hills
    glColor3f(0.3f, 0.5f, 0.2f);
    glBegin(GL_POLYGON);
        glVertex2f(-7.0f, 2.2f);
        glVertex2f(-6.0f, 2.6f);
        glVertex2f(-5.0f, 2.3f);
        glVertex2f(-3.8f, 2.7f);
        glVertex2f(-2.6f, 1.5f);
        glVertex2f(-7.0f, 1.5f);
    glEnd();
// Animated Rain
glColor3f(0.7f, 0.8f, 1.0f); // Light blue rain color

for (int i = 0; i < 3; ++i) {
    float t = 0.2f + i * 0.3f;

    float topX = WM2_A.x + t * (WM2_B.x - WM2_A.x);
    float topY = WM2_A.y + t * (WM2_B.y - WM2_A.y);
    float botX = WM2_D.x + t * (WM2_C.x - WM2_D.x);
    float botY = WM2_D.y + t * (WM2_C.y - WM2_D.y);

    float wallHeight = topY - botY;

    // raindrops loop down continuously
    float y = topY - fmod(rainOffset, wallHeight);
    float x = topX;

    glBegin(GL_LINES);
        glVertex2f(x, y);
        glVertex2f(x + 0.03f, y - 0.2f); // Slight diagonal drop
    glEnd();
}

}



void drawCeiling() {
    drawQuad(E, G, D, A,0.2745f, 0.2745f, 0.2745f);
}

void drawFloor() {
    drawQuad(F, H, C, B, 0.3f, 0.0f, 0.0f);
}

void drawFloorPatch() {
    glColor3f(0.2f, 0.25f, 0.3f);
    glBegin(GL_TRIANGLES);
        glVertex2f(B.x, B.y);
        glVertex2f(L.x, L.y);
        glVertex2f(F.x, F.y);
    glEnd();
}

void drawDecore1() {
    glColor3f(0.25f, 0.3f, 0.35f);
    glBegin(GL_POLYGON);
        glVertex2f(I.x, I.y);
        glVertex2f(J.x, J.y);
        glVertex2f(K.x, K.y);
        glVertex2f(L.x, L.y);
    glEnd();
}

void drawDecore2() {
    glColor3f(0.25f, 0.3f, 0.35f);
    glBegin(GL_POLYGON);
        glVertex2f(M.x, M.y);
        glVertex2f(N.x, N.y);
        glVertex2f(O.x, O.y);
        glVertex2f(P.x, P.y);
    glEnd();
}

void drawBulb1() {
    if (bulbsOn)
        glColor3f(1.0f, 1.0f, 0.0f); // Bright yellow
    else
        glColor3f(0.4f, 0.4f, 0.0f); // Dim yellow

    glBegin(GL_TRIANGLES);
        glVertex2f(W.x, W.y);
        glVertex2f(Z.x, Z.y);
        glVertex2f(A1.x, A1.y);
    glEnd();
}


void drawBulb2() {
    if (bulbsOn)
        glColor3f(1.0f, 1.0f, 0.0f);
    else
        glColor3f(0.4f, 0.4f, 0.0f);

    glBegin(GL_TRIANGLES);
        glVertex2f(T.x, T.y);
        glVertex2f(U.x, U.y);
        glVertex2f(V.x, V.y);
    glEnd();
}


void drawBulb3() {
    if (bulbsOn)
        glColor3f(1.0f, 1.0f, 0.0f);
    else
        glColor3f(0.4f, 0.4f, 0.0f);

    glBegin(GL_TRIANGLES);
        glVertex2f(Q.x, Q.y);
        glVertex2f(R.x, R.y);
        glVertex2f(S.x, S.y);
    glEnd();
}

void drawSpiderWeb(float cx, float cy, float radius, int layers, int spokes) {
    glColor3f(0.7f, 0.7f, 0.7f); // light gray

    // Draw radial lines
    for (int i = 0; i < spokes; ++i) {
        float angle = 2 * M_PI * i / spokes;
        float x = radius * cos(angle) + cx;
        float y = radius * sin(angle) + cy;
        glBegin(GL_LINES);
            glVertex2f(cx, cy);
            glVertex2f(x, y);
        glEnd();
    }

    // Draw concentric arcs (web rings)
    for (int j = 1; j <= layers; ++j) {
        float r = radius * j / layers;
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < spokes; ++i) {
            float angle = 2 * M_PI * i / spokes;
            float x = r * cos(angle) + cx;
            float y = r * sin(angle) + cy;
            glVertex2f(x, y);
        }
        glEnd();
    }
}
void drawSpider(float cx, float cy, float radius) {
    glColor3f(0.0f, 0.0f, 0.0f); // black spider
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for (int i = 0; i <= 20; ++i) {
            float angle = 2 * M_PI * i / 20;
            float x = radius * cos(angle) + cx;
            float y = radius * sin(angle) + cy;
            glVertex2f(x, y);
        }
    glEnd();
}
void drawMovingSpider(float cx, float cy, float radius, float timeOffset) {
    float angle = fmod(glutGet(GLUT_ELAPSED_TIME) / 500.0f + timeOffset, 2 * M_PI);
    float r = radius * 0.8f;  // Move spider on inner ring
    float x = r * cos(angle) + cx;
    float y = r * sin(angle) + cy;

    // Draw spider body (black circle)
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for (int i = 0; i <= 20; ++i) {
            float a = 2 * M_PI * i / 20;
            glVertex2f(x + 0.08f * cos(a), y + 0.08f * sin(a));
        }
    glEnd();
}
void drawCockroach(float x, float y) {
   glColor3f(0.0f, 0.0f, 0.0f); // pure black

    glBegin(GL_POLYGON);
    for (int i = 0; i <= 20; ++i) {
        float angle = 2 * M_PI * i / 20;
        glVertex2f(x + 0.1f * cos(angle), y + 0.05f * sin(angle)); // Oval shape
    }
    glEnd();

    // Antennae
    glBegin(GL_LINES);
        glVertex2f(x - 0.08f, y + 0.04f);
        glVertex2f(x - 0.15f, y + 0.1f);

        glVertex2f(x + 0.08f, y + 0.04f);
        glVertex2f(x + 0.15f, y + 0.1f);
    glEnd();
}

float spiderAngle = 0.0f;  // Angle for moving spider
float spiderSpeed = 0.05f; // Speed of movement


// --- Cockroach Control ---
bool cockroachActive = false;

const int numCockroaches = 8;

float cockroachX[numCockroaches] = {
    -8.0f, -7.0f, -6.0f, -5.0f,
    -4.0f, -3.0f, -2.0f, -1.0f
};

float cockroachY[numCockroaches] = {
    -3.5f, -3.2f, -3.8f, -3.4f,
    -3.6f, -3.1f, -3.3f, -3.7f
};

float cockroachSpeed = 0.05f;




// --- Initialization ---
/*void init() {
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // background will be black
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(-10, 10, -5, 10);
}*/

// --- Display Scene ---
void drawScene3() {
    glDisable(GL_DEPTH_TEST);

    drawCeiling();
    drawFloor();
    drawFloorPatch();
    drawDoor();
    drawLeftWall();
    drawRightWall();
    drawDecore1();
    drawDecore2();
    drawBulb1();
    drawBulb2();
    drawBulb3();
    drawWallMate();
    drawLeftWallMate();
    drawGhost();

    drawSpiderWeb(7.5f, 5.5f, 0.5f, 3, 6);
    drawSpiderWeb(-1.5f, 4.8f, 0.4f, 3, 6);
    drawSpiderWeb(1.5f, 4.8f, 0.4f, 3, 6);

    drawSpider(7.5f, 5.5f, 0.1f);
    drawSpider(-1.5f, 4.8f, 0.08f);
    drawSpider(1.5f, 4.8f, 0.08f);

    drawMovingSpider(7.5f, 5.5f, 0.5f, 0.0f);
    drawMovingSpider(-1.5f, 4.8f, 0.4f, 1.0f);
    drawMovingSpider(1.5f, 4.8f, 0.4f, 2.0f);

    float centerX = 0.0f, centerY = 7.2f, moveRadius = 0.4f;
    float spiderX = centerX + moveRadius * cos(spiderAngle);
    float spiderY = centerY + moveRadius * sin(spiderAngle);
    drawSpiderWeb(centerX, centerY, 0.5f, 4, 8);
    drawSpider(spiderX, spiderY, 0.1f);

    if (cockroachActive) {
        for (int i = 0; i < numCockroaches; ++i) {
            drawCockroach(cockroachX[i], cockroachY[i]);
        }
    }

    glEnable(GL_DEPTH_TEST);
}
//////////////////////////////////////////////////////
//Scene-4
struct Bird   { float x,y,speed,base;    };
struct Flower { float x,y,phase; bool hover,glow; };
struct Leaf   { float x,y,vy,vx;         };
struct Bug    { float x,y,vx,vy,baseY,size; };

float  sunY            = -0.42f;
const  float sunRadius = 0.15f;


bool   sunriseActive   = false;
bool   birdsActive     = false;
bool   flowerSway      = false;


float  skyBlend        = 0.0f;

std::vector<Bird>   birds;
std::vector<Flower> flowers;
std::vector<Leaf>   leaves;
std::vector<Bug>    bugs;

bool treeHover=false, groundHover=false, horizonHover=false;
// NEW separate activation flags:
bool leavesActive = false;   // leaves fall only after 'L' key press
bool beesActive   = false;   // bees fly only after 'B' key press


// ╔═ Helpers ══════════════════════════════════════════════════╗
inline float rnd(float a,float b){ return a+(b-a)*rand()/float(RAND_MAX); }
void circle(float cx,float cy,float r,int seg=64){
    glBegin(GL_TRIANGLE_FAN);
    for(int i=0;i<=seg;i++){
        float th=2*M_PI*i/seg;
        glVertex2f(cx+cos(th)*r, cy+sin(th)*r);
    }
    glEnd();
}

// ╔═ Scene pieces ════════════════════════════════════════════╗
void gradientSky(){
    float t=skyBlend;
    glBegin(GL_QUADS);
      glColor3f(0.0f*(1-t), 0.1f*(1-t)+0.55f*t, 0.2f*(1-t)+0.8f*t);
      glVertex2f(-1,1); glVertex2f(1,1);
      glColor3f(0.05f*(1-t)+t, 0.05f*(1-t)+0.6f*t, 0.1f*(1-t)+0.2f*t);
      glVertex2f(1,-0.1f); glVertex2f(-1,-0.1f);
    glEnd();
}
void sun(){
    float r=sunRadius*(horizonHover?1.18f:1);
    glColor3f(1,0.85f,(horizonHover?0.45f:0));
    circle(0.5f,sunY,r);
    if(horizonHover){
        glColor4f(1,0.9f,0.5f,0.35f);  circle(0.5f,sunY,r*2.2f,48);
        glColor4f(1,0.9f,0.5f,0.18f);  circle(0.5f,sunY,r*3.3f,48);
    }
}
void ground(){
    glColor3f(0.1f,0.3f+0.3f*skyBlend,0.15f);
    glBegin(GL_POLYGON); glVertex2f(-1,-0.6f); glVertex2f(1,-0.6f);
                         glVertex2f(1,-0.1f); glVertex2f(-1,-0.1f); glEnd();
    if(groundHover){
        glColor3f(0.16f,0.42f,0.16f);
        for(float x=-1;x<1;x+=0.05f){
            glBegin(GL_LINES);
              glVertex2f(x,-0.1f);
              glVertex2f(x,-0.05f+0.02f*std::sin(20*x+glutGet(GLUT_ELAPSED_TIME)*0.006));
            glEnd();
        }
    }
}
void bushes(){
    glColor3f(0.03f,0.4f,0.18f);

    // Right cluster
    circle(0.35f,-0.05f,0.14f);
    circle(0.55f,-0.05f,0.13f);
}
void extraBushes(){
    glColor3f(0.05f, 0.4f, 0.2f);

    // Left side of haunted house
    circle(-0.8f, -0.05f, 0.07f);
    circle(-0.87f, -0.05f, 0.07f);

    // Right side of haunted house
    circle(-0.2f, -0.05f, 0.07f);
    circle(-0.13f, -0.05f, 0.07f);
}
void hauntedHouse(){
    glColor3f(0.45f, 0.32f, 0.23f);
    glBegin(GL_POLYGON);
        glVertex2f(-0.7f, -0.1f); glVertex2f(-0.45f, -0.1f);
        glVertex2f(-0.45f, 0.15f); glVertex2f(-0.7f, 0.15f);
    glEnd();

    glColor3f(0.25f, 0.15f, 0.1f);
    glBegin(GL_TRIANGLES);
        glVertex2f(-0.7f, 0.15f); glVertex2f(-0.575f, 0.3f); glVertex2f(-0.45f, 0.15f);
    glEnd();

    glColor3f(0.4f, 0.3f, 0.2f);
    glBegin(GL_POLYGON);
        glVertex2f(-0.45f, -0.1f); glVertex2f(-0.3f, -0.1f);
        glVertex2f(-0.3f, 0.08f);  glVertex2f(-0.45f, 0.08f);
    glEnd();

    glColor3f(0.2f, 0.1f, 0.05f);
    glBegin(GL_TRIANGLES);
        glVertex2f(-0.45f, 0.08f); glVertex2f(-0.375f, 0.18f); glVertex2f(-0.3f, 0.08f);
    glEnd();

    glColor3f(0.35f, 0.25f, 0.2f);
    glBegin(GL_POLYGON);
        glVertex2f(-0.6f, 0.15f); glVertex2f(-0.52f, 0.15f);
        glVertex2f(-0.52f, 0.4f); glVertex2f(-0.6f, 0.4f);
    glEnd();

    glColor3f(0.2f, 0.1f, 0.1f);
    glBegin(GL_TRIANGLES);
        glVertex2f(-0.6f, 0.4f); glVertex2f(-0.56f, 0.5f); glVertex2f(-0.52f, 0.4f);
    glEnd();

    glColor3f(0.1f, 0.1f, 0.1f);
    for (float x = -0.68f; x <= -0.5f; x += 0.09f) {
        glBegin(GL_POLYGON);
            glVertex2f(x, 0.0f); glVertex2f(x+0.03f, 0.0f);
            glVertex2f(x+0.03f, 0.05f); glVertex2f(x, 0.05f);
        glEnd();
    }

    glBegin(GL_POLYGON);
        glVertex2f(-0.42f, 0.0f); glVertex2f(-0.39f, 0.0f);
        glVertex2f(-0.39f, 0.05f); glVertex2f(-0.42f, 0.05f);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex2f(-0.58f, 0.25f); glVertex2f(-0.55f, 0.25f);
        glVertex2f(-0.55f, 0.3f);  glVertex2f(-0.58f, 0.3f);
    glEnd();
}
void tree(){
    glColor3f(0.35f,0.25f,0.15f);
    glBegin(GL_POLYGON); glVertex2f(0.75f,-0.1f); glVertex2f(0.8f,-0.1f);
                         glVertex2f(0.78f,0.3f);  glVertex2f(0.76f,0.3f); glEnd();
    float rust=treeHover?0.02f*std::sin(glutGet(GLUT_ELAPSED_TIME)*0.01f):0;
    glColor3f(0,0.55f,0);
    circle(0.77f+rust,0.4f,0.2f); circle(0.85f+rust,0.4f,0.15f);
    circle(0.68f+rust,0.4f,0.15f); circle(0.77f+rust,0.5f,0.15f);
}
void fallingLeaves(){
     if(!leavesActive) return;
    glColor3f(0,0.45f,0);
    for(auto &L:leaves) circle(L.x,L.y,0.018f);
}
void flowerPatch(){
    if(flowers.empty()) return;
    for(auto &f:flowers){
        float amp = flowerSway?0.02f:0.0f;
        float scale=(f.hover||f.glow)?1.35f:1.0f;
        float sway = amp*std::sin(glutGet(GLUT_ELAPSED_TIME)*0.004f+f.phase);
        glColor3f(1,(f.hover||f.glow)?0.45f:0.35f,0.55f);
        circle(f.x+sway, f.y,              0.02f*scale);
        circle(f.x+0.025f+sway, f.y,       0.02f*scale);
        circle(f.x+0.0125f+sway,f.y+0.025f,0.02f*scale);
    }
}
void birdShapes(){
    if(birds.empty()) return;
    float t = glutGet(GLUT_ELAPSED_TIME) * 0.005f;  // Time for animation
    glColor3f(0, 0, 0);
    for (auto& b : birds) {
        float flap = 0.0f;
        if (birdsActive) {
            flap = 0.01f * sin(t + b.base);  // Flapping only if birdsActive
        }
        glBegin(GL_TRIANGLES);
            glVertex2f(b.x, b.y);                        // Body tip
            glVertex2f(b.x + 0.04f, b.y + 0.015f + flap); // Upper wing
            glVertex2f(b.x + 0.04f, b.y - 0.015f - flap); // Lower wing
        glEnd();
    }
}
// ─────────── Road ─────────────
void Road() {
    // Road base
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, -1.0f);
        glVertex2f(1.0f, -1.0f);
        glVertex2f(1.0f, -0.6f);
        glVertex2f(-1.0f, -0.6f);
    glEnd();

    // Road stripes (white dashes)
    glColor3f(1.0f, 1.0f, 1.0f);
    float startX = -1.0f;
    float stripeWidth = 0.1f;
    float gap = 0.1f;
    float y1 = -0.8f;
    float y2 = -0.75f;
    for(float x = startX; x < 1.0f; x += stripeWidth + gap) {
        glBegin(GL_QUADS);
            glVertex2f(x, y1);
            glVertex2f(x + stripeWidth, y1);
            glVertex2f(x + stripeWidth, y2);
            glVertex2f(x, y2);
        glEnd();
    }
}
// ─────────── Bugs (Bees) ─────────────
void initBugs() {
    bugs.clear();
    for (int i=0; i<5; ++i) {
        float fx = 0.6f + 0.05f * (i%3);
        float fy = -0.25f + 0.03f * (i/3);
        bugs.push_back({fx, fy, rnd(-0.0005f,0.0005f), rnd(-0.0003f,0.0003f), fy, 0.01f});
    }
}
void updateBugs() {
    if(!beesActive) return;  // Bugs move only if beesActive true
    for (auto &b : bugs) {
        b.x -= b.vx;
        b.y -= b.vy;

        b.y = b.baseY + 0.02f * sin(glutGet(GLUT_ELAPSED_TIME)*0.005f + b.x*10);

        if (b.x < 0.55f) b.vx = fabs(b.vx);
        if (b.x > 0.75f) b.vx = -fabs(b.vx);
        if (b.y < b.baseY - 0.05f) b.vy = fabs(b.vy);
        if (b.y > b.baseY + 0.05f) b.vy = -fabs(b.vy);
    }
}
void drawBugs() {
    if(!beesActive) return;  // Draw only if beesActive true
    glColor3f(1, 0.85f, 0);  // yellowish bee color
    for (auto &b : bugs) {
        circle(b.x, b.y, b.size);
        glColor3f(1,1,1);
        circle(b.x - b.size*1.2f, b.y + b.size*0.5f, b.size*0.6f);
        circle(b.x + b.size*1.2f, b.y + b.size*0.5f, b.size*0.6f);
        glColor3f(1, 0.85f, 0);
    }
}

// ╔═ Mouse helpers ════════════════════════════════════════════╗
float mx=-2,my=-2;
void toWorld(int x,int y){
    int w=glutGet(GLUT_WINDOW_WIDTH), h=glutGet(GLUT_WINDOW_HEIGHT);
    mx=2.f*x/w-1; my=-2.f*y/h+1;
}
bool inCircle(float cx,float cy,float r){ return (mx-cx)*(mx-cx)+(my-cy)*(my-cy)<=r*r; }
bool inRect(float x1,float y1,float x2,float y2){ return mx>=x1&&mx<=x2&&my>=y1&&my<=y2; }
void sceneInitEntities() {
    birds.clear();
    for(int i=0; i<5; i++) {
        birds.push_back({rnd(-1,1), rnd(0.1f,0.7f), rnd(0.002f,0.006f), rnd(0, 2*M_PI)});
    }
    flowers.clear();
    for(float x=0.55f; x<0.85f; x+=0.04f) {
        flowers.push_back({x, -0.25f, rnd(0, 2*M_PI), false, false});
    }
    initBugs();
}
void drawScene4(){
    glClear(GL_COLOR_BUFFER_BIT);
     glDisable(GL_DEPTH_TEST);
    gradientSky();
    sun();
    Road();
    ground();
    bushes();
    extraBushes();
    hauntedHouse();
    tree();
    fallingLeaves();
    flowerPatch();
    birdShapes();
    drawBugs();
     glEnable(GL_DEPTH_TEST);
    glFlush();
}
// ╔═ Update ═══════════════════════════════════════════════════╗
void update(int){
    if(sunriseActive && sunY < 0.65f){
        sunY += 0.0005f;
        skyBlend = std::min(1.f, skyBlend + 0.00025f);
    }

    // Leaves fall only if leavesActive true
    if(leavesActive){
        for(auto &L:leaves){
            L.x += L.vx;
            L.y += L.vy;
            if(L.y < 0.0f){
                // Reset leaves near tree top (x ~0.7-0.85, y ~0.5-0.8)
                L.x = rnd(0.7f, 0.85f);
                L.y = rnd(0.5f, 0.8f);
                L.vx = rnd(-0.001f, 0.001f);
                L.vy = -0.0015f - rnd(0, 0.0015f);  // slower fall
            }
        }
    }

    // Birds update if active
    if(birdsActive){
        for(auto &b:birds){
            b.x -= b.speed;
            if(b.x < -1.2f) b.x = 1.2f;
        }
    }

    updateBugs();

    glutPostRedisplay();
    glutTimerFunc(15, update, 0);
}


// ╔═ Mouse/Keyboard interactions ══════════════════════════════╗
void passiveMotion(int x,int y){
    toWorld(x,y);

    treeHover = inRect(0.68f,-0.1f,0.85f,0.6f);
    groundHover = inRect(-1,-0.6f,1,-0.1f);
    horizonHover = inCircle(0.5f,sunY,sunRadius*1.2f);

    bool anyFlowerHover = false;
    for(auto &f:flowers){
        f.hover = inCircle(f.x, f.y, 0.025f);
        if(f.hover) anyFlowerHover = true;
    }
    flowerSway = anyFlowerHover;

    glutPostRedisplay();
}
void mouse2(int btn,int state,int x,int y){
    if(state!=GLUT_DOWN) return;
    toWorld(x,y);
    if(inRect(0.7f,-0.1f,0.83f,0.6f))   // click tree → leaf shower
        for(int i=0;i<20;i++) leaves.push_back({ rnd(0.68f,0.82f), rnd(0.4f,0.6f),
                                                 rnd(-0.004f,-0.007f), rnd(-0.002f,0.002f)});
    for(auto &f:flowers) if(inCircle(f.x,f.y,0.05f)) f.glow = !f.glow;
}


// ╔═ Setup & main ═════════════════════════════════════════════╗
void sceneInit(){
    sunY = -0.42f;
    skyBlend = 0;

    leaves.clear();
    for(int i=0; i<50; i++){
        leaves.push_back({rnd(0.65f,0.89f), rnd(0.5f,0.8f), -0.003f - rnd(0, 0.003f), rnd(-0.002f,0.002f)});
    }

    birds.clear();
    flowers.clear();
    bugs.clear();

    treeHover = false; groundHover = false; horizonHover = false;

    leavesActive = false;
    beesActive = false;
    sunriseActive = false;
    birdsActive = false;
    flowerSway = false;
}

void init2(){
    glClearColor(1,1,1,1);
    glMatrixMode(GL_PROJECTION);
     gluOrtho2D(-1,1,-1,1);
    glMatrixMode(GL_MODELVIEW);
    sceneInit();
}
void switchToScene2(int value) {
    isScene2 = true;
    glutPostRedisplay(); // Request screen redraw
}
void timer(int value) {
    static int bulbTimer = 0;
    static int doorTimer = 0;

    // === Bulb blinking ===
    bulbTimer += 30;  // Adjusted to match 30ms timer interval
    if (bulbTimer >= bulbBlinkInterval) {
        bulbsOn = !bulbsOn;
        bulbTimer = 0;
    }

    // === Door color toggle ===
    doorTimer += 30;
    if (doorTimer >= doorColorToggleInterval) {
        doorTimer = 0;
        doorIsBrown = !doorIsBrown;
    }

    // === Arrow rotation ===
    if (rotateArrowActive) {
    if (rotateClockwise)
        rotationAngle += rotationSpeed;
    else
        rotationAngle -= rotationSpeed;

    // Keep rotationAngle within 0 to 360 degrees
    if (rotationAngle >= 360.0f)
        rotationAngle -= 360.0f;
    else if (rotationAngle < 0.0f)
        rotationAngle += 360.0f;
    }

    // === Bat sway and flap ===
    swayAngle += 0.02f;
    if (swayAngle > 6.28318f) // 2*PI
        swayAngle -= 6.28318f;

    if (flapUp) {
        wingAngle += wingSpeed;
        if (wingAngle > 20.0f)
            flapUp = 0;
    } else {
        wingAngle -= wingSpeed;
        if (wingAngle < -20.0f)
            flapUp = 1;
    }

    // === Cloud movement ===
    if (cloudRunning) {
        cloudOffset += cloudSpeed;
        if (cloudOffset > windowidth) {
            cloudOffset = -cloudWidth;
        }
    }

    // === Rain movement ===
    for (auto& drop : raindrop) {
        drop.second -= 0.3f;
        if (drop.second < -6.0f) {
            drop.second = 20.0f;
            drop.first = static_cast<float>(rand() % 230) / 10.0f - 3.0f;
        }
    }

    // === Lightning Trigger ===
    if (showRain && (rand() % 100 < 2)) {
        lightning = true;
        lightningTimer = 5;

        if (!gravestoneCracked) {
            gravestoneCracked = true;
        }
    }

    // === Lightning Timer Countdown ===
    if (lightningTimer > 0) {
        --lightningTimer;
    } else {
        lightning = false;
    }

    // === Skeleton blinking ===
    if (skeletonBlinking) {
        skeletonBlinkTimer -= 30;
        if (skeletonBlinkTimer <= 0) {
            showSkeleton = !showSkeleton;
            skeletonBlinkTimer = 10000;
        }
    }

    // === Move car ===
    if (moveCar) {
        carPosX += carSpeed;
        if (carPosX >= carTargetX) {
            carPosX = carTargetX;
            moveCar = false;
        }
    }

    // === Auto headlights toggle ===
    if (autoToggleHeadlights) {
        headlightToggleTimer -= 30;
        if (headlightToggleTimer <= 0) {
            headlightsOn = !headlightsOn;
            headlightToggleTimer = 500;
        }
    }

    // === Moving spider ===
    spiderAngle += spiderSpeed;
    if (spiderAngle > 2 * M_PI) spiderAngle -= 2 * M_PI;

    // === Move ghost faster ===
    if (showGhost && ghostY < 6.0f) {

       ghostY += ghostSpeed;
    }

    glutPostRedisplay();

    // Schedule next timer call
    glutTimerFunc(30, timer, 0);
}
void rainTimer(int value) {
    if (rainActive) {
        rainOffset += rainSpeed;
        if (rainOffset > 3.0f) rainOffset = 0.0f;

        glutPostRedisplay();
        glutTimerFunc(50, rainTimer, 0); // Call again only if still active
    }
}
void cockroachTimer(int value) {
    if (cockroachActive) {
        for (int i = 0; i < numCockroaches; ++i) {
            cockroachX[i] += cockroachSpeed;
            if (cockroachX[i] > 10.0f) cockroachX[i] = -10.0f; // Loop
        }

        glutPostRedisplay();
        glutTimerFunc(100, cockroachTimer, 0); // Keep moving
    }
}

void Keypressed(unsigned char key, int x, int y) {
    if (key == 'r' || key == 'R') {
    isRaining = !isRaining;

    #ifdef _WIN32
    if (isRaining) {
        PlaySound("rain.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    } else {
        PlaySound(NULL, 0, 0); // Stop
    }
    #endif
}
    switch (key) {
        case '1':
            currentScene = 1;
            break;
        case '2':
            currentScene = 2;
            break;
        case '3':
            currentScene = 3;
            break;
        case '4':
            currentScene = 4;
            break;
        case 'c':
        case 'C':
            candleFlickering = !candleFlickering;
            break;
        case 'k':
        case 'K':
            cockroachActive = true;
            glutTimerFunc(0, cockroachTimer, 0);
            break;

        case 'f':
        case 'F':
            fireplaceOn = !fireplaceOn;
            break;
        case 'n':
        case 'N':
            isNight = !isNight;
            break;
        /*case 'r':
        case 'R':
            isRaining = !isRaining;
            playRain();
            break:*/
            // Windows sound code...
        case 'y':
        case 'Y':
            rainState = (rainState + 1) % 3;

    switch (rainState) {
        case 0:
            showRain = false;
            currentRaindrops = 0;
            PlaySound(NULL, 0, 0);
            break;
        case 1:
            showRain = true;
            currentRaindrops = 100;
            initRain();  // Reset raindrops for light rain
            playRain();
            break;
        case 2:
            showRain = true;
            currentRaindrops = 300;
            initRain();  // Reset raindrops for heavy rain
            playRain();
            break;
    }
            break;
        case 'l':
        case 'L':
            startBlinking = !startBlinking;
            if (startBlinking) {
                glutTimerFunc(0, toggleWindows, 0);
                LightBlink();
            } else {
                PlaySound(NULL, 0, 0);
            }
            leavesActive = !leavesActive;
            break;
        case 'b':
        case 'B':
            batPaused = !batPaused;
            playFlap();
            break;
        case 'a':
        case 'A':
            startBlinking = true;
            glutTimerFunc(0, toggleWindows, 0);
            LightBlink();
            showRain = true;
            rainState = 2;
            currentRaindrops = 300;
            playRain();
            moving = true;
            playFlap();
            break;
        case 'h':
        case 'H':
            headlightsOn = !headlightsOn;
            autoToggleHeadlights = headlightsOn;
            showGhost = false;
            break;
        case 'g':
        case 'G':
            showGhost = true;
            ghost2Sound();
            break;
        case 't':
        case 'T':
            if (!moveCar) {
                moveCar = true;
                glutTimerFunc(0, timer, 0);
                CarSound();
            }
            break;
        case 'd':
        case 'D':
            showBell = !showBell;
            BellSound();
            break;
        case 'w':
        case 'W':
            rotateArrowActive = !rotateArrowActive;
            break;
            case 'z':
            case'Z':
          rotateClockwise = !rotateClockwise;
          break;
        case 'o':
        case 'O':
            showRain = true;
            rainState = 1;
            currentRaindrops = 100;
            initRain();
            playRain();
            startBlinking = true;
            glutTimerFunc(10000, toggleWindows, 0);
            LightBlink();
            moving = true;
            playFlap();
            headlightsOn = true;
            autoToggleHeadlights = true;
            moveCar = true;
            CarSound();
            glutTimerFunc(5000, switchToScene2, 0);
            if (!timerStarted) {
                glutTimerFunc(30, timer, 0);
                timerStarted = true;
            }
            break;
    }
    glutPostRedisplay();
}

// Your existing key function, for example:
void key(unsigned char k,int,int){
    switch(k){
        case 's': case 'S':
            sunriseActive = true;
            if (!birds.size() || !flowers.size() || !bugs.size())
                 BirdSound();
                sceneInitEntities();


            break;
        case 'a': case 'A':
            birdsActive = !birdsActive;
            BirdSound();
            break;
        /*case 'l': case 'L':  // Leaves toggle
            leavesActive = !leavesActive;

            break;*/
        case 'b': case 'B':  // Bees toggle
            beesActive = !beesActive;
            BirdSound();
            break;
        case 'f': case 'F':
            flowerSway = !flowerSway;
            break;
        case 'c': case 'C':
            sunriseActive = false; birdsActive = false;
            leavesActive = false; beesActive = false;
            flowerSway = false;
            break;
        case 27: exit(0);
    }
    glutPostRedisplay();
}

// Merged function that calls both Keypressed and key
void KeyBoard(unsigned char keyPressed, int x, int y) {
    Keypressed(keyPressed, x, y);  // Call your main key handling
    key(keyPressed, x, y);          // Call the other key handler
}



void mouseClick(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        switch (button) {
            case GLUT_LEFT_BUTTON:
                cloudSpeed -= 0.05f;  // Slow speed
                rotationSpeed = max(0.01f, rotationSpeed - 0.2f);
                if (stainCount > 0)
                    stainCount--;  // Decrease stain count
                break;


                break;
            case GLUT_RIGHT_BUTTON:
                cloudSpeed += 0.05f;   // Fast speed
                rotationSpeed += 0.2f;
                if (stainCount < maxStains)
                    stainCount++;
                break;
        }
    }
}
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        rainActive = !rainActive;
        if (rainActive) {
            glutTimerFunc(0, rainTimer, 0); // Start rain animation
        }
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        bulbBlinkInterval = 200;  // faster blink on left click

        // Optional: Reset blink speed after 3 seconds
        glutTimerFunc(3000, [](int) {
            bulbBlinkInterval = 1000; // back to normal
        }, 0);
    }
}


void combinedMouse(int button, int state, int x, int y) {
    mouseClick(button,state,x,y);
    handleMouse(button,state,x,y);
    mouse(button, state, x, y);
    mouse2(button, state, x, y);
}
void mousehover(int x, int y)
{
    mouseHover(x,y);
    passiveMotion(x,y);
}
void displayF() {
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (currentScene == 1) {
        gluOrtho2D(-3.0, 20.0, -6.0, 20.0);  // Scene 1 projection
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        drawScene1();
    }
    else if (currentScene == 2) {
        gluOrtho2D(-1, 1, -1, 1);             // Scene 2 projection
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        drawScene2();
    }
    else if (currentScene == 3) {
        gluOrtho2D(-10, 10, -5, 10);          // Scene 3 projection
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        drawScene3();
    }
      else if (currentScene == 4) {
         gluOrtho2D(-1,1,-1,1);         // Scene 3 projection
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        drawScene4();
    }
    glutSwapBuffers();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

    glutInitWindowSize(1000, 800);
    //glutInitWindowPosition(10, 10);
    glutCreateWindow("Haunted House");

    glutReshapeFunc(handleResize);
    init();
    init2();
    glutDisplayFunc(displayF);
    glutKeyboardFunc(KeyBoard);
     //glutMouseFunc(mouseClick);
    //glutKeyboardFunc(keyboard);
    glutTimerFunc(0, timer, 0);
    glutTimerFunc(0,update,0);

    glutPassiveMotionFunc(mousehover);
    //glutPassiveMotionFunc(passiveMotion);
    //glutKeyboardFunc(handleKeypress);

    glutMouseFunc(combinedMouse);
    glutTimerFunc(0, updateRain, 0);
    glutTimerFunc(0, updateFlame, 0);
    glutTimerFunc(0, updatePlates, 0);
    glutTimerFunc(0, updateChandelier, 0);
    glutTimerFunc(0, updateCandleFlames, 0);
    glutTimerFunc(0, updatePendulum, 0);

    glutMainLoop();
    return 0;
}
