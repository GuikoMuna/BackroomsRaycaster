#ifndef GLOBAL_H
#define GLOBAL_H

typedef struct {
    int x, y;
} Vector2Int;

#define FOV 60
#define RAY_COUNT 100
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define ORIGINAL_MAZE_WIDTH 250
#define ORIGINAL_MAZE_HEIGHT 250
#define CONVERTED_MAZE_WIDTH (2 * ORIGINAL_MAZE_WIDTH + 1)
#define CONVERTED_MAZE_HEIGHT (2 * ORIGINAL_MAZE_HEIGHT + 1)
#define CELL_SIZE 100

#endif