#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>

#define COLS 60
#define ROWS 30

int main()
{
    // hide cursor
    printf("\e[?25l");

    // Switch to canonical mode, disable echo
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // variables
    int x[1000], y[1000];
    int exit_key = 0;

    while (!exit_key)
    {
        //? tables
        printf("┌");
        for (int i = 0; i < COLS; i++)
        {
            printf("─");
        }
        printf("┐\n");
        for (int j = 0; j < ROWS; j++)
        {
            printf("│");
            for (int i = 0; i < COLS; i++)
            {
                printf("·");
            }
            printf("│\n");
        }
        printf("└");
        for (int i = 0; i < COLS; i++)
        {
            printf("─");
        }
        printf("┘\n");

        //? cursor movement botton to top
        printf("\e[%iA", ROWS + 2);

        int head = 0, tail = 0;
        x[head] = COLS / 2;
        y[head] = ROWS / 2;
        int game_over = 0;
        int xdir = 1, ydir = 0;
        int applex = -1, appley;

        while (!exit_key && !game_over)
        {
            if (applex < 0)
            {
                // create new apple
                applex = rand() % COLS;
                appley = rand() % ROWS;
                // printf("%i", applex);

                for (int i = tail; i != head; i = (i + 1) % 1000)
                {
                    if (x[i] == applex && y[i] == appley)
                    {
                        applex = -1;
                    }
                }
                if (applex >= 0)
                {
                    // draw apple
                    printf("\e[%iB\e[%iC❤", appley + 1, applex + 1);
                    printf("\e[%iF", appley + 1);
                }
            }

            // clearing snake tail
            printf("\e[%iB\e[%iC·", y[tail] + 1, x[tail] + 1);
            printf("\e[%iF", y[tail] + 1);

            if (x[head] == applex && y[head] == appley)
            {
                applex = -1;
                printf("\a");
            }
            else
            {

                tail = (tail + 1) % 1000;
            }

            int new_head = (head + 1) % 1000;
            x[new_head] = (x[head] + xdir + COLS) % COLS;
            y[new_head] = (y[head] + ydir + ROWS) % ROWS;
            head = new_head;

            for (int i = tail; i != head; i = (i + 1) % 1000)
            {
                if (x[i] == x[head] && y[i] == y[head])
                {
                    game_over = 1;
                }
            }

            // draw the head
            printf("\e[%iB\e[%iC▓", y[head] + 1, x[head] + 1);
            printf("\e[%iF", y[head] + 1);
            fflush(stdout);

            usleep(5 * 1000000 / 60);

            // read from keyboard
            struct timeval tv;
            fd_set fds;
            tv.tv_sec = 0;
            tv.tv_usec = 0;
            FD_ZERO(&fds);
            FD_SET(STDIN_FILENO, &fds);
            select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
            if (FD_ISSET(STDIN_FILENO, &fds))
            {
                int ch = getchar();

                if (ch == 27 || ch == 'q')
                {
                    exit_key = 1;
                }
                else if (ch == 'a' && xdir != 1)
                {
                    xdir = -1;
                    ydir = 0;
                }
                else if (ch == 'd' && xdir != -1)
                {
                    xdir = 1;
                    ydir = 0;
                }
                else if (ch == 's' && ydir != -1)
                {
                    xdir = 0;
                    ydir = 1;
                }
                else if (ch == 'w' && ydir != 1)
                {
                    xdir = 0;
                    ydir = -1;
                }
            }
        }
        if (!exit_key)
        {
            printf("\e[%iB\e[%iC GAME OVER !", ROWS / 2, COLS / 2 - 5);
            printf("\e[%iF", ROWS / 2);
            fflush(stdout);
            getchar();
        }
    }
    // show cursor
    printf("\e[?25h");
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return 0;
}