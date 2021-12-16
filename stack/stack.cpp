
//
//   stack.cpp : Implementation of a simple integer stack
//
//   This implementation allows up to 6-digit integers
//
//      To compile:  gcc -o stack stack.cpp -lpthread
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define STACK_MAX 255

char *myName = (char *) "stack";

// I added
static int stack[STACK_MAX+1];	//stack data structure
static int stackPos = 0;		//stack position

static void Usage()
{
    printf ("Available options are:\n");
    printf ("   q : quit\n");
    printf ("   p : Push <int>\n");
    printf ("   o : Pop\n");
}

// Skip characters until first blank character
static char * skip_to_blank(char *s){
	while ((s != NULL) && (*s != ' ')){
		s++;
	}
	return s;
}

// Skip all blank characters 
static char * skip_blanks(char *s) {
	while ((s != NULL) && (*s == ' ')){
		s++;
	}
	return s;
}

// static void clear_line(char *s, int size){
	// for (char c = *s;c;*++s){c = '\0';}
// }
static void Push (char *s)
{
	// I implemented 
	char newS[256];		//char array for digit string
	int pos = 0;		//newS position
	
	// Get digit from input string
	s = skip_to_blank(s);
	s = skip_blanks(s);
	
	//convert digit string to int, push to stack, increment stack position
	if (stackPos < STACK_MAX){
		stack[stackPos] = atoi(s);
		stackPos++;
	}	
	
	// clear input string
	//clear_line(s, sizeof(s));
}

static void Pop ()
{
	// I implemented
	// check if stack is empty
	if (stackPos == 0){
		printf("The stack is empty - nothing to pop!\n");
	}
	//	if stack is not empty, pop
	else {
		stackPos--;
		printf("%d\n",stack[stackPos]);	
	}
}

static char c = '\0';
static void *getCmdFunc (void *p)
{
    char line[256], cmd[256];
    int  opt;

    while (fgets(line, sizeof(line), stdin))
    {
        switch (line[0])
        {
            case 'q' : c = 'q';             break;
            case 'p' : Push (line);         break;
            case 'o' : Pop ();              break;
            default: Usage ();              break;
        }
        usleep (100);
		*line = '\0';
    }
    return NULL;
}

int main (int argc, char *argv[])
{
    pthread_t tid;

    //  Start thread to read input commands.
    int err = pthread_create(&tid, NULL, &getCmdFunc, NULL);
    if (err != 0)
    {
        printf ("%s_%s: Thread Creation Failed ...\n", argv[0], __func__ );
        exit (1);
    }        

    //  Loop Forever. Executing commands.
    printf ("%s_%s: Waiting for commands...\n", argv[0], __func__);
	while (c != 'q')
	{
		usleep(100);
	}    
}
