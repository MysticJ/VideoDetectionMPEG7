#pragma once

#include "../Features/FexWrite.h"
#include "../Features/Frame.h"
#include <opencv2/opencv.hpp>
#include <math.h>
#include <fstream>
#include <string>
#include "IOXml.h"

using namespace cv;

class ReadProcessFrames
{
public:

	ReadProcessFrames(const char* path, int framesFetch = 1, int offsetFrameIdx = 10);

	~ReadProcessFrames();

	int getTotalFrames();
	int getSizeWidth();
	int getSizeHeight();
	int getFetchDuration(); 
	int getFramesFetch();
	bool getIsVideoOpened();

	XM::ColorStructureDescriptor** framesToCSD(int descSize = 64, bool xmlFlag = false, bool printFlag = true);
	XM::ScalableColorDescriptor** framesToSCD(bool maskFlag = true, int numCoeff = 256, int bitPlanesDiscarded = 0, bool xmlFlag = false, bool printFlag = true);
	XM::ColorLayoutDescriptor** framesToCLD(int numYcoef = 64, int numCCoef = 28, bool xmlFlag = false, bool printFlag = true);
	XM::DominantColorDescriptor** framesToDCD(bool normalize = true, bool variance = true, bool spatial = true,
		int bin1 = 32, int bin2 = 32, int bin3 = 32, bool xmlFlag = false, bool printFlag = true);
	XM::HomogeneousTextureDescriptor** framesToHTD(int layerFlag = 1, bool xmlFlag = false, bool printFlag = true);
	XM::EdgeHistogramDescriptor** framesToEHD(bool xmlFlag = false, bool printFlag = true);
	XM::RegionShapeDescriptor** framesToRSD(bool xmlFlag = false, bool printFlag = true);


private:
	VideoCapture cap;

	std::string videoPath;
	int framesFetch;
	int offsetFrameIdx;

	int totalFrames;
	// int framePerSecond;
	int sizeWidth;
	int sizeHeight;
	int fetchDuration;
	bool isVideoOpened;

	Frame* curFrame;
	Frame** readFrames;

	void readToFrames();


};

