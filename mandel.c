/// 
//  mandel.c
//  Based on example code found here:
//  https://users.cs.fiu.edu/~cpoellab/teaching/cop4610_fall22/project3.html
//
//  Converted to use jpg instead of BMP and other minor changes
//  
///
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "jpegrw.h"
#include <pthread.h>

//struct to define thread
typedef struct {
    int start_row;
    int end_row;
    int width;
    int height;
    int max_iter;
    imgRawImage *img;
    double xmin;
    double xmax;
    double ymin;
    double ymax;
} thread_args;

// local routines
static int iteration_to_color( int i, int max );
static int iterations_at_point( double x, double y, int max );
static void compute_image( imgRawImage *img, double xmin, double xmax,
									double ymin, double ymax, int max, int num_threads);
static void show_help();
static void *compute_section(void *arg);


int main( int argc, char *argv[] )
{
	char c;

	// These are the default configuration values used
	// if no command line arguments are given.
	const char *outfile = "mandel.jpg";
	double xcenter = 0;
	double ycenter = 0;
	double xscale = 4;
	double yscale = 0; // calc later
	int    image_width = 1000;
	int    image_height = 1000;
	int    max = 1000;
	int num_threads = 1;

	// For each command line argument given,
	// override the appropriate configuration value.

	while((c = getopt(argc,argv,"x:y:s:W:H:m:o:h:t:"))!=-1) {
		switch(c) 
		{
			case 'x':
				xcenter = atof(optarg);
				break;
			case 'y':
				ycenter = atof(optarg);
				break;
			case 's':
				xscale = atof(optarg);
				break;
			case 'W':
				image_width = atoi(optarg);
				break;
			case 'H':
				image_height = atoi(optarg);
				break;
			case 'm':
				max = atoi(optarg);
				break;
			case 'o':
				outfile = optarg;
				break;
			case 't':
    			num_threads = atoi(optarg);
    			break;
			case 'h':
				show_help();
				exit(1);
				break;
		}
	}

	// Calculate y scale based on x scale (settable) and image sizes in X and Y (settable)
	yscale = xscale / image_width * image_height;

	// Display the configuration of the image.
	printf("mandel: x=%lf y=%lf xscale=%lf yscale=%1f max=%d outfile=%s threads=%d\n",xcenter,ycenter,xscale,yscale,max,outfile, num_threads);

	// Create a raw image of the appropriate size.
	imgRawImage* img = initRawImage(image_width,image_height);

	// Fill it with a black
	setImageCOLOR(img,0);

	// Compute the Mandelbrot image

	compute_image(img,xcenter-xscale/2,xcenter+xscale/2,ycenter-yscale/2,ycenter+yscale/2,max, num_threads);

	// Save the image in the stated file.
	storeJpegImageFile(img,outfile);

	// free the mallocs
	freeRawImage(img);

	return 0;
}




/*
Return the number of iterations at point x, y
in the Mandelbrot space, up to a maximum of max.
*/

int iterations_at_point( double x, double y, int max )
{
	double x0 = x;
	double y0 = y;

	int iter = 0;

	while( (x*x + y*y <= 4) && iter < max ) {

		double xt = x*x - y*y + x0;
		double yt = 2*x*y + y0;

		x = xt;
		y = yt;

		iter++;
	}

	return iter;
}

/*
Compute an entire Mandelbrot image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax), limiting iterations to "max"
*/

void compute_image(imgRawImage* img, double xmin, double xmax,
                   double ymin, double ymax, int max, int num_threads)
{
    if (num_threads < 1)
	{
		num_threads = 1;
	} 
    if (num_threads > 20)
	{
		num_threads = 20;
	} 
    if (num_threads > (*img).height)
    {
        num_threads = (*img).height;
    } 

    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    thread_args *args = malloc(num_threads * sizeof(thread_args));

    int rows_per_thread = (*img).height / num_threads;

    for (int t = 0; t < num_threads; t++) {
        args[t].start_row = t * rows_per_thread;
        args[t].end_row = (t == num_threads - 1) ? (*img).height : (t + 1) * rows_per_thread;
        args[t].width = (*img).width;
        args[t].height = (*img).height;
        args[t].max_iter = max;
        args[t].img = img;
        args[t].xmin = xmin;
        args[t].xmax = xmax;
        args[t].ymin = ymin;
        args[t].ymax = ymax;

        if (pthread_create(&threads[t], NULL, compute_section, (void *)&args[t]) != 0) {
            fprintf(stderr, "Error creating thread %d\n", t);
            exit(EXIT_FAILURE);
        }
    }

    for (int t = 0; t < num_threads; t++) {
        pthread_join(threads[t], NULL);
    }

    free(threads);
    free(args);
}


/*
Convert a iteration number to a color.
Here, we just scale to gray with a maximum of imax.
Modify this function to make more interesting colors.
*/
int iteration_to_color( int iters, int max )
{
	int color = 0xFFFFFF*iters/(double)max;
	return color;
}


// Show help message
void show_help()
{
	printf("Use: mandel [options]\n");
	printf("Where options are:\n");
	printf("-m <max>    The maximum number of iterations per point. (default=1000)\n");
	printf("-x <coord>  X coordinate of image center point. (default=0)\n");
	printf("-y <coord>  Y coordinate of image center point. (default=0)\n");
	printf("-s <scale>  Scale of the image in Mandlebrot coordinates (X-axis). (default=4)\n");
	printf("-W <pixels> Width of the image in pixels. (default=1000)\n");
	printf("-H <pixels> Height of the image in pixels. (default=1000)\n");
	printf("-o <file>   Set output file. (default=mandel.bmp)\n");
	printf("-h          Show this help text.\n");
	printf("\nSome examples are:\n");
	printf("mandel -x -0.5 -y -0.5 -s 0.2\n");
	printf("mandel -x -.38 -y -.665 -s .05 -m 100\n");
	printf("mandel -x 0.286932 -y 0.014287 -s .0005 -m 1000\n\n");
}

void *compute_section(void *arg) 
{
    thread_args *t = (thread_args *)arg;
    int width = (*t).width;
    int height = (*t).height;

    for (int j = (*t).start_row; j < (*t).end_row; j++) 
    {
        for (int i = 0; i < width; i++) {
            double x = (*t).xmin + i * ((*t).xmax - (*t).xmin) / width;
            double y = (*t).ymin + j * ((*t).ymax - (*t).ymin) / height;
            int iters = iterations_at_point(x, y, (*t).max_iter);
            setPixelCOLOR((*t).img, i, j, iteration_to_color(iters, (*t).max_iter));
        }
    }
    return NULL;
}
