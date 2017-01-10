#include<GL/glut.h>
#include<iostream>
#include<time.h>
#include<stdlib.h>
#include<iomanip>


using namespace std;
enum {FLAG=-1,UNCOVER,COVER,};
enum {MINE=-1,BLANK,ADJACENT};
enum {LOSE=-1,UNKNOWN,WIN};


class cell
{
    public :
    int content ;
    int status;
    cell()
    {
        content=BLANK;status=COVER;
    }
};

cell board[10][10];
class Minesweeper
{

    public:
        int flagged,uncovered,covered,status,rezultat;
        time_t start_time,end_time,total_time;
        int poz_m[10][2],nr_mines;
        Minesweeper()
        {
            flagged=0;
            uncovered=0;
            covered=100;
            status=1;
            rezultat=UNKNOWN;
            start_time=time(NULL);
            nr_mines=0;
            for(int i=0;i<10;i++)
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
            if(covered==0&&flagged==mines)
            {
                status=0;
                rezultat=WIN;
                end_time=time(NULL);
                total_time=end_time-start_time;
                cout<<"\""<<total_time<<"\"";
                return WIN;
            }

            for(int i=0;i<mines;i++)
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

void bombe_random()
{
    srand(time(NULL));
    int x=0,y=0,nrbombe=0;
    while(nrbombe!=10)
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
        if(game_stats.flagged<10)
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

