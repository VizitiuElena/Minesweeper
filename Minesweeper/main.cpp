#include<GL/glut.h>
#include<iostream>
#include<time.h>
#include<stdlib.h>
#include<iomanip>


using namespace std;
enum {FLAG=-1,UNCOVER,COVER,};
enum {MINE=-1,BLANK,ADJACENT};
enum {LOSE=-1,UNKNOWN,WIN};

const int dim=10,mines=10;

const int max_x=100,max_y=100,xint=max_x/dim,yint=max_y/dim;

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

cell board[dim][dim];
class Minesweeper
{

    public:
        int flagged,uncovered,covered,status,rezultat;
        time_t start_time,end_time,total_time;
        int poz_m[mines][2],nr_mines;
        Minesweeper()
        {
            flagged=0;
            uncovered=0;
            covered=dim*dim;
            status=1;
            rezultat=UNKNOWN;
            start_time=time(NULL);
            nr_mines=0;
            for(int i=0;i<mines;i++)
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
