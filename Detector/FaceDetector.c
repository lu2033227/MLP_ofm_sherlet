/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "FaceDetector.h"
#include "DCR.h"
#include "FaceFinderRef.h"

typedef struct tagNevenFaceDetector
{
	struct bpi_DCR dcr;		// data carrier handle
	struct bpi_FaceFinderRef fd;	// face finder
} NevenFaceDetector;

// ---------------------------------------------------------------------------

void* detector_create(int w, int h, int maxFaces, const char* model)
{
	NevenFaceDetector* detector = (NevenFaceDetector*)malloc(sizeof(NevenFaceDetector));

    // load the configuration file
    const int MAX_FILE_SIZE = 65536;
    void* initData = malloc( MAX_FILE_SIZE ); /* enough to fit entire file */
    //memcpy(initData, model, MAX_FILE_SIZE);
    FILE* filedesc = fopen(model, "rb");
    int initDataSize = fread(initData, 1, MAX_FILE_SIZE, filedesc);
    fclose(filedesc);

    // --------------------------------------------------------------------

    bpi_DCR_create(&detector->dcr, w, h);

	bpi_FaceFinderRef_init( &detector->fd );
	bpi_FaceFinderRef_memRead( &detector->fd,
		w, h, initData );
    bpi_FaceFinderRef_setRange(&detector->fd, 40, w / 2); /* set eye distance range */

    // free the configuration file
    free(initData);

    return detector;
}

void detector_destroy(void* hdetector)
{
	NevenFaceDetector* detector = (NevenFaceDetector*)hdetector;

	if (detector) {
		bpi_FaceFinderRef_exit(&detector->fd);
		bpi_DCR_exit(&detector->dcr);
	}
}

int detector_process(void* hdetector, const unsigned char* image, int width, int height)
{
	NevenFaceDetector* detector = (NevenFaceDetector*)hdetector;
	int numberOfFaces = 0;

    // run detection
    bpi_DCR_assignGrayByteImage(&detector->dcr, image, width, height);
    
    if (bpi_FaceFinderRef_putDcr(&detector->fd, &detector->dcr) == 0) {
        numberOfFaces = bpi_FaceFinder_faces(&detector->fd);
    }

    return numberOfFaces;
}

void detector_get_face(void* hdetector, TPointF* points, int index)
{
	NevenFaceDetector* detector = (NevenFaceDetector*)hdetector;
    bpi_Node leftEye, rightEye;

	bpi_FaceFinderRef_getDcr(&detector->fd, index, &detector->dcr);
	if (!bpi_DCR_approved(&detector->dcr)) {
		return;
	}
    bpi_DCR_getNode(&detector->dcr, 0, &leftEye);
    bpi_DCR_getNode(&detector->dcr, 1, &rightEye);

    points[0].x = (float)leftEye.x / (1 << 16);	// Q16
	points[0].y = (float)leftEye.y / (1 << 16);
	points[1].x = (float)rightEye.x / (1 << 16);
	points[1].y = (float)rightEye.y / (1 << 16);
}
