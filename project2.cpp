// sh ./filename.sh
// g++ -Xpreprocessor -fopenmp  -I/opt/homebrew/Cellar/libomp/14.0.0/include -L/opt/homebrew/Cellar/libomp/14.0.0/lib -lomp  *.cpp -o main
#include <stdio.h>
#include <omp.h>
#include <math.h>

using namespace std;

// You could use a single for-loop over all the nodes that looks like this:
/*#pragma omp parallel for default(none) . . .
for( int i = 0; i < NUMNODES*NUMNODES; i++ )
{
	int iu = i % NUMNODES;
	int iv = i / NUMNODES;
	float z = Height( iu, iv );
	. . .
}

. . .*/

// Or, you could also use the <i>collapse</i> OpenMP clause:
// The (2) means you are collapsing 2 nested for-loops into one
// The end effect is exactly like what is shown above, but without you needing to do the mod and divide
/*#pragma omp parallel for collapse(2) default(none) . . .
for( int iv = 0; iv < NUMNODES; iv++ )
{
	for( int iu = 0; iu < NUMNODES; iu++ )
	{
		float z = Height( iu, iv );
		. . .
	}
}*/

#ifndef NUMT
#define NUMT 2
#endif

#ifndef NUMNODES
#define NUMNODES 4
#endif


const float N = 2.5f;
const float R = 1.2f;

float
Height( int iu, int iv )	// iu,iv = 0 .. NUMNODES-1
{
	float x = -1.  +  2.*(float)iu /(float)(NUMNODES-1);	// -1. to +1.
	float y = -1.  +  2.*(float)iv /(float)(NUMNODES-1);	// -1. to +1.

	float xn = pow( fabs(x), (double)N );
	float yn = pow( fabs(y), (double)N );
	float rn = pow( fabs(R), (double)N );
	float r = rn - xn - yn;
	if( r <= 0. )
	        return 0.;
	float height = pow( r, 1./(double)N );
	return height;
}

// main

#define XMIN     -1.
#define XMAX      1.
#define YMIN     -1.
#define YMAX      1.

float Height( int, int );	// function prototype

int main( int argc, char *argv[ ] )
{
	#ifndef _OPENMP
        printf("No OpenMP support!\n" );
        return 1;
	#endif

	omp_set_num_threads( NUMT );

	// the area of a single full-sized tile:
	// (not all tiles are full-sized, though)

	float fullTileArea = (  ( ( XMAX - XMIN )/(float)(NUMNODES-1) )  *
				( ( YMAX - YMIN )/(float)(NUMNODES-1) )  );

	// sum up the weighted heights into the variable "volume"
	// using an OpenMP for-loop and a reduction:

	float volume = 0;

	double time1 = omp_get_wtime( );

	#pragma omp parallel for collapse(2) default(none) shared(fullTileArea) reduction(+:volume)
	for (int iv = 0; iv < NUMNODES; iv++)
	{
		for (int iu = 0; iu < NUMNODES; iu++)
		{
			float z = Height(iu, iv);
			if ((iu == 0 || iu == NUMNODES - 1) || (iv == 0 || iv == NUMNODES - 1)){
				if ((iu == 0 || iu == NUMNODES - 1) && (iv == 0 || iv == NUMNODES - 1))
					volume += fullTileArea / 4. * z;
				else
					volume += fullTileArea / 2. * z;
			}
			else
				volume += fullTileArea * z;
		}
	}

	double time2 = omp_get_wtime( );

	printf("Threads: %2d ; Nodes on each side edge: %4d ; Volume: %.4f ; Mega heights / sec: %6.3lf\n", NUMT, NUMNODES, volume, (double)(NUMNODES * NUMNODES) / (time2 - time1) / 1000000.);
}
