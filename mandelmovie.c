/**********************************************
*  Filename: mandelmovie.c
*  Description: create movie
*  Author: Elias Mouthey
*  Date: 11/14/2025
***********************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_FRAMES 50

int main(int argc, char *argv[]) 
{
    int num_procs = 1;
    int num_threads = 1;
    int opt;

    // Parse command line argument: -n <num processes>
    while ((opt = getopt(argc, argv, "n:t:")) != -1) 
    {
        switch (opt) 
        {

            case 'n':
                num_procs = atoi(optarg);
                break;
            case 't':
                num_threads = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s -n <num_processes> -t <num_threads>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (num_procs < 1)
    {
        num_procs = 1;
    } 
    if (num_procs > NUM_FRAMES)
    {
        num_procs = NUM_FRAMES;
    }

    printf("Generating %d frames using %d processes and %d threads...\n", NUM_FRAMES, num_procs, num_threads);

    double xcenter = -0.5;
    double ycenter = 0.0;
    double scale = 4.0;

    int frame = 0; //frame count
    int active = 0; //active process count

    while (frame < NUM_FRAMES)
    {
        if (active < num_procs) 
        {
            pid_t pid = fork();
            if (pid == 0) 
            { 
                char outfile[64];
                snprintf(outfile, sizeof(outfile), "frame%02d.jpg", frame);

                char xstr[32], ystr[32], sstr[32], mstr[16], tstr[32]; 
                snprintf(xstr, sizeof(xstr), "%f", xcenter); //x
                snprintf(ystr, sizeof(ystr), "%f", ycenter); //y
                snprintf(sstr, sizeof(sstr), "%f", scale); //scale
                snprintf(mstr, sizeof(mstr), "%d", 1000 + frame * 50); 
                snprintf(tstr, sizeof(tstr), "%d", num_threads);

                execl("./mandel", "mandel",
                       "-x", xstr,
                       "-y", ystr,
                       "-s", sstr,
                       "-m", mstr,
                       "-o", outfile,
                       "-t", tstr,
                       (char *)NULL);

                perror("execl failed");
                exit(EXIT_FAILURE);
                 } else if (pid > 0) 
                 {
                // Parent
                frame++;
                active++;
                scale *= 0.95;  // zooms in each time
            } else 
            {
                //if fork fails, exit
                perror("fork failed");
                exit(EXIT_FAILURE);
            }
        } else 
        {
            //waits for the child process to finish
            wait(NULL);
            active--;
        }
    }
    while (active > 0)
    {
        wait(NULL);
        active --;
    }

    printf("Done");
    return 0;
}


    
