#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#define MAX_K 1000

#include "constants.hpp"

namespace DLX {

struct Node {
    Node* left;
    Node* right;
    Node* up;
    Node* down;
    Node* head;

    int size;      // used for Column header
    int rowID[3];  // used to identify row in order to map solutions to a sudoku grid
                   // ID Format: (Candidate, Row, Column)
};

int SIZE_SQUARED = SIZE * SIZE;
int SIZE_SQRT = sqrt((double)SIZE);
int ROW_NB = SIZE * SIZE * SIZE;
int COL_NB = 4 * SIZE * SIZE;

struct Node Head;
struct Node* HeadNode = &Head;
struct Node* solution[MAX_K];
struct Node* orig_values[MAX_K];
std::vector<std::vector<bool>> matrix(ROW_NB, std::vector<bool>(COL_NB,  0));
bool isSolved = false;

//===============================================================================================================//
//----------------------------------------------- Print Functions -----------------------------------------------//
//===============================================================================================================//

void MapSolutionToGrid(std::vector<std::vector<int>>& grid) {
    for (int i = 0; solution[i] != NULL; i++) {
        grid[solution[i]->rowID[1] - 1][solution[i]->rowID[2] - 1] = solution[i]->rowID[0];
    }
    for (int i = 0; orig_values[i] != NULL; i++) {
        grid[orig_values[i]->rowID[1] - 1][orig_values[i]->rowID[2] - 1] = orig_values[i]->rowID[0];
    }
}

//===============================================================================================================//
//---------------------------------------------DLX Functions-----------------------------------------------------//
//===============================================================================================================//

void coverColumn(Node* col) {
    col->left->right = col->right;
    col->right->left = col->left;
    for (Node* node = col->down; node != col; node = node->down) {
        for (Node* temp = node->right; temp != node; temp = temp->right) {
            temp->down->up = temp->up;
            temp->up->down = temp->down;
            temp->head->size--;
        }
    }
}

void uncoverColumn(Node* col) {
    for (Node* node = col->up; node != col; node = node->up) {
        for (Node* temp = node->left; temp != node; temp = temp->left) {
            temp->head->size++;
            temp->down->up = temp;
            temp->up->down = temp;
        }
    }
    col->left->right = col;
    col->right->left = col;
}

void search(int k, std::vector<std::vector<int>>& originalGrid) {
    if (HeadNode->right == HeadNode) {
        MapSolutionToGrid(originalGrid);  // Update the original grid
        isSolved = true;
        return;
    }

    // Choose Column Object Deterministically: Choose the column with the smallest Size
    Node* Col = HeadNode->right;
    for (Node* temp = Col->right; temp != HeadNode; temp = temp->right)
        if (temp->size < Col->size) Col = temp;

    coverColumn(Col);

    for (Node* temp = Col->down; temp != Col; temp = temp->down) {
        solution[k] = temp;
        for (Node* node = temp->right; node != temp; node = node->right) {
            coverColumn(node->head);
        }

        search(k + 1, originalGrid);

        temp = solution[k];
        solution[k] = NULL;
        Col = temp->head;
        for (Node* node = temp->left; node != temp; node = node->left) {
            uncoverColumn(node->head);
        }
    }

    uncoverColumn(Col);
}

//===============================================================================================================//
//----------------------Functions to turn a Sudoku grid into an Exact Cover problem -----------------------------//
//===============================================================================================================//

//--------------------------BUILD THE INITIAL MATRIX CONTAINING ALL POSSIBILITIES--------------------------------//
void BuildSparseMatrix(std::vector<std::vector<bool>>& matrix) {
    // Constraint 1: There can only be one value in any given cell
    int j = 0, counter = 0;
    for (int i = 0; i < ROW_NB; i++) {  // iterate over all rows
        matrix[i][j] = 1;
        counter++;
        if (counter >= SIZE) {
            j++;
            counter = 0;
        }
    }

    // Constraint 2: There can only be one instance of a number in any given row
    int x = 0;
    counter = 1;
    for (j = SIZE_SQUARED; j < 2 * SIZE_SQUARED; j++) {
        for (int i = x; i < counter * SIZE_SQUARED; i += SIZE) matrix[i][j] = 1;

        if ((j + 1) % SIZE == 0) {
            x = counter * SIZE_SQUARED;
            counter++;
        } else
            x++;
    }

    // Constraint 3: There can only be one instance of a number in any given column
    j = 2 * SIZE_SQUARED;
    for (int i = 0; i < ROW_NB; i++) {
        matrix[i][j] = 1;
        j++;
        if (j >= 3 * SIZE_SQUARED) j = 2 * SIZE_SQUARED;
    }

    // Constraint 4: There can only be one instance of a number in any given region
    x = 0;
    for (j = 3 * SIZE_SQUARED; j < COL_NB; j++) {
        for (int l = 0; l < SIZE_SQRT; l++) {
            for (int k = 0; k < SIZE_SQRT; k++) matrix[x + l * SIZE + k * SIZE_SQUARED][j] = 1;
        }

        int temp = j + 1 - 3 * SIZE_SQUARED;

        if (temp % (int)(SIZE_SQRT * SIZE) == 0)
            x += (SIZE_SQRT - 1) * SIZE_SQUARED + (SIZE_SQRT - 1) * SIZE + 1;
        else if (temp % SIZE == 0)
            x += SIZE * (SIZE_SQRT - 1) + 1;
        else
            x++;
    }
}

//-------------------BUILD A TOROIDAL DOUBLY LINKED LIST OUT OF THE SPARSE MATRIX-------------------------//
void BuildDLL(std::vector<std::vector<bool>>& matrix) {
    Node* header = new Node;
    header->left = header;
    header->right = header;
    header->down = header;
    header->up = header;
    header->size = -1;
    header->head = header;
    Node* temp = header;

    // Create all Column Nodes
    for (int i = 0; i < COL_NB; i++) {
        Node* newNode = new Node;
        newNode->size = 0;
        newNode->up = newNode;
        newNode->down = newNode;
        newNode->head = newNode;
        newNode->right = header;
        newNode->left = temp;
        temp->right = newNode;
        temp = newNode;
    }

    int ID[3] = {0, 1, 1};
    // Add a Node for each 1 present in the sparse matrix and update Column Nodes accordingly
    for (int i = 0; i < ROW_NB; i++) {
        Node* top = header->right;
        Node* prev = NULL;

        if (i != 0 && i % SIZE_SQUARED == 0) {
            ID[0] -= SIZE - 1;
            ID[1]++;
            ID[2] -= SIZE - 1;
        } else if (i != 0 && i % SIZE == 0) {
            ID[0] -= SIZE - 1;
            ID[2]++;
        } else {
            ID[0]++;
        }

        for (int j = 0; j < COL_NB; j++, top = top->right) {
            if (matrix[i][j]) {
                Node* newNode = new Node;
                newNode->rowID[0] = ID[0];
                newNode->rowID[1] = ID[1];
                newNode->rowID[2] = ID[2];
                if (prev == NULL) {
                    prev = newNode;
                    prev->right = newNode;
                }
                newNode->left = prev;
                newNode->right = prev->right;
                newNode->right->left = newNode;
                prev->right = newNode;
                newNode->head = top;
                newNode->down = top;
                newNode->up = top->up;
                top->up->down = newNode;
                top->size++;
                top->up = newNode;
                if (top->down == top) top->down = newNode;
                prev = newNode;
            }
        }
    }

    HeadNode = header;
}

//-------------------COVERS VALUES THAT ARE ALREADY PRESENT IN THE GRID-------------------------//
void ConvertDLLtoGrid(std::vector<std::vector<int>>& grid) {
    int index = 0;
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (grid[i][j] > 0) {
                Node* Col = NULL;
                Node* temp = NULL;
                for (Col = HeadNode->right; Col != HeadNode; Col = Col->right) {
                    for (temp = Col->down; temp != Col; temp = temp->down)
                        if (temp->rowID[0] == grid[i][j] && (temp->rowID[1] - 1) == i && (temp->rowID[2] - 1) == j) goto ExitLoops;
                }
            ExitLoops:
                coverColumn(Col);
                orig_values[index] = temp;
                index++;
                for (Node* node = temp->right; node != temp; node = node->right) {
                    coverColumn(node->head);
                }
            }
}


bool solver(std::vector<std::vector<int>>& grid) {
    BuildSparseMatrix(matrix);
    BuildDLL(matrix);
    ConvertDLLtoGrid(grid);
    search(0, grid);

    if (isSolved) {
        MapSolutionToGrid(grid);
    }

    return isSolved;
}

void solve(std::vector<std::vector<int>>& grid) {
    if (!solver(grid)) {
        std::cout << "No solution exists!\n";
    }
}
}  // namespace DLX
