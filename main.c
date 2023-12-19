#include <curses.h>
#include <stdbool.h>

#define BIRD_SPRITE "**\n**"
#define FPS 60

// @brief Vec2 acts as a 2 dimensional vector type.
typedef struct Vec2 {
  double x, y;
} Vec2;

// @brief Main bird struct to holds its properties.
typedef struct Bird {
  char *name;
  Vec2 pos;
  Vec2 vel;
  char *sprite;
} Bird;

// @brief function to print the bird at its current position.
// @param Bird Bird object
void print_bird(Bird *_b) { printw("%s", _b->sprite); }

int main() {
  Bird bird = {"Crappy Bird", {0, 0}, {0, 0}, BIRD_SPRITE};

  // init window.
  WINDOW *window = initscr();

  // flag for main game loop.
  bool runGame = true;

  // main game loop.
  while (runGame) {
    // clears window.
    wclear(window);

    print_bird(&bird);

    // refreshs window, obviously.
    refresh();

    // sleep : locks the fps at 60.
  }

  // closes window.
  endwin();

  return 0;
}
