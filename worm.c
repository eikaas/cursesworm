#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>

#define DELAY 40000
#define NUM_WORMS 20
#define NUM_APPLES 40

#define TRUE 1
#define FALSE 0

typedef struct body {
    int pos_x;
    int pos_y;

    int vel_x;
    int vel_y;

    struct body *next;
} *Body;

typedef struct worm {
    struct body *body;
    struct worm *next;
} *Worm;

typedef struct apple {
    int pos_x;
    int pos_y;

    struct apple *next;
    struct apple *prev;
} *Apple;

// Create a new worm
Worm newWorm(int pos_x, int pos_y);

// Update the worm
void updateWorm(Worm currentWorm, int screen_height, int screen_width);

// Draw the worm
void drawWorm(Worm currentWorm);

// Create a new apple
Apple newApple();

// Next apple in the list
Apple nextApple(Apple currentApple);

// Draw the apple
void drawApple(Apple currentApple);

// Return a random number
int getRandomInt(int from, int to);

Apple newApple(int pos_x, int pos_y) {
    Apple tmpApple = malloc(sizeof(struct apple));
    tmpApple->pos_x = pos_x;
    tmpApple->pos_y = pos_y;

    tmpApple->next = NULL;

    return tmpApple;
}

Apple nextApple(Apple currentApple) {
    return currentApple->next;
}

void drawApple(Apple currentApple) {
    mvprintw(currentApple->pos_y, currentApple->pos_x, "@");
}

// INCOMPLETE, doesnt grow!
void growWorm(Worm currentWorm) {
    int bodyCount = 0;

    // Get the first body part
    Body currentBodypart = currentWorm->body;

    // Traverse the list untill we get to the last body part
    while (currentBodypart != NULL) {
        bodyCount++;
        currentBodypart = currentBodypart->next;
    }

    // Insert new bodypart at the end of the worm
    currentBodypart = malloc(sizeof(struct body));
    currentBodypart->pos_x = currentWorm->body->pos_x;
    currentBodypart->pos_y = currentWorm->body->pos_y;

    // The velocity should be the same as the parent body part
    // NB: Might run into issues here if we grow the worm near the edge of the screen
    currentBodypart->vel_x = currentWorm->body->vel_y;
    currentBodypart->vel_y = currentWorm->body->vel_y;

    currentBodypart->next = NULL;
}

Worm newWorm(int pos_x, int pos_y) {
    Worm tmpWorm = malloc(sizeof(struct worm));
    assert(tmpWorm != NULL);

    // Give the worm an initial body part
    tmpWorm->body = malloc(sizeof(struct body));
    assert(tmpWorm->body != NULL);

    tmpWorm->body->pos_x = pos_x;
    tmpWorm->body->pos_y = pos_y;

    // Set a random initial velocty
    tmpWorm->body->vel_x = getRandomInt(FALSE, TRUE) ? -1 : 1;
    tmpWorm->body->vel_y = getRandomInt(FALSE, TRUE) ? -1 : 1;

    tmpWorm->body->next = NULL;

    // This is the last worm
    tmpWorm->next = NULL;

    return tmpWorm;
}

void updateWorm(Worm currentWorm, int screen_height, int screen_width) {
    assert(currentWorm != NULL);
    assert(currentWorm->body != NULL);
    // if x position is on either left or right side of the screen, flip vel_x
    if (currentWorm->body->pos_x < 0 || currentWorm->body->pos_x > screen_width) {
        currentWorm->body->vel_x *= -1;
    }

    // if y position is on either top or bottom side of the screen, flip vel_y
    if (currentWorm->body->pos_y < 0 || currentWorm->body->pos_y > screen_height) {
        currentWorm->body->vel_y *= -1;
    }

    // Update position
    currentWorm->body->pos_x += currentWorm->body->vel_x;
    currentWorm->body->pos_y += currentWorm->body->vel_y;
}

void drawWorm(Worm currentWorm) {
    assert(currentWorm != NULL);
    assert(currentWorm->body != NULL);

    Body tmpBody = currentWorm->body;

    do {
        mvprintw(tmpBody->pos_y, tmpBody->pos_x, "O");
        tmpBody = tmpBody->next;
    } while (tmpBody != NULL);

    // Draw body
    /*
    for (i = 1; i < currentWorm->length + 1; i++) {
        offset_y = currentWorm->vel_y * i * -1;
        offset_x = currentWorm->vel_x * i * -1;
        mvprintw(currentWorm->pos_y + offset_y, currentWorm->pos_x + offset_x, " O ");
    }

    // Draw tail.
    // Need to determine if we need to increment or decrement the offset. Is there a better way to do this?
    offset_y = offset_y > 0 ? offset_y + 1 : offset_y - 1;
    offset_x = offset_x > 0 ? offset_x + 1 : offset_x - 1;
    mvprintw(currentWorm->pos_y + offset_y, currentWorm->pos_x + offset_x, " o ");
    offset_y = offset_y > 0 ? offset_y + 1 : offset_y - 1;
    offset_x = offset_x > 0 ? offset_x + 1 : offset_x - 1;
    mvprintw(currentWorm->pos_y + offset_y, currentWorm->pos_x + offset_x, " . ");
    */
}

int getRandomInt(int from, int to) {
    struct timespec seed;
    clock_gettime(CLOCK_MONOTONIC, &seed);
    srand(seed.tv_nsec);

    return from + rand() % (to - from + 1);
}

int checkCollision(Worm worm, Apple apple) {
    assert(worm != NULL);
    assert(worm->body != NULL);
    assert(apple != NULL);
    int retval = FALSE;

    int deltaX = abs(worm->body->pos_x - apple->pos_x);
    int deltaY = abs(worm->body->pos_y - apple->pos_y);

    if (deltaX <= 2 && deltaY <= 1) {
        retval = TRUE;
    } else {
        retval = FALSE;
    }

    return retval;
}

int main(int argc, const char *argv[]) {
    int running = TRUE;
    int screen_height, screen_width;
    char c;
    int i;

    // Initialize ncurses
    initscr();
    noecho();
    curs_set(FALSE);
    start_color();

    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);

    getmaxyx(stdscr, screen_height, screen_width);

    // Create worms
    Worm headWorm = newWorm(getRandomInt(0, screen_width), getRandomInt(0, screen_height));
    Worm currentWorm = headWorm;
    for (i = 0; i < NUM_WORMS; i++) {
        currentWorm->next = newWorm(getRandomInt(0, screen_width), getRandomInt(0, screen_height));
        currentWorm = currentWorm->next;
    }

    // Create apples
    Apple headApple = newApple(getRandomInt(0, screen_width), getRandomInt(0, screen_height));
    Apple currentApple = headApple;
    for (i = 0; i < NUM_APPLES; i++) {
        currentApple->next = newApple(getRandomInt(0, screen_width), getRandomInt(0, screen_height));
        currentApple = currentApple->next;
    }

        while (running == TRUE) {
        getmaxyx(stdscr, screen_height, screen_width);

        clear();

        // Draw all worms
        currentWorm = headWorm;
        attron(COLOR_PAIR(2));
        while (currentWorm != NULL) {
            updateWorm(currentWorm, screen_height, screen_width);

            /*
             * Post-Bug note: Dont forget to make a copy when traversing a list!
             * after drawing drawWorm(), currentWorm->body was a null pointer, resulting in the
             * following assertion to faile. (Well, after segfaulting in checkCollision()).
             */
            drawWorm(currentWorm);
            assert(currentWorm->body != NULL);

            currentApple = headApple;

            while (currentApple != NULL) {
                if (checkCollision(currentWorm, currentApple)) {
                    currentApple->pos_x = getRandomInt(0, screen_width);
                    currentApple->pos_y = getRandomInt(0, screen_height);

                }
                currentApple = currentApple->next;
            }

            currentWorm = currentWorm->next;
        }

        // Draw all apples
        currentApple = headApple;
        attron(COLOR_PAIR(1));
        while (currentApple != NULL) {
            drawApple(currentApple);

            currentApple = currentApple->next;
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

