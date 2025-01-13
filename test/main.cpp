#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <memory>

// -----------------------------------------------------------------------------
// 1. STRUKTURY I DEFINICJE
// -----------------------------------------------------------------------------

struct PlayerData
{
    std::string nick;
    int score = 0;
};

class GameObject
{
public:
    virtual ~GameObject() {}
    virtual void update(float deltaTime) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
};

enum class GameState
{
    MENU,
    ENTER_NAME,
    PLAY,
    TUTORIAL,       // Zamiast HELP
    SCOREBOARD,
    EXIT_CONFIRM,
    GAME_OVER,
    LEVEL_COMPLETE,
    EXIT
};

// -----------------------------------------------------------------------------
// 2. Zapisywanie / odczytywanie tablicy wyników
// -----------------------------------------------------------------------------

bool saveScoreboard(const std::vector<PlayerData>& players)
{
    std::ofstream file("scoreboard.txt");
    if (!file.is_open())
    {
        std::cerr << "Nie mozna otworzyc scoreboard.txt do zapisu!\n";
        return false;
    }

    file << players.size() << "\n";
    for (auto& p : players)
    {
        file << p.nick << " " << p.score << "\n";
    }
    file.close();
    return true;
}

bool loadScoreboard(std::vector<PlayerData>& players)
{
    std::ifstream file("scoreboard.txt");
    if (!file.is_open())
    {
        // Plik mo¿e nie istnieæ – nie uznajemy tego za krytyczny b³¹d
        std::cerr << "Brak pliku scoreboard.txt lub nie mozna go otworzyc.\n";
        return false;
    }

    size_t count;
    file >> count;
    players.clear();
    players.reserve(count);

    for (size_t i = 0; i < count; i++)
    {
        PlayerData pd;
        file >> pd.nick >> pd.score;
        players.push_back(pd);
    }

    file.close();
    return true;
}

// -----------------------------------------------------------------------------
// 3. Klasy obiektów gry: Pi³ka, Paletka, Cele
// -----------------------------------------------------------------------------

class Ball : public GameObject
{
private:
    sf::CircleShape shape;
    sf::Vector2f velocity;

public:
    Ball(float radius = 10.0f)
    {
        shape.setRadius(radius);
        shape.setFillColor(sf::Color::Yellow);
        shape.setOrigin(radius, radius);
        shape.setPosition(512.f, 384.f);
        velocity = sf::Vector2f(200.f, 200.f);
    }

    void update(float deltaTime) override
    {
        sf::Vector2f pos = shape.getPosition();
        pos += velocity * deltaTime;
        shape.setPosition(pos);

        // Odbicie od lewej/prawej krawêdzi
        if (pos.x - shape.getRadius() < 0.f)
        {
            shape.setPosition(shape.getRadius(), pos.y);
            velocity.x = -velocity.x;
        }
        else if (pos.x + shape.getRadius() > 1024.f)
        {
            shape.setPosition(1024.f - shape.getRadius(), pos.y);
            velocity.x = -velocity.x;
        }

        // Odbicie od górnej krawêdzi
        if (pos.y - shape.getRadius() < 0.f)
        {
            shape.setPosition(pos.x, shape.getRadius());
            velocity.y = -velocity.y;
        }
    }

    void draw(sf::RenderWindow& window) override
    {
        window.draw(shape);
    }

    sf::FloatRect getGlobalBounds() const
    {
        return shape.getGlobalBounds();
    }

    sf::Vector2f& getVelocity()
    {
        return velocity;
    }

    void setPosition(float x, float y)
    {
        shape.setPosition(x, y);
    }

    sf::Vector2f getPosition() const
    {
        return shape.getPosition();
    }

    float getRadius() const
    {
        return shape.getRadius();
    }
};

class Paddle : public GameObject
{
private:
    sf::RectangleShape shape;
    float speed;
public:
    Paddle(sf::Vector2f size = sf::Vector2f(80.f, 20.f))
    {
        shape.setSize(size);
        shape.setFillColor(sf::Color::Cyan);
        shape.setOrigin(size.x / 2.f, size.y / 2.f);
        shape.setPosition(512.f, 700.f);
        speed = 300.f;
    }

    void update(float deltaTime) override
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            shape.move(-speed * deltaTime, 0.f);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            shape.move(speed * deltaTime, 0.f);
        }

        // Ograniczenie ruchu (lewa/prawa strona)
        if (shape.getPosition().x - shape.getSize().x / 2.f < 0.f)
        {
            shape.setPosition(shape.getSize().x / 2.f, shape.getPosition().y);
        }
        else if (shape.getPosition().x + shape.getSize().x / 2.f > 1024.f)
        {
            shape.setPosition(1024.f - shape.getSize().x / 2.f, shape.getPosition().y);
        }
    }

    void draw(sf::RenderWindow& window) override
    {
        window.draw(shape);
    }

    sf::FloatRect getGlobalBounds() const
    {
        return shape.getGlobalBounds();
    }
};

// Bazowa klasa celu
class Target : public GameObject
{
protected:
    bool destroyed = false;
public:
    virtual sf::FloatRect getGlobalBounds() const = 0;
    bool isDestroyed() const { return destroyed; }
    void destroy() { destroyed = true; }
};

// IrregularBlock (poziom 1)
class IrregularBlock : public Target
{
private:
    sf::ConvexShape shape;
public:
    IrregularBlock(sf::Vector2f position)
    {
        shape.setPointCount(5);
        shape.setPoint(0, sf::Vector2f(0.f, 0.f));
        shape.setPoint(1, sf::Vector2f(50.f, 0.f));
        shape.setPoint(2, sf::Vector2f(70.f, 20.f));
        shape.setPoint(3, sf::Vector2f(50.f, 40.f));
        shape.setPoint(4, sf::Vector2f(0.f, 30.f));
        shape.setFillColor(sf::Color::Magenta);
        shape.setPosition(position);
    }

    void update(float) override {}
    void draw(sf::RenderWindow& window) override
    {
        if (!destroyed) window.draw(shape);
    }
    sf::FloatRect getGlobalBounds() const override
    {
        return shape.getGlobalBounds();
    }
};

// Prostok¹t (poziom 2)
class RectBlock : public Target
{
private:
    sf::RectangleShape shape;
public:
    RectBlock(sf::Vector2f position)
    {
        shape.setSize(sf::Vector2f(60.f, 30.f));
        shape.setFillColor(sf::Color::Blue);
        shape.setPosition(position);
    }
    void update(float) override {}
    void draw(sf::RenderWindow& window) override
    {
        if (!destroyed) window.draw(shape);
    }
    sf::FloatRect getGlobalBounds() const override
    {
        return shape.getGlobalBounds();
    }
};

// Ko³o (poziom 3)
class CircleTarget : public Target
{
private:
    sf::CircleShape shape;
public:
    CircleTarget(sf::Vector2f position, float radius)
    {
        shape.setRadius(radius);
        shape.setFillColor(sf::Color::Green);
        shape.setOrigin(radius, radius);
        shape.setPosition(position);
    }
    void update(float) override {}
    void draw(sf::RenderWindow& window) override
    {
        if (!destroyed) window.draw(shape);
    }
    sf::FloatRect getGlobalBounds() const override
    {
        return shape.getGlobalBounds();
    }
};

// Prostok¹ty (poziom 4)
class BigRectBlock : public Target
{
private:
    sf::RectangleShape shape;
public:
    BigRectBlock(sf::Vector2f position)
    {
        shape.setSize(sf::Vector2f(80.f, 40.f));
        shape.setFillColor(sf::Color(200, 100, 200));
        shape.setPosition(position);
    }
    void update(float) override {}
    void draw(sf::RenderWindow& window) override
    {
        if (!destroyed) window.draw(shape);
    }
    sf::FloatRect getGlobalBounds() const override
    {
        return shape.getGlobalBounds();
    }
};

// Klasa do rysowania obramowania (dla ni¿szych poziomów)
class StaticBackground
{
private:
    sf::RectangleShape border;
public:
    StaticBackground()
    {
        border.setSize(sf::Vector2f(1024.f, 768.f));
        border.setOutlineColor(sf::Color::White);
        border.setFillColor(sf::Color::Transparent);
        border.setOutlineThickness(2.f);
        border.setPosition(0.f, 0.f);
    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(border);
    }
};

// -----------------------------------------------------------------------------
// 4. KLASA GLOWNA GRY
// -----------------------------------------------------------------------------
class Game
{
private:
    sf::RenderWindow window;
    StaticBackground background;
    Ball ball;
    Paddle paddle;
    std::vector<std::unique_ptr<Target>> blocks;

    // ZAMIANA: zamiast shape'ów, u¿ywamy sprite'ów:
    std::vector<sf::Sprite> backgroundSprites; // <-- wektor sprite'ów t³a

    // Dodatkowo przechowujemy 5 tekstur (tree, cloud, bush, dog, bird)
    // ¿eby móc je przypisywaæ losowo
    std::vector<sf::Texture> backgroundTextures; // 5 za³adowanych tekstur

    // Teksty
    sf::Font font;
    sf::Text textScore;
    sf::Text textLevel;
    sf::Text textTutorial;
    sf::Text textMenu;
    sf::Text textGameOver;
    sf::Text textLevelComplete;
    sf::Text textScoreboard;
    sf::Text textEnterName;

    // Stan wprowadzania nicku
    std::string currentNickname;

    // Stan gry
    GameState currentState;

    // Dane gry
    int score;
    int level;

    // Tablica wyników
    std::vector<PlayerData> players;

    // Zegar
    sf::Clock clock;

public:
    Game()
        : window(sf::VideoMode(1024, 768), "Projekt ARiSS 2024/2025"),
        currentState(GameState::MENU),
        score(0),
        level(1)
    {
        window.setFramerateLimit(60);
        std::srand(static_cast<unsigned>(std::time(nullptr)));

        if (!font.loadFromFile("sans.ttf"))
        {
            std::cerr << "Nie udalo sie zaladowac czcionki!\n";
        }

        // 1) £adujemy wszystkie potrzebne tekstury do backgroundTextures:
        //    W razie niepowodzenia – wypisz b³¹d. 
        //    Uwaga: œcie¿ki musz¹ byæ poprawne w Twoim projekcie (np. "./images/tree.png" itp.)
        sf::Texture t1, t2, t3, t4, t5;
        if (!t1.loadFromFile("tree.png"))
            std::cerr << "Blad wczytywania tree.png\n";
        if (!t2.loadFromFile("cloud.png"))
            std::cerr << "Blad wczytywania cloud.png\n";
        if (!t3.loadFromFile("bush.png"))
            std::cerr << "Blad wczytywania bush.png\n";
        if (!t4.loadFromFile("dog.png"))
            std::cerr << "Blad wczytywania dog.png\n";
        if (!t5.loadFromFile("bird.png"))
            std::cerr << "Blad wczytywania bird.png\n";

        // Wrzucamy za³adowane (lub puste) tekstury do wektora backgroundTextures
        backgroundTextures.push_back(t1);
        backgroundTextures.push_back(t2);
        backgroundTextures.push_back(t3);
        backgroundTextures.push_back(t4);
        backgroundTextures.push_back(t5);

        // Ustawienia tekstów
        textScore.setFont(font);
        textScore.setCharacterSize(24);
        textScore.setFillColor(sf::Color::Green);
        textScore.setPosition(10.f, 10.f);

        textLevel.setFont(font);
        textLevel.setCharacterSize(24);
        textLevel.setFillColor(sf::Color::Cyan);
        textLevel.setPosition(10.f, 40.f);

        textTutorial.setFont(font);
        textTutorial.setCharacterSize(24);
        textTutorial.setFillColor(sf::Color::Yellow);
        textTutorial.setPosition(100.f, 100.f);

        textMenu.setFont(font);
        textMenu.setCharacterSize(32);
        textMenu.setFillColor(sf::Color::Yellow);
        textMenu.setPosition(200.f, 200.f);

        textGameOver.setFont(font);
        textGameOver.setCharacterSize(32);
        textGameOver.setFillColor(sf::Color::Red);
        textGameOver.setPosition(150.f, 250.f);

        textLevelComplete.setFont(font);
        textLevelComplete.setCharacterSize(32);
        textLevelComplete.setFillColor(sf::Color::Green);
        textLevelComplete.setPosition(150.f, 250.f);

        textScoreboard.setFont(font);
        textScoreboard.setCharacterSize(24);
        textScoreboard.setFillColor(sf::Color::White);
        textScoreboard.setPosition(100.f, 100.f);

        textEnterName.setFont(font);
        textEnterName.setCharacterSize(24);
        textEnterName.setFillColor(sf::Color::Yellow);
        textEnterName.setPosition(100.f, 100.f);

        loadScoreboard(players);
    }

    void run()
    {
        while (window.isOpen() && currentState != GameState::EXIT)
        {
            float deltaTime = clock.restart().asSeconds();
            handleEvents();
            update(deltaTime);
            render();
        }
    }

private:
    // Tworzenie sprite'a z losow¹ tekstur¹ z wektora backgroundTextures
    sf::Sprite createRandomSprite()
    {
        // Wybór losowego indeksu [0..4]
        int idx = rand() % backgroundTextures.size();
        sf::Sprite sprite;
        sprite.setTexture(backgroundTextures[idx], true);

        // Mo¿na ewentualnie ustawiæ skalê, jeœli grafika jest za du¿a/ma³a
        // sprite.setScale(0.5f, 0.5f);

        return sprite;
    }

    // Tworzenie klocków w zale¿noœci od poziomu
    void loadLevel(int lvl)
    {
        blocks.clear();
        backgroundSprites.clear(); // Wyczyœæ poprzednie dekoracje t³a
        int count = 0;

        if (lvl == 1)
        {
            count = 10;
            float startX = 100.f, startY = 100.f;
            float offsetX = 80.f, offsetY = 50.f;
            for (int i = 0; i < count; i++)
            {
                int row = i / 5;
                int col = i % 5;
                sf::Vector2f pos(startX + col * offsetX, startY + row * offsetY);
                blocks.push_back(std::make_unique<IrregularBlock>(pos));
            }
        }
        else if (lvl == 2)
        {
            count = 15;
            float startX = 100.f, startY = 80.f;
            float offsetX = 100.f, offsetY = 40.f;
            for (int i = 0; i < count; i++)
            {
                int row = i / 5;
                int col = i % 5;
                sf::Vector2f pos(startX + col * offsetX, startY + row * offsetY);
                blocks.push_back(std::make_unique<RectBlock>(pos));
            }
        }
        else if (lvl == 3)
        {
            count = 10;
            for (int i = 0; i < count; i++)
            {
                float x = 100.f + static_cast<float>(rand() % 800);
                float y = 50.f + static_cast<float>(rand() % 400);
                float r = 15.f + static_cast<float>(rand() % 15);
                blocks.push_back(std::make_unique<CircleTarget>(sf::Vector2f(x, y), r));
            }
        }
        else if (lvl == 4)
        {
            count = 20;
            for (int i = 0; i < count; i++)
            {
                float x = 50.f + static_cast<float>(rand() % 900);
                float y = 50.f + static_cast<float>(rand() % 500);
                blocks.push_back(std::make_unique<BigRectBlock>(sf::Vector2f(x, y)));
            }
        }
        else if (lvl == 5)
        {
            // Klocki (przyk³adowo kó³ka)
            count = 12;
            for (int i = 0; i < count; i++)
            {
                float x = 50.f + static_cast<float>(rand() % 900);
                float y = 80.f + static_cast<float>(rand() % 400);
                float r = 10.f + static_cast<float>(rand() % 20);
                blocks.push_back(std::make_unique<CircleTarget>(sf::Vector2f(x, y), r));
            }
            // Dodajemy 5 sprite'ów w tle
            for (int i = 0; i < 5; i++)
            {
                sf::Sprite spr = createRandomSprite();
                // Pozycja: 
                //  - jak to chmura -> w górnej po³owie (losowo),
                //  - jak to drzewo/zwierzê -> raczej w dolnej, itp.
                // Ale uproœæmy i wrzuæmy je w losowe miejsce:
                float x = static_cast<float>(rand() % 900 + 50);
                float y = static_cast<float>(rand() % 500 + 50);
                spr.setPosition(x, y);
                backgroundSprites.push_back(spr);
            }
        }
        else if (lvl == 6)
        {
            // Klocki
            count = 16;
            for (int i = 0; i < count; i++)
            {
                float x = 50.f + static_cast<float>(rand() % 900);
                float y = 80.f + static_cast<float>(rand() % 400);
                blocks.push_back(std::make_unique<BigRectBlock>(sf::Vector2f(x, y)));
            }
            // Znowu 5 sprite'ów
            for (int i = 0; i < 5; i++)
            {
                sf::Sprite spr = createRandomSprite();
                float x = static_cast<float>(rand() % 900 + 50);
                float y = static_cast<float>(rand() % 500 + 50);
                spr.setPosition(x, y);
                backgroundSprites.push_back(spr);
            }
        }

        // Reset pi³ki i paletki
        ball.setPosition(512.f, 384.f);
        ball.getVelocity() = sf::Vector2f(200.f, 200.f);
        paddle = Paddle();
    }

    // Obs³uga wpisywania nicku
    void handleEnterNameEvent(const sf::Event& event)
    {
        if (event.type == sf::Event::TextEntered)
        {
            if (event.text.unicode >= 32 && event.text.unicode < 128)
            {
                currentNickname += static_cast<char>(event.text.unicode);
            }
        }
        else if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Enter)
            {
                PlayerData pd;
                pd.nick = currentNickname;
                pd.score = 0;
                players.push_back(pd);

                loadLevel(level);
                score = 0;
                currentState = GameState::PLAY;
            }
            else if (event.key.code == sf::Keyboard::BackSpace)
            {
                if (!currentNickname.empty()) currentNickname.pop_back();
            }
            else if (event.key.code == sf::Keyboard::Escape)
            {
                currentState = GameState::MENU;
            }
        }
    }

    void handleEvents()
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                currentState = GameState::EXIT_CONFIRM;
            }
            if (currentState == GameState::ENTER_NAME)
            {
                handleEnterNameEvent(event);
                continue;
            }

            if (event.type == sf::Event::KeyPressed)
            {
                auto code = event.key.code;

                if (code == sf::Keyboard::Escape)
                {
                    currentState = GameState::EXIT_CONFIRM;
                }
                else if (code == sf::Keyboard::F1)
                {
                    if (currentState == GameState::MENU)
                    {
                        currentState = GameState::TUTORIAL;
                    }
                    else if (currentState == GameState::TUTORIAL)
                    {
                        currentState = GameState::MENU;
                    }
                    else if (currentState == GameState::PLAY)
                    {
                        currentState = GameState::TUTORIAL;
                    }
                }
                else if (code == sf::Keyboard::T)
                {
                    // Tablica wyników
                    if (currentState == GameState::MENU)
                    {
                        currentState = GameState::SCOREBOARD;
                    }
                }
                else if (code == sf::Keyboard::Enter)
                {
                    if (currentState == GameState::MENU)
                    {
                        level = 1;
                        currentNickname.clear();
                        currentState = GameState::ENTER_NAME;
                    }
                    else if (currentState == GameState::GAME_OVER)
                    {
                        currentState = GameState::EXIT;
                    }
                    else if (currentState == GameState::LEVEL_COMPLETE)
                    {
                        if (level == 1) { level = 2; }
                        else if (level == 2) { level = 3; }
                        else if (level == 3) { level = 4; }
                        else if (level == 4) { level = 5; }
                        else if (level == 5) { level = 6; }
                        else
                        {
                            // Poziom 6 uznany za ostatni
                            currentState = GameState::EXIT;
                            break;
                        }
                        currentNickname.clear();
                        currentState = GameState::ENTER_NAME;
                    }
                    else if (currentState == GameState::SCOREBOARD)
                    {
                        currentState = GameState::MENU;
                    }
                }
                else if (code == sf::Keyboard::Num1)
                {
                    level = 1;
                    currentNickname.clear();
                    currentState = GameState::ENTER_NAME;
                }
                else if (code == sf::Keyboard::Num2)
                {
                    level = 2;
                    currentNickname.clear();
                    currentState = GameState::ENTER_NAME;
                }
                else if (code == sf::Keyboard::Num3)
                {
                    level = 3;
                    currentNickname.clear();
                    currentState = GameState::ENTER_NAME;
                }
                else if (code == sf::Keyboard::Num4)
                {
                    level = 4;
                    currentNickname.clear();
                    currentState = GameState::ENTER_NAME;
                }
                else if (code == sf::Keyboard::Num5)
                {
                    level = 5;
                    currentNickname.clear();
                    currentState = GameState::ENTER_NAME;
                }
                else if (code == sf::Keyboard::Num6)
                {
                    level = 6;
                    currentNickname.clear();
                    currentState = GameState::ENTER_NAME;
                }
                else if (code == sf::Keyboard::S)
                {
                    saveScoreboard(players);
                }
                else if (code == sf::Keyboard::L)
                {
                    loadScoreboard(players);
                }
            }
        }
    }

    void update(float deltaTime)
    {
        switch (currentState)
        {
        case GameState::MENU:
        {
            textMenu.setString(
                "MENU:\n"
                "[Enter] Start poziom 1\n"
                "[1],[2],[3],[4],[5],[6] - wybierz poziom\n"
                "[T] Tablica wynikow\n"
                "[F1] Tutorial\n"
                "[ESC] Wyjscie"
            );
        }
        break;

        case GameState::ENTER_NAME:
        {
            std::ostringstream ss;
            ss << "Podaj nick (Poziom " << level << "):\n"
                << currentNickname
                << "\n[ENTER] - zatwierdz, [ESC] - Anuluj";
            textEnterName.setString(ss.str());
        }
        break;

        case GameState::PLAY:
        {
            ball.update(deltaTime);
            paddle.update(deltaTime);

            // Dolna krawedz -> przegrana
            if (ball.getPosition().y + ball.getRadius() > 768.f)
            {
                currentState = GameState::GAME_OVER;
                if (!players.empty())
                {
                    players.back().score = score;
                }
                saveScoreboard(players);
                break;
            }

            // Odbicie od paletki
            if (ball.getGlobalBounds().intersects(paddle.getGlobalBounds()))
            {
                sf::Vector2f& vel = ball.getVelocity();
                vel.y = -std::abs(vel.y);
            }

            bool allDestroyed = true;
            for (auto& target : blocks)
            {
                if (!target->isDestroyed())
                {
                    allDestroyed = false;
                    // Kolizja pi³ki z klockiem
                    if (ball.getGlobalBounds().intersects(target->getGlobalBounds()))
                    {
                        target->destroy();
                        score += 50;
                        sf::Vector2f& vel = ball.getVelocity();
                        vel.y = -vel.y;
                        break;
                    }
                }
            }

            if (allDestroyed && !blocks.empty())
            {
                currentState = GameState::LEVEL_COMPLETE;
                if (!players.empty())
                {
                    players.back().score = score;
                }
                saveScoreboard(players);
            }
        }
        break;

        case GameState::TUTORIAL:
            // tutorial -> powrót do menu
            break;

        case GameState::SCOREBOARD:
        {
            std::ostringstream ss;
            ss << "TABLICA WYNIKOW:\n\n";
            for (size_t i = 0; i < players.size(); i++)
            {
                ss << (i + 1) << ". " << players[i].nick
                    << " - " << players[i].score << "\n";
            }
            ss << "\n[ENTER] - powrot do MENU";
            textScoreboard.setString(ss.str());
        }
        break;

        case GameState::EXIT_CONFIRM:
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
            {
                saveScoreboard(players);
                currentState = GameState::EXIT;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::N))
            {
                currentState = GameState::PLAY;
            }
        }
        break;

        case GameState::GAME_OVER:
        {
            std::ostringstream ss;
            ss << "GAME OVER!\nWynik: " << score
                << "\n[ENTER] Wyjdz\n";
            textGameOver.setString(ss.str());
        }
        break;

        case GameState::LEVEL_COMPLETE:
        {
            std::ostringstream ss;
            ss << "LEVEL " << level << " COMPLETE!\n"
                << "Wynik: " << score << "\n"
                << "[ENTER] - kolejny poziom\n";
            textLevelComplete.setString(ss.str());
        }
        break;

        default:
            break;
        }

        // Uaktualnienie Score/Level
        if (currentState == GameState::PLAY
            || currentState == GameState::LEVEL_COMPLETE
            || currentState == GameState::GAME_OVER
            || currentState == GameState::TUTORIAL)
        {
            std::ostringstream s1;
            s1 << "Score: " << score;
            textScore.setString(s1.str());

            std::ostringstream s2;
            s2 << "Level: " << level;
            textLevel.setString(s2.str());
        }
    }

    void render()
    {
        // Dla poziomów 5 i 6 rysujemy "niebo" + "trawê", a nastêpnie sprite'y w tle
        bool isAdvancedLevel = (level == 5 || level == 6);
        if ((currentState == GameState::PLAY
            || currentState == GameState::LEVEL_COMPLETE
            || currentState == GameState::GAME_OVER)
            && isAdvancedLevel)
        {
            window.clear(sf::Color::Black);

            // "niebo" (górna po³owa ekranu)
            sf::RectangleShape sky(sf::Vector2f(1024.f, 384.f));
            sky.setPosition(0.f, 0.f);
            sky.setFillColor(sf::Color(100, 149, 237)); // cornflowerblue
            window.draw(sky);

            // "trawa" (dolna po³owa)
            sf::RectangleShape grass(sf::Vector2f(1024.f, 384.f));
            grass.setPosition(0.f, 384.f);
            grass.setFillColor(sf::Color(50, 205, 50)); // limegreen
            window.draw(grass);

            // Rysujemy sprite'y (losowe modele: tree, cloud, bush, dog, bird)
            for (auto& spr : backgroundSprites)
            {
                window.draw(spr);
            }
        }
        else
        {
            // Dla pozosta³ych poziomów/stanu MENU/TUTORIAL itp. – czarne t³o + obramowanie
            window.clear(sf::Color::Black);
            background.draw(window);
        }

        switch (currentState)
        {
        case GameState::MENU:
            window.draw(textMenu);
            break;

        case GameState::ENTER_NAME:
            window.draw(textEnterName);
            break;

        case GameState::PLAY:
        {
            window.draw(textScore);
            window.draw(textLevel);

            // Rysujemy pi³kê, paletkê, klocki
            ball.draw(window);
            paddle.draw(window);
            for (auto& target : blocks)
                target->draw(window);
        }
        break;

        case GameState::TUTORIAL:
        {
            textTutorial.setString(
                "TUTORIAL:\n"
                "Sterowanie paletka: strzalki lewo/prawo\n"
                "Zniszcz wszystkie cele, aby ukonczyc poziom.\n"
                "Dolna krawedz -> przegrana\n"
                "F1 - powrot do MENU"
            );
            window.draw(textTutorial);
        }
        break;

        case GameState::SCOREBOARD:
            window.draw(textScoreboard);
            break;

        case GameState::EXIT_CONFIRM:
        {
            sf::Text confirmText("Czy na pewno chcesz wyjsc?\n[T] - Tak, [N] - Nie",
                font, 24);
            confirmText.setFillColor(sf::Color::Red);
            confirmText.setPosition(200.f, 200.f);
            window.draw(confirmText);
        }
        break;

        case GameState::GAME_OVER:
            window.draw(textGameOver);
            break;

        case GameState::LEVEL_COMPLETE:
            window.draw(textLevelComplete);
            break;

        default:
            break;
        }

        window.display();
    }
};

// -----------------------------------------------------------------------------
// 5. FUNKCJA GLOWNA
// -----------------------------------------------------------------------------
int main()
{
    Game gra;
    gra.run();
    return 0;
}
