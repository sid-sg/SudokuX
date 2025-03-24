#pragma once

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

const int ROW_NB = SIZE * SIZE * SIZE;
const int COL_NB = 4 * SIZE * SIZE;
//===============================================================================================================//
//---------------------------------------------DLX Functions-----------------------------------------------------//
//===============================================================================================================//

void coverColumn(Node* col);

void uncoverColumn(Node* col);

void search(int k, std::vector<std::vector<int>>& originalGrid);

//===============================================================================================================//
//----------------------Functions to turn a Sudoku grid into an Exact Cover problem -----------------------------//
//===============================================================================================================//

//--------------------------BUILD THE INITIAL MATRIX CONTAINING ALL POSSIBILITIES--------------------------------//
void BuildSparseMatrix(bool matrix[ROW_NB][COL_NB]);

//-------------------BUILD A TOROIDAL DOUBLY LINKED LIST OUT OF THE SPARSE MATRIX-------------------------//
void BuildLinkedList(bool matrix[ROW_NB][COL_NB]);

//-------------------COVERS VALUES THAT ARE ALREADY PRESENT IN THE GRID-------------------------//
void TransformListToCurrentGrid(std::vector<std::vector<int>>& grid);

//===============================================================================================================//
//----------------------------------------------- Print Functions -----------------------------------------------//
//===============================================================================================================//

void MapSolutionToGrid(std::vector<std::vector<int>>& grid);

//---------------------------------PRINTS A SUDOKU GRID OF ANY SIZE---------------------------------------------//
void PrintGrid(std::vector<std::vector<int>>& grid);

//--------------------------------------------------------------------------------//

bool solver(std::vector<std::vector<int>>& grid);

void solve(std::vector<std::vector<int>>& grid);
}  // namespace DLX
