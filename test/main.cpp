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
#include <algorithm>

// -----------------------------------------------------------------------------
// 1. STRUKTURY I DEFINICJE
// -----------------------------------------------------------------------------

static const unsigned WINDOW_WIDTH = 1920;
static const unsigned WINDOW_HEIGHT = 1080;

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
    TUTORIAL,
    SCOREBOARD,
    EXIT_CONFIRM,
    GAME_OVER,
    LEVEL_COMPLETE,
    EXIT
};

// Prosta struktura do „ruchomych” dekoracji t³a (psy, chmury itp.)
struct MovingSprite
{
    sf::Sprite sprite;
    sf::Vector2f velocity;
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

    // Sort – od najwy¿szego wyniku do najni¿szego
    std::sort(players.begin(), players.end(),
        [](const PlayerData& a, const PlayerData& b) {
            return a.score > b.score;
        }
    );

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
    Ball(float radius = 12.f)
    {
        shape.setRadius(radius);
        shape.setFillColor(sf::Color::Yellow);
        shape.setOrigin(radius, radius);
        shape.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);
        velocity = sf::Vector2f(300.f, 300.f);
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
        else if (pos.x + shape.getRadius() > WINDOW_WIDTH)
        {
            shape.setPosition(WINDOW_WIDTH - shape.getRadius(), pos.y);
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
    Paddle(sf::Vector2f size = sf::Vector2f(120.f, 30.f))
    {
        shape.setSize(size);
        shape.setFillColor(sf::Color::Cyan);
        shape.setOrigin(size.x / 2.f, size.y / 2.f);
        shape.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT - 80.f);
        speed = 600.f;
    }

    void update(float deltaTime) override
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            shape.move(-speed * deltaTime, 0.f);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            shape.move(speed * deltaTime, 0.f);
        }

        // Ograniczenie ruchu (lewa/prawa strona)
        if (shape.getPosition().x - shape.getSize().x / 2.f < 0.f)
        {
            shape.setPosition(shape.getSize().x / 2.f, shape.getPosition().y);
        }
        else if (shape.getPosition().x + shape.getSize().x / 2.f > WINDOW_WIDTH)
        {
            shape.setPosition(WINDOW_WIDTH - shape.getSize().x / 2.f, shape.getPosition().y);
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
        shape.setPoint(1, sf::Vector2f(20.f, 0.f));
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
        shape.setFillColor(sf::Color::White);
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
        shape.setFillColor(sf::Color::Red);
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

// Wiêkszy prostok¹t
class BigRectBlock : public Target
{
private:
    sf::RectangleShape shape;
public:
    BigRectBlock(sf::Vector2f position)
    {
        shape.setSize(sf::Vector2f(80.f, 40.f));
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

// Klasa do rysowania prostego obramowania (dla ni¿szych poziomów)
class StaticBackground
{
private:
    sf::RectangleShape border;
public:
    StaticBackground()
    {
        border.setSize(sf::Vector2f((float)WINDOW_WIDTH, (float)WINDOW_HEIGHT));
        border.setOutlineColor(sf::Color::White);
        border.setFillColor(sf::Color::Transparent);
        border.setOutlineThickness(3.f);
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

    // Dekoracje nieruchome (np. drzewo, ptak) + wektor z "ruchomymi" obiektami
    std::vector<sf::Sprite> staticSprites;
    std::vector<MovingSprite> movingSprites;

    // Tekstury do t³a menu i do œwiata
    sf::Texture menuBackgroundTexture;
    sf::Sprite  menuBackgroundSprite;

    sf::Texture dogTexture, treeTexture;
    sf::Texture cloudTexture, birdTexture;

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
        : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Projekt ARiSS 2024/2025"),
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

        // Wczytanie t³a menu
        if (!menuBackgroundTexture.loadFromFile("menu_bg.png"))
        {
            std::cerr << "Blad wczytywania menu_bg.png\n";
        }
        // Skalowanie, aby wype³niæ ca³y ekran
        menuBackgroundSprite.setTexture(menuBackgroundTexture);
        float scaleX = (float)WINDOW_WIDTH / menuBackgroundTexture.getSize().x;
        float scaleY = (float)WINDOW_HEIGHT / menuBackgroundTexture.getSize().y;
        menuBackgroundSprite.setScale(scaleX, scaleY);

        // Wczytanie innych tekstur
        if (!dogTexture.loadFromFile("dog.png"))
            std::cerr << "Blad wczytywania dog.png\n";
        if (!treeTexture.loadFromFile("tree.png"))
            std::cerr << "Blad wczytywania tree.png\n";
        if (!cloudTexture.loadFromFile("cloud.png"))
            std::cerr << "Blad wczytywania cloud.png\n";
        if (!birdTexture.loadFromFile("bird.png"))
            std::cerr << "Blad wczytywania bird.png\n";

        // Ustawienia tekstów
        textScore.setFont(font);
        textScore.setCharacterSize(40);
        textScore.setFillColor(sf::Color::Green);
        textScore.setPosition(10.f, 10.f);

        textLevel.setFont(font);
        textLevel.setCharacterSize(40);
        textLevel.setFillColor(sf::Color::Cyan);
        textLevel.setPosition(10.f, 60.f);

        textTutorial.setFont(font);
        textTutorial.setCharacterSize(50);
        textTutorial.setFillColor(sf::Color::Yellow);
        textTutorial.setPosition(150.f, 150.f);

        textMenu.setFont(font);
        textMenu.setCharacterSize(60);
        textMenu.setFillColor(sf::Color::Yellow);
        textMenu.setPosition(200.f, 200.f);

        textGameOver.setFont(font);
        textGameOver.setCharacterSize(60);
        textGameOver.setFillColor(sf::Color::Red);
        textGameOver.setPosition(150.f, 250.f);

        textLevelComplete.setFont(font);
        textLevelComplete.setCharacterSize(60);
        textLevelComplete.setFillColor(sf::Color::Green);
        textLevelComplete.setPosition(150.f, 250.f);

        textScoreboard.setFont(font);
        textScoreboard.setCharacterSize(36);
        textScoreboard.setFillColor(sf::Color::White);
        textScoreboard.setPosition(100.f, 100.f);

        textEnterName.setFont(font);
        textEnterName.setCharacterSize(50);
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
    // Pomocnicza metoda: sprawdza czy œwiat jest typu 1 (zielony) czy typu 2 (niebieski)
    // Dla uproszczenia: poziomy nieparzyste >= 3 -> typ 1, parzyste >= 4 -> typ 2.
    bool isTypeOneWorld(int lvl) const
    {
        // Dla lvl < 3 - w ogóle inny styl
        if (lvl < 3) return false;
        // Nieparzyste -> typ 1
        return (lvl % 2 != 0);
    }

    // Tworzenie klocków w zale¿noœci od poziomu – w górnej po³owie ekranu
    void loadLevel(int lvl)
    {
        blocks.clear();
        staticSprites.clear();
        movingSprites.clear();

        int count = 0;
        // Maksymalny Y = po³owa ekranu
        float maxY = WINDOW_HEIGHT * 0.5f - 100.f;

        // W zale¿noœci od poziomu -> count i rodzaj klocków:
        switch (lvl)
        {
        case 1:
        {
            count = 20; 
            for (int i = 0; i < count; i++)
            {
                float x = 100.f + static_cast<float>(rand() % (WINDOW_WIDTH - 200));
                float y = 50.f + static_cast<float>(rand() % (int)maxY);
                blocks.push_back(std::make_unique<IrregularBlock>(sf::Vector2f(x, y)));
            }
        }
        break;

        case 2:
        {
            count = 25;
            for (int i = 0; i < count; i++)
            {
                float x = 50.f + static_cast<float>(rand() % (WINDOW_WIDTH - 100));
                float y = 50.f + static_cast<float>(rand() % (int)maxY);
                blocks.push_back(std::make_unique<RectBlock>(sf::Vector2f(x, y)));
            }
        }
        break;

        default:
            
        {
            count = 30 + (lvl * 5); 
            for (int i = 0; i < count; i++)
            {
                float x = 50.f + static_cast<float>(rand() % (WINDOW_WIDTH - 100));
                float y = 50.f + static_cast<float>(rand() % (int)maxY);

        
                if (i % 3 == 0)
                    blocks.push_back(std::make_unique<CircleTarget>(sf::Vector2f(x, y), 20.f));
                else if (i % 3 == 1)
                    blocks.push_back(std::make_unique<BigRectBlock>(sf::Vector2f(x, y)));
                else
                    blocks.push_back(std::make_unique<RectBlock>(sf::Vector2f(x, y)));
            }
        }
        break;
        }

        // Dekoracje œwiata:
        if (lvl >= 3)
        {
            if (isTypeOneWorld(lvl))
            {
                // Typ 1: zielone t³o, psy i drzewa
                // Dodajmy np. 5 drzew w losowych pozycjach (static)
                for (int i = 0; i < 5; i++)
                {
                    sf::Sprite s(treeTexture);
                    s.setScale(200.f / treeTexture.getSize().x, 200.f / treeTexture.getSize().y);
                    s.setPosition(
                        (float)(rand() % (WINDOW_WIDTH - 200)),
                        (float)(rand() % (WINDOW_HEIGHT / 2)) + WINDOW_HEIGHT / 2.f
                    );
                    staticSprites.push_back(s);
                }
                // Dodajmy 3 psy, które siê powoli poruszaj¹
                for (int i = 0; i < 3; i++)
                {
                    MovingSprite ms;
                    ms.sprite.setTexture(dogTexture);
                    ms.sprite.setScale(100.f / dogTexture.getSize().x, 100.f / dogTexture.getSize().y);
                    ms.sprite.setPosition(
                        (float)(rand() % (WINDOW_WIDTH - 200)),
                        (float)(rand() % (WINDOW_HEIGHT / 2)) + WINDOW_HEIGHT / 2.f
                    );
                    // Powolny ruch w prawo (mo¿na te¿ nadaæ ró¿ne kierunki/ prêdkoœci)
                    ms.velocity = sf::Vector2f(30.f + rand() % 20, 0.f);
                    movingSprites.push_back(ms);
                }
            }
            else
            {
                // Typ 2: niebieskie t³o, ptaki i chmury
                // Dodajmy 4 ptaki statycznie
                for (int i = 0; i < 4; i++)
                {
                    sf::Sprite s(birdTexture);
                    s.setScale(100.f / birdTexture.getSize().x, 100.f / birdTexture.getSize().y);
                    s.setPosition(
                        (float)(rand() % (WINDOW_WIDTH - 200)),
                        (float)(rand() % (WINDOW_HEIGHT / 2)) + WINDOW_HEIGHT / 2.f
                    );
                    staticSprites.push_back(s);
                }
                // Chmury – niech „przelatuj¹” przez ekran
                for (int i = 0; i < 4; i++)
                {
                    MovingSprite ms;
                    ms.sprite.setTexture(cloudTexture);
                    ms.sprite.setScale(500.f / dogTexture.getSize().x, 150.f / dogTexture.getSize().y);
                    ms.sprite.setPosition(
                        (float)(rand() % (WINDOW_WIDTH - 200)),
                        (float)(rand() % (int)(WINDOW_HEIGHT * 0.3f)) // raczej wysoko
                    );
                    // Ruch w prawo
                    ms.velocity = sf::Vector2f(60.f + rand() % 40, 0.f);
                    movingSprites.push_back(ms);
                }
            }
        }

        // Reset pi³ki i paletki
        ball.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);
        ball.getVelocity() = sf::Vector2f(300.f, 300.f);
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

                // Sort i start
                std::sort(players.begin(), players.end(),
                    [](const PlayerData& a, const PlayerData& b) {
                        return a.score > b.score;
                    }
                );
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
                        // Naprawa: wracamy do MENU, zamiast wyjœcia
                        currentState = GameState::MENU;
                    }
                    else if (currentState == GameState::LEVEL_COMPLETE)
                    {
                        if (level < 10)
                        {
                            ++level;
                            currentNickname.clear();
                            currentState = GameState::ENTER_NAME;
                        }
                        else
                        {
                            // Poziom 10 -> wracamy do MENU
                            currentState = GameState::MENU;
                        }
                    }
                    else if (currentState == GameState::SCOREBOARD)
                    {
                        currentState = GameState::MENU;
                    }
                }
                // szybki wybór poziomu
                else if (code == sf::Keyboard::Num1) { level = 1;  currentNickname.clear(); currentState = GameState::ENTER_NAME; }
                else if (code == sf::Keyboard::Num2) { level = 2;  currentNickname.clear(); currentState = GameState::ENTER_NAME; }
                else if (code == sf::Keyboard::Num3) { level = 3;  currentNickname.clear(); currentState = GameState::ENTER_NAME; }
                else if (code == sf::Keyboard::Num4) { level = 4;  currentNickname.clear(); currentState = GameState::ENTER_NAME; }
                else if (code == sf::Keyboard::Num5) { level = 5;  currentNickname.clear(); currentState = GameState::ENTER_NAME; }
                else if (code == sf::Keyboard::Num6) { level = 6;  currentNickname.clear(); currentState = GameState::ENTER_NAME; }
                else if (code == sf::Keyboard::Num7) { level = 7;  currentNickname.clear(); currentState = GameState::ENTER_NAME; }
                else if (code == sf::Keyboard::Num8) { level = 8;  currentNickname.clear(); currentState = GameState::ENTER_NAME; }
                else if (code == sf::Keyboard::Num9) { level = 9;  currentNickname.clear(); currentState = GameState::ENTER_NAME; }
                else if (code == sf::Keyboard::Num0) { level = 10; currentNickname.clear(); currentState = GameState::ENTER_NAME; }

                // S - zapis, L - wczytanie
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
                "[1..0] - wybierz poziom (0=10)\n"
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

            // Aktualizacja "ruchomych" dekoracji (psy, chmury)
            for (auto& ms : movingSprites)
            {
                sf::Vector2f pos = ms.sprite.getPosition();
                pos += ms.velocity * deltaTime;
                // Jeœli wyjdzie poza ekran w prawo, przesuwamy w lewo
                if (pos.x > WINDOW_WIDTH + 100.f)
                {
                    pos.x = -200.f;
                }
                // Ewentualnie, jeœli w lewo, to w prawo:
                // if (pos.x < -300.f) { pos.x = (float)WINDOW_WIDTH + 50.f; }
                ms.sprite.setPosition(pos);
            }

            // Dolna krawedz -> przegrana
            if (ball.getPosition().y + ball.getRadius() > (float)WINDOW_HEIGHT)
            {
                currentState = GameState::GAME_OVER;
                if (!players.empty())
                {
                    players.back().score = score;
                }
                std::sort(players.begin(), players.end(),
                    [](const PlayerData& a, const PlayerData& b) {
                        return a.score > b.score;
                    }
                );
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
                std::sort(players.begin(), players.end(),
                    [](const PlayerData& a, const PlayerData& b) {
                        return a.score > b.score;
                    }
                );
                saveScoreboard(players);
            }
        }
        break;

        case GameState::TUTORIAL:
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
    
            break;

        case GameState::GAME_OVER:
        {
            std::ostringstream ss;
            ss << "GAME OVER!\nWynik: " << score
                << "\n[ENTER] Menu\n";
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
        window.clear(sf::Color::Black);

        // Rysujemy odpowiednie t³o w zale¿noœci od stanu
        if (currentState == GameState::PLAY
            || currentState == GameState::LEVEL_COMPLETE
            || currentState == GameState::GAME_OVER)
        {
            // Jeœli lvl >=3, to sprawdzamy, czy typ 1 czy 2
            if (level >= 3)
            {
                if (isTypeOneWorld(level))
                {
                    // Zielone t³o
                    sf::RectangleShape bg(sf::Vector2f((float)WINDOW_WIDTH, (float)WINDOW_HEIGHT));
                    bg.setFillColor(sf::Color(34, 139, 34)); // ForestGreen
                    window.draw(bg);
                }
                else
                {
                    // Niebieskie t³o
                    sf::RectangleShape bg(sf::Vector2f((float)WINDOW_WIDTH, (float)WINDOW_HEIGHT));
                    bg.setFillColor(sf::Color(70, 130, 180)); // SteelBlue
                    window.draw(bg);
                }
            }
            else
            {
                // Ni¿sze poziomy – zwyk³e czarne + obramowanie
                background.draw(window);
            }

            // Rysujemy obiekty t³a (static + moving)
            for (auto& s : staticSprites)
            {
                window.draw(s);
            }
            for (auto& ms : movingSprites)
            {
                window.draw(ms.sprite);
            }
        }
        else
        {
            // Stany menu/TUTORIAL/SCOREBOARD/ENTER_NAME/EXIT_CONFIRM
            // -> rysujemy t³o menu
            window.draw(menuBackgroundSprite);
        }

        // Rysujemy resztê (tekst, obiekty gry itd.)
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
            window.draw(textTutorial);
            break;

        case GameState::SCOREBOARD:
            window.draw(textScoreboard);
            break;

        case GameState::EXIT_CONFIRM:
        {
            sf::Text confirmText("Czy na pewno chcesz wyjsc?\n[T] - Tak, [N] - Nie",
                font, 50);
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
