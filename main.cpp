/*
* GL02Primitive.cpp: Vertex, Primitive and Color
* Draw Simple 2D colored Shapes: quad, triangle and polygon.
*/
#include <windows.h>  // for MS Windows
#include <GL/glut.h>  // GLUT, include glu.h and gl.h

/* Initialize OpenGL Graphics */
void initGL() {
	// Set "clearing" or background color
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);


}
void display() {
	glClear(GL_COLOR_BUFFER_BIT);   // Clear the color buffer with current clearing color



    glBegin(GL_POLYGON);            // These vertices form a closed polygon
	glColor3f(0.0f, 0.0f, 0.0f);

	glVertex2f(-0.6f, 0.4f);
	glVertex2f(0.6f, 0.4f);
	glVertex2f(0.6f, 0.2f);
	glVertex2f(-0.6f, 0.2f);
	glEnd();

	glBegin(GL_POLYGON);            // These vertices form a closed polygon
	glColor3f(1.0f, 1.0f, 1.0f);

	glVertex2f(-0.6f, 0.2f);
	glVertex2f(0.6f, 0.2f);
	glVertex2f(0.6f, 0.0f);
	glVertex2f(-0.6f, 0.0f);
	glEnd();

   glBegin(GL_POLYGON);            // These vertices form a closed polygon
	glColor3f(0.11f, 0.51f, 0.28f);

	glVertex2f(-0.6f, 0.0f);
	glVertex2f(0.6f, 0.0f);
	glVertex2f(0.6f, -0.2f);
	glVertex2f(-0.6f, -0.2f);
	glEnd();

    glBegin(GL_TRIANGLES);            // These vertices form a closed polygon
	glColor3f(0.75f, 0.22f, 0.16f);

	glVertex2f(-0.6f, 0.4f);
	glVertex2f(-0.6f, -0.2f);
	glVertex2f(-0.25f, 0.12f);

	glEnd();

       glLineWidth(1.0f);  // Set the border thickness
    glColor3f(0.0f, 0.0f, 0.0f); // Black color
    glBegin(GL_LINE_LOOP);

    glVertex2f(0.6f, 0.4f);
    glVertex2f(0.6f, -0.2f);

    glEnd();


	glFlush();  // Render now
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char** argv) {
	glutInit(&argc, argv);          // Initialize GLUT
	glutCreateWindow("Vertex, Primitive & Color");  // Create window with the given title
	glutInitWindowSize(320, 320);   // Set the window's initial width & height
	glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
	glutDisplayFunc(display);       // Register callback handler for window re-paint event
	initGL();                       // Our own OpenGL initialization
	glutMainLoop();                 // Enter the event-processing loop
	return 0;
}
