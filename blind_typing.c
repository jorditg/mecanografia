#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <termios.h>


void disable_getchar_buffer(struct termios *old_tio)
{
	struct termios new_tio;
	unsigned char c;

	/* get the terminal settings for stdin */
	tcgetattr(STDIN_FILENO,old_tio);

	/* we want to keep the old setting to restore them a the end */
	new_tio=*old_tio;

	/* disable canonical mode (buffered i/o) and local echo */
	new_tio.c_lflag &=(~ICANON & ~ECHO);

	/* set the new settings immediately */
	tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);
}

void enable_getchar_buffer(struct termios *old_tio)
{
	/* restore the former settings */
	tcsetattr(STDIN_FILENO,TCSANOW,old_tio);
}

void help(int i)
{
  const unsigned char col[3][10] = {"central", "arriba", "abajo"};
  const unsigned char f[5][10] = {"meñique", "anular", "medio", "indice", "indice2"};

  int r = i/10;
  int c = (i - r*10);
  printf("\ni=%d, r=%d, c=%d\n", i,r,c);
  
  unsigned char hand[10];
  strcpy(hand, (c/5==0)?"Izquierda":"Derecha");
  
  unsigned char finger[10];
  strcpy(finger, (c>=5?f[5-(c-5)-1]:f[c]));
  
  printf("%s, %s, %s\n", col[r], hand, finger); 
}

int main()
{

   srand(time(NULL));
   
   struct termios old_tio;
   
   disable_getchar_buffer(&old_tio);
   
   int correctos = 0;
   int total = 0;

   unsigned char s[31] ="asdfghjklñqwertyuiopzxcvbnm,.-";
   s[9] = 195; // enye
   unsigned char c; // actual char
   unsigned char p; // previous char

   int random_from = 0;
   int random_to = 29;
   unsigned char val;

   printf("Medio(m), Abajo(b), Arriba(a), Todo(t)\n");
   c = getc(stdin);
   if(c=='m') {
	  random_from = 0; random_to = 9; 
   } else if(c=='b') {
	  random_from = 20; random_to = 29; 
   } else if(c == 'a') {
	  random_from = 10; random_to = 19; 
   }

   if(c == 'm' || c == 'b' || c == 'a')
   {
     printf("Izquierda(i), Derecha(d), Todo(t)\n");
     c = getc(stdin);
     if(c=='i') {
	  random_to -= 5; 
     } else if(c=='d') {
	  random_from += 5;  
     } 
   }
   
   printf("\nPulsa la tecla indicada: (0: fin, espacio: ayuda)\n");
   time_t begin = time(NULL); // inicio del cronometraje
   do {
     int rnd = (rand() % (random_to - random_from + 1)) + random_from;
     val = s[rnd];

     if(rnd==9) // enye
       printf("%d: enye", total);
     else
       printf("%d: %c", total, val);

     do {
	 p = c;
         c = getc(stdin);
	 //printf("%d",c);
	 total++;
         if(c == '0') {
             total--;
	     break;
	 } else if(c == val) {
	   printf("\n");
           correctos++;
	   if(rnd==9) // enye
             getc(stdin); // desestima otro valor del buffer
         } else if(c == 'h') {
           total--;
           for(int i=0;i<30;i++) {
	       if(s[i] == val) {
	         help(i);
	         break;
	       }
           }
	   c = p;
         } else {
           printf(" MAL!\n");
	 }
       } while (c != '0' && c != val);
   } while (c != '0');

   time_t end = time(NULL);
   time_t seconds = end - begin; // fin cronometraje
   struct tm * now = localtime(&end);

   printf("\nFecha    \tHora\tTotal\tAcierto\tFallo\tPCTA\tPPM\n");
   printf("%d/%d/%d\t", now->tm_mday, now->tm_mon, now->tm_year + 1900);
   printf("%d:%d\t", now->tm_hour, now->tm_min);
   printf("%d\t", total);
   printf("%d\t", correctos);
   printf("%d\t", total - correctos);
   printf("%2.1f\t", (float) (correctos*100)/(float) total);
   printf("%3.1f\n", (float) correctos * 60.0/ (float) seconds);
   enable_getchar_buffer(&old_tio);
}


