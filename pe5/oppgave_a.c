#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>

long received = 0;

int run(size_t block_size){
    int fd[2], number_of_bytes;
    pid_t childpid;
    char *str = malloc(block_size);
    memset(str, 'a', block_size-1);
    str[block_size] = '\0';
    char *readbuffer = malloc(block_size);
    memset(readbuffer, 'd', block_size-1);
    readbuffer[block_size] = '\0';

    if ( pipe(fd) != 0 ){
        perror( "Error creating pipe. " );
        exit( EXIT_FAILURE );
    }
       
    if( ( childpid = fork() ) == -1){
        perror( "Error during forking. " );
        exit( EXIT_FAILURE );
    }

    if( childpid == 0 ){
        // Child process closes up input side of pipe s
        close( fd[0] );
        while( 1 ){
            number_of_bytes = write( fd[1], str, ( strlen( str ) + 1 ));
            if ( number_of_bytes == -1 ) {
                printf( "Error %d", errno );
                exit( EXIT_FAILURE );
            }
        }
        exit(0);
    }
    else
    {
        // Parent process closes up output side of pipe 
        printf( "PARENTPROCESS PID %d \n", getpid() );
        close( fd[1] );
        while ( 1 ){
            number_of_bytes = read( fd[0], readbuffer, strlen( readbuffer ) + 1 );
            if ( number_of_bytes == -1 ) {
                printf( "Error %d", errno );     
                exit( EXIT_FAILURE );
            }
            received += number_of_bytes;
            printf("Cummulative bytes is %ld\n", received);
        }
        exit( EXIT_SUCCESS );
    }
}



int main( int argc, char *argv[] ) {
    
    size_t block_size = 1000;

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
        block_size = strtoul( argv[1], NULL, 10 );  
    }

    printf("___Benchmark___\n");
    printf("Blocksize: %ld\n\n", block_size);

    run(block_size);
}