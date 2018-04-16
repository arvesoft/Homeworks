#include "do_not_submit.h"
#include <pthread.h>
#include <semaphore.h>

sem_t semaphore[GRIDSIZE * GRIDSIZE];

struct anttype {

  int x,y,id;

};


void *func(void *ant){

  struct anttype *my_ant = (struct anttype*)ant;
  char mystate = '1';
  int x = my_ant->x;
  int y = my_ant->y;

  printf("Thread ID: %d x = %d y = %d\n", my_ant->id, my_ant->x, my_ant->y);

  while(1){
    //usleep(1000 * getDelay());
    printf("whle\n");
    printf("x = %d y = %d\n", x, y);
    int control = 1;
      if(mystate == '1'){
        // Bende yemek yoksa ve solumda yemek varsa
        if(x - 1 >= 0){
          printf("sem1 kitleyecek\n");
          sem_wait(&semaphore[ (y) * GRIDSIZE + x - 1]);
          if(lookCharAt(y, x-1) == 'o'){
            putCharTo(y, x-1, 'P');
            putCharTo(y , x, '-');
            mystate = 'P';
            x = x - 1;
            control = 0;
          }sem_post(&semaphore[ (y) * GRIDSIZE + x - 1]);
        }

        // Bende yemek yoksa ve sagimda yemek varsa
        if(x + 1 < GRIDSIZE){
          printf("sem2 kitleyecek\n");
          sem_wait(&semaphore[ (y) * GRIDSIZE + x + 1]);
          printf("sem2 kitledi\n");
          if(lookCharAt(y, x+1) == 'o'){
            putCharTo(y, x+1, 'P');
            putCharTo(y , x, '-');
            mystate = 'P';
            x = x + 1;
            control = 0;
            printf("girmemeli\n");
          }
          printf("sem2\n");
          sem_post(&semaphore[ (y) * GRIDSIZE + x + 1]);
        }


        // Bende yemek yoksa ve ustumde yemek varsa
        if(y - 1 >= 0){
          printf("sem3 kitleyecek\n");
          sem_wait(&semaphore[ (y - 1) * GRIDSIZE + x]);
          if(lookCharAt(y-1, x) == 'o'){
            putCharTo(y-1, x, 'P');
            putCharTo(y , x, '-');
            mystate = 'P';
            y = y - 1;
            control = 0;
          }sem_post(&semaphore[ (y - 1) * GRIDSIZE + x]);
        }

        // Bende yemek yoksa ve altimda yemek varsa
        if(y + 1 < GRIDSIZE){
          printf("sem4 kitleyecek\n");
          sem_wait(&semaphore[ (y + 1) * GRIDSIZE + x]);
          if(lookCharAt(y+1, x) == 'o'){
            putCharTo(y+1, x, 'P');
            putCharTo(y , x, '-');
            mystate = 'P';
            y = y + 1;
            control = 0;
          }sem_post(&semaphore[ (y + 1) * GRIDSIZE + x]);
        }


        // Bende yemek yoksa ve sol ustumde yemek varsa
        if(x - 1 >= 0 && y - 1 >= 0){
          printf("sem5 kitleyecek\n");
          sem_wait(&semaphore[ (y - 1) * GRIDSIZE + x - 1]);
          if(lookCharAt(y - 1, x - 1) == 'o'){
            putCharTo(y - 1, x - 1, 'P');
            putCharTo(y , x, '-');
            mystate = 'P';
            x = x - 1;
            y = y - 1;
            control = 0;
          }sem_post(&semaphore[ (y - 1) * GRIDSIZE + x - 1]);
        }


        // Bende yemek yoksa ve sag ustumde yemek varsa
        if(x + 1 < GRIDSIZE & y - 1 >= 0){
          printf("sem6 kitleyecek\n");
          sem_wait(&semaphore[ (y - 1) * GRIDSIZE + x + 1]);
          if(lookCharAt(y-1, x+1) == 'o'){
            putCharTo(y-1, x+1, 'P');
            putCharTo(y , x, '-');
            mystate = 'P';
            x = x + 1;
            y = y - 1;
            control = 0;
          }sem_post(&semaphore[ (y - 1) * GRIDSIZE + x + 1]);
        }


        // Bende yemek yoksa ve sol altimda yemek varsa
        if(x - 1 >= 0 && y + 1 < GRIDSIZE){
          printf("sem7 kitleyecek\n");
          sem_wait(&semaphore[ (y + 1) * GRIDSIZE + x - 1]);
          if(lookCharAt(y+1, x-1) == 'o'){
            putCharTo(y+1, x-1, 'P');
            putCharTo(y , x, '-');
            mystate = 'P';
            x = x - 1;
            y = y + 1;
            control = 0;
          }sem_post(&semaphore[ (y + 1) * GRIDSIZE + x - 1]);
        }

        // Bende yemek yoksa ve sag altimda yemek varsa
        if(x + 1 < GRIDSIZE && y + 1 < GRIDSIZE){
          printf("sem8 kitleyecek\n");
          sem_wait(&semaphore[ (y + 1) * GRIDSIZE + x + 1]);
          if(lookCharAt(y + 1, x + 1) == 'o'){
            putCharTo(y + 1, x + 1, 'P');
            putCharTo(y , x, '-');
            mystate = 'P';
            x = x + 1;
            y = y + 1;
            control = 0;
          }sem_post(&semaphore[ (y + 1) * GRIDSIZE + x + 1]);
        }

        if(control){ //Etrafimda yemek yok hareket etmem lazim
          printf("else\n");
            int random = rand() % 8;
            random = 0;
            int checker = 1;


            for(int i = 0 ; i < 8 && checker; i++, random++){

              // Etrafimda yemek yok, sola bakiyorum bos mu diye
              if(x - 1 >= 0 && (random % 8 == 0)){
                sem_wait(&semaphore[ (y) * GRIDSIZE + x - 1]);
                if(lookCharAt(y, x-1) == '-'){
                  putCharTo(y, x-1, '1');
                  putCharTo(y , x, '-');
                  x = x - 1;
                  checker = 0;
                  printf("girmeli\n");
                }sem_post(&semaphore[ (y) * GRIDSIZE + x - 1]);
              }

              // Etrafimda yemek yok, saga bakiyorum bos mu diye
              else if(x + 1 < GRIDSIZE && (random % 8 == 1)){
                sem_wait(&semaphore[ (y) * GRIDSIZE + x + 1]);
                if(lookCharAt(y, x+1) == '-'){
                  putCharTo(y, x+1, '1');
                  putCharTo(y , x, '-');
                  x = x + 1;
                  checker = 0;
                }sem_post(&semaphore[ (y) * GRIDSIZE + x + 1]);
              }


              // Etrafimda yemek yok, uste bakiyorum bos mu diye
              else if(y - 1 >= 0 && (random % 8 == 2)){
                sem_wait(&semaphore[ (y - 1) * GRIDSIZE + x]);
                if(lookCharAt(y-1, x) == '-'){
                  putCharTo(y-1, x, '1');
                  putCharTo(y , x, '-');
                  y = y - 1;
                  checker = 0;
                }sem_post(&semaphore[ (y - 1) * GRIDSIZE + x]);
              }


              // Etrafimda yemek yok, alta bakiyorum bos mu diye
              else if(y + 1 < GRIDSIZE && (random % 8 == 3)){
                sem_wait(&semaphore[ (y + 1) * GRIDSIZE + x]);
                if(lookCharAt(y+1, x) == '-'){
                  putCharTo(y+1, x, '1');
                  putCharTo(y , x, '-');
                  y = y + 1;
                  checker = 0;
                }sem_post(&semaphore[ (y + 1) * GRIDSIZE + x]);
              }

              // Etrafimda yemek yok, sol uste bakiyorum bos mu diye
              else if(x - 1 >= 0 && y - 1 >= 0 && (random % 8 == 4)){
                sem_wait(&semaphore[ (y - 1) * GRIDSIZE + x - 1]);
                if(lookCharAt(y-1, x-1) == '-'){
                  putCharTo(y - 1, x - 1, '1');
                  putCharTo(y , x, '-');
                  x = x - 1;
                  y = y - 1;
                  checker = 0;
                }sem_post(&semaphore[ (y - 1) * GRIDSIZE + x - 1]);
              }

              // Etrafimda yemek yok, sag uste bakiyorum bos mu diye
              else if(x + 1 < GRIDSIZE && y - 1 >= 0 && (random % 8 == 5)){
                sem_wait(&semaphore[ (y - 1) * GRIDSIZE + x + 1]);
                if(lookCharAt(y-1, x+1) == '-'){
                  putCharTo(y-1, x+1, '1');
                  putCharTo(y , x, '-');
                  x = x + 1;
                  y = y - 1;
                  checker = 0;
                }sem_post(&semaphore[ (y - 1) * GRIDSIZE + x + 1]);
              }

              // Etrafimda yemek yok, sol alta bakiyorum bos mu diye
              else if(x - 1 >= 0 && y + 1 < GRIDSIZE && (random % 8 == 6)){
                sem_wait(&semaphore[ (y + 1) * GRIDSIZE + x - 1]);
                if(lookCharAt(y+1, x-1) == '-'){
                  putCharTo(y+1, x-1, '1');
                  putCharTo(y , x, '-');
                  x = x - 1;
                  y = y + 1;
                  checker = 0;
                }sem_post(&semaphore[ (y + 1) * GRIDSIZE + x - 1]);
              }

              // Etrafimda yemek yok, sag alta bakiyorum bos mu diye
              else if(x + 1 < GRIDSIZE && y + 1 < GRIDSIZE && (random % 8 == 7)){
                sem_wait(&semaphore[ (y + 1) * GRIDSIZE + x + 1]);
                if(lookCharAt(y + 1, x + 1) == '-'){
                  putCharTo(y + 1, x + 1, '1');
                  putCharTo(y , x, '-');
                  x = x + 1;
                  y = y + 1;
                  checker = 0;
                }sem_post(&semaphore[ (y + 1) * GRIDSIZE + x + 1]);
              }
            }
        }
      }
    }
  }


int a,b;
int main(int argc, char *argv[]) {
    srand(time(NULL));

    int semmax = GRIDSIZE * GRIDSIZE;

    for(int i = 0; i < semmax; i++)
      sem_init(&semaphore[i], 0, 1);

    int numberofants = atoi(argv[1]);
    int numberoffoods = atoi(argv[2]);
    int maxsim = atoi(argv[3]);
    struct anttype *antstruct;
    antstruct = malloc(sizeof(struct anttype));

    //printf("%d %d %d\n", numberofants, numberoffoods, maxsim);
    //////////////////////////////
    // Fills the grid randomly to have somthing to draw on screen.
    // Empty spaces have to be -.
    // You should get the number of ants and foods from the arguments
    // and make sure that a food and an ant does not placed at the same cell.
    // You must use putCharTo() and lookCharAt() to access/change the grid.
    // You should be delegating ants to separate threads
    int i,j;
    pthread_t ant;
    for (i = 0; i < GRIDSIZE; i++) {
        for (j = 0; j < GRIDSIZE; j++) {
            putCharTo(i, j, '-');
        }
    }


    for (i = 0; i < numberoffoods; i++) {
        do {
            a = rand() % GRIDSIZE;
            b = rand() % GRIDSIZE;
        }while (lookCharAt(a,b) != '-');
        putCharTo(a, b, 'o');
    }


    for (i = 0; i < numberofants; i++) {
        do {
          a = rand() % GRIDSIZE;
          b = rand() % GRIDSIZE;
        }while (lookCharAt(a,b) != '-');
        putCharTo(a, b, '1');
        antstruct->x = b;
        antstruct->y = a;
        antstruct->id = i;
        pthread_create(&ant , NULL, func, (void*)antstruct);
    }


    //printf("ant.x = %d ant.y = %d ant.id = %d\n",antstruct.x,antstruct.y,antstruct.id);
    //////////////////////////////
    // you have to have following command to initialize ncurses.
    //startCurses();

    // You can use following loop in your program. But pay attention to
    // the function calls, they do not have any access control, you
    // have to ensure that.
    char c;
    while (TRUE) {
    /*    drawWindow();

        c = 0;
        c = getch();

        if (c == 'q' || c == ESC) break;
        if (c == '+') {
            setDelay(getDelay()+10);
        }
        if (c == '-') {
            setDelay(getDelay()-10);
        }
        if (c == '*') {
            setSleeperN(getSleeperN()+1);
        }
        if (c == '/') {
            setSleeperN(getSleeperN()-1);
        }
        usleep(DRAWDELAY);*/

        // each ant thread have to sleep with code similar to this
        //usleep(getDelay() * 1000 + (rand() % 5000));
    }

    // do not forget freeing the resources you get
    //endCurses();

    return 0;
}
