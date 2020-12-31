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

GonzalesThinning::GonzalesThinning()
{
}

GonzalesThinning::~GonzalesThinning()
{

}

//////////////////////////////////////////////////////////////////
// Public methods

/*
 * Iterate Thinning algorithm from Gonzales Text until skeleton remains
 * [in] require a binary image b&w
 * [out] return a binary skeleton image
 */
bool GonzalesThinning::Iterate(AndroidBitmapInfo infoSource,
                             void* pixelsSource,
                             AndroidBitmapInfo infoResult,
                             void* pixelsResult)
{
    bDeleted = true;
    while(bDeleted) {
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
                             void* pixelsSource,
                             AndroidBitmapInfo infoResult,
                             void* pixelsResult)
{
    bool deleted = false;
    if(thinning(infoSource, pixelsSource, infoResult, pixelsResult, STEP1)) {
        deleteBorder();
        deleted = true;
    }
    if(thinning(infoSource, pixelsSource, infoResult, pixelsResult, STEP2)) {
        deleteBorder();
        deleted = true;
    }
    return deleted;
}

//////////////////////////////////////////////////////////////////
// Protected methods

bool GonzalesThinning::thinning(AndroidBitmapInfo infoSource,
                                void* pixelsSource,
                                AndroidBitmapInfo infoResult,
                                void* pixelsResult,
                                int step)
{
    void* currentPixelsSource;
    bool bDelete = false;

    for (int y=1;y<infoSource.height-1;y++)
    {
        currentPixelsSource = (char *)pixelsSource + (infoSource.stride * y);
        rgba * srcline = (rgba *) currentPixelsSource;
        rgba * destline = (rgba *) pixelsResult;

        for (int x=1;x<infoSource.width-1;x++) {

            // if pixel is a blog
            if(srcline[x].green == BACKGROUND_COLOR) {

                // get neighbors
                listNeighbors[2] = srcline[x+1].green;
                listNeighbors[6] = srcline[x-1].green;

                void* nearPixelsSource = (char *)pixelsSource + (infoSource.stride * y-1);
                rgba* nearline = (rgba *) nearPixelsSource;
                listNeighbors[0] = srcline[x].green;
                listNeighbors[1] = nearline[x+1].green;
                listNeighbors[7] = nearline[x-1].green;

                nearPixelsSource = (char *)pixelsSource + (infoSource.stride * y+1);
                nearline = (rgba *) nearPixelsSource;
                listNeighbors[3] = srcline[x+1].green;
                listNeighbors[4] = srcline[x].green;
                listNeighbors[5] = srcline[x-1].green;

                if(correctCount()) {
                    if(transition()) {
                        if(takeStep(step)) {
                            // white out pixel
                            destline[x].alpha = 255;
                            destline[x].red = 255;      // red
                            destline[x].green = 255;    // green
                            destline[x].blue = 255;     // blue
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

void GonzalesThinning::deleteBorder()
{
    // not used for now
}

bool GonzalesThinning::takeStep(int step)
{
    switch(step) {
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
 */
bool GonzalesThinning::correctCount()
{
    int total = 0;
    for(int i=0; i<LIST_LEN; i++){
        if(listNeighbors[i]==BACKGROUND_COLOR)
            total ++;
    }
    return(2<=total&&total<=6)?true:false;
}

/*
 * pg 492 criterior (b)
 * [out] meets S(p1)==1 criterior
 */
bool GonzalesThinning::transition()
{
    int total = 0;
    for(int i=0; i<LIST_LEN; i++){
        if(listNeighbors[i]!=BACKGROUND_COLOR)
            if(listNeighbors[i+1]==BACKGROUND_COLOR)
                total ++;
    }
    if(listNeighbors[LIST_LEN-1]!=BACKGROUND_COLOR)
        if(listNeighbors[0]==BACKGROUND_COLOR)
            total ++;

    return (total==1)?true:false;
}

/*
 * pg 492 criterior (c, d) step 1
 */
bool GonzalesThinning::step1CD()
{
    if(listNeighbors[P4]!=BACKGROUND_COLOR)
        return true;

    if(listNeighbors[P6]!=BACKGROUND_COLOR)
        return true;

    if(listNeighbors[P2]!=BACKGROUND_COLOR&&
       listNeighbors[P8]!=BACKGROUND_COLOR)
        return true;

    return false;
}

/*
 * pg 492 criterior (c, d) step 2
 */
bool GonzalesThinning::step2CD()
{
    if(listNeighbors[P2]!=BACKGROUND_COLOR)
        return true;

    if(listNeighbors[P8]!=BACKGROUND_COLOR)
        return true;

    if(listNeighbors[P6]!=BACKGROUND_COLOR&&
       listNeighbors[P4]!=BACKGROUND_COLOR)
        return true;

    return false;
}