#pragma once

#include "../Features/Frame.h"
#include "../Features/Descriptors.h"
#include "../Features/Feature.h"
#include "ReadProcessFrames.h"

#include <fstream>
#include <string>

class IOXml
{

public:
	// ------------------------------------- Write to Xmls ------------------------------------------------------
	// ----------------------------------------------------------------------------------------------------------

	static bool readToFiles(const char* videoPath, int framesFetch = 10, int offsetFrameIdx = 10);

	static bool writeScalableColorDs(std::ofstream& ofObject, XM::ScalableColorDescriptor** scds, int framesFetch, 
		int numCoeff, int bitPlanesDiscarded);

	static bool writeColorStructureDs(std::ofstream& ofObject, XM::ColorStructureDescriptor** csds, int framesFetch, 
		int descSize);

	static bool writeDominantColorDs(std::ofstream& ofObject, XM::DominantColorDescriptor** dcds, int framesFetch,
		bool normalize, bool variance, bool spatial, int bin1, int bin2, int bin3);

	static bool writeColorLayoutDs(std::ofstream& ofObject, XM::ColorLayoutDescriptor** clds, int framesFetch, 
		int numYCoeff, int numCCoeff);

	static bool writeEdgeHistogramDs(std::ofstream& ofObject, XM::EdgeHistogramDescriptor** ehds, int framesFetch);

	static bool writeHomogeneousTextureDs(std::ofstream& ofObject, XM::HomogeneousTextureDescriptor** htds, int framesFetch,
		int layerFlag);

	// ------------------------------------ Read from Xmls ------------------------------------------------------
	// ----------------------------------------------------------------------------------------------------------
	static bool readHomogeneousTextureDs(std::ifstream& ifObject, const char* path, 
		XM::HomogeneousTextureDescriptor**& htds, int& framesFetch, int& layerFlag);

	static bool readEdgeHistogramDs(std::ifstream& ifObject, const char* path,
		XM::EdgeHistogramDescriptor**& ehds, int& framesFetch);

	static bool readColorLayoutDs(std::ifstream& ifObject, const char* path,
		XM::ColorLayoutDescriptor**& clds, int& framesFetch, int& numYCoeff, int& numCCoeff);

	static bool readDominantColorDs(std::ifstream& ifObject, const char* path,
		XM::DominantColorDescriptor**& dcds, int& framesFetch,
		bool& normalize, bool& vairance, bool& spatial, int& bin1, int& bin2, int& bin3);

	static bool readColorStructureDs(std::ifstream& ifObject, const char* path,
		XM::ColorStructureDescriptor**& ehds, int& framesFetch, int& numCoeff);

	static bool readScalableColorDs(std::ifstream& ifObject, const char* path,
		XM::ScalableColorDescriptor**& scds, int& framesFetch, int& numCoeff, int& bitPlanesDiscarded);

	// ------------------------------------- Verification -------------------------------------------------------
	// ----------------------------------------------------------------------------------------------------------
	static bool verifyHomogeneousTextureDs(const char* videoPath, const char* listPath);

	static bool verifyEdgeHistogramDs(const char* videoPath, const char* listPath);

	static bool verifyColorLayoutDs(const char* videoPath, const char* listPath);

	static bool verifyColorStructureDs(const char* videoPath, const char* listPath);

	static bool verifyScalableColorDs(const char* videoPath, const char* listPath);
};

