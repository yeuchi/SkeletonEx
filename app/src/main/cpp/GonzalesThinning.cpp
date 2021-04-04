//
// Created by yeuchi on 12/30/20.
//
// ==================================================================
// Module:		GonzalesThinning.cpp
//
// Description:	Thinning algorithm as defined in the text,
//				Digital Image processing by Gonzales & Woods, 1993.
//				Pages (492-493), 8.1.5 Skeleton of a Region.
//
//				Whole thing can be done in pixelbender but let me
//				try it out in actionscript first.
//
// Input:		Image with bounding pixels of a closed shape over
//				white backgroun.
//
// Output:		skeletonized line
//
// Author(s):	Chi T. Yeung	(cty)
//
// History: - port from actionscript to C++ 12/30/2020
// ==================================================================
#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>
#include "Common.h"
#include "GonzalesThinning.h"

//////////////////////////////////////////////////////////////////
// Construct / Destruct

GonzalesThinning::GonzalesThinning() {
}

GonzalesThinning::~GonzalesThinning() {

}

//////////////////////////////////////////////////////////////////
// Public methods

/*
 * Iterate Thinning algorithm from Gonzales Text until skeleton remains
 * [in] require a binary image b&w
 * [out] return a binary skeleton image
 */
bool GonzalesThinning::Iterate(AndroidBitmapInfo infoSource,
                               void *pixelsSource,
                               AndroidBitmapInfo infoResult,
                               void *pixelsResult) {
    bDeleted = true;
    while (bDeleted) {
        bDeleted = Apply(infoSource, pixelsSource,
                         infoResult, pixelsResult);
    }
    return bDeleted;
}

/*
 * Apply 1 iteration
 * Assumption:
 * - black pixels (0) as blog to be skeletonized
 * - white pixels (255, 255, 255) as background
 *
 */
bool GonzalesThinning::Apply(AndroidBitmapInfo infoSource,
                             void *pixelsSource,
                             AndroidBitmapInfo infoResult,
                             void *pixelsResult) {
    bool marked = false;
    if (thinning(infoSource, pixelsSource, infoResult, pixelsResult, STEP1)) {
        marked = true;
    }
    if (thinning(infoSource, pixelsSource, infoResult, pixelsResult, STEP2)) {
        marked = true;
    }
//    if(marked)
//        deleteBorder(infoSource, pixelsSource, infoResult, pixelsResult);

    return marked;
}

//////////////////////////////////////////////////////////////////
// Protected methods

bool GonzalesThinning::thinning(AndroidBitmapInfo infoSource,
                                void *pixelsSource,
                                AndroidBitmapInfo infoResult,
                                void *pixelsResult,
                                int step) {
    void *currentPixelsSource;
    void *abovePixelsSource;
    void *belowPixelsSource;

    bool bDelete = false;

    for (int y = 1; y < infoSource.height - 2; y++) {
        currentPixelsSource = (char *) pixelsSource + (infoSource.stride * y);
        abovePixelsSource = (char *) pixelsSource + (infoSource.stride * (y-1));
        belowPixelsSource = (char *) pixelsSource + (infoSource.stride * (y+1));

        rgba *srcline = (rgba *) currentPixelsSource;
        rgba *destline = (rgba *) pixelsResult;

        rgba *aboveline = (rgba *) abovePixelsSource;
        rgba *belowline = (rgba *) belowPixelsSource;

        for (int x = 1; x < infoSource.width - 2; x++) {

            // if pixel is green
            if (srcline[x].green == 255 && srcline[x].blue == 0 && srcline[x].red == 0) {
                /*
                 *  p9 | p2 | p3
                 *  p8 | p1 | p4
                 *  p7 | p6 | p5
                 *  subtract 1 for zeroth order array
                 */
                // get neighbors
                listNeighbors[P8] = srcline[x-1].green;
                listNeighbors[P4] = srcline[x+1].green;

                listNeighbors[P2] = aboveline[x].green;
                listNeighbors[P3] = aboveline[x+1].green;
                listNeighbors[P9] = aboveline[x-1].green;

                listNeighbors[P5] = belowline[x + 1].green;
                listNeighbors[P6] = belowline[x].green;
                listNeighbors[P7] = belowline[x - 1].green;

                if (correctCount()) {
                    if (transition()) {
                        if (takeStep(step)) {
                            // white out pixel
                            destline[x].red = 255;      // red
                            destline[x].green = 0;    // green
                            destline[x].blue = 0;     // blue
                            bDelete = true;
                        }
                    }
                }
            }
        }
        pixelsResult = (char *) pixelsResult + infoResult.stride;
    }
    return bDelete;
}

/*
 *  delete border pixels here -- after prior identification
 */
void GonzalesThinning::deleteBorder(AndroidBitmapInfo infoSource,
                                    void *pixelsSource,
                                    AndroidBitmapInfo infoResult,
                                    void *pixelsResult) {
    void *currentPixelsSource;
    bool bDelete = false;

}

bool GonzalesThinning::takeStep(int step) {

    /*
     * 1) identify / mark left border pixels in step 1 - write to mask
     * 2) identify / mark right border pixels in step 2 for remaining pixels
     * 3) delete selected pixels from above.
     */
    switch (step) {
        case STEP1:
            return step1CD();

        default:
        case STEP2:
            return step2CD();
    }
}

/*
 * pg 492 criterior (a)
 * [out] meets 2<=N(p1)<=6 criterior
 *
 * - a contour point is value = 1 and neighbor of at least 1 zero.
 */
bool GonzalesThinning::correctCount() {
    int total = 0;
    for (int i = 1; i < LIST_LEN; i++) {
        if (listNeighbors[i] == ON_COLOR)
            total++;
    }
    return (2 <= total && total <= 6) ? true : false;
}

/*
 * pg 492 criterior (b)
 * [out] meets S(p1)==1 criterior
 * only 1 count allowed for 1 -> 0 transition (center is 1)
 */
bool GonzalesThinning::transition() {
    int total = 0;
    for(int i = 1; i<8; i++) {
        if (listNeighbors[i] == 0)
            total ++;
    }
    return (total == 1);
}

/*
 * pg 492 criterior (c, d) step 1
 * left to right
 */
bool GonzalesThinning::step1CD() {
    if (listNeighbors[P4] != ON_COLOR ||
        listNeighbors[P6] != ON_COLOR )
        return true;

    else if (listNeighbors[P2] != ON_COLOR &&
             listNeighbors[P8] != ON_COLOR)
        return true;

    return false;
}

/*
 * pg 492 criterior (c, d) step 2
 * right to left
 */
bool GonzalesThinning::step2CD() {
    if (listNeighbors[P2] != ON_COLOR ||
        listNeighbors[P8] != ON_COLOR)
        return true;

    else if (listNeighbors[P4] != ON_COLOR &&
             listNeighbors[P6] != ON_COLOR)
        return true;

    return false;
}