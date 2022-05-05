#include <stdlib.h>
#include <ncurses.h>
#include <time.h>

#define FOOD '@'
#define SNAKE '#'
#define RIP 'X'
#define WALL 'W'
#define GRID ' '

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


typedef struct Coord {
    int row, col;
} Coord;

typedef struct Stack {
    Coord head, tail;
} Stack;

char **board;
Stack *snake;

int snake_draw_length = 0;
int snake_length = INIT_SNAKE_LENGTH;

int direction = LEFT;

int game_over = 0;


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
}

void finish_ncurses() {
    curs_set(1);
    clear();
    refresh();
    resetty();
    endwin();
    exit(0);
}

void draw_char(int row, int col, char ch) {
    move(row, col);
    addch(ch);
    refresh();
}

void draw_food() {
    Coord rand_coord;
    while (true) {
        rand_coord.row = rand() % (LINES - 2);
        rand_coord.col = rand() % (COLS - 2);
        if (board[rand_coord.row][rand_coord.col] == GRID) break;
    }
    board[rand_coord.row][rand_coord.col] = FOOD;
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
            else board[i][j] = GRID;
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

void init_snake() {
    snake = (Stack*)malloc(LINES * COLS * sizeof(Stack));
    
    snake[0].head.row = LINES / 2 - 1;
    snake[0].head.col = COLS / 2 - 1;
    
    draw_char(snake[0].head.row, snake[0].head.col, SNAKE);
    
    snake_draw_length++;
}

void change_tail() {
   for(int i = snake_draw_length; i > 0; i--)
        snake[i].head = snake[i - 1].tail = snake[i - 1].head;
}

void change_head() {
    switch(direction) {
        case LEFT:
            change_tail();
            snake[0].head.col--;
            break;
            
        case RIGHT:
            change_tail();
            snake[0].head.col++;
            break;
            
        case UP:
            change_tail();
            snake[0].head.row--;
            break;
            
        case DOWN:
            change_tail();
            snake[0].head.row++;
            break;
    }
}

void change_direction() {
    int keypressed = wgetch(stdscr);
    switch(keypressed) {
        case ERR:
            return;
            
        case KEY_ESC:
            finish_ncurses();
            break;
        
        case KEY_R_LOWER:
            clear_board();
            snake_length = INIT_SNAKE_LENGTH;
            snake_draw_length = 1;
            snake[0].head.row = LINES / 2;
            snake[0].head.col = COLS / 2;
            direction = rand() % 4;
            game_over = 0;
            clear();
            refresh();
            clear_board();
            draw_char(snake[0].head.row, snake[0].head.col, SNAKE);
            draw_food();
            break;
            
        case KEY_R_UPPER:
            clear_board();
            snake_length = INIT_SNAKE_LENGTH;
            snake_draw_length = 1;
            snake[0].head.row = LINES / 2;
            snake[0].head.col = COLS / 2;
            direction = rand() % 4;
            game_over = 0;
            clear();
            refresh();
            clear_board();
            draw_char(snake[0].head.row, snake[0].head.col, SNAKE);
            draw_food();
            break;
        
        case KEY_LEFT:
            if(direction != RIGHT) direction = LEFT;
            break;
            
        case KEY_RIGHT:
            if(direction != LEFT) direction = RIGHT;
            break;
            
        case KEY_UP:
            if(direction != DOWN) direction = UP;
            break;
            
        case KEY_DOWN:
            if(direction != UP) direction = DOWN;
            break;
    }
}

int verify_head() {
    if (snake[0].head.row < 1 || snake[0].head.col < 1
    || snake[0].head.row >= LINES - 1 || snake[0].head.col >= COLS - 1
    || board[snake[0].head.row][snake[0].head.col] == '#') {
        draw_char(snake[1].head.row, snake[1].head.col, RIP);
        return 1;
    }
    return 0;
}

void snake_logic() {
    change_head();
    if (verify_head()) {
        direction = FREEZE;
        game_over = 1;
        return;
    }
    if (board[snake[0].head.row][snake[0].head.col] == FOOD) {
        snake_length += SNAKE_INCREASE;
        draw_food();
    }
    board[snake[0].head.row][snake[0].head.col] = SNAKE;
    draw_char(snake[0].head.row, snake[0].head.col, SNAKE);
    
    if(snake_draw_length <= snake_length) snake_draw_length++;
    else {
        board[snake[snake_draw_length - 1].tail.row][snake[snake_draw_length - 1].tail.col] = GRID;
        draw_char(snake[snake_draw_length - 1].tail.row, snake[snake_draw_length - 1].tail.col, GRID);
    }
}

int main() {
    srand((unsigned) time(NULL));
    init_ncurses();
    init_board();
    init_snake();
    draw_food();
    while(true) {
        change_direction();
        napms(100);
        if(direction != FREEZE && !game_over) snake_logic();    
    }
    finish_ncurses();
    
    return 0;
}

