#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 20
#define HEIGHT 10

typedef struct {
    int x, y;
} Vector2;

typedef struct {
    bool walls[4]; // 0: cima, 1: direita, 2: baixo, 3: esquerda
    bool visited;
} MazeCell;

typedef struct Vector2Node {
    Vector2 data;
    struct Vector2Node* next;
} Vector2Node;

MazeCell maze[WIDTH][HEIGHT];
unsigned int rVisits = WIDTH * HEIGHT;
Vector2Node* mainStack = NULL;

// Funções da pilha
Vector2Node* push(Vector2Node* stack, Vector2 data) {
    Vector2Node* node = (Vector2Node*)malloc(sizeof(Vector2Node));
    if (!node) exit(1);
    node->data = data;
    node->next = stack;
    return node;
}

Vector2 pop(Vector2Node** stack) {
    if (*stack == NULL) {
        Vector2 v = {-1, -1}; // Valor inválido
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

// Geração do labirinto
void generate_maze() {
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            for (int i = 0; i < 4; i++) {
                maze[x][y].walls[i] = true;
            }
            maze[x][y].visited = false;
        }
    }
}

void removeWalls(Vector2 a, Vector2 b) {
    if (a.x == b.x) {
        if (a.y < b.y) {
            maze[a.x][a.y].walls[2] = false;
            maze[b.x][b.y].walls[0] = false;
        } else {
            maze[a.x][a.y].walls[0] = false;
            maze[b.x][b.y].walls[2] = false;
        }
    } else {
        if (a.x < b.x) {
            maze[a.x][a.y].walls[1] = false;
            maze[b.x][b.y].walls[3] = false;
        } else {
            maze[a.x][a.y].walls[3] = false;
            maze[b.x][b.y].walls[1] = false;
        }
    }
}

Vector2 checkNbs(Vector2 curr) {
    Vector2 nbs[4] = {
        {curr.x, curr.y - 1}, // Cima
        {curr.x + 1, curr.y}, // Direita
        {curr.x, curr.y + 1}, // Baixo
        {curr.x - 1, curr.y}  // Esquerda
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
        
        // Libera a memória da pilha temporária
        while (stack) pop(&stack);
        
        return next;
    }

    // Libera a memória da pilha temporária
    while (stack) pop(&stack);

    return pop(&mainStack);
}

void print_maze() {
    char symbols[16] = {
        'A',  // 0000
        'B',  // 0001 (baixo)
        'C',  // 0010 (direita)
        'D',  // 0011 (direita + baixo)
        'E',  // 0100 (cima)
        'F',  // 0101 (cima + baixo)
        'G',  // 0110 (cima + direita)
        'H',  // 0111 (cima + direita + baixo)
        'I',  // 1000 (esquerda)
        'J',  // 1001 (esquerda + baixo)
        'K',  // 1010 (esquerda + direita)
        'L',  // 1011 (esquerda + direita + baixo)
        'M',  // 1100 (esquerda + cima)
        'N',  // 1101 (esquerda + cima + baixo)
        'O',  // 1110 (esquerda + cima + direita)
        'P'   // 1111 (todas)
    };

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int code = (maze[x][y].walls[0] << 3) | (maze[x][y].walls[1] << 2) | 
                       (maze[x][y].walls[2] << 1) | (maze[x][y].walls[3]);
            printf("%c", symbols[code]);
        }
        printf("\n");
    }
    printf("\n"); // Add a newline at the end
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

    print_maze();
    
    // Free the remaining stack memory
    while (mainStack) pop(&mainStack);
    return 0;
}
