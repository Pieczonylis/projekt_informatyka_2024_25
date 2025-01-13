#include "Game.h"
#include <iostream>
#include <sstream>
#include <algorithm> 
#include <cstdlib>  

Game::Game()
    : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Projekt ARiSS 2024/2025"),
    currentState(GameState::MENU),
    score(0),
    level(1),
    menuAnimVelocity(80.f, 0.f)
{
    window.setFramerateLimit(60);
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Font
    if (!font.loadFromFile("sans.ttf"))
    {
        std::cerr << "Nie udalo sie zaladowac czcionki!\n";
    }

    // T�o menu
    if (!menuBackgroundTexture.loadFromFile("Resources/menu_bg.png"))
    {
        std::cerr << "Blad wczytywania menu_bg.png\n";
    }
    menuBackgroundSprite.setTexture(menuBackgroundTexture);
    float scaleX = (float)WINDOW_WIDTH / menuBackgroundTexture.getSize().x;
    float scaleY = (float)WINDOW_HEIGHT / menuBackgroundTexture.getSize().y;
    menuBackgroundSprite.setScale(scaleX, scaleY);

    // T�o scoreboardu
    if (!scoreBackgroundTexture.loadFromFile("Resources/bgScore.png"))
    {
        std::cerr << "Blad wczytywania bgScore.png\n";
    }
    scoreBackgroundSprite.setTexture(scoreBackgroundTexture);
    float sX2 = (float)WINDOW_WIDTH / scoreBackgroundTexture.getSize().x;
    float sY2 = (float)WINDOW_HEIGHT / scoreBackgroundTexture.getSize().y;
    scoreBackgroundSprite.setScale(sX2, sY2);

    // Animowany sprite w menu
    if (!menuAnimTexture.loadFromFile("Resources/menu_anim.png"))
    {
        std::cerr << "Blad wczytywania menu_anim.png\n";
    }
    menuAnimSprite.setTexture(menuAnimTexture);
    menuAnimSprite.setPosition(100.f, 500.f);
    menuAnimSprite.setScale(0.5f, 0.5f); // ewentualne skalowanie

    // Tekstury do chmury i drzewa
    if (!cloudTexture.loadFromFile("Resources/cloud.png"))
        std::cerr << "Blad wczytywania cloud.png\n";
    if (!treeTexture.loadFromFile("Resources/tree.png"))
        std::cerr << "Blad wczytywania tree.png\n";

    // Inicjalizacja tekst�w z obrysem
    auto setupText = [&](sf::Text& txt, unsigned size, sf::Color fill, float x, float y)
        {
            txt.setFont(font);
            txt.setCharacterSize(size);
            txt.setFillColor(fill);
            txt.setPosition(x, y);
            // Outline:
            txt.setOutlineColor(sf::Color::Black);
            txt.setOutlineThickness(2.f);
        };

    setupText(textScore, 36, sf::Color::Green, 10.f, 10.f);
    setupText(textLevel, 36, sf::Color::Cyan, 10.f, 50.f);

    // Tutorial: w tym stanie poka�emy opis klawiszy
    setupText(textTutorial, 40, sf::Color::Yellow, 100.f, 100.f);
    textTutorial.setString(
        "TUTORIAL:\n"
        "Sterowanie:\n"
        " - Paletka: Strzalki lewo/prawo (lub A/D)\n"
        " - ESC: Zapytanie o wyjscie\n"
        " - S/L: Zapis/Wczytanie stanu tablicy wynikow\n"
        " - F1: Powrot do gry/menu\n"
        "Cel gry:\n"
        " - Odbij pilke paletka\n"
        " - Zniszcz wszystkie klocki\n"
        " - Unikaj spadniecia pilki\n"
        "\nNacisnij F1, by wrocic."
    );

    setupText(textMenu, 60, sf::Color::Yellow, 200.f, 200.f);
    setupText(textGameOver, 60, sf::Color::Red, 150.f, 250.f);
    setupText(textLevelComplete, 60, sf::Color::Green, 150.f, 250.f);
    setupText(textScoreboard, 32, sf::Color::White, 100.f, 100.f);
    setupText(textEnterName, 50, sf::Color::Yellow, 100.f, 100.f);

    // Wczytujemy scoreboard
    loadScoreboard(players);
}

void Game::run()
{
    while (window.isOpen() && currentState != GameState::EXIT)
    {
        float deltaTime = clock.restart().asSeconds();
        handleEvents();
        update(deltaTime);
        render();
    }
}

bool Game::isTypeOneWorld(int lvl) const
{
    return (lvl >= 3 && lvl % 2 != 0);
}

void Game::handleEnterNameEvent(const sf::Event& event)
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
        switch (event.key.code)
        {
        case sf::Keyboard::Enter:
        {
            PlayerData pd;
            pd.nick = currentNickname;
            pd.score = 0;
            players.push_back(pd);

            // Sort i start
            std::sort(players.begin(), players.end(),
                [](const PlayerData& a, const PlayerData& b) { return a.score > b.score; }
            );
            loadLevel(level);
            score = 0;
            currentState = GameState::PLAY;
        }
        break;
        case sf::Keyboard::BackSpace:
        {
            if (!currentNickname.empty()) currentNickname.pop_back();
        }
        break;
        case sf::Keyboard::Escape:
        {
            currentState = GameState::MENU;
        }
        break;
        default:
            break;
        }
    }
}

void Game::handleEvents()
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
            switch (event.key.code)
            {
            case sf::Keyboard::Escape:
                currentState = GameState::EXIT_CONFIRM;
                break;

            case sf::Keyboard::F1:
                // Prze��czenie do TUTORIAL lub powr�t z TUTORIAL
                if (currentState == GameState::MENU)
                    currentState = GameState::TUTORIAL;
                else if (currentState == GameState::TUTORIAL)
                    currentState = GameState::MENU;
                else if (currentState == GameState::PLAY)
                    currentState = GameState::TUTORIAL;
                break;

            case sf::Keyboard::T:
                if (currentState == GameState::MENU)
                    currentState = GameState::SCOREBOARD;
                break;

            case sf::Keyboard::Enter:
                switch (currentState)
                {
                case GameState::MENU:
                    level = 1;
                    currentNickname.clear();
                    currentState = GameState::ENTER_NAME;
                    break;
                case GameState::GAME_OVER:
                    currentState = GameState::MENU;
                    break;
                case GameState::LEVEL_COMPLETE:
                    if (level < 10)
                    {
                        ++level;
                        currentNickname.clear();
                        currentState = GameState::ENTER_NAME;
                    }
                    else
                    {
                        currentState = GameState::MENU;
                    }
                    break;
                case GameState::SCOREBOARD:
                    currentState = GameState::MENU;
                    break;
                default:
                    break;
                }
                break;

                // szybki wyb�r poziomu
            case sf::Keyboard::Num1: { level = 1;  currentNickname.clear(); currentState = GameState::ENTER_NAME; } break;
            case sf::Keyboard::Num2: { level = 2;  currentNickname.clear(); currentState = GameState::ENTER_NAME; } break;
            case sf::Keyboard::Num3: { level = 3;  currentNickname.clear(); currentState = GameState::ENTER_NAME; } break;
            case sf::Keyboard::Num4: { level = 4;  currentNickname.clear(); currentState = GameState::ENTER_NAME; } break;
            case sf::Keyboard::Num5: { level = 5;  currentNickname.clear(); currentState = GameState::ENTER_NAME; } break;
            case sf::Keyboard::Num6: { level = 6;  currentNickname.clear(); currentState = GameState::ENTER_NAME; } break;
            case sf::Keyboard::Num7: { level = 7;  currentNickname.clear(); currentState = GameState::ENTER_NAME; } break;
            case sf::Keyboard::Num8: { level = 8;  currentNickname.clear(); currentState = GameState::ENTER_NAME; } break;
            case sf::Keyboard::Num9: { level = 9;  currentNickname.clear(); currentState = GameState::ENTER_NAME; } break;
            case sf::Keyboard::Num0: { level = 10; currentNickname.clear(); currentState = GameState::ENTER_NAME; } break;

                // S/L -> zapis / wczytanie scoreboard
            case sf::Keyboard::S:
                saveScoreboard(players);
                break;
            case sf::Keyboard::L:
                loadScoreboard(players);
                break;

            default:
                break;
            }
        }

        if (currentState == GameState::EXIT_CONFIRM)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
            {
                currentState = GameState::EXIT;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::N))
            {
                currentState = GameState::MENU;
            }
        }
    }
}

void Game::update(float deltaTime)
{
    switch (currentState)
    {
    case GameState::MENU:
    {
        // Animowany sprite w menu -> porusza si� lewo/prawo
        sf::Vector2f pos = menuAnimSprite.getPosition();
        pos += menuAnimVelocity * deltaTime;

        // Odbijamy si� od kraw�dzi
        if (pos.x < 0.f)
        {
            pos.x = 0.f;
            menuAnimVelocity.x = -menuAnimVelocity.x;
        }
        else if (pos.x + menuAnimSprite.getGlobalBounds().width > WINDOW_WIDTH)
        {
            pos.x = WINDOW_WIDTH - menuAnimSprite.getGlobalBounds().width;
            menuAnimVelocity.x = -menuAnimVelocity.x;
        }
        menuAnimSprite.setPosition(pos);

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

        // Ruchome dekoracje (np. chmura)
        for (auto& ms : movingSprites)
        {
            sf::Vector2f pos = ms.sprite.getPosition();
            pos += ms.velocity * deltaTime;
            if (pos.x > WINDOW_WIDTH + 200.f)
            {
                pos.x = -300.f;
            }
            ms.sprite.setPosition(pos);
        }

        // Sprawd�, czy pi�ka spad�a
        if (ball.getPosition().y + ball.getRadius() > (float)WINDOW_HEIGHT)
        {
            currentState = GameState::GAME_OVER;
            if (!players.empty())
            {
                players.back().score = score;
            }
            std::sort(players.begin(), players.end(),
                [](const PlayerData& a, const PlayerData& b) {return a.score > b.score;}
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

        // Kolizja z klockami
        bool allDestroyed = true;
        for (auto& target : blocks)
        {
            if (!target->isDestroyed())
            {
                allDestroyed = false;
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
                [](const PlayerData& a, const PlayerData& b) {return a.score > b.score;}
            );
            saveScoreboard(players);
        }
    }
    break;

    case GameState::TUTORIAL:
        // nic, tekst tutoriala jest w textTutorial
        break;

    case GameState::SCOREBOARD:
    {
        // Tworzymy napis z list� wynik�w
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
        // obs�uga w handleEvents
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

    // Aktualizacja Score/Level
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

void Game::loadLevel(int lvl)
{
    blocks.clear();
    staticSprites.clear();
    movingSprites.clear();

    // Ustawiamy backgroundSprite
    sf::Texture& currentBg = bgManager.getBackgroundTexture(lvl);
    backgroundSprite.setTexture(currentBg);
    float sX = (float)WINDOW_WIDTH / currentBg.getSize().x;
    float sY = (float)WINDOW_HEIGHT / currentBg.getSize().y;
    backgroundSprite.setScale(sX, sY);

    // Generujemy klocki
    int count = 10 + (lvl * 3);
    float maxY = WINDOW_HEIGHT * 0.5f - 50.f;
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

    // Dodaj 1 sprite ruchomy (chmura) w g�rnej cz�ci
    {
        MovingSprite ms;
        ms.sprite.setTexture(cloudTexture);
        ms.sprite.setScale(0.4f, 0.4f);
        float px = static_cast<float>(rand() % (WINDOW_WIDTH - 200));
        float py = static_cast<float>(rand() % 100);
        ms.sprite.setPosition(px, py);
        ms.velocity = sf::Vector2f(60.f, 0.f);
        movingSprites.push_back(ms);
    }

    // Dodaj 1 sprite nieruchomy (drzewo) w dolnej cz�ci
    {
        sf::Sprite tree(treeTexture);
        tree.setScale(0.4f, 0.4f);
        float px = static_cast<float>(rand() % (WINDOW_WIDTH - 200));
        float py = WINDOW_HEIGHT / 2.f + static_cast<float>(rand() % 200);
        tree.setPosition(px, py);
        staticSprites.push_back(tree);
    }

    // Reset pi�ki i paletki
    ball.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);
    ball.getVelocity() = sf::Vector2f(300.f, 300.f);
    paddle = Paddle();
}

void Game::render()
{
    window.clear(sf::Color::Black);

    switch (currentState)
    {
    case GameState::PLAY:
    case GameState::LEVEL_COMPLETE:
    case GameState::GAME_OVER:
        window.draw(backgroundSprite);
        for (auto& s : staticSprites)
            window.draw(s);
        for (auto& ms : movingSprites)
            window.draw(ms.sprite);
        break;

    case GameState::SCOREBOARD:
        window.draw(scoreBackgroundSprite);
        break;

    default:
        window.draw(menuBackgroundSprite);
        // animowany sprite w menu
        if (currentState == GameState::MENU)
        {
            window.draw(menuAnimSprite);
        }
        break;
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
        // Outline
        confirmText.setOutlineColor(sf::Color::Black);
        confirmText.setOutlineThickness(2.f);

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
