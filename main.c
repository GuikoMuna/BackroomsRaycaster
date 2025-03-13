#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>

#define PI 3.1415926535
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define ORIGINAL_MAZE_WIDTH 20
#define ORIGINAL_MAZE_HEIGHT 15
#define CONVERTED_MAZE_WIDTH (2 * ORIGINAL_MAZE_WIDTH + 1)
#define CONVERTED_MAZE_HEIGHT (2 * ORIGINAL_MAZE_HEIGHT + 1)
const float CELL_WIDTH = (float)WINDOW_WIDTH / (float)CONVERTED_MAZE_WIDTH;
const float CELL_HEIGHT = (float)WINDOW_HEIGHT / (float)CONVERTED_MAZE_HEIGHT;

// Maze Logic -------------------------------------------------------------
typedef struct {
    int x, y;
} Vector2Int;

typedef struct {
    int walls; // Bitmask for walls (e.g., 0b1111 for all walls)
    bool visited;
} MazeCell;

MazeCell maze[ORIGINAL_MAZE_WIDTH][ORIGINAL_MAZE_HEIGHT];
bool convertedMaze[CONVERTED_MAZE_WIDTH][CONVERTED_MAZE_HEIGHT]; // Global Maze
unsigned int rVisits = ORIGINAL_MAZE_WIDTH * ORIGINAL_MAZE_HEIGHT;

typedef struct Vector2IntNode {
    Vector2Int data;
    struct Vector2IntNode* next;
} Vector2IntNode;

Vector2IntNode* mainStack = NULL;

    // Stack functions
Vector2IntNode* push(Vector2IntNode* stack, Vector2Int data) {
    Vector2IntNode* node = (Vector2IntNode*)malloc(sizeof(Vector2IntNode));
    if (!node) exit(1);
    node->data = data;
    node->next = stack;
    return node;
}

Vector2Int pop(Vector2IntNode** stack) {
    if (*stack == NULL) {
        return (Vector2Int){-1, -1}; // Invalid value
    }
    Vector2IntNode* temp = *stack;
    Vector2Int data = temp->data;
    *stack = temp->next;
    free(temp);
    return data;
}

int getStackSize(Vector2IntNode* stack) {
    int size = 0;
    while (stack) {
        size++;
        stack = stack->next;
    }
    return size;
}

Vector2Int getFromIndex(Vector2IntNode* stack, int index) {
    for (int i = 0; i < index; i++) {
        stack = stack->next;
    }
    return stack->data;
}

void removeWalls(Vector2Int a, Vector2Int b) {
    if (a.x == b.x) {
        if (a.y < b.y) {
            maze[a.x][a.y].walls &= ~0b0100; // Clear bottom wall of a
            maze[b.x][b.y].walls &= ~0b0001; // Clear top wall of b
        } else {
            maze[a.x][a.y].walls &= ~0b0001; // Clear top wall of a
            maze[b.x][b.y].walls &= ~0b0100; // Clear bottom wall of b
        }
    } else {
        if (a.x < b.x) {
            maze[a.x][a.y].walls &= ~0b0010; // Clear right wall of a
            maze[b.x][b.y].walls &= ~0b1000; // Clear left wall of b
        } else {
            maze[a.x][a.y].walls &= ~0b1000; // Clear left wall of a
            maze[b.x][b.y].walls &= ~0b0010; // Clear right wall of b
        }
    }
}

Vector2Int checkNbs(Vector2Int curr) {
    Vector2Int nbs[4] = {
        {curr.x, curr.y - 1}, // Top
        {curr.x + 1, curr.y}, // Right
        {curr.x, curr.y + 1}, // Bottom
        {curr.x - 1, curr.y}  // Left
    };
    Vector2IntNode* stack = NULL;

    for (int i = 0; i < 4; i++) {
        if (nbs[i].x >= 0 && nbs[i].x < ORIGINAL_MAZE_WIDTH && nbs[i].y >= 0 && nbs[i].y < ORIGINAL_MAZE_HEIGHT) {
            if (!maze[nbs[i].x][nbs[i].y].visited) {
                stack = push(stack, nbs[i]);
            }
        }
    }

    int stackSize = getStackSize(stack);
    if (stackSize > 0) {
        int index = rand() % stackSize;
        Vector2Int next = getFromIndex(stack, index);
        removeWalls(curr, next);

        while (stack) pop(&stack);
        return next;
    }

    while (stack) pop(&stack);
    return (Vector2Int){-1, -1};
}

void convertMaze() {
    for (int x = 0; x < CONVERTED_MAZE_WIDTH; x++) {
        for (int y = 0; y < CONVERTED_MAZE_HEIGHT; y++) {
            convertedMaze[x][y] = true;
        }
    }

    for (int x = 0; x < ORIGINAL_MAZE_WIDTH; x++) {
        for (int y = 0; y < ORIGINAL_MAZE_HEIGHT; y++) {
            int cx = 2 * x + 1;
            int cy = 2 * y + 1;
            convertedMaze[cx][cy] = false;

            if (!(maze[x][y].walls & 0b0001)) convertedMaze[cx][cy - 1] = false;
            if (!(maze[x][y].walls & 0b0010)) convertedMaze[cx + 1][cy] = false;
            if (!(maze[x][y].walls & 0b0100)) convertedMaze[cx][cy + 1] = false;
            if (!(maze[x][y].walls & 0b1000)) convertedMaze[cx - 1][cy] = false;
        }
    }
}

void generate_maze() {
    srand(time(NULL));

    for (int x = 0; x < ORIGINAL_MAZE_WIDTH; x++) {
        for (int y = 0; y < ORIGINAL_MAZE_HEIGHT; y++) {
            maze[x][y].walls = 0b1111;
            maze[x][y].visited = false;
        }
    }

    Vector2Int curr = {0, 0};
    mainStack = push(mainStack, curr);
    maze[curr.x][curr.y].visited = true;
    rVisits--;

    while (rVisits > 0) {
        Vector2Int next = checkNbs(curr);
        if (next.x != -1) {
            mainStack = push(mainStack, next);
            maze[next.x][next.y].visited = true;
            rVisits--;
            curr = next;
        } else {
            curr = pop(&mainStack);
        }
    }

    convertMaze();
    while (mainStack) pop(&mainStack);
}

void drawMaze() {
    // Loop through the maze matrix
    for (int i = 0; i < CONVERTED_MAZE_HEIGHT; i++) {
        for (int j = 0; j < CONVERTED_MAZE_WIDTH; j++) {
            // Set the color based on the value in the matrix
            if (convertedMaze[j][i]) {
                glColor3f(1.0f, 1.0f, 1.0f); // White (wall)
            } else {
                glColor3f(0.0f, 0.0f, 0.0f); // Black (open space)
            }

            // Draw the square
            glBegin(GL_QUADS);
            glVertex2f(j * CELL_WIDTH, i * CELL_HEIGHT);
            glVertex2f((j + 1) * CELL_WIDTH, i * CELL_HEIGHT);
            glVertex2f((j + 1) * CELL_WIDTH, (i + 1) * CELL_HEIGHT);
            glVertex2f(j * CELL_WIDTH, (i + 1) * CELL_HEIGHT);
            glEnd();
        }
    }
}

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
} player = {1.5 * CELL_WIDTH, 1.5 * CELL_HEIGHT, 0, 10, 10, 0, 0.1};

bool checkCollision(bool forward)
{
    int x = player.x + (forward ? player.dx : -player.dx);
    int y = player.y + (forward ? player.dy : -player.dy);
    int cx = x / CELL_WIDTH;
    int cy = y / CELL_HEIGHT;

    if (cx >= 0 && cx < CONVERTED_MAZE_WIDTH && cy >= 0 && cy < CONVERTED_MAZE_HEIGHT)
    {
        return convertedMaze[cx][cy];
    }
    return true;
}

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
    drawMaze(); // Draw the maze
    drawPlayer(); // Draw the player
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
        if (checkCollision(false))
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

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Raycaster");

    initBeforeRender();
    generate_maze(); // Generate the maze

    glutDisplayFunc(display);
    glutKeyboardFunc(buttons);
    glutMainLoop();
    return 0;
}
