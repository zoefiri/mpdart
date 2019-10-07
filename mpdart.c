#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <malloc.h>
#include <mpd/client.h>
#include <mpd/connection.h>

void get_playing_image(struct mpd_connection *connection, char *mpd_dir){
   struct mpd_song *song;
   char *song_path;
   char *song_fullpath;
   char *song_file_fullpath;
   char *ffmpeg_cmd;
   int i;
   int art = 0;
   FILE *src, *dest;
   void *b = malloc(1);

   while(true){
      song = mpd_run_current_song(connection);
      song_path = mpd_song_get_uri(song);

      song_fullpath = malloc(sizeof(char) * 500);
      *song_fullpath = '\0';

      strncat(song_fullpath, mpd_dir, strlen(mpd_dir));
      strncat(song_fullpath, song_path, strlen(song_fullpath) + strlen(song_path));

      for(i = strlen(song_fullpath) - 1; i >= 0; i--){
         if(*(song_fullpath + i) == '/'){
            if(*(song_fullpath + i - 1) != '\\'){
               break;
            }
         }
      }
      song_file_fullpath = strdup(song_fullpath);
      song_fullpath[i+1] = '\0';

      struct dirent *current_file;
      DIR *song_directory_traversal = opendir(song_fullpath);

      while((current_file = readdir(song_directory_traversal)) != NULL){
         if(art == 0){
            for(i = strlen(current_file->d_name) - 1; i >= 0; i--){
               if(*(current_file->d_name + i) == '.'){
                  if(
                        strcmp((current_file->d_name + i), ".jpeg") == 0 ||
                        strcmp((current_file->d_name + i), ".jpg") == 0  ||
                        strcmp((current_file->d_name + i), ".png") == 0
                    ){
                     strcat(song_fullpath, current_file->d_name);

                     remove("/tmp/albumart.png");
                     printf("%s\n", song_fullpath);
                     src = fopen(song_fullpath, "r");
                     dest = fopen("/tmp/albumart.png", "w");

                     while(!feof(src)){
                        fread(b, 1, 1, src);
                        fwrite(b, 1, 1, dest);
                     }

                     fclose(src);
                     fclose(dest);
                     art = 1;
                     break;
                  }
               }
            }
         }
      }
      closedir(song_directory_traversal);

      if(art != 1){
         strcat(song_fullpath, song_path);
         ffmpeg_cmd = malloc(sizeof(char) * 1000);
         *ffmpeg_cmd = '\0';
         strcat(ffmpeg_cmd, "ffmpeg -loglevel panic -y -i \"");
         strcat(ffmpeg_cmd, song_file_fullpath);
         strcat(ffmpeg_cmd, "\" /tmp/albumart.png");
         printf("%s\n", song_file_fullpath);
         system(ffmpeg_cmd);
         free(ffmpeg_cmd);
      }
      art = 0;

      free(song_fullpath);
      free(song_file_fullpath);
      mpd_song_free(song);
      mpd_run_idle(connection);
   }
}

int main (int argc, char* argv[]){
   char *mpd_dir;
   char *mpd_addr;
   int mpd_port;

   if(argc == 1){
      system("feh -B black --scale-down --g 200x200 --zoom fill /tmp/albumart.png");
      return 0;
   }

   for(int i = argc-1; i>0; i--){
      if(strcmp(argv[argc-i], "--dir") == 0){
         mpd_dir = argv[argc-i+1];
      }
      else if(strcmp(argv[argc-i], "--host") == 0){
         mpd_addr = argv[argc-i+1];
      }
      else if(strcmp(argv[argc-i], "--port") == 0){
         mpd_port = atoi(argv[argc-i+1]);
      }
   }

   DIR *dir = opendir(mpd_dir);
   if(!dir){
      printf("ERROR: mpd dir nonexistent or access is not permitted.\n");
      return 1;
   }

   struct mpd_connection *connection = mpd_connection_new(mpd_addr, mpd_port, 10000);
   if(mpd_connection_get_error(connection) != MPD_ERROR_SUCCESS){
      printf("ERROR: mpd connection failed.\n");
      return 1;
   }

   get_playing_image(connection, mpd_dir);
}
