# Projekt Ariss 2024/2025 - Jakub Pieczulis s199284
To projekt w C++ z użyciem biblioteki SFML (Simple and Fast Multimedia Library).  
Celem gry jest odbijanie piłki paletką, aby zniszczyć wszystkie klocki na planszy.

## Spis treści

- [Opis](#opis)
- [Wymagania](#wymagania)
- [Instalacja i uruchomienie](#instalacja-i-uruchomienie)
- [Sterowanie](#sterowanie)
- [Funkcjonalności](#funkcjonalno%C5%9Bci)
- [Struktura projektu](#struktura-projektu)

---

## Opis

Projekt zawiera kilka poziomów o różnych tłach i rodzajach klocków (m.in. **IrregularBlock** o nieregularnym kształcie).  
- **Poziom 1 i 5** generuje klocki typu **IrregularBlock** (zamiast kółek).  
- Inne poziomy – kółka, prostokąty i duże prostokąty.

Dodatkowo w grze występuje:
- **Tablica wyników** (scoreboard) zapisywana do pliku `scoreboard.txt`.  
- **Animowane elementy** (chmury, poruszający się ptak w menu).  

---

## Wymagania

- Kompilator C++17 (lub nowszy).
- Biblioteka **SFML** w wersji co najmniej 2.5.1.
- Pliki graficzne w katalogu `Resources/` (m.in. `menu_bg.png`, `bgScore.png`, `bird.png`, `cloud.png`, `tree.png`, `bg...`).

---

## Instalacja i uruchomienie

1. Sklonuj repozytorium:

    ```bash
    git clone https://github.com/TwojeKonto/ArissBreakout.git
    cd Projekt_Ariss_2024_25
    ```

2. Skonfiguruj projekt w wybranym IDE (np. Visual Studio / CLion) lub użyj `CMakeLists.txt` (jeśli dostępny).
3. Upewnij się, że biblioteka **SFML** jest zainstalowana i poprawnie linkowana.
4. Uruchom projekt:
   - W Visual Studio: **Build** → **Run**  
   - Lub z poziomu konsoli (jeśli masz przygotowany `makefile`/`cmake`).

Plik wykonywalny (np. `test.exe`) powinien znajdować się w folderze z plikami graficznymi lub mieć dostęp do katalogu `Resources`.

---

## Sterowanie

- **Strzałki lewo/prawo** (lub **A/D**) – poruszanie paletką.
- **Enter** – akceptacja w menu, przejście do gry, potwierdzenie w `GAME_OVER`/`LEVEL_COMPLETE`.
- **F1** – przełączenie na ekran *TUTORIAL* (lub powrót do gry/menu).
- **ESC** – zapytanie o wyjście (stan `EXIT_CONFIRM`), potwierdzenie `[T]` lub `[N]`.
- **Klawisze 1..0** – wybór poziomu (jeśli nie wpisano nicku, najpierw wyświetla się okno wpisywania).

---

## Funkcjonalności

- **Tablica wyników** – przechowuje nick i wynik gracza, z możliwością zapisu i odczytu.
- **Różne poziomy** (1–10) z różnymi zestawami klocków i tłami.
- **Animowane tło** w menu (ptak poruszający się lewo/prawo).
- **Różne, losowo generowane cele** 
- **Zapis w tablicy wyników!!**
- **I wiele więcej!**

---

## Struktura projektu

- **main.cpp** – punkt wejścia do programu.
- **Game.h / Game.cpp** – główna klasa gry, obsługa stanów (MENU, PLAY, itp.).
- **Entities.h / Entities.cpp** – definicje obiektów (Ball, Paddle, Target, itd.).
- **Scoreboard.h / Scoreboard.cpp** – funkcje do zapisu / odczytu wyników.
- **Backgrounds.h / Backgrounds.cpp** – zarządzanie teksturami teł (np. `bgSpace1.png`, `bgForest1.png`, etc.).
- **Resources/** – katalog z plikami `.png`.

---


---

**Miłej zabawy i powodzenia w rozgrywce!**  

Wszelkie uwagi lub problemy prosimy zgłaszać w formie **Issue** w repozytorium.  
Możesz też przesłać Pull Request z nowymi funkcjonalnościami.  

---

