/*
** projet 1 du cours d'exploitation de systèmes informatiques
** Auteurs : Alessandro Dorigo, Nha Thy Truong, Danae Veyret
** fichier : main.c
**
*/

#include "utils.h"

int exec_img_dist(char *baseimg, char *otherimg)
{
   pid_t pid;
   int status;
   

   pid = fork();
   if (pid == 0)
   {
      printf("Child process\n");
      printf(" args : %s %s", baseimg, otherimg);
      if (execlp("img-dist", "-v", baseimg, otherimg, NULL) == -1)
         handle_error();
   }
   else
   {
      printf("Parent process\n");
      if (wait(&status) == -1)
         handle_error();
      if (WIFEXITED(status))
         return WEXITSTATUS(status);
   }
   return 65;
}

int main(int argc, char* argv[])
{
   char *baseimg;
   char *closest_img = NULL;
   int closest_dist = 65;
   int dist;

   if (argc != 2)
   {
      printf("Usage: %s <path to file>\n", argv[0]);
      return 1;
   }

   char *otherimg = NULL;
   baseimg = ft_strdup(argv[1]);
   while(true)
   {
      char tmp[1000];
      bzero(tmp, 1000);
      if (fgets(tmp, 1000, stdin) == NULL)
         break;

      otherimg = (char *)malloc(sizeof(char) * (strlen(tmp)));
      strncpy(otherimg, tmp, strlen(tmp)-1);
      otherimg[strlen(tmp)-1] = '\0';
      printf("You entered: %s\n", otherimg);
      printf("current working directory: %s\n", getcwd(NULL, 0));

      dist = exec_img_dist(baseimg, otherimg);
      
      printf("Distance: %d\n", dist);
      
      if (dist < closest_dist)
      {
         if (closest_img != NULL)
            free(closest_img);
         closest_dist = dist;
         closest_img = ft_strdup(otherimg);
         if (closest_img == NULL)
            handle_error();
      }
      if (otherimg != NULL)
         free(otherimg);
   }
   if (closest_dist == 65)
      printf("No similar image found (no comparison could be performed successfully).\n");
   else
   {
      printf("Most similar image found: '%s' with a distance of %d.\n", closest_img, closest_dist);
   }
   if (closest_img != NULL)
      free(closest_img);
   free(baseimg);
   return 0;
}
