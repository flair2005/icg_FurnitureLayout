#include <windows.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <math.h>
#include <string.h>

#include "icg.h"
#include "objViewer.h"

GlutMaster *glutMaster;
TobjViewer *objViewer;

int main ( int argc, char ** argv )
{
	int nx = 400, ny = 400;
	int i;
	char obj_file[FILE_NAME_LENGTH];
	obj_file[0] = '\0';

    i=1;
    while(i<argc) {
        if(!strcmp(argv[i], "-win")) {
            sscanf(argv[i+1], "%d", &nx);
            i++;
            sscanf(argv[i+1], "%d", &ny);
            i++;

		} else if(!strcmp(argv[i], "-obj")) {
			sscanf(argv[i+1], "%s", obj_file);
			i++;
		}
        i++;
    }


	glutMaster = new GlutMaster();
	objViewer = new TobjViewer(glutMaster
								, Tvector2<int>(nx, ny)
								, Tvector2<int>(0,0)
								, "Obj Display"
								, obj_file);
	glutMaster->CallGlutMainLoop();

	return(TRUE);
}

