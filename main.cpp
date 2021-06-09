/***************************************************************
 * Name:      main.cpp
 * Purpose:   Illustrate the usage of the MPEG-7 Feature Extraction library
 *            uses OpenCV for image handling (as does the library)
 * Author:    Muhammet Bastan (mubastan@gmail.com)
 * Created:   2010-02-19
 * Update: 2011-02-04 (update to OpenCV 2.2, Mat)
 * Copyright: Muhammet Bastan (https://sites.google.com/site/mubastan/)
 * License:
 **************************************************************/

#include <iostream>
// #include <ctime>
#include <time.h>
#include <stdio.h>
#include "Features/FexWrite.h"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Utils/ReadProcessFrames.h"
#include "Utils/IOXml.h"

using namespace cv;
using namespace std;

void readToFiles(const char* videoPath, int framesFetch);

int main( int argc, char* argv[] )
{

    clock_t tstart;

    tstart = clock();
    bool ifSuccess = IOXml::verifyHomogeneousTextureDs("Images/video_1_infrared.mp4", "Xmls/HTD_list.txt");
    cout << "If there a video similar to the given one: " << ifSuccess << endl;
    tstart = clock()-tstart;
    printf("It took me %d clicks (%f seconds).\n", tstart, ((float)tstart) / CLOCKS_PER_SEC);

    tstart = clock();
    ifSuccess = IOXml::verifyHomogeneousTextureDs("Images/video_elephant.mp4", "Xmls/HTD_list.txt");
    cout << "If there a video similar to the given one: " << ifSuccess << endl;
    tstart = clock() - tstart;
    printf("It took me %d clicks (%f seconds).\n", tstart, ((float)tstart) / CLOCKS_PER_SEC);

    tstart = clock();
    ifSuccess = IOXml::verifyColorStructureDs("Images/video_1_infrared.mp4", "Xmls/CSD_list.txt");
    cout << "If there a video similar to the given one: " << ifSuccess << endl;
    tstart = clock() - tstart;
    printf("It took me %d clicks (%f seconds).\n", tstart, ((float)tstart) / CLOCKS_PER_SEC);

    tstart = clock();
    ifSuccess = IOXml::verifyColorStructureDs("Images/video_elephant.mp4", "Xmls/CSD_list.txt");
    cout << "If there a video similar to the given one: " << ifSuccess << endl;
    tstart = clock() - tstart;
    printf("It took me %d clicks (%f seconds).\n", tstart, ((float)tstart) / CLOCKS_PER_SEC);


 
    /*
    Mat image;
    
    // load an image
    if( argc < 2 ){
        image = imread("Images/motor.jpg");
        if (image.dims == 0) { cout << "Empty" << endl; return 0; }
    }
    else
        image = imread( argv[1] );

    // display the image
    namedWindow("image");
    imshow("image", image);
    waitKey();

    ///-- extract global image descriptors --

    // create a Frame object (see include/Frame.h)
    // allocate memory for 3-channel color and 1-channel gray image and mask
    Frame* frame = new Frame( image.cols, image.rows, true, true, true);

    // set the image of the frame
    frame->setImage(image);

    // compute and display the descriptors for the 'image'
    // CSD of size 32
    cout << "CSD of size 32:" << endl;
    FexWrite::computeWriteCSD(frame, 32);

    // SCD of size 128
    cout << "SCD" << endl;
    FexWrite::computeWriteSCD( frame, true, 128 );

    // CLD with numberOfYCoeff (28), numberOfCCoeff (15)
    cout << "CLD" << endl;
    FexWrite::computeWriteCLD( frame, 28, 15 );

    // DCD with normalization (to MPEG-7 ranges), without variance, without spatial coherency
    cout << "DCD" << endl;
    FexWrite::computeWriteDCD( frame, true, false, false );

    // EHD
    cout << "EHD" << endl;
    FexWrite::computeWriteEHD( frame );

    // HTD: we need to compute and set the grayscale image of the frame
    // create the grayscale image
    Mat gray;
    cvtColor( image, gray, COLOR_BGR2GRAY );
    frame->setGray( gray );

    // full layer (both energy and deviation)
    cout << "HTD" << endl;
    FexWrite::computeWriteHTD( frame, 1 );
    

    // load another image
    image = imread("Images/timsah.jpg");
    if (image.dims == 0) { cout << "Empty" << endl; return 0; }
    // cout << "Image read" << endl;
    namedWindow("image");
    imshow("image", image);
    waitKey();

    // set the image of 'frame'
    // we should resize the 'frame' first, the dimensions may be different
    // therefore, reallocation may be required
    frame->resize( image.cols, image.rows);
    // Frame* frame = new Frame(image.cols, image.rows, true, true, true);

    // set the image
    frame->setImage(image);

    // here, we can compute the descriptors as above..
    // ..

    ///--- extract region-based descriptos ---

    // we need a foreground mask for the region
    // lets first create a dummy foreground mask to use as the region mask

    Mat mask = Mat( image.rows, image.cols, CV_8UC1, Scalar(0) );

    // draw a filled rectangle/circle, with foreground value 200
    int regVal = 200;   // (1...255)
    circle(mask, Point(120,100), 50, Scalar(regVal), -1);
    circle(mask, Point(120,200), 60, Scalar(regVal), -1);

    // display the mask
    namedWindow("mask");
    imshow("mask", mask);
    waitKey();
    imwrite("Images/mask2.png", mask);

    // set the mask of 'frame', pixels having regVal are set to 255, background pixels to 0
    frame->setMaskAll( mask, regVal, 255, 0 );

    // SCD of the region
    cout << "Region SCD" << endl;
    FexWrite::computeWriteSCD( frame, false, 128 );

    // RSD (region shape)
    cout << "Region RSD" << endl;
    FexWrite::computeWriteRSD( frame );

    cout << "Region CSD" << endl;
    FexWrite::computeWriteCSD(frame, 64);

    // at this point, if you want to extract descriptors
    // from the whole image, reset the mask
    frame->resetMaskAll();
    cout << "CSD" << endl;
    FexWrite::computeWriteCSD(frame, 64);
    cout << "EHD" << endl;
    FexWrite::computeWriteEHD( frame );

    // ...
    // ...

    // release frame
    delete frame;

    // destroy the windows
    destroyWindow("image");
    destroyWindow("mask");
    */

    waitKey(0);
    return 0;
}

void readToFiles(const char* videoPath, int framesFetch) {
    ReadProcessFrames* obj = new ReadProcessFrames(videoPath, framesFetch, 50);
    if (!obj->getIsVideoOpened()) { return;  }

    XM::ColorStructureDescriptor** ptr_csd = obj->framesToCSD(64, true);
    if (ptr_csd != nullptr) {
        for (int i = 0; i < obj->getFramesFetch(); ++i) {
            if (ptr_csd[i] != nullptr) { delete ptr_csd[i]; }
        }
        delete[] ptr_csd;
    }

    XM::ScalableColorDescriptor** ptr_scd = obj->framesToSCD(true, 256, 0, true);
    if (ptr_scd != nullptr) {
        for (int i = 0; i < obj->getFramesFetch(); ++i) {
            if (ptr_scd[i] != nullptr) { delete ptr_scd[i]; }
        }
        delete[] ptr_scd;
    }

    XM::ColorLayoutDescriptor** ptr_cld = obj->framesToCLD(64, 28, true);
    if (ptr_cld != nullptr) {
        for (int i = 0; i < obj->getFramesFetch(); ++i) {
            if (ptr_cld[i] != nullptr) { delete ptr_cld[i]; }
        }
        delete[] ptr_cld;
    }

    XM::HomogeneousTextureDescriptor** ptr_htd = obj->framesToHTD(1, true);
    if (ptr_htd != nullptr) {
        for (int i = 0; i < obj->getFramesFetch(); ++i) {
            if (ptr_htd[i] != nullptr) { delete ptr_htd[i]; }
        }
        delete[] ptr_htd;
    }

    XM::EdgeHistogramDescriptor** ptr_ehd = obj->framesToEHD(true);
    if (ptr_ehd != nullptr) {
        for (int i = 0; i < obj->getFramesFetch(); ++i) {
            if (ptr_ehd[i] != nullptr) { delete ptr_ehd[i]; }
        }
        delete[] ptr_ehd;
    }

    delete obj;
    obj = nullptr;
}