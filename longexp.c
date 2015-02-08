#include <stdio.h>
#include "ffmdecode.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

int gwidth;
int gheight;
int firstframe = 1;
int maxframe;
int * framenos;


double * outframe;
double * counts;

void initframes( const unsigned char * frame, int linesize )
{
	int x, y;
	firstframe = 0;

	printf( "First frame got.\n" );

	outframe = malloc( gwidth * gheight * 3 * 8);
	memset( outframe, 0, gwidth * gheight * 3 * 8 );

	counts = malloc( gwidth * gheight * 8);
	memset( counts, 0, gwidth * gheight * 8 );
}

float max( float r, float g, float b )
{
	float max = (r>g)?((r>b)?r:b):((g>b)?g:b);
	return max;
}

float variance( float r, float g, float b )
{
	float min = (r<g)?((r<b)?r:b):((g<b)?g:b);
	float max = (r>g)?((r>b)?r:b):((g>b)?g:b);
	return max-min;
}

void got_video_frame( const unsigned char * rgbbuffer, int linesize, int width, int height, int frame )
{
	int line;
	int y, lx;
	float x;

	if( firstframe )
	{
		gwidth = width;
		gheight = height;
		initframes( rgbbuffer, linesize );
	}

//	if( frame % 5 == 0 )
{
	for( x = 0; x < gwidth; x++ )
	{
		for( y = 0; y < gheight; y++ )
		{
			float r = rgbbuffer[(((int)x)*3+y*linesize)+0];
			float g = rgbbuffer[(((int)x)*3+y*linesize)+1];
			float b = rgbbuffer[(((int)x)*3+y*linesize)+2];
			float or = outframe[(((int)x)*3+y*linesize)+0];
			float og = outframe[(((int)x)*3+y*linesize)+1];
			float ob = outframe[(((int)x)*3+y*linesize)+2];

/*			float amplitudeIn = sqrt( r * r + g * g + b * b );
			float amplitudeOrig = sqrt( or * or + og * og + ob * ob );

			float varianceIn = variance( r, g, b );
			float varianceOrig = variance( or, og, ob );

			//float qin = amplitudeIn;
			//float qorig = amplitudeOrig;
*/
			float qin = max( r, g, b );
			float qorig = max( or, og, ob );

			if( qin > 30 )
			{
				outframe[(((int)x)*3+y*linesize)+0] += r;
				outframe[(((int)x)*3+y*linesize)+1] += g;
				outframe[(((int)x)*3+y*linesize)+2] += b;
				counts[(((int)x)+y*linesize/3)]++;
//				printf( "%d %d %d\n", linesize/3, gwidth, gheight );
//				printf( "%f %d %d\n", counts[(((int)x)+y*linesize)], x, y );
			}
		}
	}
}

	printf( "%d %d %d\n", frame, linesize, width );

	maxframe = frame;
}

int main( int argc, char ** argv )
{
	FILE * f;
	int y;

	if( argc != 2 )
	{
		fprintf( stderr, "Error: usage: [tool] [video file]\n" );
		return -2;
	}

	int line;
	setup_video_decode();
	video_decode( argv[1] );

	char fname[1024];
	sprintf( fname, "%s.ppm", argv[1] );

	f = fopen( fname, "wb" );
	fprintf( f, "P6\n%d %d\n255\n", gwidth, gheight );

	for( y = 0; y < gheight; y++ )
	{
		int x;
		for( x = 0; x < gwidth; x++ )
		{
			unsigned char px[3];
			px[0] = outframe[(x+y*gwidth)*3+0] / counts[x+y*gwidth];
			px[1] = outframe[(x+y*gwidth)*3+1] / counts[x+y*gwidth];
			px[2] = outframe[(x+y*gwidth)*3+2] / counts[x+y*gwidth];
			fwrite( px, 1, 3, f );
		}
	}
	fclose( f );

}
