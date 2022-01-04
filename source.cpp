#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

/* ATTRIBUTION: Javidx9 - OneLoneCoder*/

#include <iostream>
#include <Windows.h>
#include <thread>
#include <vector>
#include <stdio.h>
using namespace std;


wstring tetromino[7];
int rotate(int x, int y, int rot);
bool DoesPieceFit (int Tetromino, int Rotation, int posx, int posy); 
int xdim = 12;
int ydim = 18;
unsigned char *p_board = nullptr;

int ScreenWidth = 80;   // Console Screen Size X (row)
int ScreenHeight = 30;  // Console Screen Size Y (column)

int main() {
    // Create Screen Buffer

    wchar_t *screen = new wchar_t[ScreenWidth*ScreenHeight];
    for (int i{0}; i < ScreenWidth*ScreenHeight; i++) screen[i] = L' ';
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");

    tetromino[1].append(L"..X.");
    tetromino[1].append(L".XX.");
    tetromino[1].append(L".X..");
    tetromino[1].append(L"....");

    tetromino[2].append(L".X..");
    tetromino[2].append(L".XX.");
    tetromino[2].append(L"..X.");
    tetromino[2].append(L"....");

    tetromino[3].append(L"....");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L"....");

    tetromino[4].append(L".X.");
    tetromino[4].append(L".X..");
    tetromino[4].append(L".XX.");
    tetromino[4].append(L"....");

    tetromino[5].append(L"..X.");
    tetromino[5].append(L"..X.");
    tetromino[5].append(L".XX.");
    tetromino[5].append(L"....");

    tetromino[6].append(L".XXX");
    tetromino[6].append(L"..X.");
    tetromino[6].append(L"....");
    tetromino[6].append(L"....");

    p_board = new unsigned char[xdim*ydim]; // create field
    for (int x{0}; x < xdim; x++) {
        for (int y{0}; y < ydim; y++) {
            p_board[y*xdim + x] = (x==0 || x == xdim - 1 || y == ydim - 1)? 9 : 0;
        }
    }

    // Define Variables

    bool GameOver = false;
    int CurrentPiece = 0;
    int CurrentRotation = 0;
    int currentx = xdim/2;
    int currenty = 0;

    bool bKey[4];
    bool RotateHold = true;

    int Speed = 20;
    int SpeedCounter = 0;
    bool ForceDown = false;
    int PieceCount = 0;
    int Score = 0;

    vector<int> vLines;

    while(!GameOver) { // Game Loop

        //Game Timing
        this_thread::sleep_for(50ms);
        SpeedCounter++;
        ForceDown = (SpeedCounter == Speed);

        
        //Input
        for (int k = 0; k < 4; k++) {
            bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0; // R L D Z keys
        }

        // Game Logic

        // Player movement with keys
        currentx += (bKey[0] && (DoesPieceFit(CurrentPiece, CurrentRotation, currentx + 1, currenty))) ? 1 : 0;
        currentx -= (bKey[1] && (DoesPieceFit(CurrentPiece, CurrentRotation, currentx - 1, currenty))) ? 1 : 0;
        currenty += (bKey[2] && (DoesPieceFit(CurrentPiece, CurrentRotation, currentx + 1, currenty))) ? 1 : 0;

        // Rotate and lock for current key press
        if (bKey[3]) {

            CurrentRotation += (RotateHold  && (DoesPieceFit(CurrentPiece, CurrentRotation, currentx + 1, currenty))) ? 1 : 0;
            RotateHold = false;
        } else {
            RotateHold = true; 
        }

    if (ForceDown) {

        // Update difficulty every 50 pieces
        SpeedCounter = 0;
        PieceCount++;

        if (PieceCount % 50 == 0) {
            if (Speed >= 10) {
                Speed--;
            }
        }

        // Test if piece can move down
        if (DoesPieceFit(CurrentPiece, CurrentRotation, currentx, currenty + 1)) {
            currenty++;
            // move piece down
        } else {
            // lock current piece in field
            for (int x = 0; x < 4; x++) {
                for (int y = 0; y < 4; y++) {
                    if (tetromino[CurrentPiece][rotate(x, y, CurrentRotation)] != L'.') {
                        p_board[(currenty + y) * xdim + (currentx + x)] = CurrentPiece + 1;
                    }
                }
            }

            //check we have any lines
            for (int y = 0; y < 4; y++) {
                if(currenty + y < ydim - 1) {

                    bool Line = true;
                    for(int x = 1; x < xdim - 1; x++) {
                        Line &= (p_board[(currenty + y)* xdim + x]) != 0;
                    }

                    if (Line) {
                        // Remove Line, set to =
                        for (int x = 1; x < xdim - 1; x++) {
                            p_board[(currenty + y) * xdim + x] = 8;
                        }

                        vLines.push_back(currenty + y);
                    }
                }
            }

            Score += 25;
            if (!vLines.empty()) {
                Score += (1 << vLines.size()) * 100;
            }

            //choose next piece
            currentx = xdim /2;
            currenty = 0;
            CurrentRotation = 0;
            CurrentPiece = rand() % 7;

            // if piece does not fit at start
            GameOver = !DoesPieceFit(CurrentPiece, CurrentRotation, currentx, currenty); 
        }
    }

        // Render Output

        //Draw Field
        for (int x{0}; x < xdim; x++) {
            for (int y{0}; y < ydim; y++) {
                screen[(y + 2)*ScreenWidth + (x + 2)] = L" ABCDEFG=#"[p_board[y*xdim + x]];
            }
        }

        // Draw current piece
        for (int x = 0; x < 4; x++) {
            for (int y = 0; y < 4; y++) {
                if (tetromino[CurrentPiece][rotate(x, y, CurrentRotation)] != L'.') {
                    screen[(currenty + y + 2)*ScreenWidth + (currentx + x + 2)] = CurrentPiece + 65;
                }
            }
        }
        // Draw Score
        swprintf_s(&screen[2*ScreenWidth + xdim + 6], 16, L"SCORE: %8d", Score);

        if (!vLines.empty()) {
            // Display Frame to draw lines
            WriteConsoleOutputCharacter(hConsole, screen, ScreenWidth*ScreenHeight, {0,0}, &dwBytesWritten);
            this_thread::sleep_for(400ms); // Add delay

            for (auto &v : vLines) {
                for (int x = 1; x < xdim - 1; x++) {
                    for (int y = v; y > 0; y--) {
                        p_board[y * xdim + x] = p_board[(y - 1) * xdim + x];
                    }
                    p_board[x] = 0;
                }
            }
            vLines.clear();
        }

        //Display frame
        WriteConsoleOutputCharacter(hConsole, screen, ScreenWidth*ScreenHeight, {0,0}, &dwBytesWritten);
    }

    // Exit Game Loop: Endgame

    CloseHandle(hConsole);
    cout << "Game Over!! Score: " << Score << endl;
    system("pause");

    return 0;
}

bool DoesPieceFit (int Tetromino, int Rotation, int posx, int posy) {

    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {

            // index -> piece
            int pos = rotate(x, y, Rotation);

            // index -> field
            int fi = (posy + y) * xdim + (posx + x);

            if (posx + x >= 0 && posx + x < xdim) { // Boundary check
                if (posy + y >= 0 && posy + y < posy) {
                    // Collision check
                    if (tetromino[Tetromino][pos] != L'.' && p_board[fi] != 0) {
                        return false; // fail first hit
                    }
                }
            }
        }
    }
    return true;
}

int rotate(int x, int y, int rot) {
    int pos = 0;

    switch(rot % 4) {
        case 0: 
            pos = y*4 + x;         // original
            break;
        case 1: 
            pos = 12 + y - (4*x);  // 90 deg cw
            break;
        case 2: 
            pos = 15 - (4*y) - x;  // 180 deg cw
            break;

        case 3: 
            pos = 3 - y + (4*x);   // 270 deg cw
            break;
    }
    return pos;
}