
/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Arsal Temuri
// 23i-0016
// SE-A
// PF Project 2024
// 
/////////////////////////////////////////////////////////////////////////////////////////////////////

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
#include <fstream>
#include <iostream>
#include <string> 

#include <chrono>
#include <thread>

int linescore = 0;
int boundary = 1;


/////////////////////////////////////////////////////////////////////////////////////////////////////

//FUNCTION FOR INPUTTING PLAYER NAME :
std::string getPlayerName(sf::RenderWindow& window, sf::Font& font) {
    std::string playerName;
    sf::Text namePrompt("Enter your name and press Enter:", font, 24);
    namePrompt.setPosition(50, 70);

    sf::Text nameText("", font, 24);
    nameText.setPosition(150, 250);

    bool enteringName = true;

    while (enteringName && window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                window.close();

            if (e.type == sf::Event::TextEntered) {
                if (e.text.unicode < 128) {
                    if (e.text.unicode == '\b' && playerName.size() > 0) {
                        playerName.pop_back();
                    }
                    else if (e.text.unicode == '\r') { // Enter key pressed
                        enteringName = false;
                    }
                    else {
                        playerName += static_cast<char>(e.text.unicode);
                    }
                    nameText.setString(playerName);
                }
            }
        }

        window.clear();
        window.draw(namePrompt);
        window.draw(nameText);
        window.display();
    }

    return playerName;
}

/// 
/// ////////////////////////////////////////////////////////////////////////////////////////////////
/// 

//MOVE THE BLOCKS FUCNTION:
void move(int delta_x)
{
    for (int i = 0; i < 4; i++) {
        point_2[i][0] = point_1[i][0];
        point_2[i][1] = point_1[i][1];
        point_1[i][0] += delta_x;
    }

    if (!anamoly()) {
        for (int i = 0; i < 4; i++) {
            point_1[i][0] = point_2[i][0];
            point_1[i][1] = point_2[i][1];
        }
    }
}

/// 
/// ////////////////////////////////////////////////////////////////////////////////////////////////
/// 

//FUNCTION FOR BLOCKS ROTATION:
void rotates(int px, int py)
{
    for (int i = 0; i < 4; i++)
    {
        int x = point_1[i][1] - py;
        int y = point_1[i][0] - px;
        point_1[i][0] = px - x;
        point_1[i][1] = py + y;
    }
}

/// 
/// ////////////////////////////////////////////////////////////////////////////////////////////////
/// 

//CLEARING LINES AND CALCULATING THE SCORE FUNCTION:
void checkLines(int& score, bool isAdvancedMode)
{
    int linescore = 0;
    int k = M - 1;

    static sf::SoundBuffer lineCompletedBuffer;
    if (!lineCompletedBuffer.loadFromFile("music/lineClear.ogg")) {
        std::cerr << "Failed to load line completed sound!" << std::endl;
    }

    static sf::Sound lineCompletedSound(lineCompletedBuffer);

    for (int i = M - 1; i >= 0; i--) {
        int count = 0;
        int color = 0;
        bool sameColor = true;

        for (int j = 0; j < N; j++) {
            if (gameGrid[i][j])
                count++;

            if (j == 0)
                color = gameGrid[i][j];
            else if (gameGrid[i][j] != color)
                sameColor = false;

            gameGrid[k][j] = gameGrid[i][j];
        }

        if (count == N)
            linescore++;

        if (count < N)
            k--;

        if (sameColor && count == N)
            linescore += 500; // 500 points for the same color
    }

    if (linescore > 0) {
        lineCompletedSound.play();

        sf::Clock soundTimer;
        while (soundTimer.getElapsedTime().asSeconds() < 0.5) {
        }
    }

    if (isAdvancedMode) {
        //ADVANCED MODE SCORINGG:
        switch (linescore) {
        case 2:
            score += 60;
            break;
        case 3:
            score += 120;
            break;
        case 4:
            score += 200;
            break;
        default:
            score += linescore * 20;
        }
    }
    else {
        //BEGINNER MODE SCORING:
        switch (linescore) {
        case 2:
            score += 30;
            break;
        case 3:
            score += 60;
            break;
        case 4:
            score += 100;
            break;
        default:
            score += linescore * 10;
        }
    }
}

/// 
/// ////////////////////////////////////////////////////////////////////////////////////////////////
/// 

// FUCNTION TO MANAGE THE ADVANCE MODE :
void fallingAdvancedPiece(float& timer, float& delay, int& colorNum, bool& drop, float& bombTimer)
{
    if (timer > delay || drop)
    {
        for (int i = 0; i < 4; i++)
        {
            point_2[i][0] = point_1[i][0];
            point_2[i][1] = point_1[i][1];
            point_1[i][1] += 1;
        }

        if (!anamoly())
        {
            for (int i = 0; i < 4; i++)
                gameGrid[point_2[i][1]][point_2[i][0]] = colorNum;

            colorNum = 1 + rand() % 7;
            int n = rand() % 7;
            for (int i = 0; i < 4; i++) {

                point_1[i][0] = BLOCKS[n][i] % 2;
                point_1[i][1] = BLOCKS[n][i] / 2;
            }

            int startX = rand() % (N - 2);
            for (int i = 0; i < 4; i++) {
                point_1[i][0] += startX;
            }

            drop = false;
        }

        timer = 0;
    }
}

/// 
/// ////////////////////////////////////////////////////////////////////////////////////////////////
/// 


//FUNCTION FOR MANAGING THE BEGINNERS MODE :
void fallingBeginnerPiece(float& timer, float& delay, int& colorNum, bool& drop, float& bombTimer)
{
    if (timer > delay || drop)
    {
        for (int i = 0; i < 4; i++)
        {
            point_2[i][0] = point_1[i][0];
            point_2[i][1] = point_1[i][1];
            point_1[i][1] += 1;
        }

        if (!anamoly())
        {
            for (int i = 0; i < 4; i++)
                gameGrid[point_2[i][1]][point_2[i][0]] = colorNum;

            colorNum = 1 + rand() % 7;
            int n = rand() % 3;


            for (int i = 0; i < 4; i++) {


                point_1[i][0] = BLOCKS[n][i] % 2;
                point_1[i][1] = BLOCKS[n][i] / 2;

            }

            int startX = rand() % (N - 2); 
            for (int i = 0; i < 4; i++) {
                point_1[i][0] += startX;
            }

            drop = false;
        }

        timer = 0;
    }
}

/// 
/// ////////////////////////////////////////////////////////////////////////////////////////////////
/// 

//FUCNTION FOR MAGAING AND READING HIGHSCORES:
void readHighscore(int highscore[])
{
    std::ifstream hsfile("files/highscore.txt");

    int n;
    while (hsfile >> n)
        *highscore++ = n;

    hsfile.close();
}

/// 
/// ////////////////////////////////////////////////////////////////////////////////////////////////
/// 

// FUNCTION FOR UPDATING THE HIGHSCURE:
void updateHighscore(int highscore[], int score)
{
    for (int i = 0; i < 10; i++) {
        if (score > highscore[i]) {
            int tmp = highscore[i];
            highscore[i] = score;
            score = tmp;
        }
    }

    // Write to file
    std::ofstream hsfile("files/highscore.txt");

    for (int i = 0; i < 10; i++)
        hsfile << highscore[i] << std::endl;

    hsfile.close();
}

/// 
/// ////////////////////////////////////////////////////////////////////////////////////////////////
///

//FUCNTION FOR SHIFTING THE GRID UP:
void shiftGridUp() {
    // Shift each row up by one
    for (int i = 1; i < M; i++) {
        for (int j = 0; j < N; j++) {
            gameGrid[i - 1][j] = gameGrid[i][j];
            boundary = gameGrid[i - 1][j];
        }
    }

    // Clear the bottom row
    for (int j = 0; j < N; j++) {
        gameGrid[M - 1][j] = 0;
    }
}

/// 
/// ////////////////////////////////////////////////////////////////////////////////////////////////
/// 

//FUNCTION FOR INCREASING DIFFICULTY AFTER EVERY 40 SECONDS 
void increaseDifficulty() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(40));
        shiftGridUp();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// FUCNTION TO CALCULATE AND DISPLAY GAME LEVEL BASED ON PLAYERS SCORES:
void displayGameLevel(sf::RenderWindow& window, sf::Font& font, int score) {
    int gameLevel = (score / 100) + 1;

    sf::Text levelText("Level: " + std::to_string(gameLevel), font, 24);
    levelText.setPosition(280, 160); 

    window.draw(levelText);
}
