#include "Game.h"
#include <iostream>
#include <sstream>
#include <cstdlib> // rand
#include <algorithm> // sort

Game::Game()
    : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Projekt ARiSS 2024/2025"),
    currentState(GameState::MENU),
    score(0),
    level(1)
{
    window.setFramerateLimit(60);
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Wczytanie fontu
    if (!font.loadFromFile("sans.ttf"))
    {
        std::cerr << "Nie udalo sie zaladowac czcionki!\n";
    }

    // T³o menu (np. "Resources/menu_bg.png")
    if (!menuBackgroundTexture.loadFromFile("Resources/menu_bg.png"))
    {
        std::cerr << "Blad wczytywania Resources/menu_bg.png\n";
    }
    menuBackgroundSprite.setTexture(menuBackgroundTexture);
    float scaleX = (float)WINDOW_WIDTH / menuBackgroundTexture.getSize().x;
    float scaleY = (float)WINDOW_HEIGHT / menuBackgroundTexture.getSize().y;
    menuBackgroundSprite.setScale(scaleX, scaleY);

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

    // Wczytujemy tabelê wyników
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
    // Przyk³ad logiczny, jeœli potrzebny
    if (lvl < 3) return false;
    return (lvl % 2 != 0);
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
            // Dodaj gracza
            PlayerData pd;
            pd.nick = currentNickname;
            pd.score = 0;
            players.push_back(pd);

            // Sort
            std::sort(players.begin(), players.end(),
                [](const PlayerData& a, const PlayerData& b) {
                    return a.score > b.score;
                }
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

                // szybki wybór poziomu
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

                // zapisywanie / wczytanie scoreboard
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

        // obs³uga stanu EXIT_CONFIRM
        if (currentState == GameState::EXIT_CONFIRM)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
            {
                currentState = GameState::EXIT;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::N))
            {
                // wracamy do MENU (albo do PLAY – mo¿na wybraæ)
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

        // Ruchome dekoracje (psy, chmury itd.)
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

        // SprawdŸ, czy pi³ka wpad³a na dó³
        if (ball.getPosition().y + ball.getRadius() > (float)WINDOW_HEIGHT)
        {
            currentState = GameState::GAME_OVER;
            if (!players.empty())
            {
                players.back().score = score;
            }
            std::sort(players.begin(), players.end(),
                [](const PlayerData& a, const PlayerData& b)
                {
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

        // SprawdŸ kolizje z klockami
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
        // Jeœli wszystkie klocki zniszczone:
        if (allDestroyed && !blocks.empty())
        {
            currentState = GameState::LEVEL_COMPLETE;
            if (!players.empty())
            {
                players.back().score = score;
            }
            std::sort(players.begin(), players.end(),
                [](const PlayerData& a, const PlayerData& b)
                {
                    return a.score > b.score;
                }
            );
            saveScoreboard(players);
        }
    }
    break;

    case GameState::TUTORIAL:
        // czekamy na F1
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
        // obs³ugiwane w handleEvents
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

    // Uaktualnienie Score/Level w niektórych stanach
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

    // Ustawiamy backgroundSprite (obrazek z managera).
    sf::Texture& currentBg = bgManager.getBackgroundTexture(lvl);
    backgroundSprite.setTexture(currentBg);
    float sX = (float)WINDOW_WIDTH / currentBg.getSize().x;
    float sY = (float)WINDOW_HEIGHT / currentBg.getSize().y;
    backgroundSprite.setScale(sX, sY);

    // Przyk³adowy generator klocków
    int count = 0;
    float maxY = WINDOW_HEIGHT * 0.5f - 100.f;
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

    // (Mo¿esz dodaæ dekoracje – psy, chmury, itp. – jeœli chcesz).
    // ...

    // Reset pi³ki i paletki
    ball.setPosition(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);
    ball.getVelocity() = sf::Vector2f(300.f, 300.f);
    paddle = Paddle();
}

void Game::render()
{
    window.clear(sf::Color::Black);

    // Rysujemy t³o w zale¿noœci od stanu
    switch (currentState)
    {
    case GameState::PLAY:
    case GameState::LEVEL_COMPLETE:
    case GameState::GAME_OVER:
        window.draw(backgroundSprite);
        // Dekoracje (static + moving)
        for (auto& s : staticSprites)
            window.draw(s);
        for (auto& ms : movingSprites)
            window.draw(ms.sprite);
        break;

    default:
        window.draw(menuBackgroundSprite);
        break;
    }

    // Rysujemy resztê
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

        // Obiekty gry
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
