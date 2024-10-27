#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <corecrt_math_defines.h>

using namespace std;

enum Direction { UP, DOWN, LEFT, RIGHT };
enum GameState { MENU, PLAYING, PAUSED, LEADERBOARD, INPUTNAME, GAME_OVER};

string name = "player";
const int WIDTH = 1000;
const int HEIGHT = 800; 
const int BLOCK_SIZE = 20;

float GAME_SPEED = 0.2f; 
float BASE_GAME_SPEED = 0.2f;
float MIN_GAME_SPEED = 0.05f;
int checkScore = 0;

class InputField {
public:
    InputField() {
        font.loadFromFile("CoffeeSoda.TTF");
        inputText.setFont(font);
        inputText.setCharacterSize(30);
        inputText.setFillColor(sf::Color::White);
        inputText.setPosition(210, 200);

        inputBox.setSize(sf::Vector2f(400, 50));
        inputBox.setFillColor(sf::Color::Black);
        inputBox.setOutlineColor(sf::Color::White);
        inputBox.setOutlineThickness(2);
        inputBox.setPosition(200, 200);

        saveButton.setFont(font);
        saveButton.setString("Save");
        saveButton.setCharacterSize(30);
        saveButton.setFillColor(sf::Color::White);
        saveButton.setPosition(200, 250);
    }

    void handleEvent(sf::RenderWindow& window, const sf::Event& event, GameState& gameState) {
        if (event.type == sf::Event::TextEntered) {
            if (event.text.unicode == '\b') {
                if (!inputString.empty())
                    inputString.pop_back();
            }
            else if (event.text.unicode < 128) {
                inputString += static_cast<char>(event.text.unicode);
            }
        }
        if (inputString.length()>0 && sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (saveButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    name = inputString;
                    gameState = MENU;
                }
            }
        }
        inputText.setString(inputString);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(inputBox);
        window.draw(inputText);
        window.draw(saveButton);
    }

    string getInputString() const {
        return inputString;
    }

private:
    sf::RectangleShape inputBox;
    sf::Text inputText,saveButton;
    string inputString = name;
    sf::Font font;
};

class Leaderboard {
public:
    Leaderboard() {
        font.loadFromFile("CoffeeSoda.TTF");

        title.setFont(font);
        title.setString("Leaderboards");
        title.setCharacterSize(60);
        title.setFillColor(sf::Color::White);
        title.setPosition(WIDTH / 2.f - title.getGlobalBounds().width / 2.f, HEIGHT / 10.f - 50);

        title1.setFont(font);
        title1.setString("Top 10");
        title1.setCharacterSize(40);
        title1.setFillColor(sf::Color::White);
        title1.setPosition(WIDTH / 2.f - title1.getGlobalBounds().width / 2.f, HEIGHT / 10.f + 50);

        nameLabel.setFont(font);
        nameLabel.setString("NAME");
        nameLabel.setCharacterSize(30);
        nameLabel.setFillColor(sf::Color::White);
        nameLabel.setPosition(WIDTH / 4.f, HEIGHT / 4.f);

        scoreLabel.setFont(font);
        scoreLabel.setString("HIGHSCORE");
        scoreLabel.setCharacterSize(30);
        scoreLabel.setFillColor(sf::Color::White);
        scoreLabel.setPosition(WIDTH / 1.5f, HEIGHT / 4.f);

        backButton.setFont(font);
        backButton.setString("Back");
        backButton.setCharacterSize(30);
        backButton.setFillColor(sf::Color::White);
        backButton.setPosition(WIDTH / 2.f - backButton.getGlobalBounds().width / 2.f, HEIGHT - 100);

        loadScores();
    }

    void handleEvents(sf::RenderWindow& window, sf::Event& event, GameState& gameState) {
        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            if (backButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                gameState = MENU;
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(title);
        window.draw(title1);
        window.draw(nameLabel);
        window.draw(scoreLabel);

        float yOffset = HEIGHT / 4.f + 50;
        for (size_t i = 0; i < players.size(); ++i) {
            sf::Text playerName(players[i].first, font, 25);
            playerName.setFillColor(sf::Color::White);
            playerName.setPosition(WIDTH / 4.f, yOffset + (i * 30));

            sf::Text playerScore(to_string(players[i].second), font, 25);
            playerScore.setFillColor(sf::Color::White);
            playerScore.setPosition(WIDTH / 1.5f, yOffset + (i * 30));

            window.draw(playerName);
            window.draw(playerScore);
        }

        window.draw(backButton);
    }

    void addScore(const string& playerName, int playerScore) {
        bool playerExists = false;

        for (auto& player : players) {
            transform(player.first.begin(), player.first.end(), player.first.begin(), ::tolower);
            if (player.first == playerName) {
                playerExists = true;
                if (playerScore > player.second) {
                    player.second = playerScore;
                }
                break;
            }
        }

        if (!playerExists) {
            players.push_back(make_pair(playerName, playerScore));
        }

        sortLeaderboard();

        saveScores();
    }

private:
    sf::Text title, title1, nameLabel, scoreLabel, backButton;
    sf::Font font;

    vector<pair<string, int>> players;

    void loadScores() {
        ifstream file("leaderboard.txt");
        if (file.is_open()) {
            string name;
            int score;
            while (file >> name >> score) {
                players.push_back(make_pair(name, score));
            }
            file.close();
        }

        sortLeaderboard();
    }

    void saveScores() {
        ofstream file("leaderboard.txt");
        if (file.is_open()) {
            for (const auto& player : players) {
                file << player.first << " " << player.second << endl;
            }
            file.close();
        }
    }

    void sortLeaderboard() {
        sort(players.begin(), players.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
            return a.second > b.second;
            });

        if (players.size() > 10) {
            players.resize(10);
        }
    }

};

class Snake {
public:
    Snake() {
        body.push_back(sf::RectangleShape(sf::Vector2f(BLOCK_SIZE, BLOCK_SIZE)));
        body[0].setPosition(WIDTH / 2.f + 20, HEIGHT / 2.f + 100);
        body[0].setFillColor(sf::Color::Green);
        dir = RIGHT;
        score = 0;
    }

    void move() {
        for (size_t i = body.size() - 1; i > 0; --i) {
            body[i].setPosition(body[i - 1].getPosition());
        }
        switch (dir) {
        case UP:    body[0].move(0, -BLOCK_SIZE); break;
        case DOWN:  body[0].move(0, BLOCK_SIZE); break;
        case LEFT:  body[0].move(-BLOCK_SIZE, 0); break;
        case RIGHT: body[0].move(BLOCK_SIZE, 0); break;
        }
    }

    void grow() {
        sf::Vector2f lastSegmentPosition = body.back().getPosition();
        sf::RectangleShape newSegment(sf::Vector2f(BLOCK_SIZE, BLOCK_SIZE));
        newSegment.setFillColor(sf::Color::Green);
        newSegment.setPosition(lastSegmentPosition);
        body.push_back(newSegment);
        score += 10;
    }

    void addScore(int points) {
        score += points;
    }


    void setDirection(Direction newDir) {
        if (newDir == UP && dir != DOWN) dir = UP;
        else if (newDir == DOWN && dir != UP) dir = DOWN;
        else if (newDir == LEFT && dir != RIGHT) dir = LEFT;
        else if (newDir == RIGHT && dir != LEFT) dir = RIGHT;
    }

    vector<sf::RectangleShape>& getBody() { return body; }
    int getScore() const { return score; }

private:
    vector<sf::RectangleShape> body;
    Direction dir;
    int score;
};

class Food {
public:
    Food(vector<sf::RectangleShape> &body) {
        food.setRadius(BLOCK_SIZE/2.f);
        food.setFillColor(sf::Color::Red);
        respawn(body);
    }

    void respawn(vector<sf::RectangleShape> body) {
        do {
            int maxX = (WIDTH - 2 * BLOCK_SIZE) / BLOCK_SIZE;
            int maxY = (HEIGHT - 100 - BLOCK_SIZE) / BLOCK_SIZE;

            int randomX = rand() % maxX;
            int randomY = rand() % maxY;

            float x = BLOCK_SIZE + randomX * BLOCK_SIZE;
            float y = 100 + randomY * BLOCK_SIZE;

            food.setPosition(x, y);
        } while (isOnSnake(body));
    }

    sf::CircleShape getFood() const { return food; }

private:
    sf::CircleShape food;
    bool isOnSnake(const vector<sf::RectangleShape>& body) {
        for (const auto& segment : body) {
            sf::Vector2f segmentPos = segment.getPosition();
            if (food.getGlobalBounds().intersects(sf::FloatRect(segmentPos.x, segmentPos.y, BLOCK_SIZE, BLOCK_SIZE))) {
                return true;
            }
        }
        return false;
    }
};

class SpecialFood {
public:
    SpecialFood() {
        star = createStar(0, 0, 10, 5, 5);
        star.setFillColor(sf::Color::Cyan);
        stayDuration = sf::seconds(10);
        isActive = false;
        progressBar.setFillColor(sf::Color::Green);
        progressBar.setSize(sf::Vector2f(PROGRESS_BAR_WIDTH, PROGRESS_BAR_HEIGHT));
    }

    void draw(sf::RenderWindow& window) {
        if (isActive) {
            window.draw(star);
            drawProgressBar(window);
        }
    }

    bool checkCollision(const sf::RectangleShape& body) {
        if (isActive && star.getGlobalBounds().intersects(body.getGlobalBounds())) {
            isActive = false;
            return true;
        }
        return false;
    }

    bool isActiveState() const {
        return isActive;
    }

    void respawn(int a,int b,vector<sf::RectangleShape> &body) {
        
        do {
            int maxX = (WIDTH - 2 * BLOCK_SIZE) / BLOCK_SIZE;
            int maxY = (HEIGHT - 100 - BLOCK_SIZE) / BLOCK_SIZE;

            int randomX = rand() % maxX;
            int randomY = rand() % maxY;

            float x = 2*BLOCK_SIZE + randomX * BLOCK_SIZE;
            float y = 120 + randomY * BLOCK_SIZE;

            if (a == -50 && b == -50) {
                star.setPosition(a, b);
                isActive = false;
            }
            else {
                star.setPosition(x, y);
                isActive = true;
                timeElapsed = sf::Time::Zero;
            }
        } while (isOnSnake(body));

        progressBar.setPosition(star.getPosition().x - 20, star.getPosition().y - 20);
    }

    void updateTimer(sf::Time deltaTime) {
        if (isActive) {
            timeElapsed += deltaTime;
            float timePercent = 1.0f - (timeElapsed.asSeconds() / stayDuration.asSeconds());
            progressBar.setSize(sf::Vector2f(PROGRESS_BAR_WIDTH * timePercent, PROGRESS_BAR_HEIGHT));
            if (timeElapsed >= stayDuration) {
                isActive = false;
            }
        }
    }
    

private:
    sf::ConvexShape star;
    bool isActive;
    sf::Time timeElapsed;
    sf::Time stayDuration;

    sf::RectangleShape progressBar;
    const float PROGRESS_BAR_WIDTH = 50.0f; 
    const float PROGRESS_BAR_HEIGHT = 5.0f; 

    sf::ConvexShape createStar(float x, float y, float outerRadius, float innerRadius, int points) {
        sf::ConvexShape star;
        star.setPointCount(points * 2.f);

        float angleStep = M_PI / points;

        for (int i = 0; i < points * 2; i++) {
            float radius = (i % 2 == 0) ? outerRadius : innerRadius;
            float angle = i * angleStep;

            float px = x + cos(angle) * radius;
            float py = y + sin(angle) * radius;

            star.setPoint(i, sf::Vector2f(px, py));
        }
        star.setOutlineColor(sf::Color::Yellow);
        star.setOutlineThickness(2.f);

        return star;
    }

    bool isOnSnake(const vector<sf::RectangleShape>& body) {
        for (const auto& segment : body) {
            sf::Vector2f segmentPos = segment.getPosition();
            if (star.getGlobalBounds().intersects(sf::FloatRect(segmentPos.x, segmentPos.y, BLOCK_SIZE, BLOCK_SIZE))) {
                return true;
            }
        }
        return false;
    }

    void drawProgressBar(sf::RenderWindow& window) {
        window.draw(progressBar);
    }
};

class Game {
public:
    Game() : window(sf::VideoMode(WIDTH, HEIGHT), "Snake Game"), leaderboard(), volume(30.0f) ,food(snake.getBody()){
        window.setFramerateLimit(60);
        font.loadFromFile("CoffeeSoda.TTF");

        if (!iconTexture.loadFromFile("images/icon.png")) {
            cerr << "Error loading speaker icon image" << endl;
        }

        iconTexture.setSmooth(true);
        iconSprite.setTexture(iconTexture);
        iconSprite.setScale(0.32f, 0.32f);
        iconSprite.setPosition(420, 250);

        title.setFont(font);
        title.setString("Snake Game");
        title.setCharacterSize(100);
        title.setFillColor(sf::Color::Green);
        title.setPosition(WIDTH / 2.f - title.getGlobalBounds().width / 2.f, HEIGHT / 4.f - 150);

        welcome.setFont(font);
        welcome.setString("Welcome | " + name);
        welcome.setCharacterSize(40);
        welcome.setFillColor(sf::Color::Blue);
        welcome.setPosition(WIDTH / 2.f - welcome.getGlobalBounds().width / 2.f, HEIGHT / 4.f);

        startButton.setFont(font);
        startButton.setString("Start");
        startButton.setCharacterSize(30);
        startButton.setFillColor(sf::Color::White);
        startButton.setPosition(WIDTH / 2.f - startButton.getGlobalBounds().width / 2.f -100, HEIGHT / 2.f);

        gameOverText.setFont(font);
        gameOverText.setString("Game Over");
        gameOverText.setCharacterSize(70);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setPosition(WIDTH / 2.f - gameOverText.getGlobalBounds().width / 2.f, HEIGHT / 4.f);

        leaderboardButton.setFont(font);
        leaderboardButton.setString("Leaderboard");
        leaderboardButton.setCharacterSize(30);
        leaderboardButton.setFillColor(sf::Color::White);
        leaderboardButton.setPosition(WIDTH / 2.f - leaderboardButton.getGlobalBounds().width / 2.f -100, HEIGHT / 2.f + 50);

        inputButton.setFont(font);
        inputButton.setString("Change Name");
        inputButton.setCharacterSize(30);
        inputButton.setFillColor(sf::Color::White);
        inputButton.setPosition(WIDTH / 2.f - inputButton.getGlobalBounds().width / 2.f -100, HEIGHT / 2.f + 100);

        quitButton.setFont(font);
        quitButton.setString("Quit");
        quitButton.setCharacterSize(30);
        quitButton.setFillColor(sf::Color::White);
        quitButton.setPosition(WIDTH / 2.f - quitButton.getGlobalBounds().width / 2.f -100, HEIGHT / 2.f + 150);

        pauseButton.setFont(font);
        pauseButton.setString("Pause");
        pauseButton.setCharacterSize(25);
        pauseButton.setFillColor(sf::Color::White);
        pauseButton.setPosition(WIDTH - 150, 10);  


        quitButtonPlaying.setFont(font);
        quitButtonPlaying.setString("Menu");
        quitButtonPlaying.setCharacterSize(25);
        quitButtonPlaying.setFillColor(sf::Color::White);
        quitButtonPlaying.setPosition(WIDTH - 150, 50);

        scoreText.setFont(font);
        scoreText.setCharacterSize(30);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10, 10);

        volumeBar.setSize(sf::Vector2f(150, 10));
        volumeBar.setFillColor(sf::Color::White);
        volumeBar.setPosition(WIDTH - 200, HEIGHT - 50);

        volumeHandle.setSize(sf::Vector2f(7, 20));
        volumeHandle.setFillColor(sf::Color::Red);
        volumeHandle.setPosition(volumeBar.getPosition().x + volumeBar.getSize().x * (volume / 100.0f) - volumeHandle.getSize().x / 2, volumeBar.getPosition().y - 5);

        if (!speakerTexture.loadFromFile("images/volume.png")) {
            cerr << "Error loading speaker icon image" << endl;
        }

        speakerTexture.setSmooth(true);
        speakerSprite.setTexture(speakerTexture);
        speakerSprite.setScale(0.2f, 0.2f);
        speakerSprite.setPosition(WIDTH - 250, HEIGHT - 70);

        gameState = MENU;
        lastMoveTime = sf::Time::Zero;

        menuMusic.openFromFile("Sounds/menu.wav");
        menuMusic.setLoop(true);
        foodMusic.openFromFile("Sounds/food.wav");
        moveMusic.openFromFile("Sounds/move.wav");
        gameOverMusic.openFromFile("Sounds/gameover.wav");

        menuMusic.setVolume(volume);
        moveMusic.setVolume(volume);
        foodMusic.setVolume(volume);
        gameOverMusic.setVolume(volume);

        draggingHandle = false;
    }

    void run() {

        sf::Image icon;
        if (!icon.loadFromFile("images/icon.png")) {
            cout << "error loading image icon" << endl;
        }
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

        while (window.isOpen()) {
            handleEvents();
            window.clear();
            if (gameState == MENU) {
                drawMenu();
            }
            else if (gameState == PLAYING) {
                update();
                drawGame();
            }
            else if (gameState == PAUSED) {
                drawGame();
                drawPausedScreen();
            }
            else if (gameState == LEADERBOARD) {
                leaderboard.draw(window);
            }
            else if(gameState == INPUTNAME){
                inputField.draw(window);
            }
            else if (gameState == GAME_OVER) {
                drawGameOver();
            }
            window.display();
        }
         debounceClock.restart();
    }

private:
    sf::Clock debounceClock;
    const float DEBOUNCE_TIME = 0.3f;
    GameState gameState;
    sf::RenderWindow window;
    sf::Font font;
    sf::Text title, startButton, welcome, leaderboardButton, quitButton, pauseButton, quitButtonPlaying, scoreText, gameOverText, inputButton;
    sf::Music menuMusic,foodMusic,moveMusic,gameOverMusic;
    float volume;
    bool draggingHandle;

    sf::Texture speakerTexture,iconTexture;
    sf::Sprite speakerSprite,iconSprite;

    sf::RectangleShape volumeBar;
    sf::RectangleShape volumeHandle;

    Snake snake;
    Leaderboard leaderboard;
    InputField inputField;
    int menustate = 0,gamestate = 0;

    SpecialFood specialFood;
    Food food;
    sf::Clock clock;
    sf::Time lastMoveTime;

    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (gameState == MENU) {
                if (menustate == 0) {
                    menuMusic.play();
                    menustate = 1;
                }
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                        if (startButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            menuMusic.stop();
                            gameState = PLAYING;
                            gamestate = 0;
                            snake = Snake();
                            food = Food(snake.getBody());
                            clock.restart();
                        }
                        else if (leaderboardButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            gameState = LEADERBOARD;
                        }
                        else if (inputButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            gameState = INPUTNAME;
                        }
                        else if (quitButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            window.close();
                        }
                    }
                }
                if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (volumeHandle.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        draggingHandle = true;
                    }
                }

                if (event.type == sf::Event::MouseButtonReleased) {
                    draggingHandle = false;
                }

                if (draggingHandle) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    updateVolumeFromMouse(mousePos.x);
                }
            }

            if (gameState == GAME_OVER) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                        if (gameOverText.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            leaderboard.addScore(name, snake.getScore());
                            gameOverMusic.stop();
                            menustate = 0;
                            gamestate = 0;
                            gameState = MENU;
                        }
                    }
                }
            }

            if (gameState == LEADERBOARD) {
                if (gameState == LEADERBOARD) {
                    leaderboard.handleEvents(window, event, gameState);
                }
            }

            if (gameState == INPUTNAME) {
                if (gameState == INPUTNAME) {
                    inputField.handleEvent(window,event, gameState);
                    welcome.setString("Welcome | " + name);
                }
            }

            if (gameState == PLAYING) {
                if (gamestate == 0) {
                    moveMusic.play();
                    gamestate = 1;
                }
                if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (pauseButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        if (debounceClock.getElapsedTime().asSeconds() >= DEBOUNCE_TIME) {
                            gameState = PAUSED;
                            moveMusic.stop();
                            debounceClock.restart();
                        }
                    }
                    else if (quitButtonPlaying.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        gameState = MENU;
                        moveMusic.stop();
                        menustate = 0;
                    }
                }
                if (event.type == sf::Event::KeyPressed) {
                    switch (event.key.code) {
                    case sf::Keyboard::Up:    snake.setDirection(UP); break;
                    case sf::Keyboard::Down:  snake.setDirection(DOWN); break;
                    case sf::Keyboard::Left:  snake.setDirection(LEFT); break;
                    case sf::Keyboard::Right: snake.setDirection(RIGHT); break;
                    case sf::Keyboard::Return:
                        gameState = PAUSED;
                        moveMusic.stop(); 
                        break;
                    case sf::Keyboard::Escape: 
                        gameState = MENU;
                        moveMusic.stop();
                        menustate = 0; break;
                    default: break;
                    }
                }
            }

            if (gameState == PAUSED) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (pauseButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        if (debounceClock.getElapsedTime().asSeconds() >= DEBOUNCE_TIME) {
                            moveMusic.play();
                            pauseButton.setString("Pause");
                            gameState = PLAYING;
                            gamestate = 0;
                            debounceClock.restart();
                        }
                    }
                    else if (quitButtonPlaying.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        moveMusic.stop();
                        menustate = 0;
                        gamestate = 1;
                        gameState = MENU;
                    }
                    
                }
                if (event.type == sf::Event::KeyPressed) {
                    if(event.key.code == sf::Keyboard::Key::P){
                        moveMusic.play();
                        pauseButton.setString("Pause");
                        gameState = PLAYING;
                        gamestate = 0;
                    }
                }
            }


        }
    }

    void updateVolumeFromMouse(int mouseX) {
        float barStartX = volumeBar.getPosition().x;
        float barWidth = volumeBar.getSize().x;
        float newVolume = ((mouseX - barStartX) / barWidth) * 100.0f;

        if (newVolume < 0) newVolume = 0;
        if (newVolume > 100) newVolume = 100;

        volume = newVolume;
        menuMusic.setVolume(volume);
        moveMusic.setVolume(volume);
        foodMusic.setVolume(volume);
        gameOverMusic.setVolume(volume);

        volumeHandle.setPosition(barStartX + (barWidth * (volume / 100.0f)) - volumeHandle.getSize().x / 2, volumeHandle.getPosition().y);
    }

    void update() {
        sf::Time deltaTime = clock.restart();
        lastMoveTime += deltaTime;

        updateGameSpeed();

        if (lastMoveTime.asSeconds() >= GAME_SPEED) {
            snake.move();
            lastMoveTime = sf::Time::Zero;
            checkCollision();
            updateScore();
            manageSpecialFood();
        }
        specialFood.updateTimer(deltaTime);
    }


    void updateGameSpeed() {
        if (snake.getScore() >= 500) {
            GAME_SPEED = max(BASE_GAME_SPEED - 0.15f, MIN_GAME_SPEED);
        }
        else if (snake.getScore() >= 250) {
            GAME_SPEED = max(BASE_GAME_SPEED - 0.1f, MIN_GAME_SPEED);
        }
        else if (snake.getScore() >= 100) {
            GAME_SPEED = max(BASE_GAME_SPEED - 0.05f, MIN_GAME_SPEED);
        }
        else {
            GAME_SPEED = BASE_GAME_SPEED;
        }
    }

    void manageSpecialFood() {
        if (snake.getScore() > 0 && snake.getScore() % 100 == 0 && !specialFood.isActiveState() && snake.getScore() > checkScore) {
            specialFood.respawn(0,0,snake.getBody());
            checkScore = snake.getScore();
        }
    }

    void drawMenu() {
        window.draw(iconSprite);
        window.draw(title);
        window.draw(welcome);
        window.draw(startButton);
        window.draw(leaderboardButton);
        window.draw(inputButton);
        window.draw(quitButton);
        window.draw(volumeBar);
        window.draw(volumeHandle);
        window.draw(speakerSprite);
    }

    void drawGame() {
        for (const auto& segment : snake.getBody()) {
            window.draw(segment);
        }
        window.draw(food.getFood());
        specialFood.draw(window);
        updateScore();
        window.draw(scoreText);
        window.draw(pauseButton);
        window.draw(quitButtonPlaying);
        drawGameBoundary(window);
    }

    void drawPausedScreen() {
        sf::Text pausedText;
        pausedText.setFont(font);
        pausedText.setString("Paused");
        pausedText.setCharacterSize(50);
        pausedText.setFillColor(sf::Color::White);
        pausedText.setPosition(WIDTH / 2.f - pausedText.getGlobalBounds().width / 2.f, HEIGHT / 2.f - 50);
        pauseButton.setString("Play");
        window.draw(pausedText);
    }

    void drawGameBoundary(sf::RenderWindow& window) {
        sf::RectangleShape boundary(sf::Vector2f(WIDTH - 2 * BLOCK_SIZE, HEIGHT - 100 - BLOCK_SIZE));
        sf::RectangleShape boundary1(sf::Vector2f(WIDTH - 2 * BLOCK_SIZE, HEIGHT - 100 - BLOCK_SIZE));

        boundary.setPosition(BLOCK_SIZE, 100);
        boundary.setOutlineColor(sf::Color::Black);
        boundary.setOutlineThickness(20);
        boundary.setFillColor(sf::Color::Transparent);

        boundary1.setPosition(BLOCK_SIZE, 100);
        boundary1.setOutlineColor(sf::Color::White); 
        boundary1.setOutlineThickness(5);      
        boundary1.setFillColor(sf::Color::Transparent);  

        window.draw(boundary);
        window.draw(boundary1);
    }


    void drawGameOver() {

        window.draw(gameOverText);
    }

    void drawLeaderboard() {
        window.draw(gameOverText);
    }

    void checkCollision() {
        if (snake.getBody()[0].getPosition().x < BLOCK_SIZE ||
            snake.getBody()[0].getPosition().x >= WIDTH - BLOCK_SIZE ||
            snake.getBody()[0].getPosition().y < 100 ||
            snake.getBody()[0].getPosition().y >= HEIGHT - BLOCK_SIZE) {

            moveMusic.stop();
            gameOverMusic.play();
            gameState = GAME_OVER;
        }

        for (size_t i = 1; i < snake.getBody().size(); ++i) {
            if (snake.getBody()[0].getGlobalBounds().intersects(snake.getBody()[i].getGlobalBounds())) {
                moveMusic.stop();
                gameOverMusic.play();
                gameState = GAME_OVER;
            }
        }

        if (snake.getBody()[0].getGlobalBounds().intersects(food.getFood().getGlobalBounds())) {
            foodMusic.stop();
            foodMusic.play();
            snake.grow();
            food.respawn(snake.getBody());
        }

        if (specialFood.checkCollision(snake.getBody()[0])) {
            foodMusic.stop();
            foodMusic.play();
            snake.addScore(20);
            snake.grow();
            specialFood.respawn(-50, -50,snake.getBody());
        }
    }

    void updateScore() {
        scoreText.setString("Score: " + to_string(snake.getScore()));
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}
