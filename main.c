#include <curses.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

// basic game configurations.
#define MAX_FPS 45
#define JUMP_DELAY 8
#define GRAVITY 0.07
#define TOTAL_PIPES 5
#define JUMP_ACCELERATION 1.2
#define MAX_Y_VELOCITY 1
#define SPEED_INCREASE_MULTIPLIER 0.08

// other configurations.
#define BIRD_SPRITE "***"
#define ONE_SECOND 1000000
#define ONE_SECOND_MS 1000
#define KEY_ESC 27
#undef KEY_ENTER // remove default KEY_ENTER macro because it wasn't working.
#define KEY_ENTER 10
#define KEY_SPACE ' '
#define PIPE_SPRITE "*******"
#define PIPE_SPRITE_LENGTH 7

// max screen width and height.
uint32_t maxY, maxX;

// clang-format off
const char*CRAPPY_LOGO = 
"\n_______  ______    _______  _______  _______  __   __    _______  ___   ______    ______  \n"
"|       ||    _ |  |   _   ||       ||       ||  | |  |  |  _    ||   | |    _ |  |      | \n"
"|       ||   | ||  |  |_|  ||    _  ||    _  ||  |_|  |  | |_|   ||   | |   | ||  |  _    |\n"
"|       ||   |_||_ |       ||   |_| ||   |_| ||       |  |       ||   | |   |_||_ | | |   |\n"
"|      _||    __  ||       ||    ___||    ___||_     _|  |  _   | |   | |    __  || |_|   |\n"
"|     |_ |   |  | ||   _   ||   |    |   |      |   |    | |_|   ||   | |   |  | ||       |\n"
"|_______||___|  |_||__| |__||___|    |___|      |___|    |_______||___| |___|  |_||______| \n";

const char*WELCOME_TEXT = "\nPress 'p' or ENTER to play.\n"
  "Press 'q' or ESC to quit.\n"
  "How to play? just spam space.\n";

// clang-format on

uint64_t frameCounter = 0;

// @brief Vec2 acts as a 2 dimensional vector type.
typedef struct Vec2 {
  double x, y;
} Vec2;

// @brief Main bird struct to holds its properties.
typedef struct Bird {
  Vec2 pos;
  Vec2 vel;
  char *sprite;
} Bird;

// @brief struct to hold pipes data.
typedef struct Pipe {
  Vec2 pos;
  uint32_t height;
  bool isTop;
} Pipe;

// @brief renders paused screen stuff.
void render_pause_menu(WINDOW *w, bool renderGameOverText) {
  mvwprintw(w, maxY / 2, maxX / 2, "%s", CRAPPY_LOGO);
  if (renderGameOverText) {
    mvwprintw(w, maxY / 2 + 10, maxX / 2, "Game Over");
  }
  mvwprintw(w, maxY / 2 + 20, maxX / 2, "%s", WELCOME_TEXT);
}

// @brief function to print the bird at its current position.
// @param Bird Bird object
void render_bird(WINDOW *w, Bird *_b) {
  attron(A_BOLD);
  mvwprintw(w, _b->pos.y, _b->pos.x, "%s", _b->sprite);
  attroff(A_BOLD);
}

// @brief function to render a pipe.
// @param Pipe pipe object to render.
void render_pipe(WINDOW *w, Pipe *r_pipe) {
  if (r_pipe->pos.x < maxX - PIPE_SPRITE_LENGTH) {
    uint32_t _y = r_pipe->isTop ? 0 : maxY - r_pipe->height;
    for (size_t i = 0; i < r_pipe->height; i++) {
      mvwprintw(w, _y + i, r_pipe->pos.x, "%s", PIPE_SPRITE);
    }
  }
}

// @brief function to render current frame counter at the top left corner of the
// screen.
void render_frame_counter() { mvprintw(0, 0, "Score: %ld", frameCounter); }

// @brief get random height for pipe.
uint32_t random_height() { return (maxY / 4) + (rand() % maxY / 3); }

// @brief get random x position for pipe.
uint32_t random_x_pos() { return maxX + (rand() % 200); }

// @brief resets initial pipes data.
// @param Pipes pointer to all pipe objects array.
void reset_pipe(Pipe *pipe) {
  pipe->pos.x = random_x_pos();
  pipe->pos.y = 0;
  pipe->isTop = rand() & 1;
  pipe->height = random_height();
}

// @brief resets initial values of a bird object.
// @param Bird pointers to the bird object to reset.
void reset_bird(Bird *bird) {
  bird->pos.x = 5;
  bird->pos.y = (double)maxY / 2;
  bird->vel.x = 0;
  bird->vel.y = 0;
  bird->sprite = BIRD_SPRITE;
}

// @brief Detects collision between the bird and a pipe object.
// @param Bird the bird object.
// @param Pipe the pipe object to check collision against.
bool detect_collision(Bird *bird, Pipe *pipe) {

  return true;
}

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

  // makes the cursor invisible.
  // WARNING: Not supported by all terminals.
  curs_set(0);

  // timeouts user input.
  wtimeout(window, ONE_SECOND_MS / MAX_FPS);

  // max width and height of the window.
  getmaxyx(window, maxY, maxX);

  // init bird object.
  Bird bird;
  reset_bird(&bird);

  // init pipes.
  Pipe pipes[TOTAL_PIPES];
  Pipe *p_temp = pipes;
  // reset all pipes.
  for (size_t i = 0; i < TOTAL_PIPES; i++) {
    reset_pipe(p_temp);
    p_temp++;
  }

  // User input.
  uint32_t key_pressed = wgetch(window);

  // main loop.
  bool runGame = true;
  uint32_t frameJumpedOn = 0;
  bool gamePaused = true;
  double speedMultiplier = 0.7;
  bool gameOverText = false;

  // main game loop.
  while (runGame) {

    // user input.
    key_pressed = wgetch(window);

    // clears window.
    wclear(window);

    // refreshs window, obviously.
    wrefresh(window);

    // if esc is pressed
    if (key_pressed == KEY_ESC || key_pressed == 'Q' || key_pressed == 'q') {
      runGame = false;
      break;
    }

    // if p is pressed.
    if (key_pressed == KEY_ENTER || key_pressed == 'p' || key_pressed == 'P') {
      gamePaused = !gamePaused;
    }

    // when the game is paused or over.
    if (gamePaused) {

      // resetting stuff after a gameover.
      if (gameOverText) {
        reset_bird(&bird);

        speedMultiplier = 0.7;
        frameCounter = 0;
        // resetting pipes.
        p_temp = pipes;
        for (size_t i = 0; i < TOTAL_PIPES; i++) {
          reset_pipe(p_temp);
          p_temp++;
        }
      }

      // rendering pause menu.
      render_pause_menu(window, gameOverText);
      continue;
    }

    // checking for collision with ground.
    if (bird.pos.y >= maxY - 2) {
      gamePaused = true;
      gameOverText = true;
    }

    // checking for collision with pipes.
    p_temp = pipes;
    for (size_t i = 0; i < TOTAL_PIPES; i++) {
      if (detect_collision(&bird, p_temp)) {
        gamePaused = true;
        gameOverText = true;
      };
      p_temp++;
    }

    // process user input.
    if (key_pressed == KEY_SPACE && frameCounter - frameJumpedOn > JUMP_DELAY) {
      // if user presses space.
      frameJumpedOn = frameCounter;
      bird.vel.y = -JUMP_ACCELERATION;
    } else {
      // adding acceleration due to gravity to velocity.
      bird.vel.y += GRAVITY;
    }

    // clamping velocity.
    bird.vel.y = bird.vel.y > MAX_Y_VELOCITY ? MAX_Y_VELOCITY : bird.vel.y;

    // clamping position to screensize.
    if (bird.pos.y <= 1) {
      bird.pos.y = 1;
    }

    // adding velocity to current position to get new position.
    bird.pos.x += bird.vel.x;
    bird.pos.y += bird.vel.y;

    // updating speedMultiplier every 100 frames.
    if (frameCounter % 100 == 0) {
      speedMultiplier += SPEED_INCREASE_MULTIPLIER;
    }

    // updating pipes
    p_temp = pipes;
    for (size_t i = 0; i < TOTAL_PIPES; i++) {
      p_temp->pos.x = p_temp->pos.x - speedMultiplier;
      p_temp++;
    }

    // checking for dead pipes.
    p_temp = pipes;
    for (size_t i = 0; i < TOTAL_PIPES; i++) {
      if (p_temp->pos.x <= 0) {
        reset_pipe(p_temp);
      }
      p_temp++;
    }

    // drawing.

    // drawing pipes.
    p_temp = pipes;
    for (size_t i = 0; i < TOTAL_PIPES; i++) {
      render_pipe(window, p_temp);
      p_temp++;
    }

    // render bird.
    render_bird(window, &bird);

    // drawing frame counter.
    render_frame_counter();

    // sleep, locks the fps at FPS_MAX, dont need it because of timeout set to
    // getchw.
    // usleep(ONE_SECOND / MAX_FPS);

    // update frame counter.
    frameCounter++;
  }

  // closes window and clears up ncurses stuff.
  endwin();

  return EXIT_SUCCESS;
}
