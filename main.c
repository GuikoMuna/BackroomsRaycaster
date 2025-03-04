#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

#define PI 3.1415926535

#define WIDTH 800
#define HEIGHT 600

struct Player
{
    float x;
    float y;
    float theta;
    float v;

    float dx;
    float dy;
    float dtheta;
} player = {100, 100, 0, 10, 10, 0, 0.1};

void drawPlayer()
{
    glColor3f(1, 1, 0);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex2i(player.x, player.y);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2i(player.x, player.y);
    glVertex2i(player.x + player.dx*5, player.y + player.dy*5);
    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawPlayer();
    glutSwapBuffers();
}

void buttons(unsigned char key, int x, int y)
{
    if (key == 'w')
    {
        player.x += player.dx;
        player.y += player.dy;
    }
    if (key == 'a')
    {
        player.theta -= player.dtheta;
        if (player.theta < 0) player.theta += 2 * PI;
        player.dx = player.v * cos(player.theta);
        player.dy = player.v * sin(player.theta);
    }
    if (key == 'd')
    {
        player.theta += player.dtheta;
        if (player.theta > 2 * PI) player.theta -= 2 * PI;
        player.dx = player.v * cos(player.theta);
        player.dy = player.v * sin(player.theta);
    }
    if (key == 's')
    {
        player.x -= player.dx;
        player.y -= player.dy;
    }
    glutPostRedisplay();
}

void initBeforeRender()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gluOrtho2D(0, WIDTH, HEIGHT, 0);
}

int main(int argc, char** argv)
{ 
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutCreateWindow("Backrooms");
    glutInitWindowSize(WIDTH, HEIGHT);

    initBeforeRender();
    
    glutDisplayFunc(display);
    glutKeyboardFunc(buttons);
    glutMainLoop();
    return 0;
}
