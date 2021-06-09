#include <math.h>
#include <stdio.h>
#include <time.h>
#include "ReadProcessFrames.h"

ReadProcessFrames::ReadProcessFrames(const char* path, int framesFetch, int offsetFrameIdx) {
	this->videoPath = path;
	this->framesFetch = framesFetch;
	this->offsetFrameIdx = offsetFrameIdx;
	// 
	cap.open(path);

	if (!cap.isOpened()) {
		this->totalFrames = 0;
		// framePerSecond = 0;
		this->sizeWidth = 0;
		this->sizeHeight = 0;
		this->fetchDuration = 0;
		this->isVideoOpened = false;
		this->curFrame = nullptr;
		this->readFrames = nullptr;

		std::cout << "Invalid path or video. Video not opened." << std::endl;
	}
	else {
		std::cout << "Successfully opened the video." << std::endl;

		this->totalFrames = (int)cap.get(CAP_PROP_FRAME_COUNT);
		assert(offsetFrameIdx < this->totalFrames);

		// framePerSecond = (int)cap.get(CAP_PROP_FPS);
		this->sizeWidth = (int)cap.get(CAP_PROP_FRAME_WIDTH);
		this->sizeHeight = (int)cap.get(CAP_PROP_FRAME_HEIGHT);

		this->isVideoOpened = true;

		if ((offsetFrameIdx + framesFetch) > this->totalFrames) {
			this->framesFetch = this->totalFrames - offsetFrameIdx;
		}
		this->fetchDuration = floor((this->totalFrames - (double)offsetFrameIdx) / framesFetch);

		this->curFrame = new Frame(sizeWidth, sizeHeight, true, true, true);
		this->readFrames = new Frame*[framesFetch];

		clock_t t;
		t = clock();
		readToFrames();
		t = clock() - t;
		printf("It took me %d clicks (%f seconds) to read %d frames.\n", t, ((float)t) / CLOCKS_PER_SEC, this->framesFetch);
	}
	cap.release();
}

ReadProcessFrames::~ReadProcessFrames() {
	if (!isVideoOpened) { return;}

	if (curFrame != nullptr) { delete curFrame; }

	for (int i = 0; i < framesFetch; ++i) {
		if (readFrames[i] != nullptr) {	delete readFrames[i]; }
	}
	delete[] readFrames;
}

int ReadProcessFrames::getTotalFrames() { return totalFrames; }
int ReadProcessFrames::getSizeWidth() { return sizeWidth; }
int ReadProcessFrames::getSizeHeight() { return sizeHeight; }
int ReadProcessFrames::getFetchDuration() { return fetchDuration; }
int ReadProcessFrames::getFramesFetch() { return framesFetch; }
bool ReadProcessFrames::getIsVideoOpened() { return isVideoOpened; }

void ReadProcessFrames::readToFrames() {
	std::cout << "Total number of frames to read: " << framesFetch << std::endl;
	// namedWindow("image");
	// namedWindow("image_gray");
	for (int i = 0; i < framesFetch; ++i) {
		std::cout << "Frame " << i << ": ";

		int idx = offsetFrameIdx + i * fetchDuration;
		if (!cap.set(CAP_PROP_POS_FRAMES, (double)idx)) { readFrames[i] = nullptr; continue; }

		Mat mMat, mMatGray;
		if (!cap.read(mMat)) { readFrames[i] = nullptr; continue; }
		std::cout << "Read successful." << std::endl;
		cvtColor(mMat, mMatGray, COLOR_BGR2GRAY);

		readFrames[i] = new Frame(sizeWidth, sizeHeight, true, true, true);
		readFrames[i]->setImage(mMat);
		readFrames[i]->setGray(mMatGray);

		// imshow("image", mMat);
		// imshow("image_gray", mMatGray);
		// waitKey();
	}
	// destroyWindow("image");
	// destroyWindow("image_gray");
}

XM::ColorStructureDescriptor** ReadProcessFrames::framesToCSD(int descSize, bool xmlFlag, bool printFlag) {

	XM::ColorStructureDescriptor** csds = new XM::ColorStructureDescriptor * [framesFetch];
	if (!xmlFlag) { std::cout << "Descriptor: Color Structure (CSD)" << std::endl; }

	for (int i = 0; i < framesFetch; ++i) {
		XM::ColorStructureDescriptor* csd = Feature::getColorStructureD(readFrames[i], descSize);
		csds[i] = csd;

		if (printFlag) {
			unsigned int csd_size = csd->GetSize();
			std::cout << "Fetched Frame Number " << i << ":\n\t";
			for (unsigned int j = 0; j < csd_size; j++)
				std::cout << (int)csd->GetElement(j) << " ";
			std::cout << "\n" << std::endl;

			// delete csd;
			// csds[i] = nullptr;
		}
	}
	if (!xmlFlag) { 
		// delete[] csds; return nullptr; 
		return csds;
	}
	else {
		std::string outpath = "Xmls/" + videoPath.substr(videoPath.find("/")+1, videoPath.find(".")- videoPath.find("/")-1) + "_CSD" + std::to_string(framesFetch) + ".xml";
		std::cout << outpath << std::endl;
		std::ofstream ofObject(outpath, std::ios::trunc);
		if (!ofObject.is_open()) { ofObject.close();  return csds; }
		ofObject << "<VideoDescriptors \n"
			<< "xmlns = \"http://www.w3.org/2000/10/XMLSchema\"\n"
			<< "xmlns:mpeg7 = \"http://www.mpeg7.org/2001/MPEG-7_Schema\"\n"
			<< "mpeg7:schemaLocation = \"http://www.w3.org/2000/10/XMLSchema selfDefineMpeg7.xsd\">\n";
		bool val = IOXml::writeColorStructureDs(ofObject, csds, framesFetch, descSize);
		ofObject << "</VideoDescriptors>\n";
		ofObject.close();

		std::ofstream recObject("Xmls/CSD_list.txt", std::ios::app);
		if (recObject.is_open()) {
			recObject << outpath << "\n";
		}
		recObject.close();
	}
	return csds;
}

XM::ScalableColorDescriptor** ReadProcessFrames::framesToSCD(bool maskFlag, int numCoeff, int bitPlanesDiscarded, 
	bool xmlFlag, bool printFlag) {

	XM::ScalableColorDescriptor** scds = new XM::ScalableColorDescriptor * [framesFetch];

	if (!xmlFlag) { std::cout << "Descriptor: Scalable Color (SCD)" << std::endl; }

	for (int j = 0; j < framesFetch; ++j) {
		XM::ScalableColorDescriptor* scd = Feature::getScalableColorD(readFrames[j], maskFlag, numCoeff, bitPlanesDiscarded);
		scds[j] = scd;

		if (printFlag) {
			unsigned int scd_num_coeff = scd->GetNumberOfCoefficients();
			std::cout << "Fetched Frame Number " << j << ":\n\t";
			for (unsigned int i = 0; i < scd_num_coeff; i++)
				std::cout << (int)scd->GetCoefficient(i) << " ";
			std::cout << "\n" << std::endl;

			// delete scd;
			// scds[j] = nullptr;
		}		
	}
	if (!xmlFlag) { 
		// delete[] scds; return nullptr; 
		return scds;
	}
	else {
		std::string outpath = "Xmls/" + videoPath.substr(videoPath.find("/") + 1, videoPath.find(".") - videoPath.find("/") - 1) + "_SCD" + std::to_string(framesFetch) + ".xml";
		std::cout << outpath << std::endl;
		std::ofstream ofObject(outpath, std::ios::trunc);
		if (!ofObject.is_open()) { ofObject.close();  return scds; }
		ofObject << "<VideoDescriptors \n"
			<< "xmlns = \"http://www.w3.org/2000/10/XMLSchema\"\n"
			<< "xmlns:mpeg7 = \"http://www.mpeg7.org/2001/MPEG-7_Schema\"\n"
			<< "mpeg7:schemaLocation = \"http://www.w3.org/2000/10/XMLSchema selfDefineMpeg7.xsd\">\n";
		bool val = IOXml::writeScalableColorDs(ofObject, scds, framesFetch, numCoeff, bitPlanesDiscarded);
		ofObject << "</VideoDescriptors>\n";
		ofObject.close();

		std::ofstream recObject("Xmls/SCD_list.txt", std::ios::app);
		if (recObject.is_open()) {
			recObject << outpath << "\n";
		}
		recObject.close();
	}
	return scds;
}

XM::ColorLayoutDescriptor** ReadProcessFrames::framesToCLD(int numYcoef, int numCCoef, bool xmlFlag, bool printFlag) {

	XM::ColorLayoutDescriptor** clds = new XM::ColorLayoutDescriptor * [framesFetch];
	if (!xmlFlag) { std::cout << "Descriptor: Color Layout (CLD)" << std::endl; }

	for (int j = 0; j < framesFetch; ++j) {
		XM::ColorLayoutDescriptor* cld = Feature::getColorLayoutD(readFrames[j], numYcoef, numCCoef);
		clds[j] = cld;

		if (printFlag) {
			int numberOfYCoeff = cld->GetNumberOfYCoeff();
			int numberOfCCoeff = cld->GetNumberOfCCoeff();
			int* y_coeff, * cb_coeff, * cr_coeff;
			y_coeff = cld->GetYCoeff();
			cb_coeff = cld->GetCbCoeff();
			cr_coeff = cld->GetCrCoeff();

			std::cout << "Fetched Frame Number " << j << ":"<< std::endl;

			int i = 0;
			// Y coeff (DC and AC)
			std::cout << "\tY coeff:\t";
			for (i = 0; i < numberOfYCoeff; i++)
				std::cout << y_coeff[i] << " ";
			std::cout << std::endl;
			//Cb coeff  (DC and AC)
			std::cout << "\tCb coeff:\t";
			for (i = 0; i < numberOfCCoeff; i++)
				std::cout << cb_coeff[i] << " ";
			std::cout << std::endl;
			//Cr coeff  (DC and AC)
			std::cout << "\tCr coeff:\t";
			for (i = 0; i < numberOfCCoeff; i++)
				std::cout << cr_coeff[i] << " ";
			std::cout << "\n" <<std::endl;

			// delete cld;
			// clds[j] = nullptr;
		}
	}
	if (!xmlFlag) { 
		// delete[] clds; return nullptr; 
		return clds;
	}
	else {
		std::string outpath = "Xmls/" + videoPath.substr(videoPath.find("/") + 1, videoPath.find(".") - videoPath.find("/") - 1) + "_CLD" + std::to_string(framesFetch) + ".xml";
		std::cout << outpath << std::endl;
		std::ofstream ofObject(outpath, std::ios::trunc);
		if (!ofObject.is_open()) { ofObject.close();  return clds; }
		ofObject << "<VideoDescriptors \n"
			<< "xmlns = \"http://www.w3.org/2000/10/XMLSchema\"\n"
			<< "xmlns:mpeg7 = \"http://www.mpeg7.org/2001/MPEG-7_Schema\"\n"
			<< "mpeg7:schemaLocation = \"http://www.w3.org/2000/10/XMLSchema selfDefineMpeg7.xsd\">\n";
		bool val = IOXml::writeColorLayoutDs(ofObject, clds, framesFetch, numYcoef, numCCoef);
		ofObject << "</VideoDescriptors>\n";
		ofObject.close();

		std::ofstream recObject("Xmls/CLD_list.txt", std::ios::app);
		if (recObject.is_open()) {
			recObject << outpath << "\n";
		}
		recObject.close();
	}
	return clds;
}

XM::DominantColorDescriptor** ReadProcessFrames::framesToDCD(bool normalize, bool variance, bool spatial,
	int bin1, int bin2, int bin3, bool xmlFlag, bool printFlag) {
	XM::DominantColorDescriptor** dcds = new XM::DominantColorDescriptor * [framesFetch];
	if (!xmlFlag) { std::cout << "Descriptor: Dominant Color (DCD)" << std::endl; }

	for (int j = 0; j < framesFetch; ++j) {
		XM::DominantColorDescriptor* dcd = Feature::getDominantColorD(readFrames[j], normalize, variance, spatial, bin1, bin2, bin3);
		dcds[j] = dcd;

		int ndc = dcd->GetDominantColorsNumber();

		if (printFlag) {
			std::cout << "Fetched Frame Number " << j << ":\n\t";

			if (spatial){
				std::cout << "(spatial)";
				std::cout << dcd->GetSpatialCoherency();
			}
			// dominant colors: percentage(1) centroid value (3) color variance (3)
			XM::DOMCOL* domcol = dcd->GetDominantColors();
			for (int i = 0; i < ndc; i++)
			{
				std::cout << "\n \t(percentage)" << domcol[i].m_Percentage
					<< " \t(colorValue)[" << domcol[i].m_ColorValue[0]
					<< " " << domcol[i].m_ColorValue[1]
					<< " " << domcol[i].m_ColorValue[2] << "]";
				if (variance)
					std::cout << " \t(colorVariance)[" << domcol[i].m_ColorVariance[0]
					<< " " << domcol[i].m_ColorVariance[1]
					<< " " << domcol[i].m_ColorVariance[2] << "]";
			}

			std::cout << "\n" << std::endl;

			// delete dcd;
			// dcds[j] = nullptr;
		}
	}
	if (!xmlFlag) { 
		// delete[] dcds; return nullptr; 
		return dcds;
	}
	else {
		std::string outpath = "Xmls/" + videoPath.substr(videoPath.find("/") + 1, videoPath.find(".") - videoPath.find("/") - 1) + "_DCD" + std::to_string(framesFetch) + ".xml";
		std::cout << outpath << std::endl;
		std::ofstream ofObject(outpath, std::ios::trunc);
		if (!ofObject.is_open()) { ofObject.close();  return dcds; }
		ofObject << "<VideoDescriptors \n"
			<< "xmlns = \"http://www.w3.org/2000/10/XMLSchema\"\n"
			<< "xmlns:mpeg7 = \"http://www.mpeg7.org/2001/MPEG-7_Schema\"\n"
			<< "mpeg7:schemaLocation = \"http://www.w3.org/2000/10/XMLSchema selfDefineMpeg7.xsd\">\n";
		bool val = IOXml::writeDominantColorDs(ofObject, dcds, framesFetch, normalize, variance, spatial, bin1, bin2, bin3);
		ofObject << "</VideoDescriptors>\n";
		ofObject.close();

		std::ofstream recObject("Xmls/DCD_list.txt", std::ios::app);
		if (recObject.is_open()) {
			recObject << outpath << "\n";
		}
		recObject.close();
	}
	return dcds;
}

XM::HomogeneousTextureDescriptor** ReadProcessFrames::framesToHTD(int layerFlag, bool xmlFlag, bool printFlag) {

	XM::HomogeneousTextureDescriptor** htds = new XM::HomogeneousTextureDescriptor * [framesFetch];
	if (!xmlFlag) { std::cout << "Descriptor: Homogeneous Texture (HTD)" << std::endl; }

	for (int j = 0; j < framesFetch; ++j) {
		XM::HomogeneousTextureDescriptor* htd = Feature::getHomogeneousTextureD(readFrames[j], layerFlag);
		htds[j] = htd;

		if (printFlag) {
			std::cout << "Fetched Frame Number " << j << ":" << std::endl;
			int* values = htd->GetHomogeneousTextureFeature();

			// values[0]: mean, values[1]: std, values[2-31] base layer (energy)
			std::cout << "\t(mean): " << values[0] << " (std): " << values[1] << " \n\t[2-31]: ";
			int i;
			for (i = 2; i < 32; i++)
				std::cout << values[i] << " ";
			// if full layer, print values[32-61] (energy deviation)
			
			if (layerFlag) {
				std::cout << "\n\t[31-61]: ";
				for (i = 32; i < 62; i++)
					std::cout << values[i] << " ";
			}
			std::cout << "\n" << std::endl;

			// delete htd;
			// htds[j] = nullptr;
		}
	}
	if (!xmlFlag) { 
		// delete[] htds; return nullptr; 
		return htds;
	}
	else {
		std::string outpath = "Xmls/" + videoPath.substr(videoPath.find("/") + 1, videoPath.find(".") - videoPath.find("/") - 1) + "_HTD" + std::to_string(framesFetch) + ".xml";
		std::cout << outpath << std::endl;
		std::ofstream ofObject(outpath, std::ios::trunc);
		if (!ofObject.is_open()) { ofObject.close();  return htds; }
		ofObject << "<VideoDescriptors \n"
			<< "xmlns = \"http://www.w3.org/2000/10/XMLSchema\"\n"
			<< "xmlns:mpeg7 = \"http://www.mpeg7.org/2001/MPEG-7_Schema\"\n"
			<< "mpeg7:schemaLocation = \"http://www.w3.org/2000/10/XMLSchema selfDefineMpeg7.xsd\">\n";
		bool val = IOXml::writeHomogeneousTextureDs(ofObject, htds, framesFetch, layerFlag);
		ofObject << "</VideoDescriptors>\n";
		ofObject.close();

		std::ofstream recObject("Xmls/HTD_list.txt", std::ios::app);
		if (recObject.is_open()) {
			recObject << outpath << "\n";
		}
		recObject.close();

	}
	return htds;
}

XM::EdgeHistogramDescriptor** ReadProcessFrames::framesToEHD(bool xmlFlag, bool printFlag) {

	XM::EdgeHistogramDescriptor** ehds = new XM::EdgeHistogramDescriptor * [framesFetch];
	if (!xmlFlag) { std::cout << "Descriptor: Edge Histogram (EHD)" << std::endl; }

	for (int j = 0; j < framesFetch; ++j) {
		XM::EdgeHistogramDescriptor* ehd = Feature::getEdgeHistogramD(readFrames[j]);
		ehds[j] = ehd;

		unsigned int ehd_size = ehd->GetSize();

		if (printFlag) {
			std::cout << "Fetched Frame Number " << j << ":\n\t";
			// get a pointer to the values
			char* de = ehd->GetEdgeHistogramElement();

			// write to screen
			for (unsigned int i = 0; i < ehd_size; i++)
				std::cout << (int)de[i] << " ";
			std::cout << "\n" << std::endl;

			// delete ehd;
			// ehds[j] = nullptr;
		}
	}
	if (!xmlFlag) { 
		// delete[] ehds; return nullptr;
		return ehds;
	}
	else {
		std::string outpath = "Xmls/" + videoPath.substr(videoPath.find("/") + 1, videoPath.find(".") - videoPath.find("/") - 1) + "_EHD" + std::to_string(framesFetch) + ".xml";
		std::cout << outpath << std::endl;
		std::ofstream ofObject(outpath, std::ios::trunc);
		if (!ofObject.is_open()) { ofObject.close();  return ehds; }
		ofObject << "<VideoDescriptors \n"
			<< "xmlns = \"http://www.w3.org/2000/10/XMLSchema\"\n"
			<< "xmlns:mpeg7 = \"http://www.mpeg7.org/2001/MPEG-7_Schema\"\n"
			<< "mpeg7:schemaLocation = \"http://www.w3.org/2000/10/XMLSchema selfDefineMpeg7.xsd\">\n";
		bool val = IOXml::writeEdgeHistogramDs(ofObject, ehds, framesFetch);
		ofObject << "</VideoDescriptors>\n";
		ofObject.close();

		std::ofstream recObject("Xmls/EHD_list.txt", std::ios::app);
		if (recObject.is_open()) {
			recObject << outpath << "\n";
		}
		recObject.close();
	}
	return ehds;
}

XM::RegionShapeDescriptor** ReadProcessFrames::framesToRSD(bool xmlFlag, bool printFlag) {

	XM::RegionShapeDescriptor** rsds = new XM::RegionShapeDescriptor * [framesFetch];
	if (!xmlFlag) { std::cout << "Descriptor: Region Shape (RSD)" << std::endl; }

	for (int k = 0; k < framesFetch; ++k) {
		XM::RegionShapeDescriptor* rsd = Feature::getRegionShapeD(readFrames[k]);
		rsds[k] = rsd;

		if (printFlag) {
			std::cout << "Fetched Frame Number " << k << ":\n";
			int i, j;
			for (i = 0; i < ART_ANGULAR; i++)
				for (j = 0; j < ART_RADIAL; j++)
					if (i != 0 || j != 0)
						std::cout << (int)rsd->GetElement(i, j) << " ";

			std::cout << "\n" << std::endl;

			// delete rsd;
			// rsds[k] = nullptr;
		}
		else {}
	}
	if (!xmlFlag) { 
		// delete[] rsds; return nullptr; 
		return rsds;
	}
	return rsds;
}


