#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/wait.h>

#define PIPE(fd) socketpair(AF_UNIX, SOCK_STREAM, PF_UNIX, fd)
#define SERVER 0
#define CHILD 1


int nobstacles, nhunters, npreys, widht, height;
typedef struct coordinate {
  int x;
  int y;
} coordinate;

typedef struct server_message {
  coordinate pos;
  coordinate adv_pos;
  int object_count;
  coordinate object_pos[4];
} server_message;

typedef struct ph_message {
  coordinate move_request;
} ph_message;


int* closest_hunter(int x1, int y1, int hunters[][5],int sonuc[2]){

  int smallest = INT_MAX;
  int cord1, cord2;
  int temp = 0;
  sonuc[0] = -1;
  sonuc[1] = -1;

  for( int i = 0; i < nhunters; i++ ){

    temp += abs(x1 - hunters[i][0]);
    temp += abs(y1 - hunters[i][1]);

    if(temp < smallest && hunters[i][4]){

      smallest = temp;
      sonuc[0] = hunters[i][0];
      sonuc[1] = hunters[i][1];
    }

    temp = 0;
  }
  return sonuc;
}

int* closest_prey(int x1, int y1, int preys[][5],int sonuc[2]){

  int smallest = INT_MAX;
  int cord1, cord2;
  int temp = 0;
  sonuc[0] = -1;
  sonuc[1] = -1;

  for( int i = 0; i < npreys; i++ ){

    temp += abs(x1 - preys[i][0]);
    temp += abs(y1 - preys[i][1]);

    if(temp < smallest && preys[i][4]){

      smallest = temp;
      sonuc[0] = preys[i][0];
      sonuc[1] = preys[i][1];
    }

    temp = 0;
  }
  return sonuc;
}

int checker(int i, int j, int hunters[][5], int obstacles[][2], int preys[][5], char grid[][height]){

  int k;

  for(k = 0; k < nhunters; k++){

    //printf("%d %d\n",hunters[k][0],hunters[k][1] );
    if(hunters[k][0] == i && hunters[k][1] == j && hunters[k][4]){

      printf("H");
      grid[i][j] = 'H';
      return 1;
    }
  }

  for(k = 0; k < npreys; k++){

    if(preys[k][0] == i && preys[k][1] == j && preys[k][4]){

      printf("P");
      grid[i][j] = 'P';
      return 1;
    }
  }

  for(k = 0; k < nobstacles; k++){

    if(obstacles[k][0] == i && obstacles[k][1] == j){

      printf("X");
      grid[i][j] = 'X';
      return 1;
    }
  }

  return 0;
}


void mapper(int height,int widht,int hunters[][5], int obstacles[][2], int preys[][5], char grid[][height]){

  int i, j;

  for(j = 0; j < height + 2; j++){

    for(i = 0; i < widht + 2; i++){

      if( (j == 0 || j == height + 1) && ( i == 0 || i == widht + 1 ) ) printf("+");
      else if( ( j == 0 || j == height + 1 ) ) printf("-");
      else if( i == 0 || i == widht + 1 ) printf("|");
      else if( !checker(j-1, i-1, hunters, obstacles, preys, grid) ){ printf(" "); grid[j-1][i-1] = ' ';}
    }

    printf("\n");
  }

}

server_message server_message_creater(int x1, int y1, int hunterorprey,
   server_message message, int hunters[][5], int preys[][5],char grid[][height]){

  int sonuc[2];
  int count = 0;

  message.pos.x = x1;
  message.pos.y = y1;
  if(hunterorprey == 0){

    closest_prey(x1, y1, preys, sonuc);
    message.adv_pos.x = sonuc[0];
    message.adv_pos.y = sonuc[1];
  }
  else{

    closest_hunter(x1,y1,hunters,sonuc);
    message.adv_pos.x = sonuc[0];
    message.adv_pos.y = sonuc[1];
  }

  if(x1 + 1 < widht && grid[x1][y1] == 'P' && (grid[x1 + 1][y1] == 'X' || grid[x1 + 1][y1] == 'P')){
    message.object_pos[count].x = x1 + 1;
    message.object_pos[count].y = y1;
    count++;
  }

  if(x1 - 1 >= 0 && grid[x1][y1] == 'P' && (grid[x1 - 1][y1] == 'X' || grid[x1 - 1][y1] == 'P')){
    message.object_pos[count].x = x1 -1;
    message.object_pos[count].y = y1;
    count++;
  }

  if(y1 + 1 < height && grid[x1][y1] == 'P' && (grid[x1][y1 + 1] == 'X' || grid[x1][y1 + 1] == 'P')){
    message.object_pos[count].x = x1;
    message.object_pos[count].y = y1 + 1;
    count++;
  }

  if(y1 - 1 >= 0 && grid[x1][y1] == 'P' && (grid[x1][y1 - 1] == 'X' || grid[x1][y1 - 1] == 'P')){
    message.object_pos[count].x = x1;
    message.object_pos[count].y = y1 - 1;
    count++;
  }

  if(x1 + 1 < widht && grid[x1][y1] == 'H' && (grid[x1 + 1][y1] == 'X' || grid[x1 + 1][y1] == 'H')){
    message.object_pos[count].x = x1 + 1;
    message.object_pos[count].y = y1;
    count++;
  }

  if(x1 - 1 >= 0 && grid[x1][y1] == 'H' && (grid[x1 - 1][y1] == 'X' || grid[x1 - 1][y1] == 'H')){
    message.object_pos[count].x = x1 -1;
    message.object_pos[count].y = y1;
    count++;
  }

  if(y1 + 1 < height && grid[x1][y1] == 'H' && (grid[x1][y1 + 1] == 'X' || grid[x1][y1 + 1] == 'H')){
    message.object_pos[count].x = x1;
    message.object_pos[count].y = y1 + 1;
    count++;
  }

  if(y1 - 1 >= 0 && grid[x1][y1] == 'H' && (grid[x1][y1 - 1] == 'X' || grid[x1][y1 - 1] == 'H')){
    message.object_pos[count].x = x1;
    message.object_pos[count].y = y1 - 1;
    count++;
  }
  message.object_count = count;
  return message;
}

int main(int argc, char const *argv[]) {

	//TODO STRUCT POLLFD ARRAYI NHUNTER + NPREY +

  int i, j, k;
  ph_message phmessage;
  int status;


  scanf(" %d %d", &widht, &height);
  scanf(" %d", &nobstacles);
  char grid[widht][height];
  int obstacles[nobstacles][2];

  for(i = 0; i < nobstacles; i++){

    int temp1,temp2;
    scanf("%d %d\n",&temp1, &temp2 );
    obstacles[i][0] = temp1;
    obstacles[i][1] = temp2;
  }

  scanf(" %d",&nhunters);

  int hunters[nhunters][5];

  for(i = 0; i < nhunters; i++){

    int temp1, temp2, temp3;

    scanf(" %d %d %d", &temp1, &temp2, &temp3);
    hunters[i][0] = temp1;
    hunters[i][1] = temp2;
    hunters[i][2] = temp3;
    hunters[i][4] = 1;
  }

  scanf(" %d", &npreys);
  int huntercount = nhunters, preycount = npreys;

  int preys[npreys][5];

  for(i = 0; i < npreys; i++){

    int temp1, temp2, temp3;

    scanf(" %d %d %d", &temp1, &temp2, &temp3);
    preys[i][0] = temp1;
    preys[i][1] = temp2;
    preys[i][2] = temp3;
    preys[i][4] = 1;
  }
  struct pollfd fds[nhunters + npreys];

  mapper(height, widht, hunters, obstacles, preys, grid);

  char *args[3];
  char tmpstr1[30];
  char tmpstr2[30];

  server_message message;

  snprintf(tmpstr1, sizeof(tmpstr1), "%d", widht);
  snprintf(tmpstr2, sizeof(tmpstr2), "%d", height);
  args[0] = tmpstr1;
  args[1] = tmpstr2;
  args[2] = NULL;

  char buffer[2048];
  int fd[nhunters + npreys][2];
  int pid;

  for( i = 0; i < nhunters; i++ ){
    PIPE(fd[i]);
    fds[i].fd = fd[i][0];
    fds[i].events = POLLIN;
    pid = fork();
    hunters[i][3] = pid;

    if( !pid){
      close(fd[i][0]);
      dup2(fd[i][1],0);
      dup2(fd[i][1],1);
      close(fd[i][1]);
      execv("hunter",args);
    }
  }

  for( i = 0; i < npreys; i++ ){
    PIPE(fd[i + nhunters]);
    fds[i + nhunters].fd = fd[i + nhunters][0];
    fds[i + nhunters].events = POLLIN;
    pid = fork();
    preys[i][3] = pid;
    if( !pid ){
      //fprintf(stderr, " Prey %d\n",getpid());
      close(fd[i + nhunters][0]);
      dup2(fd[i + nhunters][1],0);
      dup2(fd[i + nhunters][1],1);
      close(fd[i + nhunters][1]);
      execv("prey",args);
    }
    //fprintf(stderr, " Preyasd -> %d\n",preys[i][3]);
  }
  for( i = 0 ; i < nhunters + npreys ; i++){

    if(i < nhunters){
      message = server_message_creater(hunters[i][0],hunters[i][1],0,message,hunters,preys,grid);
      write(fd[i][0], &message, sizeof(message));
      //fprintf(stderr, " hunters-> %d\n",hunters[i][3]);
      //read(fd[i][0],&phmessage,sizeof(phmessage));
    }
    else{
      message = server_message_creater(preys[i - nhunters][0],preys[i - nhunters][1],1,message,hunters,preys,grid);
      write(fd[i][0], &message, sizeof(message));
      //read(fd[i][0],&phmessage,sizeof(phmessage));
    }
  }

  while(preycount && huntercount){

    int ret = poll(fds, npreys + nhunters, 0);

      if(ret == -1){
        printf("RET = -1\n");
        return 1;
      }

      else if(ret == 0){
      }

      else{

        //printf("%d %d\n",preycount,huntercount);
        fflush(stdout);
        for(i = 0; i < nhunters + npreys && preycount && huntercount; i++){

          if(fds[i].revents & POLLIN){
            //printf("%d %d\n",preycount,huntercount);
            fflush(stdout);
            //printf("read message\n");
            read(fds[i].fd,&phmessage,sizeof(phmessage));
            if(i < nhunters){
              //printf("gitmek istedigim adres = %d %d \n",phmessage.move_request.x,phmessage.move_request.y);
              //printf("my adress = %d %d \n",hunters[i][0],hunters[i][1]);
              if(hunters[i][2] > 0 && !((hunters[i][0] == phmessage.move_request.x && hunters[i][1] == phmessage.move_request.y)
                || grid[phmessage.move_request.x][phmessage.move_request.y] == 'H' )){

                hunters[i][2]--;
                if(grid[phmessage.move_request.x][phmessage.move_request.y] == 'P'){
                  grid[phmessage.move_request.x][phmessage.move_request.y] == 'H';
                  for(int z = 0; z < npreys; z++){
                    if(preys[z][0] == phmessage.move_request.x && preys[z][1] == phmessage.move_request.y){

                      // prey olecek
                      hunters[i][2] += preys[z][2];
                      preys[z][4] = 0;
                      preycount--;
                      //fprintf(stderr, " Prey -> %d\n",preys[z][3]);
                      close(fd[z + nhunters][0]);
                      close(fd[z + nhunters][1]);
                      kill(preys[z][3], SIGTERM);
                      waitpid(preys[z][3],&status,0);
                    }
                  }
                }
                hunters[i][0] = phmessage.move_request.x;
                hunters[i][1] = phmessage.move_request.y;
                if(preycount && huntercount)
                  mapper(height, widht, hunters, obstacles, preys, grid);

              }
              else if(hunters[i][2] == 0){
                hunters[i][4] = 0;
                huntercount--;
                close(fd[i][0]);
                close(fd[i][1]);
                kill(hunters[i][3],SIGTERM);
                waitpid(hunters[i][3],&status,0);
                //fprintf(stderr, " Hunters -> %d\n",hunters[i][3]);
              }
              message = server_message_creater(hunters[i][0],hunters[i][1],0,message,hunters,preys,grid);
              if(preycount && huntercount)
                write(fds[i].fd, &message, sizeof(message));

            }
            else{
              if(preys[i -nhunters][4] && !((preys[i- nhunters][0] == phmessage.move_request.x && preys[i - nhunters][1] == phmessage.move_request.y)
                 || grid[phmessage.move_request.x][phmessage.move_request.y] == 'P')){
                preys[i- nhunters][0] = phmessage.move_request.x;
                preys[i - nhunters][1] = phmessage.move_request.y;
                if(preycount && huntercount)
                  mapper(height, widht, hunters, obstacles, preys, grid);
            }
              message = server_message_creater(preys[i - nhunters][0],preys[i - nhunters][1],1,message,hunters,preys,grid);
              if(preycount && huntercount)
                write(fds[i].fd, &message, sizeof(message));
            }

          }
        }


        //if (fds[1].revents & POLLOUT) printf ("stdout is writable\n");
      }
  }
  mapper(height, widht, hunters, obstacles, preys, grid);
  for(i = 0 ; i < nhunters + npreys; i++){

    if(i < nhunters){
      if(hunters[i][4]){
        close(fd[i][0]);
        close(fd[i][1]);
        kill(hunters[i][3],SIGTERM);
        waitpid(hunters[i][3],&status,0);
    }
  }
    else{
      if(preys[i - nhunters][4]){
        close(fd[i][0]);
        close(fd[i][1]);
        kill(preys[i - nhunters][3], SIGTERM);
        waitpid(preys[i - nhunters][3],&status,0);
       }
    }
  }
  /*for ( i = 0; i < nhunters+npreys; i++) {
    if(i < nhunters)
      printf("PID2 %ld exited with status 0x%x %d.\n", (long)waitpid(hunters[i][3],&status,0), status,i);
    else
      printf("PID2 %ld exited with status 0x%x %d.\n", (long)waitpid(preys[i - nhunters][3],&status,0), status,i);
  }*/
  return 0;
}
