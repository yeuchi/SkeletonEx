//
// Created by yeuchi on 12/30/20.
//
// ==================================================================
// Module:		GonzalesThinning.h
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

#ifndef SKELETONEX_GONZALESTHINNING_H
#define SKELETONEX_GONZALESTHINNING_H


class GonzalesThinning {

public:
    GonzalesThinning();
    ~GonzalesThinning();

public:
    bool Apply(AndroidBitmapInfo infoSource,
               void* pixelsSource,
               AndroidBitmapInfo infoResult,
               void* pixelsResult);

    bool Iterate(AndroidBitmapInfo infoSource,
               void* pixelsSource,
               AndroidBitmapInfo infoConvolved,
               void* pixelsConvolved);

protected:
    bool thinning(AndroidBitmapInfo infoSource,
                  void* pixelsSource,
                  AndroidBitmapInfo infoResult,
                  void* pixelsResult, int step);
    void deleteBorder();
    bool takeStep(int step);
    bool correctCount();
    bool transition();
    bool step1CD();
    bool step2CD();

protected:
    static const int STEP1 = 1;
    static const int STEP2 = 2;
    static const int LIST_LEN = 8;
    static const int BACKGROUND_COLOR = 255;

    bool bDeleted = true;
    int listNeighbors[LIST_LEN];
};


#endif //SKELETONEX_GONZALESTHINNING_H
