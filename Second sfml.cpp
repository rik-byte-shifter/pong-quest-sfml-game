#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>

using namespace std;

enum GameState { MENU, GAME, CREDITS, GAMEOVER };

// Paddle Class
class Paddle {
public:
    sf::RectangleShape shape;
    sf::Texture texture;
    float speed = 2.0f;

    Paddle() {
        shape.setSize(sf::Vector2f(120, 60));  // Paddle dimensions
        if (!texture.loadFromFile("brickwall.png")) {
            cout << "Error loading paddle texture!" << endl;
        }
        shape.setTexture(&texture);
        shape.setPosition(600, 660);
    }

    void moveLeft() {
        if (shape.getPosition().x > 0)
            shape.move(-speed, 0);
    }

    void moveRight(float windowWidth) {
        if (shape.getPosition().x < windowWidth - shape.getSize().x)
            shape.move(speed, 0);
    }
};

// Ball Class
class Ball {
public:
    sf::CircleShape shape;
    sf::Texture texture;
    sf::Vector2f velocity = { 1.2f, 1.2f };

    Ball() {
        shape.setRadius(31);
        if (!texture.loadFromFile("bowling.png")) {
            cout << "Error loading ball texture!" << endl;
        }
        shape.setTexture(&texture);
        shape.setPosition(400, 10);
    }

    void move() {
        shape.move(velocity);
    }

    void bounceX() {
        velocity.x = -velocity.x;
    }

    void bounceY() {
        velocity.y = -velocity.y;
    }

    void increaseSpeed() {
        velocity.x *= 1.1f;
        velocity.y *= 1.1f;
    }

    bool isOffScreen(float windowHeight) {
        return shape.getPosition().y >= windowHeight - shape.getRadius() * 2;
    }
};

// Game Class
class Game {
public:
    sf::RenderWindow window;
    Paddle paddle;
    Ball ball;
    GameState gameState = MENU;
    int score = 0, level = 1, highestScore = loadHighestScore();
    bool gameOver = false;
    sf::Clock gameOverClock;

    sf::Font font;
    sf::Text menuTitle, playButton, creditsButton, exitButton, creditsText, backButton, gameOverText;
    sf::Text scoreText, levelText, highestScoreText;

    sf::Texture openingBgTexture, gameBgTexture;
    sf::Sprite openingBackground, gameBackground;

    sf::Music music, gameOverMusic;

    Game() : window(sf::VideoMode(1280, 720), "PONG QUEST") {
        window.setFramerateLimit(200);


        if (!openingBgTexture.loadFromFile("b99.jpg") || !gameBgTexture.loadFromFile("b20.png")) {
            cout << "Error loading background textures!" << endl;
        }

        openingBackground.setTexture(openingBgTexture);
        gameBackground.setTexture(gameBgTexture);


        if (!font.loadFromFile("StonePunk-eZD4g.ttf")) {
            cout << "Error loading font!" << endl;
        }

        // Text setup
        setupText(menuTitle, "PONG QUEST", 105, sf::Color::White, { 170, 100 });
        setupText(playButton, "Play", 45, sf::Color::White, { 300, 300 });
        setupText(creditsButton, "Credits", 45, sf::Color::White, { 300, 400 });
        setupText(exitButton, "Exit", 45, sf::Color::White, { 300, 450 });
        setupText(backButton, "Back to Menu", 25, sf::Color::White, { 1000, 620 });
        setupText(gameOverText, "Game Over!", 80, sf::Color::Red, { 470, 290 });
        setupText(creditsText, "Credits:\nGame developed by Ranesh Das Rik.\nLinkedIn: linkedin.com/in/ranesh-das-rik\nETE, RUET\nRoll: 2204051", 55, sf::Color::White, { 150, 250 });
        setupText(scoreText, "Score: 0", 37, sf::Color::White, { 10, 10 });
        setupText(levelText, "Level: 1", 37, sf::Color::White, { 1163, 10 });
        setupText(highestScoreText, "Highest Score: " + to_string(highestScore), 45, sf::Color::White, { 300, 350 });


        if (!music.openFromFile("music.ogg") || !gameOverMusic.openFromFile("gameover.ogg")) {
            cout << "Error loading music!" << endl;
        }
        music.setLoop(true);
        music.play();
    }

   void run() {
    while (window.isOpen()) {
        handleEvents();
        update();
        render();
    }
}

private:

void handleEvents() {
    sf::Event event;

    while (window.pollEvent(event)) {

        if (event.type == sf::Event::Closed)
            window.close();

        if (gameState == MENU && event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                if (playButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    gameState = GAME;
                    ball.shape.setPosition(400, 10);
                    ball.velocity = { 1.2f, 1.2f };
                    paddle.shape.setPosition(600, 660);
                    music.play();
                }

                else if (creditsButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    gameState = CREDITS;
                }
                else if (exitButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    window.close();
                }
            }
        }

        if (gameState == CREDITS && event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left && backButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                gameState = MENU;
            }
        }
    }
}


void update() {

    if (gameState == GAME && !gameOver) {

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            paddle.moveLeft();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            paddle.moveRight(window.getSize().x);
        }


        ball.move();

        // Check if the ball hits the left or right walls
        if (ball.shape.getPosition().x <= 0 || ball.shape.getPosition().x >= window.getSize().x - ball.shape.getRadius() * 2) {
            ball.bounceX();
        }
        // Check if the ball hits the top of the window
        if (ball.shape.getPosition().y <= 0) {
            ball.bounceY();
        }
        // Check if the ball goes off the bottom of the screen (missed by the paddle)
        if (ball.isOffScreen(window.getSize().y)) {
            gameOver = true;
            gameOverClock.restart();
            music.stop();
            gameOverMusic.play();
        }


        if (ball.shape.getGlobalBounds().intersects(paddle.shape.getGlobalBounds())) {
            ball.bounceY();
            float paddleTop = paddle.shape.getPosition().y;
            ball.shape.setPosition(ball.shape.getPosition().x, paddleTop - ball.shape.getRadius() * 2 - 1.f);
            score++;

            if (score % 5 == 0) {
                level++;
                ball.increaseSpeed();
            }

            updateText();
        }
    }

    // Handle Game Over scenario
    if (gameOver && gameOverClock.getElapsedTime().asSeconds() > 5.0f) {
        // After 5 seconds of game over, reset the game
        if (score > highestScore) {
            highestScore = score;  // Update the highest score if needed
            saveHighestScore(highestScore);  // Save it to a file or storage
        }

        resetGame();  // Reset game for a new round
    }
}


    // Render game objects to the window
    void render() {
        window.clear();

        if (gameState == MENU) {
            window.draw(openingBackground);
            window.draw(menuTitle);
            window.draw(playButton);
            window.draw(highestScoreText);
            window.draw(creditsButton);
            window.draw(exitButton);}
            else if (gameState == CREDITS) {
            window.draw(openingBackground);
            window.draw(creditsText);
            window.draw(backButton);}
            else if (gameState == GAME) {
            window.draw(gameBackground);
            window.draw(paddle.shape);
            window.draw(ball.shape);
            window.draw(scoreText);
            window.draw(levelText);
            if (gameOver) {
            window.draw(gameOverText);
                // Draw the score during game over
            sf::Text finalScoreText;
            setupText(finalScoreText, "Final Score: " + to_string(score), 40, sf::Color::White, { 600, 400 });
            window.draw(finalScoreText);
        }
    }

        window.display();
    }

    // Utility function to set up text objects
    void setupText(sf::Text &text, const string &str, int size, sf::Color color, sf::Vector2f position) {
        text.setFont(font);
        text.setString(str);
        text.setCharacterSize(size);
        text.setFillColor(color);
        text.setPosition(position);
    }

    // Update score, level, and highest score texts
    void updateText() {
        scoreText.setString("Score: " + to_string(score));
        levelText.setString("Level: " + to_string(level));
        highestScoreText.setString("Highest Score: " + to_string(highestScore));
    }

    // Reset the game state for a new game
    void resetGame() {
        gameState = MENU;
        gameOver = false;
        score = 0;
        level = 1;
        music.play();
        updateText();
        gameOverMusic.stop();
    }

    // Function to save the highest score to a file
    void saveHighestScore(int highestScore) {
        ofstream outFile("highest_score.txt");
        if (outFile.is_open()) {
            outFile << highestScore;
            outFile.close();
        } else {
            cout << "Error saving highest score!" << endl;
        }
    }

    // Function to load the highest score from a file
    int loadHighestScore() {
        int highestScore = 0;
        ifstream inFile("highest_score.txt");
        if (inFile.is_open()) {
            inFile >> highestScore;
            inFile.close();
        } else {
            cout << "Error loading highest score, setting to 0." << endl;
        }
        return highestScore;
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}
