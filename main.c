#include <curses.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <signal.h>

#define FOOD '@'
#define SNAKE '#'
#define RIP 'X'
#define WALL 'W'

#define INIT_SNAKE_LENGTH 5
#define MAX_SNAKE_LENGTH 500
#define SNAKE_INCREASE 3

#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3
#define FREEZE 4

#define KEY_R_LOWER 114
#define KEY_R_UPPER 82
#define KEY_ESC 27

#define FRAME_SPEED 100

volatile sig_atomic_t signal_status = 0;

void sighandler(int s) {
    signal_status = s;
}

void init_ncurses() {
    initscr();
    savetty();
    nonl();
    cbreak();
    noecho();
    timeout(0);
    leaveok(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);
    
    signal(SIGINT, sighandler);
    signal(SIGQUIT, sighandler);
    signal(SIGWINCH, sighandler);
    signal(SIGTSTP, sighandler);
}

void finish_ncurses() {
    curs_set(1);
    clear();
    refresh();
    resetty();
    endwin();
    exit(0);
}

typedef struct Coord {
    int row, col;
} Coord;

Coord snake[MAX_SNAKE_LENGTH];

char **board;

Coord get_random_coord() {
    Coord rand_coord;
    while (true) {
        rand_coord.row = rand() % (LINES - 1);
        rand_coord.col = rand() % (COLS - 1);
        if (board[rand_coord.row][rand_coord.col] == ' ') {
            break;
        }
    }
    return rand_coord;
}

void draw_char(int row, int col, char ch) {
    board[row][col] = ch;
    move(row, col);
    addch(ch);
}

void draw_food() {
    Coord rand_coord = get_random_coord();
    draw_char(rand_coord.row, rand_coord.col, FOOD);
}

void clear_board() {
    for (int i = 0; i < LINES; i++)
        for (int j = 0; j < COLS; j++)
            if(i == 0 || i == LINES - 1 
            || j == 0 || j == COLS - 1) {
                board[i][j] = WALL;
                draw_char(i, j, WALL);
            }
            else board[i][j] = ' ';
}

void init_board() {
    board = (char**) malloc(LINES * sizeof(char*));
    if (!board) {
        finish_ncurses();
    }
    for (int i = 0; i < LINES; i++) {
        board[i] = (char*) malloc(COLS * sizeof(char));
        if (!board[i]) {
            finish_ncurses();
        }
    }
    clear_board();
}


int snake_size = 1;
int snake_length = INIT_SNAKE_LENGTH;

int direction;
int game_over = 0;

void change_direction() {
    int keypress;
    keypress = wgetch(stdscr);
    if (keypress == ERR) {
        return;
    }
    if (keypress == KEY_ESC) {
        finish_ncurses();
    }
    if (game_over) {
        if (keypress == KEY_R_LOWER || keypress == KEY_R_UPPER) {
            clear_board();
            snake_length = INIT_SNAKE_LENGTH;
            snake_size = 1;
            snake[0] = get_random_coord();
            direction = rand() % 4;
            game_over = 0;
            clear();
            refresh();
            clear_board();
            draw_char(snake[0].row, snake[0].col, SNAKE);
            draw_food();
        }
        return;
    }
    if (keypress == KEY_LEFT && direction != RIGHT) {
        direction = LEFT;
    } else if (keypress == KEY_RIGHT && direction != LEFT) {
        direction = RIGHT;
    } else if (keypress == KEY_UP && direction != DOWN) {
        direction = UP;
    }  else if (keypress == KEY_DOWN && direction != UP) {
        direction = DOWN;
    }
}

void change_head() {
    if (direction == RIGHT) {
        snake[0].col++;
    } else if (direction == LEFT) {
        snake[0].col--;
    } else if (direction == UP) {
        snake[0].row--;
    } else if (direction == DOWN) {
        snake[0].row++;
    }
}

int verify_head() {
    if (snake[0].row < 1 || snake[0].col < 1
    || snake[0].row >= LINES - 1 || snake[0].col >= COLS - 1
    || board[snake[0].row][snake[0].col] == '#') {
        draw_char(snake[1].row, snake[1].col, RIP);
        return 1;
    }
    return 0;
}

void snake_logic() {
    memmove(&snake[1], &snake[0], sizeof(Coord) * snake_size);
    change_head();
    if (verify_head()) {
        direction = FREEZE;
        game_over = 1;
        return;
    }
    if (board[snake[0].row][snake[0].col] == FOOD) {
        snake_length += SNAKE_INCREASE;
        draw_food();
    }
    draw_char(snake[0].row, snake[0].col, SNAKE);
    if (snake_size < snake_length) {
        snake_size++;
    } else {
        draw_char(snake[snake_size].row, snake[snake_size].col, ' ');
    }
}

int main() {
    srand((unsigned) time(NULL));
    direction = rand() % 4;
    
    init_ncurses();
    init_board();
    snake[0] = get_random_coord();
    draw_char(snake[0].row, snake[0].col, SNAKE);
    draw_food();
    while (true) {
        if (signal_status) {
            finish_ncurses();
        }
        if (direction != FREEZE && !game_over) {
            snake_logic();
        }
        change_direction();
        napms(FRAME_SPEED);
        change_direction();
    }
    finish_ncurses();
    
    return 0;
}

