#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "global.h"
#include "maze.h"

typedef struct {
    int walls; // Bitmask for walls (e.g., 0b1111 for all walls)
    bool visited;
} MazeCell;

MazeCell maze[ORIGINAL_MAZE_WIDTH][ORIGINAL_MAZE_HEIGHT];
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

bool** genMaze() {
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

    while (mainStack) pop(&mainStack);

    bool** result = malloc(CONVERTED_MAZE_WIDTH * sizeof(bool*));
    for (int i = 0; i < CONVERTED_MAZE_WIDTH; i++) {
        result[i] = malloc(CONVERTED_MAZE_HEIGHT * sizeof(bool));
        
        for (int j = 0; j < CONVERTED_MAZE_HEIGHT; j++) {
            result[i][j] = true;
        }
    }

    for (int x = 0; x < ORIGINAL_MAZE_WIDTH; x++) {
        for (int y = 0; y < ORIGINAL_MAZE_HEIGHT; y++) {
            int cx = 2 * x + 1;
            int cy = 2 * y + 1;
            
            result[cx][cy] = false;
            
            if (!(maze[x][y].walls & 0b0001)) result[cx][cy - 1] = false; // Top
            if (!(maze[x][y].walls & 0b0010)) result[cx + 1][cy] = false; // Right
            if (!(maze[x][y].walls & 0b0100)) result[cx][cy + 1] = false; // Bottom
            if (!(maze[x][y].walls & 0b1000)) result[cx - 1][cy] = false; // Left
        }
    }

    return result;
}

void freeMaze(bool** maze, int width) {
    if (maze) {
        for (int i = 0; i < width; i++) {
            free(maze[i]);
        }
        free(maze);
    }
}