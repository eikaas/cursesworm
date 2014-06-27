#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>

#define DELAY 40000
#define NUM_WORMS 1
#define NUM_APPLES 40

#define TRUE 1
#define FALSE 0

/*
 * Bugs:
 *  * FIXED: screen_height and screen_width gets mixed up somewhere
 *  * FIXED: worms spawning only on one side.
 *  * when resizing, if a worm is outside the screen its lost untill resized back.
 *  * Worms dont grow.
 *  * FIXED: Worms travel 1px outside the edge on right and bottom side.
 */

typedef struct control {
    int screen_width;
    int screen_height;
    int running;
} *Control;

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

// To be removed
Control control;

void drawDebug();

// Struct for holding global control vars.
Control newControl();

// Create a new worm
Worm newWorm(int pos_x, int pos_y);

// Update the worm
void updateWorm(Worm currentWorm); 
// Draw the worm
void drawWorm(Worm currentWorm);

// Create a new apple
Apple newApple();

// Draw the apple
void drawApple(Apple currentApple);

// Return a random number
int getRandomInt(int from, int to);

// Logic
void logic(Worm headWorm, Apple headApple);
// Update
void update(Worm headWorm, Apple headApple);
// Render
void render(Worm headWorm, Apple headApple);

Control newControl() {
    Control tmpControl;

    if ( (tmpControl = malloc(sizeof(struct control))) != NULL ) {
        tmpControl->screen_width  = 0;
        tmpControl->screen_height = 0;
        tmpControl->running = TRUE;
    }

    return tmpControl;
}

Apple newApple(int pos_x, int pos_y) {
    Apple tmpApple = malloc(sizeof(struct apple));
    tmpApple->pos_x = pos_x;
    tmpApple->pos_y = pos_y;

    tmpApple->next = NULL;

    return tmpApple;
}

void drawApple(Apple currentApple) {
    mvprintw(currentApple->pos_y, currentApple->pos_x, "@");
}

void growWorm(Worm currentWorm) {
    // Allways store a pointer to the previous body part
    Body prevBodypart = currentWorm->body;

    // Get the first body part
    Body currentBodypart = currentWorm->body;

    // Traverse the list untill we get to the last body part
    while (currentBodypart != NULL) {
        prevBodypart = currentBodypart;
        currentBodypart = currentBodypart->next;
    }

    // Insert new bodypart at the end of the worm
    currentBodypart = malloc(sizeof(struct body));

    // Need to decrement or increment depending on the velocity of the
    // previous body part for the next bodypart to end up in the right place.

    // If the previous bodypart is traveling to the right
    if (prevBodypart->vel_x > 0) {
        currentBodypart->pos_x = prevBodypart->pos_x - 1;
    // Else if the previous bodypart is traveling to the left
    } else {
        currentBodypart->pos_x = prevBodypart->pos_x + 1;
    }

    // If the previous bodypart is traveling down
    if (prevBodypart->vel_y > 0) {
        currentBodypart->pos_y = prevBodypart->pos_y - 1;
    // else if the previous bodypart is traveling up
    } else {
        currentBodypart->pos_y = prevBodypart->pos_y + 1;
    }

    // Set the same velocity as the previous body part.
    currentBodypart->vel_x = prevBodypart->vel_x;
    currentBodypart->vel_y = prevBodypart->vel_y;

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

// TODO: Needs to check all the links and flip each link when it hits the side.
void updateWorm(Worm currentWorm) {
    assert(currentWorm != NULL);
    assert(currentWorm->body != NULL);

    Body currentBodypart = currentWorm->body;
    // Traverse the body of the worm.
    while (currentBodypart != NULL) {
        // if x position is on either left or right side of the screen, flip vel_x
        if (currentBodypart->pos_x < 0 || currentBodypart->pos_x >= control->screen_width)
            currentBodypart->vel_x *= -1;

        // if y position is on either top or bottom side of the screen, flip vel_y
        if (currentBodypart->pos_y < 0 || currentBodypart->pos_y >= control->screen_height)
            currentBodypart->vel_y *= -1;

        // Update the position
        currentBodypart->pos_x += currentBodypart->vel_x;
        currentBodypart->pos_y += currentBodypart->vel_y;

        currentBodypart = currentBodypart->next;
    }
}

/*
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


 // TODO: This should not be done in the draw function. Update to deal with the new link body and only set position. No drawing
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
}
*/

void drawDebug() {
    mvprintw(1, 1, "screen_width: %d, screen_height: %d", control->screen_width, control->screen_height);
}

void drawWorm(Worm currentWorm) {
    assert(currentWorm != NULL);
    assert(currentWorm->body != NULL);

    Body currentBodypart = currentWorm->body;

    while (currentBodypart != NULL) {
        mvprintw(currentBodypart->pos_y, currentBodypart->pos_x, "O");
        currentBodypart = currentBodypart->next;
    }
}

int getRandomInt(int from, int to) {
    struct timespec seed;
    clock_gettime(CLOCK_MONOTONIC, &seed);
    srand(seed.tv_nsec);

    return from + rand() % (to - from + 1);
}

int checkCollision(Worm worm, Apple apple) {
    int retval = FALSE;

    assert(worm != NULL);
    assert(worm->body != NULL);
    assert(apple != NULL);

    // The "head" of the worm is the first body part
    Body wormHead = worm->body;

    // If we eat an apple
    if (wormHead->pos_x == apple->pos_x && wormHead->pos_y == apple->pos_y) {
        retval = TRUE;
    } else {
        retval = FALSE;
    }

    return retval;
}

int main(int argc, const char *argv[]) {
    int running = TRUE;
    int i;
    char c;

    control = newControl();

    // Initialize ncurses
    initscr();
    noecho();
    curs_set(FALSE);

    /* getmaxyx takes args in y,x. There is a bug related to this. make it go away */
    getmaxyx(stdscr, control->screen_height, control->screen_width);

    // Create worms
    Worm headWorm = newWorm(getRandomInt(0, control->screen_width), getRandomInt(0, control->screen_height));
    Worm currentWorm = headWorm;
    for (i = 0; i < NUM_WORMS - 1; i++) {
        currentWorm->next = newWorm(getRandomInt(0, control->screen_width), getRandomInt(0, control->screen_height));
        currentWorm = currentWorm->next;
    }

    // Create apples
    Apple headApple = newApple(getRandomInt(0, control->screen_width), getRandomInt(0, control->screen_height));
    Apple currentApple = headApple;
    for (i = 0; i < NUM_APPLES; i++) {
        currentApple->next = newApple(getRandomInt(0, control->screen_width), getRandomInt(0, control->screen_height));
        currentApple = currentApple->next;
    }

    // Mail Loop
    while (running == TRUE) {
        // We need to set the terminal dimensions to screen_height and screen_width for each
        // iteration of this loop to make resizing possible
        getmaxyx(stdscr, control->screen_height, control->screen_width);

        logic(headWorm, headApple);
        update(headWorm, headApple);
        render(headWorm, headApple);

        // Quit if q is pressed
        timeout(1);
        if ((c = getch()) == 'q')
            running = FALSE;
    }

    endwin();

    return EXIT_SUCCESS;
}

void logic(Worm headWorm, Apple headApple) {
    Worm currentWorm = headWorm;
    Apple currentApple = headApple;

    // For every worm
    while (currentWorm != NULL) {
        // Check for collision with apples
        while (currentApple != NULL) {
            // If we have a collision
            if (checkCollision(currentWorm, currentApple)) {
                // Move the apple
                currentApple->pos_x = getRandomInt(0, control->screen_width);
                currentApple->pos_y = getRandomInt(0, control->screen_height);
                // Grow the worm
                growWorm(currentWorm);
            }
            // Next apple
            currentApple = currentApple->next;
        }
        // Next worm
        currentWorm = currentWorm->next;
        // Reset applepointer
        currentApple = headApple;
    }

}

void update(Worm headWorm, Apple headApple) {
    Worm currentWorm = headWorm;

    // For every worm
    while (currentWorm != NULL) {
        updateWorm(currentWorm);
        currentWorm = currentWorm->next;
    }
}

void render(Worm headWorm, Apple headApple) {
    Worm currentWorm = headWorm;
    Apple currentApple = headApple;

    // Clear the screen
    clear();

    // Initialize color
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);

    // Set Worm color
    attron(COLOR_PAIR(2));
    // Render every worm
    while (currentWorm != NULL) {
        drawWorm(currentWorm);
        currentWorm = currentWorm->next;
    }

    // Set Apple color
    attron(COLOR_PAIR(1)); 
    // Render all apples
    while (currentApple != NULL) {
        drawApple(currentApple);
        currentApple = currentApple->next;
    }

    drawDebug();

    refresh();
    usleep(DELAY);
}

