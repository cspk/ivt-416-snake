#include <stdlib.h>
#include <ncurses.h>
#include <time.h>

#define FOOD '@'
#define SNAKE '#'
#define RIP 'X'
#define WALL 'W'

#define INIT_SNAKE_LENGTH 5
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

typedef struct Snake
{
    int head_row, head_col;
    int tail_row, tail_col;
} Snake;

Snake *body;

int snake_draw_length = 0;
int snake_length = INIT_SNAKE_LENGTH;

int direction;

int food_row, food_col;

bool game_over = false;

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
    mvaddch(row, col, ch);
    refresh();
}

void erase_char(int row, int col) {
    mvaddch(row, col, ' ');
    refresh();
}

char get_char(int row, int col) {
    return mvinch(row, col);
}

void init_board() {
    for(int col = 0; col < COLS; col++) {
        draw_char(0, col, WALL);
        draw_char(LINES - 1, col, WALL);
    }
    for(int row = 0; row < LINES; row++) {
        draw_char(row, 0, WALL);
        draw_char(row, COLS - 1, WALL);
    }
}

void draw_food() {
    food_row = 2 + rand() % (LINES - 1);
    food_col = 2 + rand() % (COLS - 1);
    draw_char(food_row, food_col, FOOD);
}

void init_snake() {
    body = malloc((LINES - 2) * (COLS - 1) * sizeof(Snake));
    body[0].head_row = LINES / 2;
    body[0].head_col = COLS / 2;
    
    direction = rand() % 4;

    draw_char(body[0].head_row, body[0].head_col, SNAKE);

    snake_draw_length++;
}

void change_tail() {
   for(int i = snake_draw_length; i > 0; i--) {
       body[i].head_row = body[i - 1].head_row;
       body[i].head_col = body[i - 1].head_col;
       body[i - 1].tail_row = body[i - 1].head_row;
       body[i - 1].tail_col = body[i - 1].head_col;
   }
}

void change_head() {
    switch(direction) {
        case LEFT:
            body[0].head_col--;
            break;
            
        case RIGHT:
            body[0].head_col++;
            break;
            
        case UP:
            body[0].head_row--;
            break;
            
        case DOWN:
            body[0].head_row++;
            break;
    }

    if(get_char(body[0].head_row, body[0].head_col) != SNAKE) {
        draw_char(body[0].head_row, body[0].head_col, SNAKE);
    }
    else {
        game_over = true;
        draw_char(body[0].head_row, body[0].head_col, RIP);
    }
}

int verify_wall() {
    if (body[0].head_row == 0 || body[0].head_col == 0
    || body[0].head_row == LINES - 1 || body[0].head_col == COLS - 1) {
        draw_char(body[0].head_row, body[0].head_col, RIP);
        return 1;
    }
    return 0;
}

void snake_logic() {
    change_tail();
    change_head();

    if (verify_wall()) {
        game_over = true;
    }

    if(game_over) {
        return;
    }

    if (body[0].head_row == food_row && body[0].head_col == food_col) {
        snake_length += SNAKE_INCREASE;
        draw_food();
    }
    
    if(snake_draw_length <= snake_length) {
        snake_draw_length++;
    }
    else {
        erase_char(body[snake_draw_length - 1].tail_row, body[snake_draw_length - 1].tail_col);
    }
}

void restart_game() {
    snake_length = INIT_SNAKE_LENGTH;
    snake_draw_length = 1;
    body[0].head_row = LINES / 2;
    body[0].head_col = COLS / 2;
    direction = rand() % 4;
    game_over = false;
    clear();
    init_board();
    refresh();
    draw_char(body[0].head_row, body[0].head_col, SNAKE);
    draw_food();
}

void change_direction() {
    int keypressed = getch();
    switch (keypressed) {
        case KEY_LEFT:
            if(direction != RIGHT) {
               direction = LEFT; 
            }
            break;
        
        case KEY_RIGHT:
            if(direction != LEFT) {
               direction = RIGHT; 
            }
            break;
        
        case KEY_UP:
            if(direction != DOWN) {
               direction = UP; 
            }
            break;
        
        case KEY_DOWN:
            if(direction != UP) {
               direction = DOWN; 
            }
            break;

        case KEY_ESC:
            finish_ncurses();
            break;

        case KEY_R_LOWER:
            restart_game();
            break;
        
        case KEY_R_UPPER:
            restart_game();
            break;
    }
}

int main() {
    srand((unsigned) time(NULL));
    init_ncurses();
    init_board();
    draw_food();
    init_snake();
    while(true) {
        change_direction();
        napms(FRAME_SPEED);
        if(!game_over) {
            snake_logic();
        }
    }
    finish_ncurses();
    
    return 0;
}