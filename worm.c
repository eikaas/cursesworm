#include <ncurses.h>
#include <unistd.h>

#define DELAY 50000

#define TRUE 1
#define FALSE 0

int main(int argc, const char *argv[]) {
    int running = TRUE;

    int pos_x = 0, pos_y = 0;
    int vel_x = 1, vel_y = 1;
    int tail_x = -1, tail_y = -1;

    int screen_width = 200, screen_height = 81;

    initscr();				// to initialize the window
    noecho();					// do not echo keypresses
    curs_set(FALSE);	// do not display a cursor

    while(running == TRUE) {
        getmaxyx(stdscr, screen_height, screen_width);
        clear();

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

        // Draw Tail
        mvprintw(pos_y + (tail_y * 4), pos_x + (tail_x * 4), " , ");
        mvprintw(pos_y + (tail_y * 3), pos_x + (tail_x * 3), " . ");
        mvprintw(pos_y + (tail_y * 2), pos_x + (tail_x * 2), " o ");
        mvprintw(pos_y + tail_y, pos_x + tail_x, " O ");
        // Draw Head
        mvprintw(pos_y, pos_x, "(:)");

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

        refresh();
        usleep(DELAY);

    }

    endwin();

}

