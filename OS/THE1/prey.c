#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/socket.h>
#include <stdlib.h>
#include <string.h>

#define PIPE(fd) socketpair(AF_UNIX, SOCK_STREAM, PF_UNIX, fd)


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

int main(int argc, char const *argv[]) {

  char buffer[2048];
  server_message message;
  coordinate newrequest;
  ph_message newphmessage;
  //fprintf(stderr, "atoiiiiii%d %d\n",atoi(argv[0]),atoi(argv[1]) );
  while(1){

    read(1,&message,sizeof(message));
    newrequest.x = message.pos.x;
    newrequest.y = message.pos.y;
    int x1 = 1, x2 = 1, y1 = 1, y2 = 1;

    if(message.object_count == 0){

      if(message.pos.y <= message.adv_pos.y && message.pos.y - 1 >= 0){
        newrequest.x = message.pos.x;
        newrequest.y = message.pos.y - 1;
      }

      else if(message.pos.x >= message.adv_pos.x && message.pos.x + 1 < atoi(argv[1])){
        newrequest.x = message.pos.x + 1;
        newrequest.y = message.pos.y;
      }
      else if(message.pos.x <= message.adv_pos.x && message.pos.x -1 >= 0){
        newrequest.x = message.pos.x - 1;
        newrequest.y = message.pos.y;
      }

      else if(message.pos.y >= message.adv_pos.y && message.pos.y + 1 < atoi(argv[0])){
        newrequest.x = message.pos.x;
        newrequest.y = message.pos.y + 1;
      }

    }

    else{
      for(int i = 0; i < message.object_count; i++){

        if(message.object_pos[i].x == message.pos.x - 1 && message.object_pos[i].y == message.pos.y)
          x1 = 0;
        else if(message.object_pos[i].x == message.pos.x +1 && message.object_pos[i].y == message.pos.y)
          x2 = 0;
        else if(message.object_pos[i].y == message.pos.y - 1 && message.object_pos[i].x == message.pos.x)
          y1 = 0;
        else
          y2 = 0;
      }
      if(x1 && message.pos.x < message.adv_pos.x && message.pos.x -1 >= 0)
        newrequest.x = message.pos.x - 1;
      else if(x2 && message.pos.x > message.adv_pos.x && message.pos.x + 1 < atoi(argv[1]))
        newrequest.x = message.pos.x + 1;
      else if(y1 && message.pos.y < message.adv_pos.y && message.pos.y - 1 >= 0 )
        newrequest.y = message.pos.y - 1;
      else if(y2 && message.pos.y > message.adv_pos.y && message.pos.y + 1 < atoi(argv[0]))
        newrequest.y = message.pos.y + 1;
    }
    //fprintf(stderr, "Im prey and \nmy pos is %d %d \nand i wanna go %d %d \nand objectcount is %d\n",message.pos.x,message.pos.y,message.adv_pos.x,message.adv_pos.y,message.object_count);
    //fflush(stdout);
    //fprintf(stderr, "I can go %d %d\n\n",newrequest.x,newrequest.y);
    //fflush(stdout);
    newphmessage.move_request = newrequest;

    write(0,&newphmessage,sizeof(newphmessage));
    usleep(10000*(1+rand()%9));
  }

  return 0;
}
