#include <ncurses.h>
#include <unistd.h>

#define DELAY 50000

#define TRUE 1
#define FALSE 0

typedef struct worm {
    int pos_x;
    int pos_y;

    int vel_x;
    int vel_y;

    int tail_x;
    int tail_y;
    struct worm *next;
} *Worm;

Worm newWorm(int pos_x, int pos_y);
Worm nextWorm(Worm currentWorm);

Worm newWorm(int pox_x, int pos_y) {
    Worm tmpWorm = malloc(sizeof(struct worm));

    if (tmpWorm) {
        tmpWorm->pos_x = pos_x;
        tmpWorm->pos_y = pos_y;
        tmpWorm->vel_x = 1;
        tmpWorm->vel_y = 1;
        tmpWorm->tail_x = -1;
        tmpWorm->tail_y = -1;
        tmpWorm->next = NULL;
    }

    return tmpWorm;
}

Worm nextWorm(Worm currentWorm) {
    return currentWorm->next;
}

int main(int argc, const char *argv[]) {
    int running = TRUE;
    int pos_x = 0, pos_y = 0;
    int vel_x = 1, vel_y = 1;
    int tail_x = -1, tail_y = -1;
    int screen_width, screen_height;
    char c;

    initscr();
    noecho();
    curs_set(FALSE);

    while(running == TRUE) {
        getmaxyx(stdscr, screen_height, screen_width);

        // If we are moving down (vel_y is positive), tail is on the top side (tail_y is negative)
        if (vel_y > 0) {
            tail_y = -1;
        } else {
            tail_y = 1;
        }

        // If we are moving to the right (vel_x is positive), tail is on the left side (tail_x is negative)
        if (vel_x > 0) {
            tail_x = -1;
        } else {
            tail_x = 1;
        }

        // if x position is on either left or right side of the screen, flip vel_x
        if (pos_x < 0 || pos_x > screen_width) {
            vel_x *= -1;
        }

        // if y position is on either top or bottom side of the screen, flip vel_y
        if (pos_y < 0 || pos_y > screen_height) {
            vel_y *= -1;
        }

        // Update position
        pos_x += vel_x;
        pos_y += vel_y;

        clear();

        // Draw Tail
        mvprintw(pos_y + (tail_y * 4), pos_x + (tail_x * 4), " , ");
        mvprintw(pos_y + (tail_y * 3), pos_x + (tail_x * 3), " . ");
        mvprintw(pos_y + (tail_y * 2), pos_x + (tail_x * 2), " o ");
        mvprintw(pos_y + tail_y, pos_x + tail_x, " O ");
        // Draw Head
        mvprintw(pos_y, pos_x, "o_o");

        refresh();
        usleep(DELAY);

        // Exit with q
        timeout(1);
        if ((c = getch()) == 'q')
            running = FALSE;

    }

    endwin();

}

