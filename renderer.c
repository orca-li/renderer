#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#if defined(_WIN32)
#include "windows.h"
#define rsleep(_ms) Sleep(_ms)
#define sclear() WIN_ClearScreen();

unsigned char
WIN_ClearScreen(void)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    if (hConsole == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "INVALID_HANDLE_VALUE\n");
        return 1;
    }

    COORD coord = {0, 0};
    DWORD dwCount;
    if (!FillConsoleOutputCharacter(hConsole, ' ', coord.X * coord.Y, coord, &dwCount)) {
        fprintf(stderr, "INVALID_CLEAR\n");
        return 1;
    }

    SetConsoleCursorPosition(hConsole, coord);

    return 0;
}

#else
#define rsleep(_ms) usleep(atoi(#_ms ## "000"))
#define sclear() system("clear");
#endif

#define XNUMS 40
#define YNUMS 30
#define ZNUMS 20
#define PI_8 3.14159265
#define BSIZE 20

typedef double angle;
typedef struct struct_point{
    double x;
    double y;
    double z;
} point;
typedef struct struct_index{
    int x;
    int y;
} idisplay;

                        /* 0. Оглавление */
/**
 * 0. Оглавление
 * 1. Функции дисплея 
 * 2. Функции рисования
 * 3. Другие функции
*/

                        /* 1. Функции дисплея */
void new_frame();                                           // Очистить дисплей для записи нового изображения
void move_point();                                          // Записать точку на дисплей
void show_display();                                        // Показать дисплей

                        /*  2. функции рисования */
/**
 * angle alpha - угол между абсциссой(x) и ординатой(y)
 * angle beta - угол между абсциссой(x) и аппликатом(z)
 * angle gamma - угол между аппликатом(z) и ординатой(y)
*/
void align(point);                                                          // Выравнить точку по центру дисплея
point rotate(point, angle alpha, angle beta, angle gamma);                  // Повернуть точки относительно центра дисплея
void draw_line(point, int length, angle alpha, angle beta, angle gamma);    // Нарисовать линию определенную параметрами(длина, углы наклонов)
void point_offset(int x, int y);                                            // Смещение центра оси

                        /*  3. Другие функции */
double in_radian(double degrees);                           // Перевод градусов в радианы

int display[XNUMS][YNUMS];
idisplay element, offset;

void show_display(){
    for(int y = 0; y < YNUMS; y++){
        for(int x = 0; x < XNUMS; x++)
            printf(display[x][y] == 0x3a ? "%c ": "%c ", display[x][y]);
        putchar('\n');
    }
}

void move_point(){
    for(int y = 0; y < YNUMS; y++){
        for(int x = 0; x < XNUMS; x++){
            if(element.y + offset.y == y)
                display[x][y] = (element.x + offset.x == x) ? 0x3a : display[x][y]; // 0x3a == ':'
            else
                break;
        }
    }
}

void draw_line(point p, int length, angle alpha, angle beta, angle gamma){
    point temp = p;
    for(int i = 0; i < length; i++){
        temp = rotate(p, alpha, beta, gamma);
        align(temp);
        move_point();
        p.x++;
    }
}

void new_frame(){
    for(int y = 0; y < YNUMS; y++){
        for(int x = 0; x < XNUMS; x++)
            display[x][y] = ' ';
    }
}

void align(point p){
    point center;
    center.x = (XNUMS - (XNUMS % 2)) / 2;
    center.y = ((YNUMS - (YNUMS % 2)) / 2);

    element.x = (int)rint(center.x + p.x);
    element.y = (int)rint(center.y - p.y);
}

point rotate(point p, angle alpha, angle beta, angle gamma){
    point temp;
    temp.x = p.x;
    temp.y = p.y;
    temp.z = p.z;

    if(alpha){
        p.x = temp.x * cos(in_radian(alpha)) - temp.y * sin(in_radian(alpha));
        p.y = temp.x * sin(in_radian(alpha)) + temp.y * cos(in_radian(alpha));
        temp.x = p.x;
        temp.y = p.y;
    }
    
    if(beta){
        p.x = temp.x * cos(in_radian(beta)) + temp.z * sin(in_radian(beta));
        p.z = -1*(temp.x * sin(in_radian(beta))) - temp.z * cos(in_radian(beta));
        temp.x = p.x;
        temp.z = p.z;
    }
    
    if(gamma){
        p.y = temp.y * cos(in_radian(gamma)) - temp.z * sin(in_radian(gamma));
        p.z = temp.y * sin(in_radian(gamma)) + temp.z * cos(in_radian(gamma));
    }
    
    return p;
}

void point_offset(int x, int y){
    offset.x = x;
    offset.y = y;
}

double in_radian(double degrees){
    double radian = (degrees * PI_8) / 180;
    return radian;
}

#define GAMMA 28

int main(int argc, char *argv[]){
    point tungent;
    tungent.x = -7;
    tungent.y = 7;
    tungent.z = 7;

    if (argc < 3) {
        fprintf(stderr, "Syntax:\r\n"
            "\trenderer <image=%%s> <size=%%d>\r\n\r\n"
            "<image>:\r\n\tsqure\r\n\tcube\r\n\r\n"
            "Example\r\n"
            "renderer square 15\r\n"
            "renderer cube 15\r\n");
        return 1;
    }

    int type = 0;
    int size = atoi(argv[2]);

    if (0 == strcmp("square", argv[1])) {
        type = 1;
    } else if (0 == strcmp("cube", argv[1])) {
        type = 2;
    } else {
        fprintf(stderr, "unknown type\r\n");
        return 1;
    }

    for(int i = 0; i < 1440; i += 4) {
        int alpha, beta;
        alpha = beta = i;

    new_frame(); 
    point_offset(1, 0);


    switch (type)
    {
        case 1:
            draw_line(tungent, size, 0 + alpha, -beta, 0);
            draw_line(tungent, size, 90 + alpha, -beta, 0);
            draw_line(tungent, size, 180 + alpha, -beta, 0);
            draw_line(tungent, size, 270 + alpha, -beta, 0);
            break;

        case 2:
            draw_line(tungent, size, 0, beta, GAMMA);
            draw_line(tungent, size, 90, beta, GAMMA);
            draw_line(tungent, size, 180, beta, GAMMA);
            draw_line(tungent, size, 270, beta, GAMMA);

            draw_line(tungent, size, 0, -beta, -GAMMA);
            draw_line(tungent, size, 0, beta + 90, -GAMMA);
            draw_line(tungent, size, 0, -beta + 180, -GAMMA);
            draw_line(tungent, size, 0, beta + 270, -GAMMA);

            draw_line(tungent, size, 180, -beta, -GAMMA);
            draw_line(tungent, size, 180, beta + 90, -GAMMA);
            draw_line(tungent, size, 180, -beta + 180, -GAMMA);
            draw_line(tungent, size, 180, beta + 270, -GAMMA);

            draw_line(tungent, size, 0, 180 + beta, GAMMA);
            draw_line(tungent, size, 90, 180 + beta, GAMMA);
            draw_line(tungent, size, 180, 180 + beta, GAMMA);
            draw_line(tungent, size, 270, 180 + beta, GAMMA);
            break;
        
        default:
            printf("unknown image type\r\n");
            return 1;
        }   

        show_display();
        rsleep(15);
        sclear();
    }

    return 0;
}