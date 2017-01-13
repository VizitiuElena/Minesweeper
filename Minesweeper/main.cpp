#include<GL/glut.h>
#include<iostream>
#include<time.h>
#include<stdlib.h>
#include<iomanip>
#define MINES_NUMBER 20


using namespace std;
enum {FLAG=-1,UNCOVER,COVER,};
enum {MINE=-1,BLANK,ADJACENT};
enum {LOSE=-1,UNKNOWN,WIN};


struct cell
{
    int content=BLANK;
    int status=COVER;
};

cell board[10][10];
class Minesweeper
{

    public:
        int flagged,uncovered,covered,status,rezultat;
        time_t start_time,end_time,total_time;
        int poz_m[MINES_NUMBER][2],nr_mines;
        Minesweeper()
        {
            flagged=0;
            uncovered=0;
            covered=100;
            status=1;
            rezultat=UNKNOWN;
            start_time=time(NULL);
            nr_mines=0;
            for(int i=0;i<MINES_NUMBER;i++)
                for(int j=0;j<2;j++)
                    poz_m[i][j]=-1;
        }

        void add_mine(int x,int y)
        {
            poz_m[nr_mines][0]=x;
            poz_m[nr_mines][1]=y;
            nr_mines++;
        }

        int verif_status()
        {
            if(rezultat!=UNKNOWN)
                return rezultat;
            if(covered==0&&flagged==MINES_NUMBER)
            {
                status=0;
                rezultat=WIN;
                end_time=time(NULL);
                total_time=end_time-start_time;
                cout<<"\""<<total_time<<"\"";
                return WIN;
            }

            for(int i=0;i<MINES_NUMBER;i++)
            {
                if(UNCOVER==board[poz_m[i][0]][poz_m[i][1]].status)
                {
                    status=0;
                    rezultat=LOSE;
                    end_time=time(NULL);
                    total_time=end_time-start_time;
                    cout<<"\""<<total_time<<"\"";
                    return LOSE;
                }
            }
            return UNKNOWN;
        }
};


Minesweeper game_stats;

void initializare_joc();
void draw_board();
void bombe_random();
void bombe_adiacente();
void nr_bombe_imprejur(int i,int j);
void uncover_cell(int i, int j);
void uncover_area(int i, int j);
void uncover_area_verif_status(int k, int l);
void left_click(int i,int j);
void right_click(int i,int j);
void game_over(int result);

void draw_square(int x,int y,int color);
void user_input(int button,int state,int x,int y);
void show_content(int i,int j);

void gl_init();
void display();
void printMessage(char *msg);
void printString(char *s);
void makeRasterFont(void);

int main(int argc, char *argv[])
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowSize(400,400);
    glutInitWindowPosition(50,50);
    glutCreateWindow("Minesweeper");
    glutDisplayFunc(display);
    glutMouseFunc(user_input);

    gl_init();
    initializare_joc();

    glutMainLoop();
    return 0;
}


void bombe_random()
{
    srand(time(NULL));
    int x=0,y=0,nrbombe=0;
    while(nrbombe!=MINES_NUMBER)
    {
        x=rand()%10;
        y=rand()%10;

        if(board[x][y].content!=MINE)
        {
            board[x][y].content=MINE;
            nrbombe++;
            game_stats.add_mine(x,y);
        }
    }
}


void bombe_adiacente()
{
    int x,y,nr_bombe_adiacente;
    for(x=0;x<10;x++)
        for(y=0;y<10;y++)
            if(board[x][y].content!=MINE)
            {
                nr_bombe_imprejur(x,y);
                nr_bombe_adiacente++;
            }
}


void nr_bombe_imprejur(int line,int column)
{
    int lin,col;
    int dl[]={-1,-1,-1,0,1,1,1,0};
    int dc[]={-1,0,1,1,1,0,-1,-1};
    for(int i=0;i<8;i++)
    {
        lin=line+dl[i];
        col=column+dc[i];
        if(lin>=0&&lin<10&&col>=0&&col<10)
           if(board[lin][col].content==MINE)
                board[line][column].content++;
    }

}

void initializare_joc()
{
    bombe_random();
    bombe_adiacente();
    draw_board();
}

void uncover_cell(int x , int y)
{
    show_content(x,y);
    if(board[x][y].content==BLANK)
        uncover_area(x,y);
}


void uncover_area_verif_status(int x, int y)
{
    if(board[x][y].status==COVER)
    {
        if(board[x][y].content!=MINE)
        {
            board[x][y].status=UNCOVER;
            game_stats.covered--;
            game_stats.uncovered++;
            show_content(x,y);
            if(board[x][y].content==BLANK)
                uncover_area(x,y);
        }
    }
}


void uncover_area(int x, int y)
{
    if(x-1>=0)
        uncover_area_verif_status(x-1,y);
    if(x+1<10)
        uncover_area_verif_status(x+1,y);
    if(y-1>=0)
        uncover_area_verif_status(x,y-1);
    if(y+1<10)
        uncover_area_verif_status(x,y+1);
}

void left_click(int x,int y)
{
    if(board[x][y].status==COVER)
    {
        if(UNKNOWN!=game_stats.verif_status())
        {
            game_over(game_stats.verif_status());
            return;
        }
        board[x][y].status=UNCOVER;
        game_stats.covered--;
        game_stats.uncovered++;
        draw_square(x,y,UNCOVER);
        uncover_cell(x,y);
    }
}


void right_click(int x, int y)
{
    if(UNKNOWN!=game_stats.verif_status())
        {
            game_over(game_stats.verif_status());
            return;
        }
    if(board[x][y].status==FLAG)
    {
        board[x][y].status=COVER;
        game_stats.flagged--;
        game_stats.covered++;
    }
    else if(board[x][y].status==COVER)
    {
        if(game_stats.flagged<MINES_NUMBER)
        {
            board[x][y].status=FLAG;
            game_stats.covered--;
            game_stats.flagged++;

        }
        else
            printMessage("NO MORE FLAGS!");
    }
    draw_square(x,y,board[x][y].status);
}


void game_over(int rezultat)
{
    if(rezultat!=UNKNOWN)
    {
        glutMouseFunc(NULL);
        if(rezultat==WIN)
            printMessage("YOU WIN!");
        else
            printMessage("YOU LOSE!");
    }
}

/**Grafica**/

void user_input(int button,int state,int x,int y)
{

    int square_x=x/40;
    int square_y=(400-y)/40;
    if(button==GLUT_LEFT_BUTTON&&state==GLUT_DOWN)
        left_click(square_x,square_y);
else if(button==GLUT_RIGHT_BUTTON&&state==GLUT_UP)
        right_click(square_x,square_y);

}


void draw_board()
{
    int x_coord=0,y_coord=0;
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINES);
    for(x_coord=0;x_coord<=100;x_coord+=10)
    {
        glVertex2i(x_coord,0);
        glVertex2i(x_coord,100);
    }
    for(y_coord=0;y_coord<=100;y_coord+=10)
    {
        glVertex2i(0,y_coord);
        glVertex2i(100,y_coord);
    }
    for(int i=0;i<10;i++)
    {
        for(int j=0;j<10;j++)
        {
            draw_square(i,j,board[i][j].status);
            if(board[i][j].status==UNCOVER)
                show_content(i,j);
        }
    }

    glEnd();
    glFlush();
}


void draw_square(int i,int j,int color)
{
    int x,y;
    if(color==FLAG)
        glColor3f(0.8,0,0);
    else if(color==COVER)
        glColor3f(0.5,0.1,0.3);
    else if(color==UNCOVER)
        glColor3f(0,0.5,0.5);
    else glColor3f(1,1,1);
    x=i*10;
    y=j*10;

    glBegin(GL_POLYGON);
    glVertex2i(x+1,y+1);
    glVertex2i(x+1,y+10-1);
    glVertex2i(x+10-1,y+10-1);
    glVertex2i(x+10-1,y+1);

    glEnd();
    glFlush();
}


void printMessage(char *msg)
{
    glColor3f(255.0,255.0,255.0);
    glRasterPos2i(10*4,10*4);
    printString(msg);
    glFlush();
}


void show_content(int i,int j)
{
    char element=board[i][j].content+48;
    if(board[i][j].content==MINE)
        element=66; //B
    if(board[i][j].content==BLANK)
        element=32; //spatiu
    glColor3f(0.0,0.0,0.0);
    glRasterPos2i(i*10+4,j*10+4);
    printString(&element);
    glFlush();

}


void gl_init()
{
    glClearColor(0.0,0.0,0.0,0);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glOrtho(0.0,100.0,0.0,100.0,-100.0,100.0);
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
    makeRasterFont();

}


void display()
{
    draw_board();
    game_over(game_stats.verif_status());

}

/**font**/

GLubyte space[] =
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
GLubyte letters[][13] = {
    {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0x66, 0x3c, 0x18},
    {0x00, 0x00, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe},
    {0x00, 0x00, 0x7e, 0xe7, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e},
    {0x00, 0x00, 0xfc, 0xce, 0xc7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc7, 0xce, 0xfc},
    {0x00, 0x00, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0, 0xc0, 0xc0, 0xff},
    {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0, 0xc0, 0xff},
    {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xcf, 0xc0, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e},
    {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
    {0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e},
    {0x00, 0x00, 0x7c, 0xee, 0xc6, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06},
    {0x00, 0x00, 0xc3, 0xc6, 0xcc, 0xd8, 0xf0, 0xe0, 0xf0, 0xd8, 0xcc, 0xc6, 0xc3},
    {0x00, 0x00, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0},
    {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xdb, 0xff, 0xff, 0xe7, 0xc3},
    {0x00, 0x00, 0xc7, 0xc7, 0xcf, 0xcf, 0xdf, 0xdb, 0xfb, 0xf3, 0xf3, 0xe3, 0xe3},
    {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xe7, 0x7e},
    {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe},
    {0x00, 0x00, 0x3f, 0x6e, 0xdf, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x66, 0x3c},
    {0x00, 0x00, 0xc3, 0xc6, 0xcc, 0xd8, 0xf0, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe},
    {0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x07, 0x7e, 0xe0, 0xc0, 0xc0, 0xe7, 0x7e},
    {0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xff},
    {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
    {0x00, 0x00, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
    {0x00, 0x00, 0xc3, 0xe7, 0xff, 0xff, 0xdb, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
    {0x00, 0x00, 0xc3, 0x66, 0x66, 0x3c, 0x3c, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3},
    {0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3},
    {0x00, 0x00, 0xff, 0xc0, 0xc0, 0x60, 0x30, 0x7e, 0x0c, 0x06, 0x03, 0x03, 0xff}
};
GLubyte digits[][13] = {
    {0xff, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xff},
    {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},
    {0xff, 0x80, 0x80, 0x80, 0x80, 0x80, 0xff, 0x01, 0x01, 0x01, 0x01, 0x01, 0xff},
    {0xff, 0x01, 0x01, 0x01, 0x01, 0x01, 0xff, 0x01, 0x01, 0x01, 0x01, 0x01, 0xff},
    {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xff, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81},
    {0xff, 0x01, 0x01, 0x01, 0x01, 0x01, 0xff, 0x80, 0x80, 0x80, 0x80, 0x80, 0xff},
    {0xff, 0x81, 0x81, 0x81, 0x81, 0x81, 0xff, 0x80, 0x80, 0x80, 0x80, 0x80, 0xff},
    {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xff},
    {0xff, 0x81, 0x81, 0x81, 0x81, 0x81, 0xff, 0x81, 0x81, 0x81, 0x81, 0x81, 0xff},
    {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xff, 0x81, 0x81, 0x81, 0x81, 0x81, 0xff}
    };
GLuint fontOffset;

void makeRasterFont(void)
{
   GLuint i, j;
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   fontOffset = glGenLists (128);
   for (i = 0,j = 'A'; i < 26; i++,j++) {
      glNewList(fontOffset + j, GL_COMPILE);
      glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, letters[i]);
      glEndList();
   }
   for (i = 0,j = '0'; i < 10; i++,j++) {
      glNewList(fontOffset + j, GL_COMPILE);
      glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, digits[i]);
      glEndList();
   }
   glNewList(fontOffset + ' ', GL_COMPILE);
   glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, space);
   glEndList();
}

void printString(char *s)
{
   glPushAttrib (GL_LIST_BIT);
   glListBase(fontOffset);
   glCallLists(strlen(s), GL_UNSIGNED_BYTE, (GLubyte *) s);
   glPopAttrib ();
}


