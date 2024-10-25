#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include "raylib.h"
#include "dice.h"
#include "board.h"
#include <chrono>// for time delay
#include <thread>// for time delay

//enum Color { RED, GREEN, BLUE, YELLOW };

enum GameState { MENU, PLAYING, GAMEOVER };
GameState currentGameState = MENU;

const int screenWidth = 1000;
const int screenHeight = 1000;
const int boardSize = 810;
const int cellSize = boardSize / 15;
const Color playersColors[4] = { RED,  GREEN,  YELLOW ,BLUE, };
const int diceSize = 50;   // Size of the dice
const float rollDuration = 0.5f; // Duration of dice roll animation
float dicePosX = screenWidth / 2 - diceSize / 2;
float dicePosY = screenHeight / 2 - diceSize / 2;

class Position {
public:
    int x, y;
    Position() {
        x = 0;
        y = 0;
    }
    Position(int a, int b) {
        x = a;
        y = b;
    }
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

void DrawPiece(Position position1, Color color) {
    Vector2 position = { static_cast<float>(position1.x), static_cast<float>(position1.y) };
    // Draw the base of the piece (a slightly larger circle)
    DrawCircleV(position, 23, DARKGRAY);

    // Draw the main body of the piece (a circle)
    DrawCircleV(position, 20, color);

    // Add a gradient effect by overlaying a semi-transparent circle on top
    DrawCircleGradient(position.x, position.y, 20, Fade(WHITE, 0.3f), Fade(color, 0.3f));

    // Add a small circle on top to give a rounded top
    DrawCircleV(position, 10, WHITE);
}


std::vector<Position> StarPath = {
    {176,446},{446,824} ,{ 824,554 },{554,176},{230,554},{554,770},{770,446},{446,230}

};

bool isInStar(Position pos) {
    for (int i = 0; i < 8; i++) {
        if (StarPath[i] == pos) {
            return true;
        }
        
    }
    return false;
}

std::vector<Position> ludoPath = {
    {176,446},{230,446},{284,446},{338,446},{392,446},{446,392},{446,338},{446,284},{446,230},{446,176},{446,122},{500,122},{554,122},{554,176},
    {554,230},{554,284},{554,338},{554,392},{608,446},{662,446},{716,446},{770,446},{824,446},{878,446},{878,500},{878,554},{824,554},{770,554},{716,554},
    {662,554},{608,554},{554,608},{554,662},{554,716},{554,770},{554,824},{554,878},{500,878},{446,878},{446,824},{446,770},{446,716},{446,662},{446,608},
    {392,554},{338,554},{284,554},{230,554},{176,554},{122,554},{122,500},{122,446}
};

std::vector<Position> PiecePath[4]{
 {//red path
    {176,446},{230,446},{284,446},{338,446},{392,446},{446,392},{446,338},{446,284},{446,230},{446,176},{446,122},{500,122},{554,122},{554,176},
    {554,230},{554,284},{554,338},{554,392},{608,446},{662,446},{716,446},{770,446},{824,446},{878,446},{878,500},{878,554},{824,554},{770,554},{716,554},
    {662,554},{608,554},{554,608},{554,662},{554,716},{554,770},{554,824},{554,878},{500,878},{446,878},{446,824},{446,770},{446,716},{446,662},{446,608},
    {392,554},{338,554},{284,554},{230,554},{176,554},{122,554},{122,500},{176,500},{230,500},{284,500},{338,500},{392,500},{446,500}

},

 {//Green path
    {554,176},{554,230},{554,284},{554,338},{554,392},{608,446},{662,446},{716,446},{770,446},{824,446},{878,446},{878,500},{878,554},{824,554},{770,554},{716,554},
    {662,554},{608,554},{554,608},{554,662},{554,716},{554,770},{554,824},{554,878},{500,878},{446,878},{446,824},{446,770},{446,716},{446,662},{446,608},
    {392,554},{338,554},{284,554},{230,554},{176,554},{122,554},{122,500},{122,446},{176,446},{230,446},{284,446},{338,446},{392,446},{446,392},{446,338},
    {446,284},{446,230},{446,176},{446,122},{500,122},{500,176},{500,230},{500,284},{500,338},{500,392},{500,446}
},
 {// Yellow path
    {824,554},{770,554},{716,554},{662,554},{608,554},{554,608},{554,662},{554,716},{554,770},{554,824},{554,878},{500,878},{446,878},{446,824},{446,770},{446,716},{446,662},{446,608},
    {392,554},{338,554},{284,554},{230,554},{176,554},{122,554},{122,500},{122,446}, {176,446},{230,446},{284,446},{338,446},{392,446},{446,392},{446,338},{446,284},{446,230},{446,176},{446,122},{500,122},{554,122},{554,176},
    {554,230},{554,284},{554,338},{554,392},{608,446},{662,446},{716,446},{770,446},{824,446},{878,446},{878,500},{824,500},{770,500},{716,500},{662,500},{608,500},{554,500}
},
{//blue path
    {446,824},{446,770},{446,716},{446,662},{446,608},{392,554},{338,554},{284,554},{230,554},{176,554},{122,554},{122,500},{122,446},{176,446},{230,446},{284,446},{338,446},{392,446},{446,392},{446,338},{446,284},{446,230},{446,176},{446,122},{500,122},{554,122},{554,176},
    {554,230},{554,284},{554,338},{554,392},{608,446},{662,446},{716,446},{770,446},{824,446},{878,446},{878,500},{878,554},{824,554},{770,554},{716,554},
    {662,554},{608,554},{554,608},{554,662},{554,716},{554,770},{554,824},{554,878},{500,878},{500,824},{500,770},{500,716},{500,662},{500,608},{500,554}
}
};

std::vector<Position> Home[4]{
 {//redHome
    {203,203},{311,203},{203,311},{311,311}
},
{//greenHome
    {689,203},{797,203},{689,311},{797,311}
},
 {//yellowHome
    {689,689},{797,689},{689,797},{797,797}
},
{//blueHome
    {203,689},{311,689},{203,797},{311,797}
},


};
class Piece {
public:
    Position position;
    bool inPlay;
    Color color;
    bool inHome;
    bool inFinalHome;
    const Position InitialPosition;
    int ColorIndex;
    int PathIndex;


    // initializer list

    Piece(Color color, Position InitialPosition, int ci) : InitialPosition(InitialPosition), position(InitialPosition), inPlay(false), color(color), inFinalHome(false), inHome(true) {
        ColorIndex = ci;
    }

    void setPosition(const Position& newPos) {
        position = newPos;
    }
    void setInitialPos() {
        position = InitialPosition;
        inPlay = false;
        inHome = true;
    }
    Position getPosition() {
        return position;
    }

    void move(int steps) {
        if ((56 - PathIndex) < steps) {
            return;
                  }
        if (inHome && steps == 1) {
            inHome = false;
            inPlay = true;
            setPosition(PiecePath[ColorIndex][0]);
            PathIndex = 0;

        }
        else {

            PathIndex += steps;
            if (PathIndex == 56) {
                inFinalHome = true;
            }
            setPosition(PiecePath[ColorIndex][PathIndex]);
            
        }

    }


    bool isInHome() {
        return inHome;
    }

    bool isInPlay() const {
        return inPlay;
    }
    
};


class Player {
public:
    Color color;
    int colorIndex;
    Piece pieces[4];
    


    Player(Color color, int ci) : color(color), colorIndex(ci),
        pieces{ Piece(color,Home[ci][0],ci) ,Piece(color,Home[ci][1],ci), Piece(color,Home[ci][2],ci), Piece(color,Home[ci][3],ci) } { }


    int rollDice() {
        return (rand() % 6) + 1;
    }

    void movePiece(int pieceIndex, int steps) {
        if (pieces[pieceIndex].isInPlay() || (steps == 1 && pieces[pieceIndex].isInHome())) {
            pieces[pieceIndex].move(steps);
        }
        else {

        }
    }
    
   
};

class Board {
public:

    Player players[4];

    Board() : currentPlayerIndex(0),
        players{ Player(playersColors[0],0), Player(playersColors[1],1), Player(playersColors[2],2), Player(playersColors[3],3) } {

    }

    void startGame() {
        while (!gameOver()) {
            update();
        }

        std::cout << "Player " << currentPlayerIndex + 1 << " has won the game!" << std::endl;
    }

private:
    int currentPlayerIndex;
    static int count;

    void update() {
        Player& currentPlayer = players[currentPlayerIndex];
        static int diceValue = 1;
        bool rolling = false; // Flag for dice rolling state
        float rollTime = 0.0f; // Timer for dice roll animation
        int flag = 0;

        while (!flag) {
            if (IsKeyPressed(KEY_SPACE)) {
                rolling = true;
            }

            if (rolling) {
                rollTime += GetFrameTime(); // Increment roll timer
                if (rollTime >= rollDuration) {
                    rolling = false;
                    diceValue = currentPlayer.rollDice(); // Random dice value between 1 and 6
                    flag = 1;
                }
                else {
                    // Optional: Simulate rolling by randomly changing diceValue
                    diceValue = currentPlayer.rollDice();
                }
            }
            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawLudoBoard();
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    DrawPiece(players[i].pieces[j].getPosition(), playersColors[i]);
                }
            }

            DrawDice((int)dicePosX, (int)dicePosY, diceValue, currentPlayer.color);
            EndDrawing();
        }

        int moveIndex = 0;
        int flag1 = 0;
        int flag3 = 1;

        // Check if step is not equal to 1 and all pieces are in home then we have to skip turn for the current player
        if (diceValue != 1) {
            for (int i = 0; i < 4; i++) {

                if (currentPlayer.pieces[i].isInPlay() && (56 - currentPlayer.pieces[i].PathIndex) >= diceValue) {
                    flag3 = 0;
                    break;
                }

            }
        }
        else {
            flag3 = 0;
        }

        if (diceValue == 1 ) {
            count++;
            if (count == 3) {

                int largest = 0;
                int index = 0;
                for (int i = 0; i < 4; i++) {

                    if ( (largest < currentPlayer.pieces[i].PathIndex) && currentPlayer.pieces[i].PathIndex!=56) {
                        largest = currentPlayer.pieces[i].PathIndex;
                        index = i;
                    }
                }
                currentPlayer.pieces[index].setInitialPos();
                nextTurn();
                return;
            }
        }

        // If at least one player is in play or dice value is 1
        if (flag3 == 0) {
            while (true) {
                int flag2 = 0;
                BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawLudoBoard();
                DrawDice((int)dicePosX, (int)dicePosY, diceValue, currentPlayer.color);

                for (int i = 0; i < 4; i++) {
                    for (int j = 0; j < 4; j++) {
                        DrawPiece(players[i].pieces[j].getPosition(), playersColors[i]);
                    }
                }

                for (int i = 0; i < 4; i++) {
                    Position p = currentPlayer.pieces[i].getPosition();
                    Vector2 CircleCenter = { static_cast<float>(p.x), static_cast<float>(p.y) };
                    float CircleRadius1 = 23.0f;

                    if (CheckCollisionPointCircle(GetMousePosition(), CircleCenter, CircleRadius1)) {
                        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            moveIndex = i;
                            flag1 = 1; // User selects piece
                            // If dice not equal to 1 and piece user selecting is in home
                            if ((diceValue != 1 && currentPlayer.pieces[moveIndex].isInHome()) || ((56 - currentPlayer.pieces[moveIndex].PathIndex) < diceValue)) {
                                DrawText("Please select the allowed pieces", 100, 46, 20, BLACK);
                                // flag2 is 1 when unwanted piece is selected
                                flag2 = 1;
                                flag1 = 0; // As unwanted piece is selected then user should reselect valid piece
                            }
                            break;
                        }
                    }
                }
                EndDrawing();
                // For the message of "please select the allowed pieces" to last 2 seconds on screen
                if (flag2 == 1) {
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                }

                // If one valid piece is selected then loop breaks
                if (flag1 == 1) {
                    break;
                }
            }
        }

        // Move piece is only run when at least one piece is moveable
                
        if (flag3 == 0){
            if ( !(diceValue == 1 && currentPlayer.pieces[moveIndex].isInHome()) ) {
                int InitialIndex = currentPlayer.pieces[moveIndex].PathIndex;
                for (int k = 0; k < diceValue; k++) {
                    
                    BeginDrawing();
                    ClearBackground(RAYWHITE);
                    DrawLudoBoard();
                    for (int i = 0; i < 4; i++) {
                        for (int j = 0; j < 4; j++) {
                            if (!((i == currentPlayerIndex) && (j == moveIndex))) {
                                DrawPiece(players[i].pieces[j].getPosition(), playersColors[i]);
                            }
                            else {
                                DrawPiece(PiecePath[currentPlayer.colorIndex][InitialIndex + k], currentPlayer.color);
                            }
                        }
                                               
                    }
                    DrawDice((int)dicePosX, (int)dicePosY, diceValue, currentPlayer.color);
                    EndDrawing();
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                }
            }
            currentPlayer.movePiece(moveIndex, diceValue);
        }
      

        for (int i = 0; i < 4; i++) {

            for (int j = 0; j < 4; j++) {
                if ((i != currentPlayerIndex) && (!isInStar(players[i].pieces[j].getPosition()))) {
                    if (currentPlayer.pieces[moveIndex].getPosition() == players[i].pieces[j].getPosition()) {
                        players[i].pieces[j].setInitialPos();
                    }

                }
            }
        }
        
        
        if (diceValue == 1 || diceValue == 6){
            if (diceValue == 6) {
                count = 0;
            }
        }
        else {
            if (!(gameOver()) ){
                nextTurn();
            }
        } 
    }


    void nextTurn() {
        count = 0;
        currentPlayerIndex = (currentPlayerIndex + 1) % 4;
    }

    bool gameOver() const {
        for (int i = 0; i < 4; i++) {
            if (!(players[currentPlayerIndex].pieces[i].inFinalHome)) {
                return false;
            }
        }
        return true;
    }
};

int Board:: count = 0;

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));
    InitWindow(screenWidth, screenHeight, "Ludo Game");
    SetTargetFPS(60);

    
    Board board;
    float textAlpha = 0.0f;  

    while (!WindowShouldClose()) {
        BeginDrawing();

      
        ClearBackground(DARKGREEN);
        //DrawRectangleGradientV(0, 0, screenWidth, screenHeight, RED, BLUE);
        DrawRectangle(0, 0, screenWidth / 2, screenHeight / 2, RED);
        DrawRectangle( screenWidth / 2, 0, screenWidth,  screenHeight / 2, BLUE);
        DrawRectangle( 0,  screenHeight / 2, screenWidth / 2, screenHeight, GREEN);
        DrawRectangle(screenWidth / 2, screenHeight / 2, screenWidth, screenHeight, YELLOW);

        if (currentGameState == MENU) {
          
            DrawText("Welcome to Ludo Game!", screenWidth / 2 - 250, screenHeight / 3, 40, GOLD);

            textAlpha = sin(GetTime() * 2) * 0.5f + 0.5f; 
            DrawTextEx(GetFontDefault(), "Press ENTER to Start",
                Vector2{ screenWidth / 2 - 150, screenHeight / 4 +30},
                20, 2, Fade(BLACK, textAlpha));

      
            DrawText("Contributors: Papu Chaudhary, Rhythm Adhikari, Roshan Koirala, Sabin Shrestha",
                20, screenHeight - 60, 24, BLACK);

            if (IsKeyPressed(KEY_ENTER)) {
                currentGameState = PLAYING; 
            }
        }
        else if (currentGameState == PLAYING) {
           
            DrawLudoBoard();  
            board.startGame();  
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
