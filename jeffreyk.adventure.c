/*Adventure - game*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pthread.h>

/*****************************************************
 * Looks for files in directory, puts names in array *
 ****************************************************/
void find_rooms(char dir[], char* files[]){
   char buffer[50];
   struct dirent* parse;
   DIR *pdir = opendir(dir);
   int i = 0;
   int j;
   while((parse = readdir(pdir)) != NULL){//while more files are in dir
      if(parse->d_type == DT_REG){//if file is correct type
	 memset(buffer, '\0', 50);
	 strcpy(buffer, dir);
	 strcat(buffer, "/");
	 strcat(buffer, parse->d_name);
	 files[i] = (char*)malloc(sizeof(char) * (strlen(buffer) + 1));
	 strcpy(files[i], parse->d_name);//put name of file in array
	 i++;
      }
   }
   closedir(pdir);
}

/******************************
 * Returns name of start room *
 *****************************/
char* find_start_room(char dir[], char* files[]){
   FILE* curfile;
   char filepath[50];
   char type[10];
   char line[40];
   char* start = "START_ROOM";//string to check for
   int i;
   for(i = 0; i < 7; i++){
      memset(filepath, '\0', 50);
      strcpy(filepath, dir);
      strcat(filepath, "/");
      strcat(filepath, files[i]);//set each file path
      if(curfile = fopen(filepath, "r")){
	 while((fgetc(curfile)) != EOF){
	    fgets(line, 40, curfile);
	    sscanf(line, "%*s %*s %s", &type);
	    if(strcmp(type, start) == 0)
	       return files[i];//return name of room
	 }
      }
   }
   fclose(curfile);
}

/*******************************************************
 * Prints room name and connections, returns room type *
 ******************************************************/
void print_location(char dir[], char loc[], char roomtype[]){
   char filepath[50];
   memset(filepath, '\0', 50);
   strcpy(filepath, dir);//set up filepath with directory
   strcat(filepath, "/");
   strcat(filepath, loc);
   char name[20];
   char connect[50];
   char line[50];
   char buff[30];
   memset(name, '\0', 20);//memset everything!
   memset(connect, '\0', 50);
   memset(buff, '\0', 30);
   FILE* filep = fopen(filepath, "r");
   fgets(line, 50, filep);
   sscanf(line, "%*s %*s %s", &name);//get name of room
   printf("CURRENT LOCATION: %s\n", name);
   printf("POSSIBLE CONNECTIONS:");
   while((fgetc(filep)) != 'R'){//while first letter is not R for Room
      fgets(line, 50, filep);
      sscanf(line, "%*s %s", &connect);//get each connection
      printf(" %s,", connect);
   }
   printf(".\n");
   fgets(line, 50, filep);
   sscanf(line, "%*s %*s %s", &buff);//get room type
   strcpy(roomtype, buff);
   fclose(filep);
}

/***********************
 * Prints current time *
 **********************/
void get_time(){
   char filename[20];
   memset(filename, '\0', 20);
   strcpy(filename, "currentTime.txt");
   int filedescriptor = open(filename, O_RDWR | O_CREAT, 0755);
   time_t curtime;
   struct tm* timeinfo;
   time(&curtime);
   timeinfo = localtime(&curtime);
   printf("\n%s\n", asctime(timeinfo));
   size_t written = write(filedescriptor, asctime(timeinfo), strlen(asctime(timeinfo)) * sizeof(char));
}

/*******************************
 * Check if string is in files *
 ******************************/
int check_input(char* input, char* files[]){
   int i;
   int ret = 0;//0 if not in array
   for(i = 0; i < 7; i++){
      if(strcmp(input, files[i]) == 0)
	 ret = 1;//when found set to 1
   }
   return ret;
}

/*******************
 * Main game logic *
 ******************/
void play_game(char dir[], char* files[]){
   int counter = 0;//for number of steps
   char input[30];
   char* path[20];
   char roomtype[50];
   memset(roomtype, '\0', 50);
   char* start = find_start_room(dir, files);//set start room
   print_location(dir, start, roomtype);//set roomtype to start
   while(strcmp(roomtype, "END_ROOM") != 0){
      printf("WHERE TO? >");
      scanf("%s", input);//get user input
      if(strcmp(input, "time") == 0){
	 get_time();//if user enters time show time
      }
      else if(check_input(input, files) == 0){//if user enters room not in file
	 printf("\nTHAT ROOM IS INVALID. TRY AGAIN.\n\n");
      }
      else{
	 printf("\n");
	 path[counter] = (char*)malloc(sizeof(char) * (strlen(input) +1));
	 strcpy(path[counter], input);//add room to path
	 print_location(dir, input, roomtype);//set roomtype to room
	 counter++;//update number of steps
      }
   }
   printf("\nYOU FOUND THE END ROOM!\nYOU TOOK %d STEPS. YOUR PATH WAS:\n", counter);
   int i;
   for(i = 0; i < counter; i++){
      printf("%s\n", path[i]);
      free(path[i]);
   }
}

/**********************
 * Frees files memory *
 *********************/
void clear_files(char* files[]){
   int i;
   for(i = 0; i < 7; i++){
      free(files[i]);
   }
}

int main(){
   struct stat filestat;
   struct dirent *ent;
   DIR *currdir = opendir(".");
   time_t recent = 0;
   char dir_name[20];
   memset(dir_name, '\n', 20);
   char* files[7];
   while((ent = readdir(currdir)) != NULL){
      memset(&filestat, 0, sizeof(filestat));
      if(stat(ent->d_name, &filestat) < 0){//check file works
	 printf("Error getting into file\n");
	 continue;
      }
      if((filestat.st_mode & S_IFDIR) != S_IFDIR || ent->d_name[0] == '.'){
	 continue;
      }
      if(filestat.st_mtime > recent){//check for more recent time
	 strcpy(dir_name, ent->d_name);
	 recent = filestat.st_mtime;//update latest time
      }
   }
   //printf("Directory: %s\n", dir_name);
   find_rooms(dir_name, files);
   play_game(dir_name, files);
   clear_files(files);
   closedir(currdir);
   return 0;
}
