#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>

bool** genMaze(); 

void freeMaze(bool** maze, int width);

#endif