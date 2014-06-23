#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define DELAY 40000
#define NUM_WORMS 200

#define TRUE 1
#define FALSE 0

typedef struct worm {
    int pos_x;
    int pos_y;

    int vel_x;
    int vel_y;

    struct worm *next;
} *Worm;

// Create a new worm
Worm newWorm(int pos_x, int pos_y);

// Get the next worm in the list
Worm nextWorm(Worm currentWorm);

// Update the worm
void updateWorm(Worm currentWorm, int screen_height, int screen_width);
// Draw the worm
void drawWorm(Worm currentWorm, int screen_height, int screen_width);

// Return a random number
int getRandomInt(int from, int to);

Worm newWorm(int pos_x, int pos_y) {
    Worm tmpWorm = malloc(sizeof(struct worm));

    if (tmpWorm) {
        tmpWorm->pos_x = pos_x;
        tmpWorm->pos_y = pos_y;

        tmpWorm->vel_x = getRandomInt(0, 1) ? 1 : -1;
        tmpWorm->vel_y = getRandomInt(0, 1) ? 1 : -1;

        tmpWorm->next = NULL;
    }

    return tmpWorm;
}

Worm nextWorm(Worm currentWorm) {
    return currentWorm->next;
}

void updateWorm(Worm currentWorm, int screen_height, int screen_width) {
    // if x position is on either left or right side of the screen, flip vel_x
    if (currentWorm->pos_x < 0 || currentWorm->pos_x > screen_width) {
        currentWorm->vel_x *= -1;
    }

    // if y position is on either top or bottom side of the screen, flip vel_y
    if (currentWorm->pos_y < 0 || currentWorm->pos_y > screen_height) {
        currentWorm->vel_y *= -1;
    }

    // Update position
    currentWorm->pos_x += currentWorm->vel_x;
    currentWorm->pos_y += currentWorm->vel_y;
}

void drawWorm(Worm currentWorm, int screen_height, int screen_width) {
    // Draw Head
    mvprintw(currentWorm->pos_y, currentWorm->pos_x, "o_o");

    // Draw Tail
    mvprintw(currentWorm->pos_y + (currentWorm->vel_y * -1 * 4), currentWorm->pos_x + (currentWorm->vel_x * -1 * 4), " , ");
    mvprintw(currentWorm->pos_y + (currentWorm->vel_y * -1 * 3), currentWorm->pos_x + (currentWorm->vel_x * -1 * 3), " . ");
    mvprintw(currentWorm->pos_y + (currentWorm->vel_y * -1 * 2), currentWorm->pos_x + (currentWorm->vel_x * -1 * 2), " o ");
    mvprintw(currentWorm->pos_y + (currentWorm->vel_y * -1), currentWorm->pos_x + (currentWorm->vel_x * -1), " O ");
}

int getRandomInt(int from, int to) {
    struct timespec seed;
    clock_gettime(CLOCK_MONOTONIC, &seed);
    srand(seed.tv_nsec);

    return from + rand() % (to - from + 1);
}

int main(int argc, const char *argv[]) {
    int running = TRUE;
    int screen_height, screen_width;
    char c;
    int i;

    initscr();
    noecho();
    curs_set(FALSE);

    getmaxyx(stdscr, screen_height, screen_width);

    Worm headWorm = newWorm(getRandomInt(0, screen_width), getRandomInt(0, screen_height));

    Worm currentWorm = headWorm;

    // Create more worms
    for (i = 0; i < NUM_WORMS; i++) {
        currentWorm->next = newWorm(getRandomInt(0, screen_width), getRandomInt(0, screen_height));
        currentWorm = currentWorm->next;
    }


        while (running == TRUE) {
        getmaxyx(stdscr, screen_height, screen_width);

        clear();

        currentWorm = headWorm;
        while (currentWorm != NULL) {
            updateWorm(currentWorm, screen_height, screen_width);
            drawWorm(currentWorm, screen_height, screen_width);

            currentWorm = currentWorm->next;
        }

        refresh();
        usleep(DELAY);

        timeout(1);
        if ((c = getch()) == 'q')
            running = FALSE;

    }

    endwin();

    return EXIT_SUCCESS;
}

