
#include <iostream>
#include <fstream> // For file I/O to save and load highest score
#include <string>

using namespace std;

enum GameState { MENU, GAME, CREDITS, GAMEOVER };

int main() {
    int score = 0, level = 1, highestScore = 0;
    GameState gameState = MENU;

    // Load highest score from a file
    ifstream inputFile("highscore.txt");
    if (inputFile.is_open()) {
        inputFile >> highestScore;
        inputFile.close();
    }

    // Simple menu and game logic
    while (true) {
        if (gameState == MENU) {
            cout << "PONG QUEST\n";
            cout << "1. Play\n";
            cout << "2. Credits\n";
            cout << "3. Exit\n";
            cout << "Highest Score: " << highestScore << "\n";
            int choice;
            cin >> choice;
            if (choice == 1) {
                gameState = GAME;
            } else if (choice == 2) {
                gameState = CREDITS;
            } else if (choice == 3) {
                break;
            }
        }

        if (gameState == CREDITS) {
            cout << "Credits:\n";
            cout << "Game developed by Rik.\n";
            cout << "ETE, RUET\nRoll: 2204051\n";
            cout << "Press any key to return to menu.\n";
            cin.ignore();
            cin.get();
            gameState = MENU;
        }

        if (gameState == GAME) {
            // Simulate game mechanics via text (no graphics)
            bool gameOver = false;
            while (!gameOver) {
                // Move the ball and check for collision with a paddle (text-based simulation)
                cout << "Ball moving...\n";
                cout << "Paddle moving...\n";
                score++;

                if (score % 5 == 0) {
                    level++;
                    cout << "Level up! Level: " << level << "\n";
                }

                // Simulate a "ball misses paddle" scenario leading to game over
                if (score >= 10) {
                    gameOver = true;
                    gameState = GAMEOVER;
                }
            }
        }

        if (gameState == GAMEOVER) {
            cout << "Game Over!\n";
            cout << "Your Score: " << score << "\n";

            // Check if the current score is the highest score
            if (score > highestScore) {
                highestScore = score;
                cout << "New highest score: " << highestScore << "\n";

                // Save the highest score to a file
                ofstream outputFile("highscore.txt");
                if (outputFile.is_open()) {
                    outputFile << highestScore;
                    outputFile.close();
                }
            }

            // Reset for new game
            score = 0;
            level = 1;
            cout << "Press any key to return to menu.\n";
            cin.ignore();
            cin.get();
            gameState = MENU;
        }
    }

    return 0;
}
