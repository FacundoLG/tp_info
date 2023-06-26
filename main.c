#include <stdio.h>
#include <termcap.h>
#include <windows.h>
#include <conio.h>


struct Ui {
    int col_size;
    int row_size;
};
typedef struct Ui UI;
enum Keys {
    LEFT_KEY = 'a',
    RIGHT_KEY = 'd',
    UP_KEY = 'w',
    DOWN_KEY = 's',
    EXIT_KEY = 'c',
};
typedef enum Keys KEY;

struct Cursor {
    int x_pos;
    int y_pos;
    char direction; // 'h' Horizontal / 'v' Vertical
};
typedef struct Cursor CURSOR;


struct Figure {
    int x_pos[100];
    int y_pos[100];
    int count;
};
typedef struct Figure FIGURE;


CURSOR cursor;
UI ui;
FIGURE figure = {
    .count = 0
};

void load_terminal_data(UI * ui);
char* format_keys();
void clear_terminal();
void calculate_area(FIGURE figure);
void add_dot(int y,int x,FIGURE * figure);

void Exit();
void moveLeft();
void moveRight();
void moveUp();
void moveDown();

void on_press(char prs_key,char matrix[ui.row_size][ui.col_size]);
void print_matrix(UI ui,char matrix[ui.row_size][ui.col_size]);


int EXIT = FALSE;
int CONTINUE = TRUE;

//Control functions


int main(){
    //Before-Loop
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    //Gets terminal information
    load_terminal_data(&ui);
    //Save original user cursor info
    CONSOLE_CURSOR_INFO original_cursor_info;
    GetConsoleCursorInfo(console,&original_cursor_info);
    //Creates new cursor struct instance
    CONSOLE_CURSOR_INFO cursor_info;
    //Hide cursor
    cursor_info.dwSize = 100;
    cursor_info.bVisible = FALSE;
    SetConsoleCursorInfo(console,&cursor_info);
    
    char * controls = format_keys();
    while(CONTINUE){
        EXIT = FALSE;
        char matrix[ui.row_size][ui.col_size];
        cursor.x_pos=0;
        cursor.y_pos=0;
        cursor.x_pos = ui.col_size/2;
        cursor.y_pos = ui.row_size/2;
        //Clean figure information;
        for(int i = 0;i < figure.count; i++){
            figure.x_pos[i] = 0;
            figure.y_pos[i] = 0;
        }
        figure.count = 0;
        add_dot(cursor.y_pos,cursor.x_pos,&figure);
            
        //UI Loop
        for(int i = 0; i < ui.row_size; i++){
                for(int z = 0;z < ui.col_size;z++){
                    if(i == 0 || i == ui.row_size-1 || z == 0 || z == ui.col_size - 1){
                        matrix[i][z] = '&';
                    } else {
                        matrix[i][z] = ' ';
                    }
                    if(i == cursor.y_pos && z == cursor.x_pos){
                        matrix[i][z] = 'o';
                    }
                }
        }
        while(!EXIT){
            clear_terminal();
            print_matrix(ui,matrix);
            printf("%s",controls);
            char prs_key = getch();
            //Key binding
            on_press(prs_key,matrix);
            if (EXIT && figure.count > 4 && cursor.x_pos == figure.x_pos[0] && cursor.y_pos == figure.y_pos[0]){
                print_matrix(ui,matrix);
                calculate_area(figure);
            }else {
                clear_terminal();
            }
        }
        if(CONTINUE){
            printf("Crear otra figura?(s/n): ");
            char answer;
            scanf(" %c",&answer);
            if(answer != 's'){
                  CONTINUE = FALSE;
            }
        }
    }
    //After Loop
    SetConsoleCursorInfo(console,&original_cursor_info);
    return 0;
}

void load_terminal_data(UI * ui){
    CONSOLE_SCREEN_BUFFER_INFO screen_info;
    HANDLE terminal = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(terminal,&screen_info);
    ui->row_size = screen_info.srWindow.Bottom - screen_info.srWindow.Top - 1 ;
    ui->col_size = (screen_info.srWindow.Right - screen_info.srWindow.Left) / 2;
    if(ui->col_size > 200){
        ui->col_size = 200;
    }
}

char* format_keys(){
    char format[] = "ARRIBA[%c] ABAJO[%c] IZQUIERDA[%c] DERECHA[%c] SALIR[%c]\n";
    static char key_str[sizeof(format)/sizeof(char)];
    sprintf(key_str,format,UP_KEY,DOWN_KEY,LEFT_KEY,RIGHT_KEY,EXIT_KEY);
    return key_str;
}

void clear_terminal(){
    system("clear");
    system("cls");
}
void calculate_area(FIGURE figure){
    int sum = 0;
    for(int i = 0;i < figure.count - 1;i++){
        sum += figure.x_pos[i] * figure.y_pos[i+1];
        sum -= figure.y_pos[i] * figure.x_pos[i+1];
    }
    int area = abs(sum)/2.0;
    printf("El area es: %d\n", area);
}
void add_dot(int y,int x,FIGURE * figure){
    figure->y_pos[figure->count] = y;
    figure->x_pos[figure->count] = x;
    figure->count++;
};

void Exit(){
    EXIT = TRUE;
    CONTINUE = FALSE;
}

void moveLeft(){
    if(cursor.x_pos > 1)
        cursor.x_pos -= 1;
}

void moveRight(){
    if(cursor.x_pos < ui.col_size)
        cursor.x_pos += 1;
}

void moveUp(){
    if(cursor.y_pos > 1)
        cursor.y_pos -= 1;
    
}

void moveDown(){
    if(cursor.y_pos < ui.row_size)
        cursor.y_pos += 1;
}
void on_press(char prs_key,char matrix[ui.row_size][ui.col_size]){
    int last_x_pos = cursor.x_pos;
    int last_y_pos = cursor.y_pos;
    int last_direction = cursor.direction;

    switch (prs_key)
    {
    case UP_KEY:
        moveUp();
        break;
    case DOWN_KEY:
        moveDown();
        break;
    case LEFT_KEY:
        moveLeft();
        break;
    case RIGHT_KEY:
        moveRight();
        break;
    case EXIT_KEY:
        Exit();
    }
    

    int isVertical = last_y_pos != cursor.y_pos;
    int isHorizontal = last_x_pos != cursor.x_pos;

    matrix[cursor.y_pos][cursor.x_pos] = 'o';

    if(isVertical){
        matrix[last_y_pos][last_x_pos] = '|';
        cursor.direction = 'v';
    }
    if(isHorizontal){
        matrix[last_y_pos][last_x_pos] = '-';
        cursor.direction = 'h';
    }
    if((last_direction == 'h' && isVertical)||(last_direction == 'v' && isHorizontal)){
        matrix[last_y_pos][last_x_pos] = '+';
        add_dot(last_y_pos,last_x_pos,&figure);
    }

    matrix[figure.y_pos[0]][figure.x_pos[0]] = 'O'; //Origin

    if(cursor.x_pos == figure.x_pos[0] && cursor.y_pos == figure.y_pos[0]){
            add_dot(figure.y_pos[0],figure.x_pos[0],&figure);
            EXIT = TRUE;
        }
    if(EXIT){
        matrix[figure.y_pos[0]][figure.x_pos[0]] = '+';
    }


}


void print_matrix(UI ui,char matrix[ui.row_size][ui.col_size]){
    for(int i = 0;i < ui.row_size; i++){
        for(int z = 0; z < ui.col_size; z++){
            char fix[4] = {matrix[i][z],' '};
            printf("%s",fix);
        }
        printf("\n");
    }
}
