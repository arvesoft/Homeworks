#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define GAP -2
#define MATCH 4
#define MISMATCH -3

#define MAXLENGTH 4200

int max(int A, int B, int C)
{
    if (A>B && A>C) return A;
    else if (B>A && B>C) return B;
    else return C;
}

char Tmax(int A, int B, int C)
{
    if (A>B && A>C) return 'D';
    else if (B>A && B>C) return 'L';
    else return 'U';
}

int m(char p, char q)
{
    if (p==q) return MATCH;
    else return MISMATCH;
}

void append(char *st,int L,char c)
{
     int i;
     for (i=L;i>0;i--)
         st[i]=st[i-1];
     st[L+1] = '\0';
     st[0] = c;
}

int main(int argc, char **argv)
{
    FILE *fp, *sp;

    char A[1000];
    char B[5000];
    char RA[1000];
    char RM[1000];
    char RB[1000];
    char ch;
    int N,M,L;
    size_t n = 0;

    int i, j, line_count = 0;

    int S[50][MAXLENGTH];
    char T[50][MAXLENGTH];

    if (argc!=3)
    {
                printf("Usage: align <input file>\n");
                exit(1);
    }

    fp = fopen(argv[1],"r");
    sp = fopen(argv[2],"r");


    while ((ch = fgetc(sp)) != EOF){
        if(ch != '\n')
            B[n++] = (char) ch;
    }

    printf("Sequence B: %s\n", B);

    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int cnt = 0;

    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("Retrieved line of length %zu:\n", read);
        //printf("%s", line);

        //printf("Sequence B: %s\n",B);


        N = strlen(line);
        line[N - 1] = '\0';
        //printf("Sequence A: %s\n",line);
        M = strlen(B);
        S[0][0] = 0;
        T[0][0] = 'D';
        for (i=0;i<=N;i++)
        {
            S[i][0] = 0;
            T[i][0] = 'U';
        }
        for (i=0;i<=M;i++)
        {
            S[0][i] = 0;
            T[0][i] = 'L';
        }

        for (i=1;i<=N;i++)
            for (j=1;j<=M;j++)
            {
                S[i][j] = max(S[i-1][j-1]+m(line[i-1],B[j-1]),S[i][j-1]+GAP,S[i-1][j]+GAP);
                T[i][j] = Tmax(S[i-1][j-1]+m(line[i-1],B[j-1]),S[i][j-1]+GAP,S[i-1][j]+GAP);
            }


        if(S[N][M] >= 30){
            printf("The score of the alignment is : %d\n\n",S[N][M]);
            cnt++;
        }

        memset(S, 0, sizeof(S));
        memset(T, 0, sizeof(T));
    }

    printf("+30 = %d\n",cnt );

    /*

    i=N;
    j=M;
    L=0;
    RA[0]='\0';
    RB[0]='\0';
    RM[0]='\0';

    while (i!=0 || j!=0)
    {
          if (T[i][j]=='D')
          {
             append(RA,L,A[i-1]);
             append(RB,L,B[j-1]);
             if (A[i-1]==B[j-1]) append(RM,L,'|');
             else append(RM,L,'*');
             i--; j--;
          }
          else if (T[i][j]=='L')
          {
             append(RA,L,'-');
             append(RB,L,B[j-1]);
             append(RM,L,' ');
             j--;
          }
          else if (T[i][j]=='U')
          {
             append(RA,L,A[i-1]);
             append(RB,L,'-');
             append(RM,L,' ');
             i--;
          }

          L++;

    }

    printf("%s\n",RA);
    printf("%s\n",RM);
    printf("%s\n",RB);*/
}
