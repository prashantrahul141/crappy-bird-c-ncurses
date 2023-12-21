#include <curses.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#define BIRD_SPRITE "***"
#define MAX_FPS 30
#define ONE_SECOND 1000000
#define ONE_SECOND_MS 1000
#define KEY_ESC 27

uint64_t frameCounter = 0;

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
void print_bird(WINDOW *w, Bird *_b) {
  attron(A_BOLD);
  mvwprintw(w, _b->pos.y, _b->pos.x, "%s", _b->sprite);
  attroff(A_BOLD);
}

void print_frame_counter() { printw("%ld", frameCounter); }

// entry point.
int main() {

  // init window.
  WINDOW *window = initscr();

  // disables stuff like line buffering, for now disabled because ctrl+c doesnt
  // close the program if enabled.
  // raw();
  cbreak();

  // disables eching of key press.
  noecho();

  // lets us poll keyboard events.
  keypad(window, true);

  // set initial cursor to 0
  curs_set(0);

  // timeouts user input.
  wtimeout(window, ONE_SECOND_MS / MAX_FPS);

  // max width and height of the window.
  uint32_t maxY, maxX;
  getmaxyx(window, maxY, maxX);

  // init bird object.
  Bird bird = {
      "Crappy Bird", {(int)(maxY / 2), (int)(maxX / 2)}, {0, 0}, BIRD_SPRITE};

  // User input.
  int key_pressed = wgetch(window);

  // main loop.
  bool runGame = true;

  // main game loop.
  while (runGame) {

    // user input.
    key_pressed = wgetch(window);

    // clears window.
    wclear(window);

    print_bird(&bird);

    // refreshs window, obviously.
    refresh();

    // sleep : locks the fps at 60.
  }

  // closes window and clears up ncurses stuff.
  endwin();

  return 0;
}