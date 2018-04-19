#include "do_not_submit.h"
#include <pthread.h>
#include <semaphore.h>

sem_t semaphore[GRIDSIZE * GRIDSIZE];
sem_t mutex;

struct anttype {

  int x,y,id,timer;

};

int emptyoryemek(int y, int x, int cord[], int cnt[],int controller){

  int returnval = 0;
  int random = rand() % 8;
  if(controller == 0){
    for(int i = 0 ; i < 8 && (!returnval) ; i++, random++){

      // Etrafimda yemek yok, sola bakiyorum bos mu diye
      if(cnt[1] && (random % 8 == 0)){
        if(lookCharAt(y, x-1) == '-'){
          x = x - 1;
          returnval = 1;
        }
      }

      // Etrafimda yemek yok, saga bakiyorum bos mu diye
      else if(cnt[2] && (random % 8 == 1)){
        if(lookCharAt(y, x+1) == '-'){
          x = x + 1;
          returnval = 1;
        }
      }


      // Etrafimda yemek yok, uste bakiyorum bos mu diye
      else if(cnt[3] && (random % 8 == 2)){
        if(lookCharAt(y-1, x) == '-'){
          y = y - 1;
          returnval = 1;
        }
      }


      // Etrafimda yemek yok, alta bakiyorum bos mu diye
      else if(cnt[4] && (random % 8 == 3)){
        if(lookCharAt(y+1, x) == '-'){
          y = y + 1;
          returnval = 1;
        }
      }

      // Etrafimda yemek yok, sol uste bakiyorum bos mu diye
      else if(cnt[5] && (random % 8 == 4)){
        if(lookCharAt(y-1, x-1) == '-'){
          x = x - 1;
          y = y - 1;
          returnval = 1;
        }
      }

      // Etrafimda yemek yok, sag uste bakiyorum bos mu diye
      else if(cnt[6] && (random % 8 == 5)){
        if(lookCharAt(y-1, x+1) == '-'){
          x = x + 1;
          y = y - 1;
          returnval = 1;
        }
      }

      // Etrafimda yemek yok, sol alta bakiyorum bos mu diye
      else if(cnt[7] && (random % 8 == 6)){
        if(lookCharAt(y+1, x-1) == '-'){
          x = x - 1;
          y = y + 1;
          returnval = 1;
        }
      }

      // Etrafimda yemek yok, sag alta bakiyorum bos mu diye
      else if(cnt[8] && (random % 8 == 7)){
        if(lookCharAt(y + 1, x + 1) == '-'){
          x = x + 1;
          y = y + 1;
          returnval = 1;
        }
      }
    }
  }
  else if(controller == 1){

    // Bende yemek yoksa ve solumda yemek varsa
      if(cnt[1] && lookCharAt(y, x-1) == 'o'){
        x = x - 1;
        returnval = 1;
      }

    // Bende yemek yoksa ve sagimda yemek varsa
      else if(cnt[2] && lookCharAt(y, x+1) == 'o'){
        x = x + 1;
        returnval = 1;
      }


    // Bende yemek yoksa ve ustumde yemek varsa
      else if(cnt[3] && lookCharAt(y-1, x) == 'o'){
        y = y - 1;
        returnval = 1;
      }


    // Bende yemek yoksa ve altimda yemek varsa

      else if(cnt[4] && lookCharAt(y+1, x) == 'o'){
        y = y + 1;
        returnval = 1;
      }



    // Bende yemek yoksa ve sol ustumde yemek varsa
      else if(cnt[5] && lookCharAt(y - 1, x - 1) == 'o'){
        x = x - 1;
        y = y - 1;
        returnval = 1;
      }


    // Bende yemek yoksa ve sag ustumde yemek varsa
      else if(cnt[6] && lookCharAt(y-1, x+1) == 'o'){
        x = x + 1;
        y = y - 1;
        returnval = 1;
      }


    // Bende yemek yoksa ve sol altimda yemek varsa
      else if(cnt[7] && lookCharAt(y+1, x-1) == 'o'){
        x = x - 1;
        y = y + 1;
        returnval = 1;
      }

    // Bende yemek yoksa ve sag altimda yemek varsa
      else if(cnt[8] && lookCharAt(y + 1, x + 1) == 'o'){
        x = x + 1;
        y = y + 1;
        returnval = 1;
      }


  }
  cord[0] = y;
  cord[1] = x;
  return returnval;
}


void *func(void *ant){
  int start = time(NULL);
  struct anttype *my_ant = (struct anttype*)ant;
  char mystate = '1';
  int x = my_ant->x;
  int y = my_ant->y;
  int myid = my_ant->id;
  sem_post(&mutex);

  //printf("Thread ID: %d x = %d y = %d\n", my_ant->id, my_ant->x, my_ant->y);
  char c;
  while(1){

    int cnt[9] = {0,0,0,0,0,0,0,0,0};
    int now = time(NULL);
    if(now - start > my_ant->timer)
      break;

    int randomtime = rand() % 10;
    usleep(1000 * (getDelay() + randomtime));
    int oldx = x, oldy = y;
    int control = 1;

    int cord[2];

    sem_wait(&mutex);
    if(x - 1 >= 0){
      sem_wait(&semaphore[ (y) * GRIDSIZE + x - 1]);
      cnt[1] = 1;
    }

    if(x + 1 < GRIDSIZE){
      sem_wait(&semaphore[ (y) * GRIDSIZE + x + 1]);
      cnt[2] = 1;
    }

    if(y - 1 >= 0){
      sem_wait(&semaphore[ (y - 1) * GRIDSIZE + x]);
      cnt[3] = 1;
    }

    if(y + 1 < GRIDSIZE){
      sem_wait(&semaphore[ (y + 1) * GRIDSIZE + x]);
      cnt[4] = 1;
    }

    if(x - 1 >= 0 && y - 1 >= 0){
      sem_wait(&semaphore[ (y - 1) * GRIDSIZE + x - 1]);
      cnt[5] = 1;
    }

    if(x + 1 < GRIDSIZE & y - 1 >= 0){
      sem_wait(&semaphore[ (y - 1) * GRIDSIZE + x + 1]);
      cnt[6] = 1;
    }

    if(x - 1 >= 0 && y + 1 < GRIDSIZE){
      sem_wait(&semaphore[ (y + 1) * GRIDSIZE + x - 1]);
      cnt[7] = 1;
    }

    if(x + 1 < GRIDSIZE && y + 1 < GRIDSIZE){
        sem_wait(&semaphore[ (y + 1) * GRIDSIZE + x + 1]);
        cnt[8] = 1;
    }
    sem_post(&mutex);


      if(myid < getSleeperN()){
        if(mystate == 'P')
          putCharTo(y, x, '$');
        else if(mystate == '1' || mystate == 'T')
          putCharTo(y, x, 'S');
      }



      else if(mystate == 'T'){

        putCharTo(y, x, '1');

        if(emptyoryemek(y,x,cord,cnt,0)){
          y = cord[0];
          x = cord[1];
          putCharTo(y,x,'1');
          putCharTo(oldy,oldx,'-');
          mystate = '1';
        }

      }
      else if(mystate == 'P'){

        putCharTo(y, x, 'P');

        if(emptyoryemek(y,x,cord,cnt,1)){
          if(emptyoryemek(y,x,cord,cnt,0)){
            y = cord[0];
            x = cord[1];
            putCharTo(y,x,'1');
            putCharTo(oldy,oldx,'o');
            mystate = 'T';
          }
        }
        else if(emptyoryemek(y,x,cord,cnt,0)){
          y = cord[0];
          x = cord[1];
          putCharTo(y,x,'P');
          putCharTo(oldy,oldx,'-');
        }
      }


      else if(mystate == '1'){

        putCharTo(y, x, '1');
        if(emptyoryemek(y,x,cord,cnt,1)){
          y = cord[0];
          x = cord[1];
          putCharTo(y,x,'P');
          putCharTo(oldy,oldx,'-');
          mystate = 'P';

        }
        else{//Etrafimda yemek yok hareket etmem lazim

          if(emptyoryemek(y,x,cord,cnt,0)){
            y = cord[0];
            x = cord[1];
            putCharTo(y,x,'1');
            putCharTo(oldy,oldx,'-');
          }
        }
      }

        if(cnt[1])
          sem_post(&semaphore[ (oldy) * GRIDSIZE + oldx - 1]);

        if(cnt[2])
          sem_post(&semaphore[ (oldy) * GRIDSIZE + oldx + 1]);

        if(cnt[3])
          sem_post(&semaphore[ (oldy - 1) * GRIDSIZE + oldx]);

        if(cnt[4])
          sem_post(&semaphore[ (oldy + 1) * GRIDSIZE + oldx]);

        if(cnt[5])
          sem_post(&semaphore[ (oldy - 1) * GRIDSIZE + oldx - 1]);

        if(cnt[6])
          sem_post(&semaphore[ (oldy - 1) * GRIDSIZE + oldx + 1]);

        if(cnt[7])
          sem_post(&semaphore[ (oldy + 1) * GRIDSIZE + oldx - 1]);

        if(cnt[8])
          sem_post(&semaphore[ (oldy + 1) * GRIDSIZE + oldx + 1]);
      }
    return 0;
    }


int a,b;
int main(int argc, char *argv[]) {
    srand(time(NULL));
    int start = time(NULL);
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
    pthread_t ant[numberofants];
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
        sem_wait(&mutex);
        putCharTo(a, b, '1');
        antstruct->x = b;
        antstruct->y = a;
        antstruct->id = i;
        antstruct->timer = maxsim;
        pthread_create(ant+i, NULL, func, (void*)antstruct);
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

        int now = time(NULL);
        if(now - start > maxsim)
          break;
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

    for(int i = 0; i < numberofants; i++)
      pthread_join(ant[i], NULL);
    free(antstruct);

    return 0;
}
