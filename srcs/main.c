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
      if (execlp("img-dist", "-v", baseimg, otherimg, NULL) == -1)
         handle_error();
   }
   else
   {
      if (wait(&status) == -1)
         handle_error();
      if (WIFEXITED(status))
         return WEXITSTATUS(status);
   }
   return 65;
}

char *get_input()
{
   char *otherimg;
   static char tmp[1000];
   bzero(tmp, 1000);
   if (fgets(tmp, 1000, stdin) == NULL)
      return NULL;

   otherimg = (char *)malloc(sizeof(char) * (strlen(tmp)-1));
   if (otherimg == NULL)
      handle_error();

   memcpy(otherimg, tmp, strlen(tmp)-1);
   otherimg[strlen(tmp)-1] = '\0';
   return otherimg;
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

   baseimg = ft_strdup(argv[1]);
   while(true)
   {
      char *otherimg = get_input();
      if (otherimg == NULL)
         break;
      // compare images
      dist = exec_img_dist(baseimg, otherimg);

      // update closest image
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
