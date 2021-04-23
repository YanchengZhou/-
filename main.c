//
//  main.c
//  PJ生命游戏
//
//  Created by 周彦铖 on 2020/12/27.
//  Copyright © 2020 Yancheng Zhou. All rights reserved.
//


#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <curses.h>
#include <sys/select.h>


//宏定义基本操作，增强可读性
#define PRINT "\\p"
#define HELP "\\h"
#define LOAD "\\l"
#define SAVE "\\s"
#define DESIGN "\\d"
#define QUIT "\\q"
#define GEN "\\g"
#define RUN "\\r"
#define EXIT "\\e"



//细胞图数组，数组长宽，文件名多个方法都需要调用，使用全局变量
char arr[50][50];
//默认长宽为10
int row = 10;
int col = 10;
char fileRead[20];
char fileWrite[20];


//函数提前声明
void load(void);
void print(void);
void save(void);
void help(void);
void design(void);
void generate(void);
int countAliveNeighbors(int map[row][col], int x, int y);
void run(void);
int _kbhit(void);



int main(){
    printf("*********************************\n");
    printf("*********************************\n\n");
    
    printf("\t\t生   命   游   戏\n\n");
    printf("\t\t  欢迎来到生命游戏\n  在这里您将体会到元胞自动机的乐趣\n\n");
    
    printf("\t\t\t准备好了吗\n\n");
    printf("*********************************\n");
    printf("*********************************\n\n");
    
    
    help();

    char s[20];
    
    //if else选择语句，根据输入决定调用哪个对应函数
    while(1){
        printf("请输入指令: ");
        scanf("%s", s);
        
        //如果是LOAD / SAVE，则再输入一个字符串作为filename
        if(strcmp(s, LOAD) == 0) {
            scanf("%s", fileRead);
            load();
        }
        
        else if(strcmp(s, SAVE) == 0){
            scanf("%s", fileWrite);
            save();
        }
        
        else if(strcmp(s, PRINT) == 0){
            system("clear");
            print();
        }
        else if(strcmp(s, HELP) == 0)
            help();
        else if(strcmp(s, DESIGN) == 0)
            design();
        else if(strcmp(s, GEN) == 0)
            generate();
        else if(strcmp(s, RUN) == 0)
            run();
        else if(strcmp(s, QUIT) == 0)
            printf("还没有进入设计模式，无法退出!\n");
        else if(strcmp(s, EXIT) == 0)
            printf("还没有进入运行模式，无法停止!\n");
        else if(strcmp(s, "end") == 0){
            system("clear");
            printf("Game over!\n");
            exit(0);
        }
        else
            printf("输入不合法!\n");
    }
    
    return 0;
}


void help(){
    system("clear");
    
    printf("\t[\\h]\t打印命令提示\n");
    printf("\t[\\p]\t打印当前地图\n");
    printf("\t[\\l <filename>]\t导入地图\n");
    printf("\t[\\s <filename>]\t保存地图\n");
    printf("\t[\\d]\t进入地图设计模式\n");
    printf("\t[\\q]\t退出地图设计模式\n");
    printf("\t[\\g]\t生成下一代生命\n");
    printf("\t[\\r]\t开始生命游戏\n");
    printf("\t[\\e]\t停止生命游戏\n");
    printf("\t[end]\t退出游戏\n");
}


void print(){
    printf("当前细胞图:\n");
    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            if(arr[i][j] == 1)
                printf("■ ");
            else
                printf("□ ");
        }
        printf("\n");
    }
}



//Xcode把txt文件放在product下terminal(右键show in finder,拖入txt文件)，之后可以被读入
void load(){
    system("clear");
    printf("<已经创建的地图文件: 1.txt, 2.txt, 3.txt>\n");
    FILE* fp=fopen(fileRead, "r");
    if(fp == NULL)
        printf("该文件地址为空，无法导入\n");
    else{
        //先读取第一行的两个数作为row col
        fscanf(fp, "%d", &row);
        fscanf(fp, "%d", &col);
        fscanf(fp, "\n");
        
        int i, j;
        for(i = 0; i < row; i++){
            for(j = 0; j < col; j++){
                fscanf(fp,"%d", &arr[i][j]);
            }

            fscanf(fp, "\n");
        }
        system("clear");
        printf("读取自<%s>:\n ", fileRead);
        print();
    }
}


//写入文件也在debug文件夹内，先写row col，之后写数组
void save(){
    system("clear");
    FILE *fp = fopen(fileWrite, "w");
    
    if(fp == NULL)
        printf("该文件地址为空，无法保存\n");
    else{
        fprintf(fp, "%d ", row);
        fprintf(fp, "%d ", col);
        fprintf(fp, "%c", '\n');
        
        for(int i = 0; i < row; i++){
            for(int j = 0; j < col; j++){
                fprintf(fp, "%d ", arr[i][j]);
            }
            fprintf(fp, "%c", '\n');
        }
    
        fclose(fp);
        
        print();
        
        printf("数据已保存至<%s>\n", fileWrite);
    }
    
}


void design(){
    system("clear");
    printf("进入地图设计模式：<默认初始每个空格为死亡>\n");
    
    printf("输入地图长宽(大于0的整数)<格式例如：'5 4'>: \n");
    
    //如果有数没有读入（不等于2），则重新输入，需要清空缓存区
    while(scanf("%d%d", &row, &col)!=2){
        printf("输入长宽不合法，重新输入：\n");
        fflush(stdin);
    }
    

    //初始化
    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            arr[i][j] = 0;
        }
    }
    
    system("clear");
    print();
    
    
    //同时处理\q和数字两种类型：fgets读取整行，分割字符串，用atoi转换成int类型
    char s[20];
    char s1[10];
    char s2[10];
    
    while(strcmp(s, QUIT) != 0){
        printf("输入存活细胞位置<格式例如：1 1>,输入'%s'退出设计模式:\n", QUIT);
        fflush(stdin);
        fgets(s, 20, stdin);
        
        //分割字符串后分别赋值
        int index = strlen(s);
        for(int i = 0; i < strlen(s); i++){
            if(s[i] == ' '){
                index = i;
                break;
            }
        }
        
        for(int i = 0; i < index; i ++)
            s1[i] = s[i];
       
        
        for(int i = index+1; i < strlen(s); i++)
            s2[i-index-1] = s[i];
        
        int m = -1, n = -1;
        
        //因为atoi转换无效和用户输入0，返回值都为0，所以需要判断用户输入值中是否有0，如果没有才能被判断为格式出错
        int hasZero = 0;
        for(int i = 0; i < strlen(s)-1; i++){
            if((s[i] == '0' && s[i+1] == ' ') || (s[i] == ' ' && s[i+1] == '0')){
                hasZero = 1;
                break;
            }
        }
        
        m = atoi(s1);
        n = atoi(s2);
        
        
        //atoi转换后需要设置最后一位为\0才能参与字符串比较，如果s1 = \q，直接跳出while进入下面专门对\q的判断语句
        s1[strlen(s1)-1] = '\0';
        if(strcmp(s1, QUIT) == 0)
            break;
        
        
        system("clear");
        
        //分出格式错，界限超出，正常赋值三种
        if((m == 0 || n == 0) && hasZero == 0){
            print();
            printf("输入格式出错，重新输入!\n");
            continue;
        }
        
       if(m < 0 || m >= row || n < 0 || n >= col){
           print();
           printf("输入位置超出界限，重新输入!\n");
        }
        
        else{
            arr[m][n] = 1;
            print();
        }
        
    }
    
    if(strcmp(s1, QUIT) == 0){
        system("clear");
        printf("已退出设计模式，请及时保存!\n");
        print();
        printf("输入'%s <filename>'把保存设计地图，输入其他任意值放弃保存:\n", SAVE);
        
        char s2[10];
        scanf("%s", s2);
        if(strcmp(s2, SAVE) == 0){
            scanf("%s", fileWrite);
            system("clear");
            save();
        }
    }
    
    system("clear");
    printf("已退出设计模式\n");
}


void generate(){
    system("clear");
    int aliveNeighbors[row][col];
    //新建一个大小为row col的数组，作为countAliveNeighbors的参数，否则大小不一样会出bug
    int copy[row][col];
    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            copy[i][j] = arr[i][j];
            aliveNeighbors[i][j] = 0;
        }
    }
    
    for(int i = 0; i < row; i++)
        for(int j = 0; j < col; j++){
            aliveNeighbors[i][j] = countAliveNeighbors(copy, i, j);
        }
    
    for(int i = 0; i < row; i++)
        for(int j = 0; j < col; j++){
            if(arr[i][j] == 1){
                if(aliveNeighbors[i][j] < 2 || aliveNeighbors[i][j] > 3)
                    arr[i][j] = 0;
            }
            else{
                if(aliveNeighbors[i][j] == 3)
                    arr[i][j] = 1;
            }
        }

    print();
}

int countAliveNeighbors(int map[][col], int x, int y){
    int count = 0;
    int i, j;
    //根据位置，在3*3范围内循环，如果超出界限不计入count，最后把自己本身位置减掉
    for(i = x-1; i <= x+1; i++){
        for(j = y-1; j <= y+1; j++){
            if(i >= 0 & i < row & j >= 0 & j < col){
                if(map[i][j] == 1)
                    count++;
            }
        }
    }
    
    if(map[x][y] == 1)
        count--;
    
    return count;
}



//mac实现kbhit
int _kbhit()
{
       static const int STDIN = 0;
       static bool initialized = false; if (! initialized)
       {
           // Use termios to turn off line buffering
           struct termios term;
           tcgetattr(STDIN, &term);
           term.c_lflag &= ~ICANON;
           tcsetattr(STDIN, TCSANOW, &term);
           setbuf(stdin, NULL);
           initialized = true;
       } int bytesWaiting;
       ioctl(STDIN, FIONREAD, &bytesWaiting);
       return bytesWaiting;
       
 }


void run(){
    system("clear");
    char buff[10];
    
    while(1){
        generate();
        printf("输入回车键暂停运行\n");
        sleep(2);
      
        if(_kbhit()){
            fgets(buff, 10, stdin);
            printf("输入'%s'停止生命游戏, 再次输入回车继续游戏\n", EXIT);
            fflush(stdin);
            fgets(buff, 10, stdin);
            system("clear");
            
            if(strncmp(buff, EXIT, strlen(EXIT)) == 0){
                printf("生命游戏已停止\n");
                break;
            }
        }
    }
}

