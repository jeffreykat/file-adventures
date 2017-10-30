/*Buildrooms - rooms*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

typedef struct Room room;

struct Room {
   char name[20];
   struct Room* connection[6];
   int num_connect;
   char type[15];
};

/************************************************
 * Create an array of 10 unique random integers *
 ***********************************************/
void creat_rand(int arr[]){
   int i, j, check, r;
   time_t t;
   srand((unsigned int) time(&t));
   i = 1;
   arr[0] = rand() % 10;
   while(i < 10){
      r = rand() % 10;
      check = 0;
      for(j = 0; j < i; j++){
	 if(r == arr[j])
	    check = 1;
      }
      if(check != 1){
	 arr[i] = r;
	 i++;
      }
   }
}

/******************************************************
 * Set each room's name to random name in names array *
 *****************************************************/
void set_room_names(struct Room* rooms[], char* names[]){
   int i;
   int arr[10];
   creat_rand(arr);
   for(i = 0; i < 7; i++){
      strcpy(rooms[i]->name, names[arr[i]]);
   }
}

/*************************************
 * Checks if two rooms are connected *
 ************************************/
int has_connection(struct Room* first, struct Room* second, int connected){
   int i;
   for(i = 0; i < connected; i++){
      if(strcmp(first->connection[i]->name, second->name) == 0)
	 return 1;
   }
   return 0;
}

/**********************************************************
 * Puts random room in each connection space of place     * 
 * Checks if random is the same room or already connected *
 *********************************************************/
void fill_connections(struct Room* rooms[], struct Room* place, int r){
   int i, random;
   time_t t;
   srand((unsigned int) time(&t));
   i = 0;
   while(i < r){
      random = rand() % 7;
      if(strcmp(place->name, rooms[random]->name) != 0 && has_connection(place, rooms[random], i) == 0){
	 place->connection[i] = rooms[random];
	 i++;
      }
      random = rand() % 7;
   }
}

/*******************************************************
 * Sets random number 3-6 of connections for each room *
 ******************************************************/
void set_room_connections(struct Room* rooms[]){
   int i, j, r;
   time_t t;
   srand((unsigned int) time(&t));
   r = (rand() % 3) + 3;
   for(i = 0; i < 7; i++){
      r = (rand() % 3) + 3;
      rooms[i]->num_connect = r;
      fill_connections(rooms, rooms[i], r);
      r = (rand() % 3) + 3;
   }
}

/**********************************
 * Creates an array of types      *
 * Start and End rooms are random *
 *********************************/
void creat_types(char* types[]){
   int i, r;
   time_t t;
   srand((unsigned int) time(&t));
   for(i = 0; i < 7; i++){
      types[i] = "MID_ROOM";
   }
   r = rand() % 7;
   types[r] = "START_ROOM";
   i = r;
   while(i == r){
      i = rand() % 7;
   }
   types[i] = "END_ROOM";
}

/***********************************************
 * Gets array of types and sets each room type *
 **********************************************/
void set_room_types(struct Room* rooms[]){
   int i;
   char* types[7];
   creat_types(types);
   for(i = 0; i < 7; i++){
      strcpy(rooms[i]->type, types[i]);
   }
}

/*********************************************
 * Prints the room's connections to the file *
 ********************************************/
void set_file_connections(int filedescriptor, struct Room* connects[], int num){
   size_t written;
   int i;
   char finput[35];
   memset(finput, '\0', 35);
   for(i = 0; i < num; i++){
      strcpy(finput, "CONNECTION: ");
      strcat(finput, connects[i]->name);
      strcat(finput, "\n");
      written = write(filedescriptor, finput, strlen(finput) * sizeof(char));
   }
}

/****************************
 * Prints room type to file *
 ***************************/
void set_file_types(int filedescriptor, char type[]){
   size_t written;
   char finput[30];
   memset(finput, '\0', 30);
   strcpy(finput, "ROOM TYPE: ");
   strcat(finput, type);
   strcat(finput, "\n");
   written = write(filedescriptor, finput, strlen(finput) * sizeof(char));
}

/****************************************************
 * Creates room files and prints room names to them * 
 ***************************************************/
void set_room_files(struct Room* rooms[], char dirname[]){
   int i;
   size_t written;
   char filepath[50];
   char finput[30];
   for(i = 0; i < 7; i++){
      memset(filepath, '\0', 50);
      memset(finput, '\0', 30);
      strcpy(filepath, dirname);
      strcat(filepath, "/");
      strcat(filepath, rooms[i]->name);
      int filedescriptor = open(filepath, O_RDWR | O_APPEND | O_CREAT, 0755);
      strcat(finput, "ROOM NAME: ");
      strcat(finput, rooms[i]->name);
      strcat(finput, "\n");
      written = write(filedescriptor, finput, strlen(finput) * sizeof(char));
      set_file_connections(filedescriptor, rooms[i]->connection, rooms[i]->num_connect);
      set_file_types(filedescriptor, rooms[i]->type);
   }
}

/*******************************
 * Prints out each room's info *
 ******************************/
void print_rooms(struct Room* rooms[]){
   int i, j;
   for(i = 0; i < 7; i++){
      printf("ROOM_NAME: %s\n", rooms[i]->name);
      for(j = 0; j < rooms[i]->num_connect; j++){
	 printf("CONNECTION %d: %s\n", j+1, rooms[i]->connection[j]->name);
      }
      printf("ROOM_TYPE: %s\n", rooms[i]->type);
   }
}

/*****************************************
 * Allocates space for array of 10 Rooms *
 ****************************************/
void creat_rooms(struct Room* rooms[]){
   int i;
   for(i = 0; i < 10; i++){
      rooms[i] = (struct Room*) malloc(sizeof(struct Room));
   }
}

/**************************************
 * Frees space for each Room in array *
 *************************************/
void clear_rooms(struct Room* rooms[]){
   int i;
   for(i = 0; i < 10; i++){
      free(rooms[i]);
   }
}

int main () {
   struct Room* rooms[10];
   char* roomnames[10] = {"Serenity", "Enterprise", "Tardis", "Waverider", "Millennium_Falcon", "Death_Star", "Helicarrier", "Milano", "Blackbird", "Batwing"};
   int pid = getpid();
   char dirname[21];
   memset(dirname, '\0', 21);
   char* dir = "jeffreyk.rooms.";
   snprintf(dirname, 21, "%s%d", dir, pid);//creates directory name
   //printf("%s\n", dirname);
   struct stat st = {0};
   if (stat(dirname, &st) == -1){
      mkdir(dirname, 0755);//makes directory
   }
   creat_rooms(rooms);
   set_room_names(rooms, roomnames);
   set_room_types(rooms);
   set_room_connections(rooms);
   //print_rooms(rooms);
   set_room_files(rooms, dirname);
   clear_rooms(rooms);
   return 0;
}
