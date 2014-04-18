// Dynamical Systems - basins of attraction drawer
// autonomous system
// potential field with 2 minima
// (modify the rk4 function to inspect a different system)

// Below, you can find informations about how to make this code work.
// Check the comments and you should find an answer to problems that could arise.

// COMPILATION
// 	A C compiler is needed in order to compile this code.
// 	gcc is good for this task, just launch a shell and type:
// 		gcc basins.c -o basins.o

// OUTPUT (main function)
//	 0 : execution completed correctly
//	-1 : wrong number of input
//	-2 : wrong values for #horizPixels and/or #vertPixels

// USAGE
//	./basins.o #horizPixels #vertPixels
// 	where #horizPixels is the integer number (>0) of horizontal pixels
// 	and #verPixels is the integer number (>0) of vertical pixels of the
// 	output image. Use the same number for horizontal and vertical pixels
// 	in order to get a better image (square).

// The comments and the code refer to a unix-like machine.

// DISTRIBUTIONS
// 	Feel free to modify the code and distribute it to whoever you want!

// INFO
// 	skills.avid@gmail.com
// 	If you enjoy this code, please send me an e-mail!

// WEBSITE
//	wwww.paolovagnini.blogspot.it
//	For more code from this website, please send an e-mail to the address
//	in the INFO section.

// GITHUB
//	skillsAvid

// DATE
// 	october 2012
// 	minor revisions: april 2014

// NOTE
// 	when i first wrote the code i was not as good as i am now at coding
// 	- and now i'm still a bad coder, of course!
//	So, please forgive me for the bad stuff you'll find in here!
//	I could make this code much better, but for now i think i'll stop here.
//	Maybe one day i'll create a repo on github ( skillsAvid ) with this project.

// Enjoy the beauty of basins of attractions and dynamical systems!


#include <stdio.h>
#include <stdlib.h> 
#include <math.h>
#define RK4_ITERATIONS 1000

typedef struct {
    float x;
    float y;
} point;

typedef struct {
    int i; // x coordinate of the pixel
    int j; // y coordinate of the pixel
} pixel;

typedef struct {
    point lowLeft;
    point topRight;
    int xPixels;
    int yPixels;
} grid;



// ** FUNCTION PROTOTYPES **
pixel pixelFromPoint(point p, float deltaX, float deltaY, float negativeXPixels, float negativeYPixels); // Returns the pixel corresponding to the point in the grid
point rk4(point p); // Returns the point passed as argument, after one iteration using RK4
point evolveUsingRK4(point p); // Returns the point passed as argument, after 1000 iteration using RK4


int main(int argc, const char * argv[]) {

    // Check that all the needed input are passed as argument from shell to the program
    if (argc != 3) {
    	printf("Wrong input! Usage: basins.o #horizPixels #vertPixels\n");
	return -1;
    }
    // The number of horizontal and vertical pixels are, respectively, the first
    // and second arguments after the program name (from shell)
    unsigned int const horizPixels = atoi(argv[1]);
    unsigned int const vertPixels = atoi(argv[2]);
    if (horizPixels <= 0 || vertPixels <= 0) {
    	printf("Wrong input: number of rows and columns must be positive integers!\n");
    	return -2;
    }

    printf("\n\n\t***\n");
    printf("Input set correctly: running program %s\n", argv[0]);
    printf("for %d horizontal and %d vertical pixels\n", horizPixels, vertPixels);

    // modify xMin,xMax,yMin,yMax if you want to change the range of the inspected window
    // (for this system, the interesting part happens around the points (-0.5, 0.0), (0.5,0.0)
    double const xMin = -2.;
    double const xMax =  2.;
    double const yMin = -2.;
    double const yMax =  2.;
    printf("--\n");
    printf("\tthe ranges of the inspected window are\n"\
    	    "\t\tyMax:%8.2f\nxMin:%8.2f\t\t\txMax:%8.2f\n\t\tyMin:%8.2f\n",\
    					yMax, xMin, xMax, yMin);
    float const deltaX = (xMax - xMin)/horizPixels;
    float const deltaY = (yMax - yMin)/vertPixels;
    float negativeXPixels = abs(xMin)/deltaX;
    float negativeYPixels = abs(yMin)/deltaY;

    int matrix[vertPixels][horizPixels];
    const int startColor = 3; // change this to change the colors of the plot
    int actualColor = startColor;
    
    // Set all the point in the matrix to color 1
    int row;
    int column;

    for (row=0; row<vertPixels; row++) {
        for (column=0; column<horizPixels; column++) {
            matrix[row][column] = startColor;
        }
    }

    // change filesPath if you want to save the plot somewhere else
    char filesPath[] = "./";
    char dataFile[100];
    sprintf(dataFile,"%s%s",filesPath,"basinsData.dat");
    FILE *basinsId = fopen(dataFile, "w");
    
    // How this works:
    // Scan all the matrix of points
    //  -convert the pixel into point
    //  -save the point as "ic" (initial condition)
    //  -make it evolve in time for 1000 steps of Runge-Kutta 4 algorithm (for systems of 2 equations)
    //  -check if the absolute value of the point is less than the width of the inspected window
    //  -transform the point into pixel
    //  -check if the pixel is yet colored
    //     -if it is not colored, color it and the initial point with a new color
    //	    (increment actual color and then assign the actual color)
    //     -if it is colored, color the initial point with actual color
    point ic; // actual initial condition
    ic.x = xMin;
    ic.y = yMin;
    printf("--\n");
    printf("calculations start now, please wait... :) :| :( ...\n");
    for (row=0; row<vertPixels; row++) {
        for (column=0; column<horizPixels; column++) {
            
            point endPoint = evolveUsingRK4(ic);
            pixel reachedPixel = pixelFromPoint(endPoint, deltaX, deltaY, negativeXPixels, negativeYPixels);
            
            if (matrix[reachedPixel.j][reachedPixel.i] == startColor) {
                actualColor++;
                // Color the reached pixel
                matrix[reachedPixel.j][reachedPixel.i] = actualColor;
            }
    		
            // Color the i.c.
            matrix[row][column] = matrix[reachedPixel.j][reachedPixel.i];

            // WRITE TO FILE (column --> x ; row --> y ; pixelColor)
            fprintf(basinsId, "%d %d %d\n", column, row, matrix[row][column]);
            
            ic.x += deltaX;
        } // Column loop
        ic.x = xMin;
        ic.y += deltaY;
    } // Row loop
    
    printf("--\n");
    printf("calculations ended\n");

    // Close the file with basins data
    fclose(basinsId);

    // Use gnuplot to plot the data
    // (this may not be the best way to plot with gnuplot,
    //  but i'm sure you'll find a best way to accomplish this task ;) )
    char plotFile[200];
    sprintf(plotFile,"%sbasinsPlot.png",filesPath); // destination of the plot file
    char plotScript[500];
    sprintf(plotScript,"set term png size %i,%i\n", horizPixels, vertPixels);
    sprintf(plotScript,"%sset output '%s'\n", plotScript, plotFile);
    sprintf(plotScript,"%sset lmargin 0 ; set rmargin 0 ; set tmargin 0 ; set bmargin 0\n", plotScript);
    sprintf(plotScript,"%splot '%s' with dots lc variable\n", plotScript, dataFile);
    char scriptFile[200];
    sprintf(scriptFile, "%sscript.gp", filesPath);
    FILE *scriptId = fopen(scriptFile, "w");
    fprintf(scriptId, "%s", plotScript);
    fclose(scriptId);
    char plotCommand[200];
    sprintf(plotCommand, "gnuplot %s", scriptFile);
    system(plotCommand);
    // remove data file and script file
    char rmCommand[300];
    sprintf(rmCommand, "rm %s %s", dataFile, scriptFile);
    system(rmCommand); // comment this line if you want to keep the data and the script files

    // ask the user if she wants to open the plot file
    // (a program is needed in order to open the png plot file)
    printf("--\n");
    printf("plot file is at %s\n", plotFile);
    printf("do you want to open it?\n(type  y  if yes or type  n  if no and press enter): ");
    char plotAnswer;
    scanf("%c", &plotAnswer);
    getchar();
    while (plotAnswer != 'y' && plotAnswer != 'n') {
        printf("please just type the letter  y  or the letter  n  and then press enter: ");
        scanf("%c", &plotAnswer);
	getchar();
    }
    if (plotAnswer == 'y') {
        char openCommand[200];
	sprintf(openCommand, "open %s", plotFile);
	system(openCommand);
    }

    printf("--\n");
    printf("program execution finished correctly, peace!\n");

    printf("\t***\n\n");

    return 0;
}

point rk4(point p) {
    const double a = 0.25;
    const double b = 1.;
    const double c = 0.05;
    const double deltaT = 0.5; // change this parameter if a different precision is needed
    double xNew, yNew;
    double AX, AY, BX, BY, CX, CY, DX, DY;
    
    AX = (p.y)*deltaT;
    AY = -c*(p.y) + a*(p.x) - b*pow(p.x, 3);
    
    BX = ( (p.y) + AY/2. ) * deltaT;
    BY = ( -c*( (p.y) + AY/2. ) + a*( (p.x) + AX/2. ) - b*pow(( (p.x) + AX/2.), 3) ) * deltaT;
    
    CX = ( (p.y) + BY/2. ) *deltaT;
    CY = ( -c*( (p.y) + BY/2. ) + a*( (p.x) + BX/2. ) - b*pow(( (p.x) + BX/2.), 3) ) * deltaT;
    
    DX = ( (p.y) + CY )*deltaT;
    DY = ( -c*( (p.y) + CY ) + a*( (p.x) + CX ) - b*pow(( (p.x) + CX), 3) ) * deltaT;
    
    xNew = p.x + (AX + BX/2. + CX/2. + DX)/6.;
    yNew = p.y + (AY + BY/2. + CY/2. + DY)/6.;
    
    p.x = xNew;
    p.y = yNew;
    
    return p;
}

point evolveUsingRK4(point p) {
    int i;
    for (i=0; i<RK4_ITERATIONS; i++) {
        p = rk4(p);
    }
    return p;
}

pixel pixelFromPoint(point p, float deltaX, float deltaY, float negativeXPixels, float negativeYPixels) {
    pixel pix;
    float xTemp = p.x/deltaX + negativeXPixels;
    float yTemp = p.y/deltaY + negativeYPixels;
    pix.i = xTemp;
    pix.j = yTemp;
    return pix;
}
