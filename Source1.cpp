
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

#include "headerfiles/utils.h"
#include "headerfiles/piece.h"
#include "headerfiles/functionality.h"

using namespace sf;

/////////////////////////////////////////////////////////////////////////////////////////////////////


int main(void)
{
    srand(time(0));

    RenderWindow window(VideoMode(480, 500), "TETRIS by Arsal Temuri");//

    /////////////////////////////////////////////////////////////////////////////////////////////////////


    Texture obj1, obj2, obj3, obj4, obj5, obj6, obj7, obj8, obj9, obj10, obj11, obj12, obj13;
    obj1.loadFromFile("img/tiles.png");//tetris tiles
    obj2.loadFromFile("img/game_board.png");//game board image display
    obj4.loadFromFile("img/overlay.png");//overlay
    obj5.loadFromFile("img/lose.png"); //lose display screen
    obj6.loadFromFile("img/highscore.png");//highscore display screen 
    obj7.loadFromFile("img/settings_display.png");//settings display screen 
    obj8.loadFromFile("img/instructions_display.png"); //instructions display screen
    obj9.loadFromFile("img/main_display.png"); //main display screen
    obj10.loadFromFile("img/game_play.png");//gamePlay option select screen 
    obj12.loadFromFile("img/tilesX.png");//circle tiles
    obj13.loadFromFile("img/block_selection_display.png");//blocks selection display screen 

    //sprites:
    Sprite sprite(obj1), background(obj2), frame(obj3), overlay(obj4), lose(obj5), hiscore(obj6), settings(obj7), instructions(obj8), mainDisplay(obj9), gamePlay(obj10), bomb(obj11), sprite2(obj12), blocksettings(obj13);

    /////////////////////////////////////////////////////////////////////////////////////////////////////

    Font font;
    font.loadFromFile("DIGITAL-7 (MONO).TTF");//font 


    std::string playerName = getPlayerName(window, font); //geting player name 

    // Store player's name in a file
    std::ofstream playerFile("files/player.txt");
    if (playerFile.is_open()) {
        playerFile << playerName;
        playerFile.close();
    }
    else {
        std::cerr << "Unable to open player file for writing." << std::endl;
        return EXIT_FAILURE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////

    // Load main menu image
    Texture mainMenuTexture;
    mainMenuTexture.loadFromFile("img/main_display.png");
    Sprite mainMenuSprite(mainMenuTexture);

    // Draw main menu
    window.clear();
    window.draw(mainMenuSprite);
    window.display();

    /////////////////////////////////////////////////////////////////////////////////////////////////////

    // Text objct for dispplayin score
    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(500, 50); 

    Text playerNameText;
    playerNameText.setFont(font);
    playerNameText.setCharacterSize(22); 
    playerNameText.setFillColor(Color::Yellow);
    playerNameText.setPosition(280, 120); 

    /////////////////////////////////////////////////////////////////////////////////////////////////////

    // Game state
    Clock clock;
    bool paused = false;
    bool lost = false;
    bool hs = false;
    int colorNum = 1;
    float timer = 0;
    int score = 0;
    bool level = false;
    float fallSpeed = 0.3f;

    bool isAdvancedMode = false;
    bool isBeginnerMode = true; 

    float bombTimer = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////////////

    // Controls
    bool restart = false;
    bool rotate = false;
    bool drop = false;
    float delay = 0.3;
    int delta_x = 0;

    /////////////////////////////////////////////////////////////////////////////////////////////////////

    // Highscore
    int highscore[10] = { 0 };
    readHighscore(highscore);

    std::thread difficultyThread(increaseDifficulty);
    difficultyThread.detach(); // Detach the thread to let it run independently

    /////////////////////////////////////////////////////////////////////////////////////////////////////

    //Music for the game:
    Music music;
    if (!music.openFromFile("music/bg_music.ogg")) {
        std::cerr << "Failed to load music file!" << std::endl;
        return EXIT_FAILURE;
    }
    music.setLoop(true);

    sf::SoundBuffer lineCompletedBuffer;
    if (!lineCompletedBuffer.loadFromFile("music/lineClear.ogg")) {
        std::cerr << "Failed to load line completed sound!" << std::endl;
    }
    sf::Sound lineCompletedSound(lineCompletedBuffer);

    sf::SoundBuffer gameOverBuffer;
    if (!gameOverBuffer.loadFromFile("music/game_over.ogg")) {
        std::cerr << "Failed to load game over sound!" << std::endl;
    }
    sf::Sound gameOverSound(gameOverBuffer);

    // Play music
    music.play();
    bool musicMuted = false;
    bool playGameOverSound = false;

    /////////////////////////////////////////////////////////////////////////////////////////////////////

    // flag variables:
    bool startGame = false;  
    bool showInstructions = false;  
    bool showSettings = false;  
    bool circleTile = false;

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////


    while (!startGame && window.isOpen()) {

        Event e;
        while (window.pollEvent(e)) {

            if (e.type == Event::Closed)
                window.close();

            if (e.type == Event::KeyPressed) {

                /////////////////////////////////////////////////////////////////////////////////////////////////////

                //Display Highscore  Screen:
                if (e.key.code == Keyboard::L) {
                    hs = true; 
                    if (hs) {
                        window.clear(Color::Black);
                        window.draw(hiscore);

                        Text text;
                        text.setFont(font);
                        text.setFillColor(Color::White);
                        text.setStyle(Text::Bold);
                        text.setCharacterSize(24);
                        for (int i = 0; i < 10; i++) {
                            std::string s;
                            if (i < 9)
                                s += " ";
                            s += std::to_string(i + 1);

                            s += "...............";

                            int score = highscore[i];
                            if (score < 10)
                                s += "  ";
                            else if (score < 100)
                                s += " ";
                            s += std::to_string(highscore[i]);
                            text.setString(s);

                            text.setPosition(45, 80 + i * text.getCharacterSize());
                            window.draw(text);
                        }

                        window.display();
                        continue;
                    }
                }

                hs = false;
                /////////////////////////////////////////////////////////////////////////////////////////////////////

                //EXIT SCREEN:
                if (e.key.code == Keyboard::E) {
                    window.close();  
                }

                /////////////////////////////////////////////////////////////////////////////////////////////////////

                // GAME PLAY MODE SCREEN:
                if (e.key.code == Keyboard::G) {       
                    Texture gamePlayTexture;
                    gamePlayTexture.loadFromFile("img/game_play.png");
                    Sprite gamePlaySprite(gamePlayTexture);

                    window.clear();
                    window.draw(gamePlaySprite);
                    window.display();

                    // SELECT EITHER BEGINNER OR ADVANCE MODE:
                    bool selectingMode = true;

                    /////////////////////////////////////////////////////////////////////////////////////////////////////

                    while (selectingMode && window.isOpen()) {

                        Event e;
                        while (window.pollEvent(e)) {
                            if (e.type == Event::Closed)
                                window.close();

                            if (e.type == Event::KeyPressed) {

                                /////////////////////////////////////////////////////////////////////////////////////////////////////
                                // EXIT SCREEN:
                                if (e.key.code == Keyboard::E) {
                                    window.close(); 
                                }

                                /////////////////////////////////////////////////////////////////////////////////////////////////////
                                // 
                                //ADVANCE MODE SELECTION:
                                if (e.key.code == Keyboard::A) {
                                   
                                    Texture block_settings;
                                    block_settings.loadFromFile("img/block_selection_display.png");
                                    Sprite B_settings_sprite(block_settings);

                                    window.clear();
                                    window.draw(B_settings_sprite);
                                    window.display();

                                    bool blockchoice = true;
                                    level = true;

                                    /////////////////////////////////////////////////////////////////////////////////////////////////////

                                    while (blockchoice && window.isOpen()) {

                                        Event e;
                                        while (window.pollEvent(e)) {
                                            if (e.type == Event::Closed)
                                                window.close();
                                            /////////////////////////////////////////////////////////////////////////////////////////////////////
                                            if (e.type == Event::KeyPressed) {
                                                // EXIIT SCREEN:
                                                if (e.key.code == Keyboard::E) {
                                                    window.close();
                                                }

                                                /////////////////////////////////////////////////////////////////////////////////////////////////////
                                                //SELECTIING CIRCLE SHAPED BLOCKS:
                                                if (e.key.code == Keyboard::C) {
                                                    startGame = true;
                                                    selectingMode = false;  
                                                    blockchoice = false;
                                                    circleTile = true;
                                                    if (!isAdvancedMode) {

                                                        startGame = true;
                                                        fallSpeed = 0.2f; 
                                                        isAdvancedMode = true;
                                                        delay = 0.2f;
                                                    }

                                                }
                                                /////////////////////////////////////////////////////////////////////////////////////////////////////
                                                // SELECTING SQUARE SHAPED BLOCKS:
                                                else if (e.key.code == Keyboard::U) {
                                                    startGame = true;
                                                    selectingMode = false;  // Exit the loop
                                                    blockchoice = false;
                                                    fallSpeed = 0.2f;
                                                    if (!isAdvancedMode) {

                                                        startGame = true;
                                                        // Set faster falling speed for advanced mode
                                                        fallSpeed = 0.2f; // Adjust the speed as needed
                                                        isAdvancedMode = true;
                                                        // Additional logic if needed for 'A' key
                                                        delay = 0.2f;
                                                    }

                                                }

                                            }

                                        }

                                    }

                                }

                                /////////////////////////////////////////////////////////////////////////////////////////////////////
                                // 
                                // BEGINNER MODE SELECTION:
                                else if (e.key.code == Keyboard::B) {

                                    Texture block_settings;
                                    block_settings.loadFromFile("img/block_selection_display.png");
                                    Sprite B_settings_sprite(block_settings);

                                    window.clear();
                                    window.draw(B_settings_sprite);
                                    window.display();

                                    bool blockchoice = true;
                                    /////////////////////////////////////////////////////////////////////////////////////////////////////

                                    while (blockchoice && window.isOpen()) {

                                        Event e;
                                        while (window.pollEvent(e)) {

                                            /////////////////////////////////////////////////////////////////////////////////////////////////////
                                            if (e.type == Event::Closed)
                                                window.close();

                                            if (e.type == Event::KeyPressed) {
                                                //EXIT:
                                                if (e.key.code == Keyboard::E) {
                                                    window.close();
                                                }
                                                //CIRCLE BLOCKS:
                                                if (e.key.code == Keyboard::C) {
                                                    startGame = true;
                                                    selectingMode = false;  // Exit the loop
                                                    blockchoice = false;
                                                    circleTile = true;




                                                }
                                                //SQUUARE BLOCKS:
                                                else if (e.key.code == Keyboard::U) {
                                                    startGame = true;
                                                    selectingMode = false;  // Exit the loop
                                                    blockchoice = false;



                                                }
                                                // MAIN MENU :
                                                else if (e.key.code == Keyboard::M) {
                                                    showInstructions = false;
                                                    showSettings = false;
                                                    startGame = false;

                                                    Texture mainMenuTexture;
                                                    mainMenuTexture.loadFromFile("img/main_display.png");
                                                    Sprite mainMenuSprite(mainMenuTexture);

                                                    window.clear();
                                                    window.draw(mainMenuSprite);
                                                    window.display();
                                                }
                                                //MUTE:
                                                else if (e.key.code == Keyboard::Q) {
                                                    if (musicMuted) {
                                                        music.play(); // Resume music if it was muted
                                                        musicMuted = false;
                                                    }
                                                    else {
                                                        music.pause(); // Pause music
                                                        musicMuted = true;
                                                    }

                                                }

                                            }

                                        }

                                    }

                                }

                            }

                        }

                    }

                }
                /////////////////////////////////////////////////////////////////////////////////////////////////////
                // iNSTRUCTIONS DISPLAY SCREEN:
                else if (e.key.code == Keyboard::I) {
                    showInstructions = true; 
                    Texture instructionsTexture;
                    instructionsTexture.loadFromFile("img/instructions_display.png");
                    Sprite instructionsSprite(instructionsTexture);

                    window.clear();
                    window.draw(instructionsSprite);
                    window.display();
                }
                /////////////////////////////////////////////////////////////////////////////////////////////////////
                //SETTINGS DISPLAY SCREEN:
                else if (e.key.code == Keyboard::S) {
                    showSettings = true; 
                    Texture settingsTexture;
                    settingsTexture.loadFromFile("img/settings_display.png");
                    Sprite settingsSprite(settingsTexture);

                    window.clear();
                    window.draw(settingsSprite);
                    window.display();
                }
                /////////////////////////////////////////////////////////////////////////////////////////////////////
                //MAIN MENU:
                else if (e.key.code == Keyboard::M) {
                    showInstructions = false;
                    showSettings = false;
                    startGame = false;

                    Texture mainMenuTexture;
                    mainMenuTexture.loadFromFile("img/main_display.png");
                    Sprite mainMenuSprite(mainMenuTexture);

                    window.clear();
                    window.draw(mainMenuSprite);
                    window.display();
                }
                else if (e.key.code == Keyboard::Q) {
                    if (musicMuted) {
                        music.play(); // Resume music if it was muted
                        musicMuted = false;
                    }
                    else {
                        music.pause(); // Pause music
                        musicMuted = true;
                    }

                }

            }

        }

    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////


    while (window.isOpen()) {
        float time = clock.getElapsedTime().asSeconds(); //TIME
        clock.restart();
        timer += time;
        bombTimer += time;

        delta_x = 0;//POSITION
        rotate = false;

        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed)
                window.close();


            if (e.type == Event::KeyPressed) {
                switch (e.key.code) {
                case Keyboard::Up:
                    rotate = true;
                    break;
                case Keyboard::Left:
                    delta_x = -1;
                    break;
                case Keyboard::Right:
                    delta_x = 1;
                    break;
                case Keyboard::Space:
                    drop = true;
                    break;
                case Keyboard::P:
                    paused = !paused;
                    if (!paused) music.play();
                    break;
                case Keyboard::H:
                    hs = (paused || lost) ? !hs : hs;
                    break;
                case Keyboard::R:
                    restart = paused || lost;
                    break;
                case Keyboard::Q:
                    if (musicMuted) {
                        music.play(); // RESUME MUSIC 
                        musicMuted = false;
                    }
                    else {
                        music.pause(); // PAUSE MUSIC
                        musicMuted = true;
                    }
                    break;


                    // RETURN TO MAIN MENU SCREEN:
                case Keyboard::M:
                {
                    Texture mainMenuTexture;
                    mainMenuTexture.loadFromFile("img/main_display.png");
                    Sprite mainMenuSprite(mainMenuTexture);

                    window.clear();
                    window.draw(mainMenuSprite);
                    window.display();
                }
                // EXIT:
                case Keyboard::E:
                    window.close();
                    break;
                default:
                    break;
                };

            }

        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////

        // GAME RESTART:
        if (restart) {
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < N; j++)
                    gameGrid[i][j] = 0;
            }

            for (int i = 0; i < 4; i++) {
                point_1[i][0] = 0;
                point_1[i][1] = 0;
            }

            timer = 0;
            bombTimer = 0;
            colorNum = 1;
            score = 0;
            restart = false;
            paused = false;
            lost = false;
            drop = false;

            music.play();
            playGameOverSound = false;
            continue;
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////

        // HIGHSCORE DISPLAY SCREEN:
        if (hs) {
            window.clear(Color::Black);
            window.draw(hiscore);

            Text text;
            text.setFont(font);
            text.setFillColor(Color::White);
            text.setStyle(Text::Bold);
            text.setCharacterSize(24);
            for (int i = 0; i < 10; i++) {
                std::string s;
                if (i < 9)
                    s += " ";
                s += std::to_string(i + 1);

                s += "...............";

                int score = highscore[i];
                if (score < 10)
                    s += "  ";
                else if (score < 100)
                    s += " ";
                s += std::to_string(highscore[i]);
                text.setString(s);

                text.setPosition(45, 80 + i * text.getCharacterSize());
                window.draw(text);
            }

            window.display();
            continue;
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////

        // PAUSE SCREEN DISPLAY:
        if (paused) {
            window.clear(Color::Black);
            window.draw(overlay);
            window.display();
            continue;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////

        // LOST SCREEN DISPLAY:
        if (lost) {
            window.clear(Color::Black);
            window.draw(lose);
            window.display();
            // Stop the music
            music.stop();

            // GAME OVER SOUND:
            if (!playGameOverSound) {
                gameOverSound.play();
                playGameOverSound = true;
            }
            continue;
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////

        // Update pieces
        hs = false;

        if (Keyboard::isKeyPressed(Keyboard::Down))
            delay = 0.05;
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //ROTATIONS: 
        if (rotate) {
            rotates(point_1[1][0], point_1[1][1]);

            if (!anamoly()) {
                for (int i = 0; i < 4; i++) {
                    point_1[i][0] = point_2[i][0];
                    point_1[i][1] = point_2[i][1];
                }
            }
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////

        move(delta_x);
        float delay = (isAdvancedMode) ? 0.1f : 0.3f; 

        /////////////////////////////////////////////////////////////////////////////////////////////////////

        if (isAdvancedMode) {
            fallSpeed = 0.1f; // FAST FALLING SPEED FOR ADVANCED MODE
            fallingAdvancedPiece(timer, delay, colorNum, drop, bombTimer);

        }
        else {
            fallSpeed = 0.3f; // NORMAL FALLING SPEED FOR BEGINNER MODE
        }

        if (isBeginnerMode) {
            fallingBeginnerPiece(timer, delay, colorNum, drop, bombTimer);
        }
        else {
            fallingAdvancedPiece(timer, delay, colorNum, drop, bombTimer);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////

        // Update score and check for lines cleared
        checkLines(score, isAdvancedMode);

        /////////////////////////////////////////////////////////////////////////////////////////////////////

        // Check for lost
        for (int i = 0; i < N; i++) {
            if (gameGrid[0][i]) {
                updateHighscore(highscore, score);
                lost = true;
                break;
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////

        scoreText.setString("Score: " + std::to_string(score));
        playerNameText.setString("Player: " + playerName); // Update player's name

        window.clear(Color::Black);
        window.draw(background);

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        // CREATING THE SHAPES AND SPRITE:
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                if (!gameGrid[i][j])
                    continue;

                if (circleTile) {
                    sprite2.setTextureRect(IntRect(gameGrid[i][j] * 18, 0, 18, 18));
                    sprite2.setPosition(j * 18, i * 18);
                    sprite2.move(28, 31); //offset
                    window.draw(sprite2);
                }
                else {
                    sprite.setTextureRect(IntRect(gameGrid[i][j] * 18, 0, 18, 18));
                    sprite.setPosition(j * 18, i * 18);
                    sprite.move(28, 31); //offset
                    window.draw(sprite);
                }

            }
        }

        for (int i = 0; i < 4; i++) {
            if (circleTile) {
                sprite2.setTextureRect(IntRect(colorNum * 18, 0, 18, 18));
                sprite2.setPosition(point_1[i][0] * 18, point_1[i][1] * 18);
                sprite2.move(28, 31); //offset
                window.draw(sprite2);
            }
            else {
                sprite.setTextureRect(IntRect(colorNum * 18, 0, 18, 18));
                sprite.setPosition(point_1[i][0] * 18, point_1[i][1] * 18);
                sprite.move(28, 31); //offset
                window.draw(sprite);
            }

        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////

        scoreText.setPosition(280, 80); 
        window.draw(frame);
        window.draw(scoreText);
        window.draw(playerNameText);
        displayGameLevel(window, font, score);


        window.display();

    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////

}

/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////
