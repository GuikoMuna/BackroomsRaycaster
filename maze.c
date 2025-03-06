#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 20
#define HEIGHT 10
#define WIDTHCONVERTED (2 * WIDTH + 1)
#define HEIGHTCONVERTED (2 * HEIGHT + 1)

typedef struct {
    int x, y;
} Vector2;

typedef struct {
    int walls; // Bitmask for walls: 0b0001 (top), 0b0010 (right), 0b0100 (bottom), 0b1000 (left)
    bool visited;
} MazeCell;

typedef struct Vector2Node {
    Vector2 data;
    struct Vector2Node* next;
} Vector2Node;

MazeCell maze[WIDTH][HEIGHT];
bool convertedMaze[WIDTHCONVERTED][HEIGHTCONVERTED];
unsigned int rVisits = WIDTH * HEIGHT;
Vector2Node* mainStack = NULL;

// Stack functions
Vector2Node* push(Vector2Node* stack, Vector2 data) {
    Vector2Node* node = (Vector2Node*)malloc(sizeof(Vector2Node));
    if (!node) exit(1);
    node->data = data;
    node->next = stack;
    return node;
}

Vector2 pop(Vector2Node** stack) {
    if (*stack == NULL) {
        Vector2 v = {-1, -1}; // Invalid value
        return v;
    }
    Vector2Node* temp = *stack;
    Vector2 data = temp->data;
    *stack = temp->next;
    free(temp);
    return data;
}

int getStackSize(Vector2Node* stack) {
    int size = 0;
    while (stack) {
        size++;
        stack = stack->next;
    }
    return size;
}

Vector2 getFromIndex(Vector2Node* stack, int index) {
    for (int i = 0; i < index; i++) {
        stack = stack->next;
    }
    return stack->data;
}

// Maze generation
void generate_maze() {
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            maze[x][y].walls = 0b1111; // All walls initially present
            maze[x][y].visited = false;
        }
    }
}

void removeWalls(Vector2 a, Vector2 b) {
    if (a.x == b.x) {
        if (a.y < b.y) {
            // Remove bottom wall of a and top wall of b
            maze[a.x][a.y].walls &= ~0b0100; // Clear bottom wall of a
            maze[b.x][b.y].walls &= ~0b0001; // Clear top wall of b
        } else {
            // Remove top wall of a and bottom wall of b
            maze[a.x][a.y].walls &= ~0b0001; // Clear top wall of a
            maze[b.x][b.y].walls &= ~0b0100; // Clear bottom wall of b
        }
    } else {
        if (a.x < b.x) {
            // Remove right wall of a and left wall of b
            maze[a.x][a.y].walls &= ~0b0010; // Clear right wall of a
            maze[b.x][b.y].walls &= ~0b1000; // Clear left wall of b
        } else {
            // Remove left wall of a and right wall of b
            maze[a.x][a.y].walls &= ~0b1000; // Clear left wall of a
            maze[b.x][b.y].walls &= ~0b0010; // Clear right wall of b
        }
    }
}

Vector2 checkNbs(Vector2 curr) {
    Vector2 nbs[4] = {
        {curr.x, curr.y - 1}, // Top
        {curr.x + 1, curr.y}, // Right
        {curr.x, curr.y + 1}, // Bottom
        {curr.x - 1, curr.y}  // Left
    };
    Vector2Node* stack = NULL;

    for (int i = 0; i < 4; i++) {
        if (nbs[i].x >= 0 && nbs[i].x < WIDTH && nbs[i].y >= 0 && nbs[i].y < HEIGHT) {
            if (!maze[nbs[i].x][nbs[i].y].visited) {
                stack = push(stack, nbs[i]);
            }
        }
    }

    int stackSize = getStackSize(stack);
    if (stackSize > 0) {
        int index = rand() % stackSize;
        Vector2 next = getFromIndex(stack, index);
        removeWalls(curr, next);

        // Free the temporary stack
        while (stack) pop(&stack);

        return next;
    }

    // Free the temporary stack
    while (stack) pop(&stack);

    return pop(&mainStack);
}

void convertMaze() {
    // Initialize convertedMaze with walls
    for (int x = 0; x < WIDTHCONVERTED; x++) {
        for (int y = 0; y < HEIGHTCONVERTED; y++) {
            convertedMaze[x][y] = true; // Start with all walls
        }
    }

    // Convert the maze
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            int cx = 2 * x + 1;
            int cy = 2 * y + 1;

            convertedMaze[cx][cy] = false; // Cell is open

            // Check walls
            if (!(maze[x][y].walls & 0b0001)) { // Top wall
                convertedMaze[cx][cy - 1] = false;
            }
            if (!(maze[x][y].walls & 0b0010)) { // Right wall
                convertedMaze[cx + 1][cy] = false;
            }
            if (!(maze[x][y].walls & 0b0100)) { // Bottom wall
                convertedMaze[cx][cy + 1] = false;
            }
            if (!(maze[x][y].walls & 0b1000)) { // Left wall
                convertedMaze[cx - 1][cy] = false;
            }
        }
    }
}

void print_maze() {
    for (int y = 0; y < HEIGHTCONVERTED; y++) {
        for (int x = 0; x < WIDTHCONVERTED; x++) {
            printf(convertedMaze[x][y] ? "1" : "0");
        }
        printf("\n");
    }
}

int main() {
    srand(time(NULL));
    generate_maze();

    Vector2 curr = {0, 0};
    mainStack = push(mainStack, curr);

    while (rVisits > 0) {
        if (!maze[curr.x][curr.y].visited) {
            maze[curr.x][curr.y].visited = true;
            rVisits--;
            mainStack = push(mainStack, curr);
        }
        curr = checkNbs(curr);
        if (curr.x == -1 && curr.y == -1) {
            break; // No more cells to visit
        }
    }

    convertMaze();
    print_maze();

    // Free the remaining stack memory
    while (mainStack) pop(&mainStack);
    return 0;
}
