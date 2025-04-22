#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include "./../HeaderFiles/FileReader.hpp"
using namespace std;

// Global Data Structures
vector<vector<char>> board;
map<vector<int>, vector<vector<int>>> same; // map<cell -> vector<cell>>
map<vector<int>, vector<vector<int>>> diff; // map<cell -> vector<cell>>
queue<vector<int>> BFS;                     // queue<cell>
vector<vector<int>> rowCount = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
vector<vector<int>> colCount = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};

// Helper Functions
bool readBoard();
void writeSolution();
int showBoard();
char opposite(char currVal);
void fillValue(int x, int y, char val);
void workOnCell(int x, int y);
void fillByTrio(int x1, int y1, int x2, int y2, int x3, int y3);
void fillByRelation(int x, int y);
void fillByAntiTrio(int x1, int y1, int x2, int y2, int x3, int y3);
void fillByRows(int x);
void fillByCols(int y);

// Main Function
int main()
{
    // Read the puzzle and initialize the data structures
    bool valid = readBoard();
    if (!valid)
    {
        // if the puzzle is invalid (or the file not found)
        printf("Invalid Input");
        return 0;
    }
    // First pass:
    // - iterate over each cell and check if it can be filled by some logic
    // - if it can be filled, fill it and push it in the queue
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            if (board[i][j] != ' ')
                workOnCell(i, j);
        }
    }
    // Second pass:
    // - iterate over all the filled cells and check if they unlocked some new cell by some logic
    // - if some new cell can be filled, fill it and push it in the queue
    while (!BFS.empty())
    {
        vector<int> currCell = BFS.front();
        BFS.pop();
        int i = currCell[0], j = currCell[1];
        workOnCell(i, j);
    }
    writeSolution();
    return 0;
}

// Function Definitions
bool readBoard()
{
    FILE *filePointer = fopen("puzzle.txt", "r"); // returns NULL if file not found
    if (!filePointer)
    {
        cout << "Error: file not found!!\n";
        return false;
    }

    // Read the puzzle and initialize the data structures
    skip_lines(filePointer, 1);
    for (int i = 0; i < 6; i++)
    {
        skip_chars(filePointer, 1);
        vector<char> row;
        for (int j = 0; j < 6; j++)
        {
            skip_chars(filePointer, 1);
            char cellValue = read_char(filePointer); // either '0' or '1' or none(' ')
            row.push_back(cellValue);
            if (cellValue != ' ')
            {
                rowCount[i][cellValue - '0'] += 1;
                colCount[j][cellValue - '0'] += 1;
            }
            skip_chars(filePointer, 1);
            char relation = read_char(filePointer); // either same('=') or diff('x') or none('|')
            if (relation == '=')
            {
                vector<int> left = {i, j};
                vector<int> right = {i, j + 1};
                same[left].push_back(right);
                same[right].push_back(left);
            }
            if (relation == 'x')
            {
                vector<int> left = {i, j};
                vector<int> right = {i, j + 1};
                diff[left].push_back(right);
                diff[right].push_back(left);
            }
        }
        board.push_back(row);
        skip_lines(filePointer, 1);

        for (int j = 0; j < 6; j++)
        {
            skip_chars(filePointer, 2);
            char relation = read_char(filePointer); // either same('=') or diff('x') or none('-')
            if (relation == '=')
            {
                vector<int> up = {i, j};
                vector<int> down = {i + 1, j};
                same[up].push_back(down);
                same[down].push_back(up);
            }
            if (relation == 'x')
            {
                vector<int> up = {i, j};
                vector<int> down = {i + 1, j};
                diff[up].push_back(down);
                diff[down].push_back(up);
            }
            skip_chars(filePointer, 1);
        }
        skip_lines(filePointer, 1);
    }
    fclose(filePointer);
    return true;
}
void writeSolution()
{
    FILE *filePointer = fopen("solution.txt", "w"); // if the file doesn't exist, it will be created
    string hr = "+---+---+---+---+---+---+";

    // Write the solution in the file
    fprintf(filePointer, "%s\n", hr.c_str());
    for (int i = 0; i < board.size(); i++)
    {
        for (int j = 0; j < board[i].size(); j++)
        {
            fprintf(filePointer, "| %c ", board[i][j]);
        }
        fprintf(filePointer, "|\n%s\n", hr.c_str());
    }
}
int showBoard()
{
    int blankCount = 0;
    string hr = "+---+---+---+---+---+---+";

    // Print the board in the terminal
    printf("%s\n", hr.c_str());
    for (int i = 0; i < board.size(); i++)
    {
        for (int j = 0; j < board[i].size(); j++)
        {
            printf("| %c ", board[i][j]);
            if (board[i][j] == ' ')
                blankCount += 1;
        }
        printf("|\n%s\n", hr.c_str());
    }
    // Return the number of blank cells
    return blankCount;
}
char opposite(char currVal)
{
    return (currVal == '0') ? '1' : '0';
}
void fillValue(int x, int y, char val)
{
    if (board[x][y] == ' ') // Fill the cell only if it is empty
    {
        board[x][y] = val;
        // Update the data structures
        BFS.push({x, y});
        rowCount[x][val - '0'] += 1;
        colCount[y][val - '0'] += 1;
        // showBoard();
    }
}
void workOnCell(int i, int j)
{
    // Checking Horizontal Trios
    fillByTrio(i, j, i, (j + 1) % 6, i, (j + 2) % 6); // X x _
    fillByTrio(i, j, i, (j + 2) % 6, i, (j + 1) % 6); // X _ x
    fillByTrio(i, j, i, (j + 5) % 6, i, (j + 4) % 6); // _ x X
    fillByTrio(i, j, i, (j + 4) % 6, i, (j + 5) % 6); // x _ X
    fillByTrio(i, j, i, (j + 1) % 6, i, (j + 5) % 6); // _ X x
    fillByTrio(i, j, i, (j + 5) % 6, i, (j + 1) % 6); // x X _
    // Checking Vertical Trios
    fillByTrio(i, j, (i + 1) % 6, j, (i + 2) % 6, j); // X x _
    fillByTrio(i, j, (i + 2) % 6, j, (i + 1) % 6, j); // X _ x
    fillByTrio(i, j, (i + 5) % 6, j, (i + 4) % 6, j); // _ x X
    fillByTrio(i, j, (i + 4) % 6, j, (i + 5) % 6, j); // x _ X
    fillByTrio(i, j, (i + 1) % 6, j, (i + 5) % 6, j); // _ X x
    fillByTrio(i, j, (i + 5) % 6, j, (i + 1) % 6, j); // x X _
    // Checking Horizontal Anti-Trios
    fillByAntiTrio(i, j, i, (j + 1) % 6, i, (j + 2) % 6); // X _ _
    fillByAntiTrio(i, j, i, (j + 5) % 6, i, (j + 4) % 6); // _ _ X
    // Checking Vertical Anti-Trios
    fillByAntiTrio(i, j, (i + 1) % 6, j, (i + 2) % 6, j); // X _ _
    fillByAntiTrio(i, j, (i + 5) % 6, j, (i + 4) % 6, j); // _ _ X
    // Checking Relations
    fillByRelation(i, j);
    // Checking Row/Col Count
    fillByRows(i);
    fillByCols(j);

    // TODO : add more logics here
}
void fillByTrio(int x1, int y1, int x2, int y2, int x3, int y3)
{
    // if two cell in the trio has the same value
    if (board[x1][y1] == board[x2][y2])
    {
        // and the third cell is empty
        if (board[x3][y3] == ' ')
        {
            // then the third cell can be filled with the opposite value
            fillValue(x3, y3, opposite(board[x1][y1]));
        }
    }
}
void fillByRelation(int x, int y)
{
    // if the cell has a 'same' relation
    if (same.find({x, y}) != same.end())
    {
        for (int i = 0; i < same[{x, y}].size(); i++)
        {
            // if the cell has a 'same' relation with another cell
            int X = same[{x, y}][i][0];
            int Y = same[{x, y}][i][1];
            // and the partner cell is empty
            if (board[X][Y] == ' ')
            {
                // then the partner cell can be filled with the same value
                fillValue(X, Y, board[x][y]);
            }
        }
    }
    // if the cell has a 'diff' relation
    if (diff.find({x, y}) != diff.end())
    {
        for (int i = 0; i < diff[{x, y}].size(); i++)
        {
            // if the cell has a 'diff' relation with another cell
            int X = diff[{x, y}][i][0];
            int Y = diff[{x, y}][i][1];
            // and the partner cell is empty
            if (board[X][Y] == ' ')
            {
                // then the partner cell can be filled with the same value
                fillValue(X, Y, opposite(board[x][y]));
            }
        }
    }
}
void fillByAntiTrio(int x1, int y1, int x2, int y2, int x3, int y3)
{
    // if cell2 & cell3 are related by '=' relation
    if (same.find({x2, y2}) != same.end())
    {
        for (int i = 0; i < same[{x2, y2}].size(); i++)
        {
            // if cell2 & cell3 are related by '=' relation
            if (x3 == same[{x2, y2}][i][0] && y3 == same[{x2, y2}][i][1])
            {
                // if cell2 is empty, then fill it with the opposite value of cell1
                if (board[x2][y2] == ' ')
                {
                    fillValue(x2, y2, opposite(board[x1][y1]));
                }
                // if cell2 is empty, then fill it with the opposite value of cell1
                if (board[x3][y3] == ' ')
                {
                    fillValue(x3, y3, opposite(board[x1][y1]));
                }
            }
        }
    }
}
void fillByRows(int x)
{
    for (int val = 0; val <= 1; val++) // for '0' and '1'
    {
        // if the row has 3 cells with the same value
        if (rowCount[x][val] == 3)
        {
            // then all the empty cells in the row can be filled with the opposite value
            for (int k = 0; k < 6; k++)
            {
                if (board[x][k] == ' ')
                {
                    fillValue(x, k, opposite(val + '0'));
                }
            }
        }
    }
}
void fillByCols(int y)
{
    for (int val = 0; val < 2; val++) // for '0' and '1'
    {
        // if the column has 3 cells with the same value
        if (colCount[y][val] == 3)
        {
            for (int k = 0; k < 6; k++)
            {
                // then all the empty cells in the column can be filled with the opposite value
                if (board[k][y] == ' ')
                {
                    fillValue(k, y, opposite(val + '0'));
                }
            }
        }
    }
}
