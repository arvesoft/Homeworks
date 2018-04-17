#include "do_not_submit.h"
#include <pthread.h>
#include <semaphore.h>

sem_t semaphore[GRIDSIZE * GRIDSIZE];
sem_t mutex;

struct anttype {

  int x,y,id;

};


void *func(void *ant){

  struct anttype *my_ant = (struct anttype*)ant;
  char mystate = '1';
  int x = my_ant->x;
  int y = my_ant->y;
  int myid = my_ant->id;

  //printf("Thread ID: %d x = %d y = %d\n", my_ant->id, my_ant->x, my_ant->y);

  while(1){
    int randomtime = rand() % 10;

    usleep(1000 * (getDelay() + randomtime));


    int oldx = x, oldy = y;
    //printf("whle\n");
    //printf("x = %d y = %d\n", x, y);
    int control = 1;


      if(myid < getSleeperN()){

        if(mystate == 'P')
          putCharTo(y, x, '$');
        else if(mystate == '1')
          putCharTo(y, x, 'S');
      }


      else if(mystate == 'P'){

        putCharTo(y, x, 'P');
        oldx = x;
        oldy = y;

        int cnt1 = 0, cnt2 = 0, cnt3 = 0, cnt4 = 0, cnt5 = 0, cnt6 = 0, cnt7 = 0, cnt8 = 0;

        sem_wait(&mutex);

        if(x - 1 >= 0){
          sem_wait(&semaphore[ (y) * GRIDSIZE + x - 1]);
          cnt1 = 1;
        }

        if(x + 1 < GRIDSIZE){
          sem_wait(&semaphore[ (y) * GRIDSIZE + x + 1]);
          cnt2 = 1;
        }

        if(y - 1 >= 0){
          sem_wait(&semaphore[ (y - 1) * GRIDSIZE + x]);
          cnt3 = 1;
        }

        if(y + 1 < GRIDSIZE){
          sem_wait(&semaphore[ (y + 1) * GRIDSIZE + x]);
          cnt4 = 1;
        }

        if(x - 1 >= 0 && y - 1 >= 0){
          sem_wait(&semaphore[ (y - 1) * GRIDSIZE + x - 1]);
          cnt5 = 1;
        }

        if(x + 1 < GRIDSIZE & y - 1 >= 0){
          sem_wait(&semaphore[ (y - 1) * GRIDSIZE + x + 1]);
          cnt6 = 1;
        }

        if(x - 1 >= 0 && y + 1 < GRIDSIZE){
          sem_wait(&semaphore[ (y + 1) * GRIDSIZE + x - 1]);
          cnt7 = 1;
        }

        if(x + 1 < GRIDSIZE && y + 1 < GRIDSIZE){
            sem_wait(&semaphore[ (y + 1) * GRIDSIZE + x + 1]);
            cnt8 = 1;
        }

        sem_post(&mutex);

        if(cnt1 && lookCharAt(y, x-1) == 'o'){
          putCharTo(y , x, 'o');
          mystate = '1';
        }

      // Bende yemek yoksa ve sagimda yemek varsa
        else if(cnt2 && lookCharAt(y, x+1) == 'o'){
          putCharTo(y , x, 'o');
          mystate = '1';
        }


      // Bende yemek yoksa ve ustumde yemek varsa
        else if(cnt3 && lookCharAt(y-1, x) == 'o'){
          putCharTo(y , x, 'o');
          mystate = '1';
        }


      // Bende yemek yoksa ve altimda yemek varsa

        else if(cnt4 && lookCharAt(y+1, x) == 'o'){
          putCharTo(y , x, 'o');
          mystate = '1';
        }



      // Bende yemek yoksa ve sol ustumde yemek varsa
        else if(cnt5 && lookCharAt(y - 1, x - 1) == 'o'){
          putCharTo(y , x, 'o');
          mystate = '1';
        }


      // Bende yemek yoksa ve sag ustumde yemek varsa
        else if(cnt6 && lookCharAt(y-1, x+1) == 'o'){
          putCharTo(y , x, 'o');
          mystate = '1';
        }


      // Bende yemek yoksa ve sol altimda yemek varsa
        else if(cnt7 && lookCharAt(y+1, x-1) == 'o'){
          putCharTo(y , x, 'o');
          mystate = '1';
        }

      // Bende yemek yoksa ve sag altimda yemek varsa
        else if(cnt8 && lookCharAt(y + 1, x + 1) == 'o'){
          putCharTo(y , x, 'o');
          mystate = '1';
        }

        int random = rand() % 8;
        int checker = 1;


        for(int i = 0 ; i < 8 && checker; i++, random++){

          // Etrafimda yemek yok, sola bakiyorum bos mu diye
          if(cnt1 && (random % 8 == 0)){
            if(lookCharAt(y, x-1) == '-'){
              if(mystate == '1')
                putCharTo(y, x-1, '1');
              else{
                putCharTo(y, x-1, 'P');
                putCharTo(y, x, '-');
              }
              x = x - 1;
              checker = 0;
            }
          }

          // Etrafimda yemek yok, saga bakiyorum bos mu diye
          else if(cnt2 && (random % 8 == 1)){
            if(lookCharAt(y, x+1) == '-'){
              if(mystate == '1')
                putCharTo(y, x+1, '1');
              else{
                putCharTo(y, x+1, 'P');
                putCharTo(y, x, '-');
              }
              x = x + 1;
              checker = 0;
            }
          }


          // Etrafimda yemek yok, uste bakiyorum bos mu diye
          else if(cnt3 && (random % 8 == 2)){
            if(lookCharAt(y-1, x) == '-'){
              if(mystate == '1')
                putCharTo(y-1, x, '1');
              else{
                putCharTo(y-1, x, 'P');
                putCharTo(y, x, '-');
              }
              y = y - 1;
              checker = 0;
            }
          }


          // Etrafimda yemek yok, alta bakiyorum bos mu diye
          else if(cnt4 && (random % 8 == 3)){
            if(lookCharAt(y+1, x) == '-'){
              if(mystate == '1')
                putCharTo(y+1, x, '1');
              else{
                putCharTo(y+1, x, 'P');
                putCharTo(y, x, '-');
              }
              y = y + 1;
              checker = 0;
            }
          }

          // Etrafimda yemek yok, sol uste bakiyorum bos mu diye
          else if(cnt5 && (random % 8 == 4)){
            if(lookCharAt(y-1, x-1) == '-'){
              if(mystate == '1')
                putCharTo(y - 1, x - 1, '1');
              else{
                putCharTo(y - 1, x - 1, 'P');
                putCharTo(y , x , '-');
              }
              x = x - 1;
              y = y - 1;
              checker = 0;
            }
          }

          // Etrafimda yemek yok, sag uste bakiyorum bos mu diye
          else if(cnt6 && (random % 8 == 5)){
            if(lookCharAt(y-1, x+1) == '-'){
              if(mystate == '1')
                putCharTo(y-1, x+1, '1');
              else{
                putCharTo(y-1, x+1, 'P');
                putCharTo(y, x, '-');
              }
              x = x + 1;
              y = y - 1;
              checker = 0;
            }
          }

          // Etrafimda yemek yok, sol alta bakiyorum bos mu diye
          else if(cnt7 && (random % 8 == 6)){
            if(lookCharAt(y+1, x-1) == '-'){
              if(mystate == '1')
                putCharTo(y+1, x-1, '1');
              else{
                putCharTo(y+1, x-1, 'P');
                putCharTo(y, x, '-');
              }
              x = x - 1;
              y = y + 1;
              checker = 0;
            }
          }

          // Etrafimda yemek yok, sag alta bakiyorum bos mu diye
          else if(cnt8 && (random % 8 == 7)){
            if(lookCharAt(y + 1, x + 1) == '-'){
              if(mystate == '1')
                putCharTo(y + 1, x + 1, '1');
              else{
                putCharTo(y + 1, x + 1, 'P');
                putCharTo(y, x, '-');
              }
              x = x + 1;
              y = y + 1;
              checker = 0;
            }
          }
        }

        if(checker){
          mystate = 'P';
          putCharTo(y, x, 'P');
        }


        //          sem_wait(&mutex);
        if(cnt1)
          sem_post(&semaphore[ (oldy) * GRIDSIZE + oldx - 1]);

        if(cnt2)
          sem_post(&semaphore[ (oldy) * GRIDSIZE + oldx + 1]);

        if(cnt3)
          sem_post(&semaphore[ (oldy - 1) * GRIDSIZE + oldx]);

        if(cnt4)
          sem_post(&semaphore[ (oldy + 1) * GRIDSIZE + oldx]);

        if(cnt5)
          sem_post(&semaphore[ (oldy - 1) * GRIDSIZE + oldx - 1]);

        if(cnt6)
          sem_post(&semaphore[ (oldy - 1) * GRIDSIZE + oldx + 1]);

        if(cnt7)
          sem_post(&semaphore[ (oldy + 1) * GRIDSIZE + oldx - 1]);

        if(cnt8)
          sem_post(&semaphore[ (oldy + 1) * GRIDSIZE + oldx + 1]);

        //          sem_post(&mutex);


      }


      else if(mystate == '1'){

        putCharTo(y, x, '1');
        oldx = x;
        oldy = y;

        int cnt1 = 0, cnt2 = 0, cnt3 = 0, cnt4 = 0, cnt5 = 0, cnt6 = 0, cnt7 = 0, cnt8 = 0;

        sem_wait(&mutex);

        if(x - 1 >= 0){
          sem_wait(&semaphore[ (y) * GRIDSIZE + x - 1]);
          cnt1 = 1;
        }

        if(x + 1 < GRIDSIZE){
          sem_wait(&semaphore[ (y) * GRIDSIZE + x + 1]);
          cnt2 = 1;
        }

        if(y - 1 >= 0){
          sem_wait(&semaphore[ (y - 1) * GRIDSIZE + x]);
          cnt3 = 1;
        }

        if(y + 1 < GRIDSIZE){
          sem_wait(&semaphore[ (y + 1) * GRIDSIZE + x]);
          cnt4 = 1;
        }

        if(x - 1 >= 0 && y - 1 >= 0){
          sem_wait(&semaphore[ (y - 1) * GRIDSIZE + x - 1]);
          cnt5 = 1;
        }

        if(x + 1 < GRIDSIZE & y - 1 >= 0){
          sem_wait(&semaphore[ (y - 1) * GRIDSIZE + x + 1]);
          cnt6 = 1;
        }

        if(x - 1 >= 0 && y + 1 < GRIDSIZE){
          sem_wait(&semaphore[ (y + 1) * GRIDSIZE + x - 1]);
          cnt7 = 1;
        }

        if(x + 1 < GRIDSIZE && y + 1 < GRIDSIZE){
            sem_wait(&semaphore[ (y + 1) * GRIDSIZE + x + 1]);
            cnt8 = 1;
        }

        sem_post(&mutex);

        // Bende yemek yoksa ve solumda yemek varsa
          if(cnt1 && lookCharAt(y, x-1) == 'o'){
            putCharTo(y, x-1, 'P');
            putCharTo(y , x, '-');
            mystate = 'P';
            x = x - 1;
          }

        // Bende yemek yoksa ve sagimda yemek varsa
          else if(cnt2 && lookCharAt(y, x+1) == 'o'){
            putCharTo(y, x+1, 'P');
            putCharTo(y , x, '-');
            mystate = 'P';
            x = x + 1;
          }


        // Bende yemek yoksa ve ustumde yemek varsa
          else if(cnt3 && lookCharAt(y-1, x) == 'o'){
            putCharTo(y-1, x, 'P');
            putCharTo(y , x, '-');
            mystate = 'P';
            y = y - 1;
          }


        // Bende yemek yoksa ve altimda yemek varsa

          else if(cnt4 && lookCharAt(y+1, x) == 'o'){
            putCharTo(y+1, x, 'P');
            putCharTo(y , x, '-');
            mystate = 'P';
            y = y + 1;
          }



        // Bende yemek yoksa ve sol ustumde yemek varsa
          else if(cnt5 && lookCharAt(y - 1, x - 1) == 'o'){
            putCharTo(y - 1, x - 1, 'P');
            putCharTo(y , x, '-');
            mystate = 'P';
            x = x - 1;
            y = y - 1;
          }


        // Bende yemek yoksa ve sag ustumde yemek varsa
          else if(cnt6 && lookCharAt(y-1, x+1) == 'o'){
            putCharTo(y-1, x+1, 'P');
            putCharTo(y , x, '-');
            mystate = 'P';
            x = x + 1;
            y = y - 1;
          }


        // Bende yemek yoksa ve sol altimda yemek varsa
          else if(cnt7 && lookCharAt(y+1, x-1) == 'o'){
            putCharTo(y+1, x-1, 'P');
            putCharTo(y , x, '-');
            mystate = 'P';
            x = x - 1;
            y = y + 1;
          }

        // Bende yemek yoksa ve sag altimda yemek varsa
          else if(cnt8 && lookCharAt(y + 1, x + 1) == 'o'){
            putCharTo(y + 1, x + 1, 'P');
            putCharTo(y , x, '-');
            mystate = 'P';
            x = x + 1;
            y = y + 1;
          }


        else{ //Etrafimda yemek yok hareket etmem lazim
            int random = rand() % 8;
            int checker = 1;


            for(int i = 0 ; i < 8 && checker; i++, random++){

              // Etrafimda yemek yok, sola bakiyorum bos mu diye
              if(cnt1 && (random % 8 == 0)){
                if(lookCharAt(y, x-1) == '-'){
                  putCharTo(y, x-1, '1');
                  putCharTo(y , x, '-');
                  x = x - 1;
                  checker = 0;
                }
              }

              // Etrafimda yemek yok, saga bakiyorum bos mu diye
              else if(cnt2 && (random % 8 == 1)){
                if(lookCharAt(y, x+1) == '-'){
                  putCharTo(y, x+1, '1');
                  putCharTo(y , x, '-');
                  x = x + 1;
                  checker = 0;
                }
              }


              // Etrafimda yemek yok, uste bakiyorum bos mu diye
              else if(cnt3 && (random % 8 == 2)){
                if(lookCharAt(y-1, x) == '-'){
                  putCharTo(y-1, x, '1');
                  putCharTo(y , x, '-');
                  y = y - 1;
                  checker = 0;
                }
              }


              // Etrafimda yemek yok, alta bakiyorum bos mu diye
              else if(cnt4 && (random % 8 == 3)){
                if(lookCharAt(y+1, x) == '-'){
                  putCharTo(y+1, x, '1');
                  putCharTo(y , x, '-');
                  y = y + 1;
                  checker = 0;
                }
              }

              // Etrafimda yemek yok, sol uste bakiyorum bos mu diye
              else if(cnt5 && (random % 8 == 4)){
                if(lookCharAt(y-1, x-1) == '-'){
                  putCharTo(y - 1, x - 1, '1');
                  putCharTo(y , x, '-');
                  x = x - 1;
                  y = y - 1;
                  checker = 0;
                }
              }

              // Etrafimda yemek yok, sag uste bakiyorum bos mu diye
              else if(cnt6 && (random % 8 == 5)){
                if(lookCharAt(y-1, x+1) == '-'){
                  putCharTo(y-1, x+1, '1');
                  putCharTo(y , x, '-');
                  x = x + 1;
                  y = y - 1;
                  checker = 0;
                }
              }

              // Etrafimda yemek yok, sol alta bakiyorum bos mu diye
              else if(cnt7 && (random % 8 == 6)){
                if(lookCharAt(y+1, x-1) == '-'){
                  putCharTo(y+1, x-1, '1');
                  putCharTo(y , x, '-');
                  x = x - 1;
                  y = y + 1;
                  checker = 0;
                }
              }

              // Etrafimda yemek yok, sag alta bakiyorum bos mu diye
              else if(cnt8 && (random % 8 == 7)){
                if(lookCharAt(y + 1, x + 1) == '-'){
                  putCharTo(y + 1, x + 1, '1');
                  putCharTo(y , x, '-');
                  x = x + 1;
                  y = y + 1;
                  checker = 0;
                }
              }
            }
          }

//          sem_wait(&mutex);

          if(cnt1)
            sem_post(&semaphore[ (oldy) * GRIDSIZE + oldx - 1]);

          if(cnt2)
            sem_post(&semaphore[ (oldy) * GRIDSIZE + oldx + 1]);

          if(cnt3)
            sem_post(&semaphore[ (oldy - 1) * GRIDSIZE + oldx]);

          if(cnt4)
            sem_post(&semaphore[ (oldy + 1) * GRIDSIZE + oldx]);

          if(cnt5)
            sem_post(&semaphore[ (oldy - 1) * GRIDSIZE + oldx - 1]);

          if(cnt6)
            sem_post(&semaphore[ (oldy - 1) * GRIDSIZE + oldx + 1]);

          if(cnt7)
            sem_post(&semaphore[ (oldy + 1) * GRIDSIZE + oldx - 1]);

          if(cnt8)
            sem_post(&semaphore[ (oldy + 1) * GRIDSIZE + oldx + 1]);

//          sem_post(&mutex);
        }
      }
    }


int a,b;
int main(int argc, char *argv[]) {
    srand(time(NULL));

    int semmax = GRIDSIZE * GRIDSIZE;

    for(int i = 0; i < semmax; i++)
      sem_init(&semaphore[i], 0, 1);
    sem_init(&mutex, 0, 1);

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
    startCurses();

    // You can use following loop in your program. But pay attention to
    // the function calls, they do not have any access control, you
    // have to ensure that.
    char c;
    while (TRUE) {
        drawWindow();

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
        usleep(DRAWDELAY);

        // each ant thread have to sleep with code similar to this
        //usleep(getDelay() * 1000 + (rand() % 5000));
    }

    // do not forget freeing the resources you get
    endCurses();

    return 0;
}
