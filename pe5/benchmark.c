#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>

long received = 0;
long double bandwith;


void sig_handler(int signum){
    // As it is called once every second, not nescessary for now to divide by elapsed time ( 1 sec ) - Might want to change this for higher accuracy/ error handling" 

    printf("Bandwidth (B/s): %LF | %f \n", bandwith);
    
    bandwith = 0;
    
    alarm(1);
}

void sigusr_handler(int signum){
    printf("Current accumulated received bytes: %ld \n", received);
}

int run(int block_size){
    int fd[2], number_of_bytes;
    pid_t childpid;
    char *str = malloc(block_size);
    memset(str, 'a', block_size-1);
    str[block_size] = '\0';
    char *readbuffer = malloc(block_size);
    memset(readbuffer, 'd', block_size-1);
    readbuffer[block_size] = '\0';
    signal(SIGALRM, sig_handler);
    signal(SIGUSR1, sigusr_handler);

    if ( pipe(fd) != 0 ){
        perror( "Error creating pipe. " );
        exit( EXIT_FAILURE );
    }
       
    if( ( childpid = fork() ) == -1){
        perror( "Error during forking. " );
        exit( EXIT_FAILURE );
    }

    if( childpid == 0 ){
        /* Child process closes up input side of pipe */
        close( fd[0] );
        //int j = 0;
        //printf("len %ld \n", strlen(str) + 1);
        while( 1 ){
            number_of_bytes = write( fd[1], str, ( strlen( str ) + 1 ));
            if ( number_of_bytes == -1 ) {
                printf( "Error %d", errno );
                exit( EXIT_FAILURE );
            }
            //j += 1;
            //printf("SENT: %d\n", number_of_bytes);
        }
        exit(0);
    }
    else
    {
        /* Parent process closes up output side of pipe */
        printf( "PARENTPROCESS PID %d \n", getpid() );
        close( fd[1] );
        //int i = 0;
        alarm( 1 ); //first alarm
        //printf("len %ld \n", strlen(readbuffer) + 1);
        // Setting start timestamp
        while ( 1 ){
            number_of_bytes = read( fd[0], readbuffer, strlen( readbuffer ) + 1 );
            if ( number_of_bytes == -1 ) {
                printf( "Error %d", errno );     
                exit( EXIT_FAILURE );
            }
            //printf("RECIEVED: %d\n", number_of_bytes);
            received += number_of_bytes;
            bandwith += number_of_bytes;
            //i += 1;
            //printf("Cummulative bytes is %d and string is %s \n", received, readbuffer);
        }
        exit( EXIT_SUCCESS );
    }
}



int main( int argc, char *argv[] ) {
    
    int block_size = 1000;

    if (argc > 2) {
        printf("Too many arguments. Program takes only one argument!\n");
        exit(EXIT_FAILURE);
    }

    else if ( argc < 2) {
        printf("No argument given. Using standard block size of 1000 bytes.\n");
    }
    
    else {
        //Checking input    
        int i = 0;
        while( i < strlen( argv[1] )){
            if (! isdigit( argv[1][i] ) ){
                printf( "Argument must be a valid number!\n" );
                exit( EXIT_FAILURE );
            }
            i++;
        }
        block_size = atoi( argv[1] );  
    }

    printf("___Benchmark___\n");
    printf("Blocksize: %d\n\n", block_size);

    run(block_size);
}