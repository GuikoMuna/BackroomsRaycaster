#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>

#include "global.h"
#include "maze.h"

#define PI 3.1415926535

// Player Logic -------------------------------------------------------------

struct Player
{
    float x;
    float y;
    float theta;
    float v;

    float dx;
    float dy;
    float dtheta;
} player = {1.5 * CELL_SIZE, 1.5 * CELL_SIZE, 0, 10, 10, 0, 0.1};

bool** convertedMaze;

bool checkCollision(bool forward)
{
    int x = player.x + (forward ? player.dx : -player.dx);
    int y = player.y + (forward ? player.dy : -player.dy);
    int cx = x / CELL_SIZE;
    int cy = y / CELL_SIZE;

    if (cx >= 0 && cx < CONVERTED_MAZE_WIDTH && cy >= 0 && cy < CONVERTED_MAZE_HEIGHT)
    {
        return convertedMaze[cx][cy];
    }
    return true;
}

// Raycasting Logic ---------------------------------------------------------

void castRays() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int col = 0; col < WINDOW_WIDTH; col++) {

        float rayAngle = player.theta + ((col - WINDOW_WIDTH / 2.0f) / WINDOW_WIDTH) * FOV * (PI / 180.0);

        float dirX = cos(rayAngle);
        float dirY = sin(rayAngle);

        float rayX = player.x;
        float rayY = player.y;

        float distance = 0;
        while (distance < 1000) {
            rayX += dirX * 4;
            rayY += dirY * 4;
            distance += 4;

            int mapX = (int)(rayX / CELL_SIZE);
            int mapY = (int)(rayY / CELL_SIZE);

            if (mapX >= 0 && mapX < CONVERTED_MAZE_WIDTH && mapY >= 0 && mapY < CONVERTED_MAZE_HEIGHT && convertedMaze[mapX][mapY] == 1) {
                break;
            }
        }

        if (distance == 0) distance = 1;

        float correctedDistance = distance * cos(rayAngle - player.theta);

        int wallHeight = (CELL_SIZE * WINDOW_HEIGHT) / correctedDistance;

        int wallStart = (WINDOW_HEIGHT / 2) - (wallHeight / 2);
        int wallEnd = (WINDOW_HEIGHT / 2) + (wallHeight / 2);

        float shade = 1.0 - (distance / 500.0);
        if (shade < 0.1) shade = 0.1;
        glColor3f(shade, shade, 0);

        glBegin(GL_LINES);
        glVertex2i(col, wallStart);
        glVertex2i(col, wallEnd);
        glEnd();
    }

    glutSwapBuffers();
}

void buttons(unsigned char key, int x, int y)
{
    if (key == 'w')
    {
        if (!checkCollision(true))
        {
            player.x += player.dx;
            player.y += player.dy;
        }
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
        if (!checkCollision(false))
        {
            player.x -= player.dx;
            player.y -= player.dy;
        }
    }
    glutPostRedisplay();
}

void initBeforeRender()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
}

void cleanup()
{
    if (convertedMaze) freeMaze(convertedMaze, CONVERTED_MAZE_WIDTH);
}

int main(int argc, char** argv)
{
    atexit(cleanup);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Raycaster");

    initBeforeRender();
    convertedMaze = genMaze();

    glutDisplayFunc(castRays);
    glutKeyboardFunc(buttons);
    glutMainLoop();

    freeMaze(convertedMaze, CONVERTED_MAZE_WIDTH);
    return 0;
}
