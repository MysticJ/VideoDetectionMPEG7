#include "IOXml.h"
#include <iostream>

using namespace std;


// ------------------------------------- Write to Xmls ------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------

bool IOXml::readToFiles(const char* videoPath, int framesFetch, int offsetFrameIdx) {
	ReadProcessFrames* obj = new ReadProcessFrames(videoPath, framesFetch, 50);
	if (!obj->getIsVideoOpened()) { return false; }

	XM::ColorStructureDescriptor** ptr_csd = obj->framesToCSD(64, true, false);
	if (ptr_csd != nullptr) {
		for (int i = 0; i < obj->getFramesFetch(); ++i) {
			if (ptr_csd[i] != nullptr) { delete ptr_csd[i]; }
		}
		delete[] ptr_csd;
	}

	XM::ScalableColorDescriptor** ptr_scd = obj->framesToSCD(true, 256, 0, true, false);
	if (ptr_scd != nullptr) {
		for (int i = 0; i < obj->getFramesFetch(); ++i) {
			if (ptr_scd[i] != nullptr) { delete ptr_scd[i]; }
		}
		delete[] ptr_scd;
	}

	XM::ColorLayoutDescriptor** ptr_cld = obj->framesToCLD(64, 28, true, false);
	if (ptr_cld != nullptr) {
		for (int i = 0; i < obj->getFramesFetch(); ++i) {
			if (ptr_cld[i] != nullptr) { delete ptr_cld[i]; }
		}
		delete[] ptr_cld;
	}

	XM::HomogeneousTextureDescriptor** ptr_htd = obj->framesToHTD(1, true, false);
	if (ptr_htd != nullptr) {
		for (int i = 0; i < obj->getFramesFetch(); ++i) {
			if (ptr_htd[i] != nullptr) { delete ptr_htd[i]; }
		}
		delete[] ptr_htd;
	}

	XM::EdgeHistogramDescriptor** ptr_ehd = obj->framesToEHD(true, false);
	if (ptr_ehd != nullptr) {
		for (int i = 0; i < obj->getFramesFetch(); ++i) {
			if (ptr_ehd[i] != nullptr) { delete ptr_ehd[i]; }
		}
		delete[] ptr_ehd;
	}

	delete obj;
	obj = nullptr;
	return true;
}

bool IOXml::writeScalableColorDs(ofstream& ofObject, XM::ScalableColorDescriptor** scds, int framesFetch, 
	int numCoeff, int bitPlanesDiscarded) {

	if (ofObject.is_open()) {
		ofObject << "\t<SCDs numOfFrames=\"" << framesFetch << "\">\n";
		
		for (int i = 0; i < framesFetch; ++i) {
			ofObject << "\t\t<Frame numOfCoeff=\"" << numCoeff << "\" numOfBitPlanesD=\"" << bitPlanesDiscarded << "\">\n";
			ofObject << "\t\t\t<Coeff>\n";

			XM::ScalableColorDescriptor* scd = scds[i];
			unsigned int scd_num_coeff = scd->GetNumberOfCoefficients();
			string coeff = "\t\t\t\t";
			int counter = 0;
			for (unsigned int j = 0; j < scd_num_coeff; ++j) {
				coeff += to_string((int)scd->GetCoefficient(j));
				coeff += " ";
				counter += 1;
				if ((counter >= 8) && (j < scd_num_coeff-1)) {
					coeff += "\n\t\t\t\t";
					counter = 0;
				}
			}
			coeff += "\n";
			ofObject << coeff;

			ofObject << "\t\t\t</Coeff>\n";
			ofObject << "\t\t</Frame>\n";
		}

		ofObject << "\t</SCDs>\n";
		return true;
		
	}
	else { return false; }
}

bool IOXml::writeColorStructureDs(ofstream& ofObject, XM::ColorStructureDescriptor** csds, int framesFetch, int descSize) {
	if (ofObject.is_open()) {
		ofObject << "\t<CSDs numOfFrames=\"" << framesFetch << "\">\n";

		for (int i = 0; i < framesFetch; ++i) {
			ofObject << "\t\t<Frame numOfCoeff=\"" << descSize << "\">\n";
			ofObject << "\t\t\t<Coeff>\n";

			XM::ColorStructureDescriptor* csd = csds[i];
			unsigned int csd_num_coeff = csd->GetSize();
			string coeff = "\t\t\t\t";
			int counter = 0;
			for (unsigned int j = 0; j < csd_num_coeff; ++j) {
				coeff += to_string((int)csd->GetElement(j));
				coeff += " ";
				counter += 1;
				if ((counter >= 8) && (j < csd_num_coeff - 1)) {
					coeff += "\n\t\t\t\t";
					counter = 0;
				}
			}
			coeff += "\n";
			ofObject << coeff;

			ofObject << "\t\t\t</Coeff>\n";
			ofObject << "\t\t</Frame>\n";
		}

		ofObject << "\t</CSDs>\n";
		return true;

	}
	else { return false; }
}

bool IOXml::writeDominantColorDs(ofstream& ofObject, XM::DominantColorDescriptor** dcds, int framesFetch, 
	bool normalize, bool variance, bool spatial, int bin1, int bin2, int bin3) {
	if (ofObject.is_open()) {
		ofObject << "\t<DCDs numOfFrames=\"" << framesFetch << "\">\n";

		for (int i = 0; i < framesFetch; ++i) {

			string ofNormaize = (normalize) ? "true" : "false";
			string ofVariance = (variance) ? "true" : "false";
			string ofSpatial = (spatial) ? "true" : "false";

			XM::DominantColorDescriptor* dcd = dcds[i];
			int ndc = dcd->GetDominantColorsNumber();

			ofObject << "\t\t<Frame \n";
			ofObject << "\t\tifNormalize=\"" << ofNormaize << "\" ifVariance=\"" << ofVariance << "\" ifSpatial=\"" << ofSpatial << "\"\n";
			ofObject << "\t\tbin1=\"" << bin1 << "\" bin2=\"" << bin2 << "\" bin3=\"" << bin3 << "\" numOfColors=\"" << ndc << "\">\n";

			if (spatial) {
				ofObject << "\t\t\t<SpatialCoherency>" << to_string(dcd->GetSpatialCoherency()) << "</SpatialCoherency>\n";
			}
			XM::DOMCOL* domcol = dcd->GetDominantColors();
			for (int j = 0; j < ndc; ++j) {
				ofObject << "\t\t\t<ColorInfo>";
				string colorInfo;
				for (int k = 0; k < 3; ++k) {
					colorInfo += to_string(domcol[j].m_ColorValue[k]);
					colorInfo += " ";
				}
				if (variance) {
					for (int k = 0; k < 3; ++k) {
						colorInfo += to_string(domcol[j].m_ColorVariance[k]);
						colorInfo += " ";
					}
				}
				// colorInfo += "\n";
				ofObject << colorInfo;
				ofObject << "</ColorInfo>\n";
			}
			ofObject << "\t\t</Frame>\n";
		}

		ofObject << "\t</DCDs>\n";
		return true;

	}
	else { return false; }
}

bool IOXml::writeColorLayoutDs(ofstream& ofObject, XM::ColorLayoutDescriptor** clds, int framesFetch, 
	int numYCoeff, int numCCoeff) { 
	if (ofObject.is_open()) {
		ofObject << "\t<CLDs numOfFrames=\"" << framesFetch << "\">\n";

		for (int i = 0; i < framesFetch; ++i) {
			ofObject << "\t\t<Frame numOfYCoeff=\"" << numYCoeff << "\" numOfCCoeff=\"" << numCCoeff << "\">\n";
			XM::ColorLayoutDescriptor* cld = clds[i];

			int numberOfYCoeff = cld->GetNumberOfYCoeff();
			int numberOfCCoeff = cld->GetNumberOfCCoeff();

			int j, counter;

			ofObject << "\t\t\t<YCoeff>\n";
			int* y_coeff = cld->GetYCoeff();
			string s_y_coeff = "\t\t\t\t";
			counter = 0;
			for (j = 0; j < numberOfYCoeff; j++){
				s_y_coeff += to_string(y_coeff[j]);
				s_y_coeff += " ";
				counter += 1;
				if ((counter >= 8) && (j < numberOfYCoeff - 1)) {
					s_y_coeff += "\n\t\t\t\t";
					counter = 0;
				}
			}
			s_y_coeff += "\n";
			ofObject << s_y_coeff;
			ofObject << "\t\t\t</YCoeff>\n";

			ofObject << "\t\t\t<CbCoeff>\n";
			int* cb_coeff = cld->GetCbCoeff();
			string s_cb_coeff = "\t\t\t\t";
			counter = 0;
			for (j = 0; j < numberOfCCoeff; j++) {
				s_cb_coeff += to_string(cb_coeff[j]);
				s_cb_coeff += " ";
				counter += 1;
				if ((counter >= 8) && (i < numberOfCCoeff - 1)) {
					s_cb_coeff += "\n\t\t\t\t";
					counter = 0;
				}
			}
			s_cb_coeff += "\n";
			ofObject << s_cb_coeff;
			ofObject << "\t\t\t</CbCoeff>\n";

			ofObject << "\t\t\t<CrCoeff>\n";
			int* cr_coeff = cld->GetCrCoeff();
			string s_cr_coeff = "\t\t\t\t";
			counter = 0;
			for (j = 0; j < numberOfCCoeff; j++) {
				s_cr_coeff += to_string(cr_coeff[j]);
				s_cr_coeff += " ";
				counter += 1;
				if ((counter >= 8) && (i < numberOfCCoeff - 1)) {
					s_cr_coeff += "\n\t\t\t\t";
					counter = 0;
				}
			}
			s_cr_coeff += "\n";
			ofObject << s_cr_coeff;
			ofObject << "\t\t\t</CrCoeff>\n";

			ofObject << "\t\t</Frame>\n";
		}

		ofObject << "\t</CLDs>\n";
		return true;

	}
	else { return false; }
}

bool IOXml::writeEdgeHistogramDs(ofstream& ofObject, XM::EdgeHistogramDescriptor** ehds, int framesFetch) {
	if (ofObject.is_open()) {
		ofObject << "\t<EHDs numOfFrames=\"" << framesFetch << "\">\n";

		for (int i = 0; i < framesFetch; ++i) {

			XM::EdgeHistogramDescriptor* ehd = ehds[i];
			unsigned int ehd_num_coeff = ehd->GetSize();

			ofObject << "\t\t<Frame numOfCoeff=\"" << ehd_num_coeff << "\">\n";

			ofObject << "\t\t\t<Coeff>\n";
			string coeff = "\t\t\t\t";
			int counter = 0;
			char* de = ehd->GetEdgeHistogramElement();
			for (unsigned int j = 0; j < ehd_num_coeff; ++j) {
				coeff += to_string((int)de[j]);
				coeff += " ";
				counter += 1;
				if ((counter >= 8) && (j < ehd_num_coeff - 1)) {
					coeff += "\n\t\t\t\t";
					counter = 0;
				}
			}
			coeff += "\n";
			ofObject << coeff;

			ofObject << "\t\t\t</Coeff>\n";
			ofObject << "\t\t</Frame>\n";
		}

		ofObject << "\t</EHDs>\n";
		return true;

	}
	else { return false; }
}

bool IOXml::writeHomogeneousTextureDs(ofstream& ofObject, XM::HomogeneousTextureDescriptor** htds, int framesFetch, int layerFlag) {
	if (ofObject.is_open()) {
		ofObject << "\t<HTDs numOfFrames=\"" << framesFetch << "\">\n";

		for (int i = 0; i < framesFetch; ++i) {
			ofObject << "\t\t<Frame layerFlag=\"" << layerFlag << "\">\n";

			XM::HomogeneousTextureDescriptor* htd = htds[i];
			int* values = htd->GetHomogeneousTextureFeature();

			ofObject << "\t\t\t<mean>" << values[0] << "</mean>\n";
			ofObject << "\t\t\t<std>" << values[1] << "</std>\n";

			int counter, j;

			ofObject << "\t\t\t<Coeff1>\n";
			string coeff1 = "\t\t\t\t";
			counter = 0;
			for (j = 2; j < 32; ++j) {
				coeff1 += to_string(values[j]);
				coeff1 += " ";
				counter += 1;
				if ((counter >= 8) && (j < 32 - 1)) {
					coeff1 += "\n\t\t\t\t";
					counter = 0;
				}
			}
			coeff1 += "\n";
			ofObject << coeff1;
			ofObject << "\t\t\t</Coeff1>\n";

			if (layerFlag) {
				ofObject << "\t\t\t<Coeff2>\n";
				string coeff2 = "\t\t\t\t";
				counter = 0;
				for (j = 32; j < 62; ++j) {
					coeff2 += to_string(values[j]);
					coeff2 += " ";
					counter += 1;
					if ((counter >= 8) && (j < 62 - 1)) {
						coeff2 += "\n\t\t\t\t";
						counter = 0;
					}
				}
				coeff2 += "\n";
				ofObject << coeff2;
				ofObject << "\t\t\t</Coeff2>\n";
			}

			ofObject << "\t\t</Frame>\n";
		}

		ofObject << "\t</HTDs>\n";
		return true;

	}
	else { return false; }
}


// ------------------------------------ Read from Xmls ------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------
bool IOXml::readHomogeneousTextureDs(std::ifstream& ifObject, const char* path, 
	XM::HomogeneousTextureDescriptor**& htds, int& framesFetch, int& layerFlag) {
	if (!ifObject.is_open()) { cout << "File not open" << endl; return false; }
	string s;
	int idx = 0;
	int frameCounter = 0;
	int featureCounter = 0;
	bool inHTDs = false;
	bool inFrame = false;
	bool inCoeff1 = false;
	bool inCoeff2 = false;
	int* feature = nullptr;
	while (getline(ifObject, s)) {
		// read number of frames / framesFetch and setup htds.
		if ((!inHTDs) && ((int)s.find("<HTDs numOfFrames=\"") >= 0)) {
			inHTDs = true; 
			framesFetch = stoi(s.substr(s.find("=\"") + 2, s.find("\">") - s.find("=\"") - 2));
			htds = new XM::HomogeneousTextureDescriptor * [framesFetch];
			// cout << "framesFetch: " << framesFetch << endl;
			continue;
		}
		// mark end of file and exit reading.
		if (inHTDs && ((int)s.find("</HTDs>") >= 0)) {
			inHTDs = false; break; 
		}
		// find start of a frame, read layerFlag and setup int* feature.
		if (inHTDs && (!inFrame) && ((int)s.find("<Frame layerFlag=\"") >= 0)) {
			inFrame = true; 
			layerFlag = stoi(s.substr(s.find("=\"") + 2, s.find("\">") - s.find("=\"") - 2));
			if (layerFlag) { feature = new int[62]; }
			else { inCoeff2 = false;  feature = new int[32]; }
			// cout << "In Frame " << frameCounter << " - layerFlag: " << layerFlag << endl;
			continue;
		}
		// read contents in a frame.
		if (inHTDs && inFrame) {
			// read end of frame.
			if ((int)s.find("</Frame>") >= 0) {
				inFrame = false; 
				// set htd instance.
				htds[frameCounter] = new XM::HomogeneousTextureDescriptor(true);
				htds[frameCounter]->SetHomogeneousTextureFeature(true, feature);
				frameCounter += 1;
				featureCounter = 0;
				// cout << "End of Frame " << frameCounter - 1 << "." << endl;
				continue; 
			}
			// read mean.
			else if ((int)s.find("<mean>") >= 0) {
				feature[0] = stoi(s.substr(s.find("n>") + 2, s.find("</") - s.find("n>") - 2));
				// cout << "Mean: " << feature[0] << endl;
				featureCounter += 1; continue;
			}
			// read std.
			else if ((int)s.find("<std>") >= 0) {
				feature[1] = stoi(s.substr(s.find("d>") + 2, s.find("</") - s.find("d>") - 2));
				// cout << "std: " << feature[1] << endl;
				featureCounter += 1; continue;
			}
			// find start of <Coeff1>.
			else if ((int)s.find("<Coeff1>") >= 0) {
				inCoeff1 = true; featureCounter = 2; continue;
			}
			// find start of <Coeff2>.
			else if ((int)s.find("<Coeff2>") >= 0) {
				inCoeff2 = true; featureCounter = 32; continue;
			}
		}
		// in reading Coeff1.
		if (inHTDs && inFrame && inCoeff1) {
			if ((int)s.find("</Coeff1>") >= 0) { inCoeff1 = false; continue; }
			// keep only the numbers and spaces in the string
			string tmp = s.substr(s.find_first_of("0123456789"), s.find_last_of("0123456789") - s.find_first_of("0123456789") + 1);
			// reading 8 numbers
			for (int i = 0; i < 8; ++i) {
				feature[featureCounter] = stoi(tmp.substr(0, tmp.find(" "))); 
				// cout << feature[featureCounter] << " ";
				featureCounter += 1;
				// remove the last read number from the string.
				tmp = tmp.substr(tmp.find(" ") + 1, string::npos);
				if (featureCounter >= 32) { break; }
			}
			// cout << endl;
			continue;
		}
		// in reading Coeff2, same strategy as in Coeff1.
		if (inHTDs && inFrame && inCoeff2 && layerFlag) {
			if ((int)s.find("</Coeff2>") >= 0) { inCoeff2 = false; continue; }
			string tmp = s.substr(s.find_first_of("0123456789"), s.find_last_of("0123456789") - s.find_first_of("0123456789") + 1);
			for (int i = 0; i < 8; ++i) {
				feature[featureCounter] = stoi(tmp.substr(0, tmp.find(" "))); 
				// cout << feature[featureCounter] << " ";
				featureCounter += 1;
				tmp = tmp.substr(tmp.find(" ") + 1, string::npos);
				if (featureCounter >= 62) { break; }
			}
			// cout << endl;
			continue;
		}
	}
	return true;
}


bool IOXml::readEdgeHistogramDs(std::ifstream& ifObject, const char* path,
	XM::EdgeHistogramDescriptor**& ehds, int& framesFetch) {
	if (!ifObject.is_open()) { cout << "File not open" << endl; return false; }
	string s;
	int frameCounter = 0;
	int featureCounter = 0;
	bool inEHDs = false;
	bool inFrame = false;
	bool inCoeff = false;
	int numCoeff = 0;
	char* feature = nullptr;
	while (getline(ifObject, s)) {
		// read number of frames / framesFetch and setup htds.
		if ((!inEHDs) && ((int)s.find("<EHDs numOfFrames=\"") >= 0)) {
			inEHDs = true;
			framesFetch = stoi(s.substr(s.find("=\"") + 2, s.find("\">") - s.find("=\"") - 2));
			ehds = new XM::EdgeHistogramDescriptor * [framesFetch];
			// cout << "framesFetch: " << framesFetch << endl;
			continue;
		}
		// mark end of file and exit reading.
		if (inEHDs && ((int)s.find("</EHDs>") >= 0)) {
			inEHDs = false; break;
		}
		// find start of a frame, read layerFlag and setup int* feature.
		if (inEHDs && (!inFrame) && ((int)s.find("<Frame numOfCoeff=\"") >= 0)) {
			inFrame = true;
			numCoeff = stoi(s.substr(s.find("=\"") + 2, s.find("\">") - s.find("=\"") - 2));
			feature = new char[numCoeff]; 
			// cout << "In Frame " << frameCounter << " - numOfCoeff: " << numCoeff << endl;
			continue;
		}
		// read contents in a frame.
		if (inEHDs && inFrame) {
			// read end of frame.
			if ((int)s.find("</Frame>") >= 0) {
				inFrame = false;
				// set htd instance.
				ehds[frameCounter] = new XM::EdgeHistogramDescriptor();
				ehds[frameCounter]->SetEdgeHistogramElement(feature);
				frameCounter += 1;
				featureCounter = 0;
				// cout << "End of Frame " << frameCounter - 1 << "." << endl;
				continue;
			}
			// find start of <Coeff>.
			else if ((int)s.find("<Coeff>") >= 0) {
				inCoeff = true; featureCounter = 0; continue;
			}
		}
		// in reading Coeff1
		if (inEHDs && inFrame && inCoeff) {
			if ((int)s.find("</Coeff>") >= 0) { inCoeff = false; continue; }
			// keep only the numbers and spaces in the string.
			string tmp = s.substr(s.find_first_of("0123456789"), s.find_last_of("0123456789") - s.find_first_of("0123456789") + 1);
			// reading 8 numbers until there are 80 values.
			for (int i = 0; i < 8; ++i) {
				feature[featureCounter] = stoi(tmp.substr(0, tmp.find(" ")));
				// cout << (int)feature[featureCounter] << " ";
				featureCounter += 1;
				// remove the last read number from the string.
				tmp = tmp.substr(tmp.find(" ") + 1, string::npos);
				if (featureCounter >= numCoeff) { break; }
			}
			// cout << endl;
			continue;
		}
	}
	return true;
}

bool IOXml::readColorLayoutDs(std::ifstream& ifObject, const char* path,
	XM::ColorLayoutDescriptor**& clds, int& framesFetch, int& numYCoeff, int& numCCoeff) {
	if (!ifObject.is_open()) { cout << "File not open" << endl; return false; }
	string s;
	int idx = 0;
	int frameCounter = 0, YCoeffCounter = 0, CCoeffCounter = 0;
	bool inCLDs = false, inFrame = false;
	bool inYCoeff = false, inCbCoeff = false, inCrCoeff = false;
	int* YCoeff = nullptr;
	int* CbCoeff = nullptr;
	int* CrCoeff = nullptr;
	while (getline(ifObject, s)) {
		// read number of frames / framesFetch and setup htds.
		if ((!inCLDs) && ((int)s.find("<CLDs numOfFrames=\"") >= 0)) {
			inCLDs = true;
			framesFetch = stoi(s.substr(s.find("=\"") + 2, s.find("\">") - s.find("=\"") - 2));
			clds = new XM::ColorLayoutDescriptor * [framesFetch];
			// cout << "framesFetch: " << framesFetch << endl;
			continue;
		}
		// mark end of file and exit reading.
		if (inCLDs && ((int)s.find("</CLDs>") >= 0)) {
			inCLDs = false; break;
		}
		// find start of a frame, read layerFlag and setup int* feature.
		if (inCLDs && (!inFrame) && ((int)s.find("<Frame ") >= 0)) {
			inFrame = true;
			numYCoeff = stoi(s.substr(s.find("numOfYCoeff") + 13, s.find("\" numOfCCoeff") - s.find("numOfYCoeff") - 13));
			numCCoeff = stoi(s.substr(s.find("numOfCCoeff") + 13, s.find("\">") - s.find("numOfCCoeff") - 13));
			YCoeff = new int[numYCoeff];
			CbCoeff = new int[numCCoeff];
			CrCoeff = new int[numCCoeff];
			// cout << "In Frame " << frameCounter << " - numYCoeff: " << numYCoeff << ", numCCoeff: " << numYCoeff << endl;
			continue;
		}
		// read contents in a frame.
		if (inCLDs && inFrame) {
			// read end of frame.
			if ((int)s.find("</Frame>") >= 0) {
				inFrame = false;
				// set cld instance.
				clds[frameCounter] = new XM::ColorLayoutDescriptor(numYCoeff, numCCoeff);
				clds[frameCounter]->SetYCoeff(YCoeff);
				clds[frameCounter]->SetCbCoeff(CbCoeff);
				clds[frameCounter]->SetCrCoeff(CrCoeff);
				frameCounter += 1;
				YCoeffCounter = 0; CCoeffCounter = 0;
				// cout << "End of Frame " << frameCounter - 1 << "." << endl;
				continue;
			}
			// find start of <YCoeff>.
			else if ((int)s.find("<YCoeff>") >= 0) {
				inYCoeff = true; YCoeffCounter = 0; continue;
			}
			// find start of <CbCoeff>.
			else if ((int)s.find("<CbCoeff>") >= 0) {
				inCbCoeff = true; CCoeffCounter = 0; continue;
			}
			// find start of <CrCoeff>.
			else if ((int)s.find("<CrCoeff>") >= 0) {
				inCrCoeff = true; CCoeffCounter = 0; continue;
			}
		}
		// in reading YCoeff.
		if (inCLDs && inFrame && inYCoeff) {
			if ((int)s.find("</YCoeff>") >= 0) { inYCoeff = false; continue; }
			// keep only the numbers and spaces in the string
			string tmp = s.substr(s.find_first_of("0123456789"), s.find_last_of("0123456789") - s.find_first_of("0123456789") + 1);
			// reading 8 numbers
			for (int i = 0; i < 8; ++i) {
				YCoeff[YCoeffCounter] = stoi(tmp.substr(0, tmp.find(" ")));
				// cout << YCoeff[YCoeffCounter] << " ";
				YCoeffCounter += 1;
				// remove the last read number from the string.
				tmp = tmp.substr(tmp.find(" ") + 1, string::npos);
				if (YCoeffCounter >= numYCoeff) { break; }
			}
			// cout << endl;
			continue;
		}
		// in reading CbCoeff.
		if (inCLDs && inFrame && inCbCoeff) {
			if ((int)s.find("</CbCoeff>") >= 0) { inCbCoeff = false; continue; }
			// keep only the numbers and spaces in the string
			string tmp = s.substr(s.find_first_of("0123456789"), s.find_last_of("0123456789") - s.find_first_of("0123456789") + 1);
			// reading 8 numbers
			for (int i = 0; i < 8; ++i) {
				CbCoeff[CCoeffCounter] = stoi(tmp.substr(0, tmp.find(" ")));
				// cout << CbCoeff[CCoeffCounter] << " ";
				CCoeffCounter += 1;
				// remove the last read number from the string.
				tmp = tmp.substr(tmp.find(" ") + 1, string::npos);
				if (CCoeffCounter >= numCCoeff) { break; }
			}
			// cout << endl;
			continue;
		}
		// in reading CrCoeff.
		if (inCLDs && inFrame && inCrCoeff) {
			if ((int)s.find("</CrCoeff>") >= 0) { inCrCoeff = false; continue; }
			// keep only the numbers and spaces in the string
			string tmp = s.substr(s.find_first_of("0123456789"), s.find_last_of("0123456789") - s.find_first_of("0123456789") + 1);
			// reading 8 numbers
			for (int i = 0; i < 8; ++i) {
				CrCoeff[CCoeffCounter] = stoi(tmp.substr(0, tmp.find(" ")));
				// cout << CrCoeff[CCoeffCounter] << " ";
				CCoeffCounter += 1;
				// remove the last read number from the string.
				tmp = tmp.substr(tmp.find(" ") + 1, string::npos);
				if (CCoeffCounter >= numCCoeff) { break; }
			}
			// cout << endl;
			continue;
		}
	}
	return true;
}

bool IOXml::readDominantColorDs(std::ifstream& ifObject, const char* path,
	XM::DominantColorDescriptor**& dcds, int& framesFetch,
	bool& normalize, bool& vairance, bool& spatial, int& bin1, int& bin2, int& bin3) {
	return true;
}

bool IOXml::readColorStructureDs(std::ifstream& ifObject, const char* path,
	XM::ColorStructureDescriptor**& csds, int& framesFetch, int& numCoeff) {
	if (!ifObject.is_open()) { cout << "File not open" << endl; return false; }
	string s;
	int frameCounter = 0;
	unsigned long featureCounter = 0;
	bool inCSDs = false, inFrame = false,  inCoeff = false;
	int feature_instance = 0;
	while (getline(ifObject, s)) {
		// read number of frames / framesFetch and setup htds.
		if ((!inCSDs) && ((int)s.find("<CSDs numOfFrames=\"") >= 0)) {
			inCSDs = true;
			framesFetch = stoi(s.substr(s.find("=\"") + 2, s.find("\">") - s.find("=\"") - 2));
			csds = new XM::ColorStructureDescriptor * [framesFetch];
			// cout << "framesFetch: " << framesFetch << endl;
			continue;
		}
		// mark end of file and exit reading.
		if (inCSDs && ((int)s.find("</CSDs>") >= 0)) {
			inCSDs = false; break;
		}
		// find start of a frame, read layerFlag and setup int* feature.
		if (inCSDs && (!inFrame) && ((int)s.find("<Frame numOfCoeff=\"") >= 0)) {
			inFrame = true;
			numCoeff = stoi(s.substr(s.find("=\"") + 2, s.find("\">") - s.find("=\"") - 2));
			// cout << "In Frame " << frameCounter << " - numOfCoeff: " << numCoeff << endl;
			// set htd instance.
			csds[frameCounter] = new XM::ColorStructureDescriptor();
			csds[frameCounter]->SetSize((unsigned long)numCoeff);
			continue;
		}
		// read contents in a frame.
		if (inCSDs && inFrame) {
			// read end of frame.
			if ((int)s.find("</Frame>") >= 0) {
				inFrame = false;
				frameCounter += 1;
				featureCounter = 0;
				// cout << "End of Frame " << frameCounter - 1 << "." << endl;
				continue;
			}
			// find start of <Coeff>.
			else if ((int)s.find("<Coeff>") >= 0) {
				inCoeff = true; featureCounter = 0; continue;
			}
		}
		// in reading Coeff1
		if (inCSDs && inFrame && inCoeff) {
			if ((int)s.find("</Coeff>") >= 0) { inCoeff = false; continue; }
			// keep only the numbers and spaces in the string.
			string tmp = s.substr(s.find_first_of("0123456789"), s.find_last_of("0123456789") - s.find_first_of("0123456789") + 1);
			// reading 8 numbers until there are 80 values.
			for (int i = 0; i < 8; ++i) {
				int val = stoi(tmp.substr(0, tmp.find(" ")));
				csds[frameCounter]->SetElement(featureCounter, val);
				// cout << val << " ";
				featureCounter += 1;
				// remove the last read number from the string.
				tmp = tmp.substr(tmp.find(" ") + 1, string::npos);
				if (featureCounter >= numCoeff) { break; }
			}
			// cout << endl;
			continue;
		}
	}
	return true;
}

bool IOXml::readScalableColorDs(std::ifstream& ifObject, const char* path,
	XM::ScalableColorDescriptor**& scds, int& framesFetch, int& numCoeff, int& bitPlanesDiscarded) {

	if (!ifObject.is_open()) { cout << "File not open" << endl; return false; }
	string s;
	int frameCounter = 0, featureCounter = 0;
	bool inSCDs = false, inFrame = false, inCoeff = false;
	int feature_instance = 0;
	while (getline(ifObject, s)) {
		// read number of frames / framesFetch and setup htds.
		if ((!inSCDs) && ((int)s.find("<SCDs numOfFrames=\"") >= 0)) {
			inSCDs = true;
			framesFetch = stoi(s.substr(s.find("=\"") + 2, s.find("\">") - s.find("=\"") - 2));
			scds = new XM::ScalableColorDescriptor * [framesFetch];
			// cout << "framesFetch: " << framesFetch << endl;
			continue;
		}
		// mark end of file and exit reading.
		if (inSCDs && ((int)s.find("</SCDs>") >= 0)) {
			inSCDs = false; break;
		}
		// find start of a frame, read layerFlag and setup int* feature.
		if (inSCDs && (!inFrame) && ((int)s.find("<Frame numOfCoeff") >= 0)) {
			inFrame = true;
			numCoeff = stoi(s.substr(s.find("numOfCoeff") + 12, s.find("numOfBit") - 2 - s.find("numOfCoeff") - 12));
			bitPlanesDiscarded = stoi(s.substr(s.find("numOfBitPlanesD") + 17, s.find("\">") - s.find("numOfBitPlanesD") - 17));
			// cout << "In Frame " << frameCounter << " - numOfCoeff: " << numCoeff << endl;
			scds[frameCounter] = new XM::ScalableColorDescriptor();
			scds[frameCounter]->SetNumberOfCoefficients(numCoeff);
			scds[frameCounter]->SetNumberOfBitplanesDiscarded(bitPlanesDiscarded);
			continue;
		}
		// read contents in a frame.
		if (inSCDs && inFrame) {
			// read end of frame.
			if ((int)s.find("</Frame>") >= 0) {
				inFrame = false;
				frameCounter += 1;
				featureCounter = 0;
				// cout << "End of Frame " << frameCounter - 1 << "." << endl;
				continue;
			}
			// find start of <Coeff>.
			else if ((int)s.find("<Coeff>") >= 0) {
				inCoeff = true; featureCounter = 0; continue;
			}
		}
		// in reading Coeff1
		if (inSCDs && inFrame && inCoeff) {
			if ((int)s.find("</Coeff>") >= 0) { inCoeff = false; continue; }
			// keep only the numbers and spaces in the string.
			string tmp = s.substr(s.find_first_of("-0123456789"), s.find_last_of("-0123456789") - s.find_first_of("-0123456789") + 1);
			// reading 8 numbers until there are 80 values.
			for (int i = 0; i < 8; ++i) {
				int val = stoi(tmp.substr(0, tmp.find(" ")));
				scds[frameCounter]->SetCoefficient((unsigned long)featureCounter, val);
				// cout << val << " ";
				featureCounter += 1;
				// remove the last read number from the string.
				tmp = tmp.substr(tmp.find(" ") + 1, string::npos);
				if (featureCounter >= numCoeff) { break; }
			}
			// cout << endl;
			continue;
		}
	}
	return true;
}

// ------------------------------------- Verification -------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------
bool IOXml::verifyHomogeneousTextureDs(const char* videoPath, const char* listPath) {

	bool ifHave = false;

	ReadProcessFrames* obj = nullptr;
	XM::HomogeneousTextureDescriptor** ptr = nullptr;
	int framesFetch = 0;

	ifstream listObject(listPath);
	if (!listObject.is_open()) { cout << "Invalid list path." << endl; return false; }

	string s;
	while (getline(listObject, s)) {

		ifstream ifObject(s);
		if (!ifObject.is_open()) { cout << "Invalid path in the list." << endl; ifObject.close(); continue; }

		XM::HomogeneousTextureDescriptor** htds = nullptr;
		int layerFlag;
		if (readHomogeneousTextureDs(ifObject, NULL, htds, framesFetch, layerFlag)) {
			if (obj == nullptr) {
				obj = new ReadProcessFrames(videoPath, framesFetch);
				ptr = obj->framesToHTD(layerFlag, false, false);
			}
			else if (obj->getFramesFetch() != framesFetch) {
				for (int i = 0; i < obj->getFramesFetch(); ++i) {
					delete ptr[i];
				}
				delete[] ptr;
				delete obj;
				obj = new ReadProcessFrames(videoPath, framesFetch);
				ptr = obj->framesToHTD(layerFlag, false, false);
			}
			double dist = 0.0;
			for (int i = 0; i < framesFetch; ++i) {
				dist += ptr[i]->distance(htds[i]);

				delete htds[i];
			}
			dist = dist / framesFetch;
			delete[] htds;
			cout << dist << endl; // " " << s << endl;
			ifHave = ( ifHave || (dist <= 0.1) );
		}
		else {}
		ifObject.close();
	}
	listObject.close();

	if (ptr != nullptr) { for (int i = 0; i < framesFetch; ++i) { delete ptr[i]; } }
	delete[] ptr;
	delete obj;

	return ifHave;
}

bool IOXml::verifyEdgeHistogramDs(const char* videoPath, const char* listPath) {

	bool ifHave = false;

	ReadProcessFrames* obj = nullptr;
	XM::EdgeHistogramDescriptor** ptr = nullptr;
	int framesFetch = 0;

	ifstream listObject(listPath);
	if (!listObject.is_open()) { cout << "Invalid list path." << endl; return false; }

	string s;
	while (getline(listObject, s)) {

		ifstream ifObject(s);
		if (!ifObject.is_open()) { cout << "Invalid path in the list." << endl; ifObject.close(); continue; }

		XM::EdgeHistogramDescriptor** ehds = nullptr;
		if (readEdgeHistogramDs(ifObject, NULL, ehds, framesFetch)) {
			if (obj == nullptr) {
				obj = new ReadProcessFrames(videoPath, framesFetch);
				ptr = obj->framesToEHD(false, false);
			}
			else if (obj->getFramesFetch() != framesFetch) {
				for (int i = 0; i < obj->getFramesFetch(); ++i) {
					delete ptr[i];
				}
				delete[] ptr;
				delete obj;
				obj = new ReadProcessFrames(videoPath, framesFetch);
				ptr = obj->framesToEHD(false, false);
			}
			double dist = 0.0;
			for (int i = 0; i < framesFetch; ++i) {
				dist += ptr[i]->distanceNorm(ehds[i]);

				delete ehds[i];
			}
			dist = dist / framesFetch;
			delete[] ehds;
			cout << dist << endl; // " " << s << endl;
			ifHave = (ifHave || (dist <= 0.1));
		}
		else {}
		ifObject.close();
	}
	listObject.close();

	if (ptr != nullptr) { for (int i = 0; i < framesFetch; ++i) { delete ptr[i]; } }
	delete[] ptr;
	delete obj;

	return ifHave;
}

bool IOXml::verifyColorLayoutDs(const char* videoPath, const char* listPath) {

	bool ifHave = false;

	ReadProcessFrames* obj = nullptr;
	XM::ColorLayoutDescriptor** ptr = nullptr;
	int framesFetch = 0;

	ifstream listObject(listPath);
	if (!listObject.is_open()) { cout << "Invalid list path." << endl; return false; }

	string s;
	while (getline(listObject, s)) {

		ifstream ifObject(s);
		if (!ifObject.is_open()) { cout << "Invalid path in the list." << endl; ifObject.close(); continue; }

		XM::ColorLayoutDescriptor** clds = nullptr;
		int numYCoeff = 0, numCCoeff = 0;
		if (readColorLayoutDs(ifObject, NULL, clds, framesFetch, numYCoeff, numCCoeff)) {
			if (obj == nullptr) {
				obj = new ReadProcessFrames(videoPath, framesFetch);
				ptr = obj->framesToCLD(numYCoeff, numCCoeff, false, false);
			}
			else if (obj->getFramesFetch() != framesFetch) {
				for (int i = 0; i < obj->getFramesFetch(); ++i) {
					delete ptr[i];
				}
				delete[] ptr;
				delete obj;
				obj = new ReadProcessFrames(videoPath, framesFetch);
				ptr = obj->framesToCLD(numYCoeff, numCCoeff, false, false);
			}
			double dist = 0.0;
			for (int i = 0; i < framesFetch; ++i) {
				dist += ptr[i]->distanceNorm(clds[i]);

				delete clds[i];
			}
			dist = dist / framesFetch;
			delete[] clds;
			cout << dist << endl; // " " << s << endl;
			ifHave = (ifHave || (dist <= 0.1));
		}
		else {}
		ifObject.close();
	}
	listObject.close();

	if (ptr != nullptr) { for (int i = 0; i < framesFetch; ++i) { delete ptr[i]; } }
	delete[] ptr;
	delete obj;

	return ifHave;
}

bool IOXml::verifyColorStructureDs(const char* videoPath, const char* listPath) {

	bool ifHave = false;

	ReadProcessFrames* obj = nullptr;
	XM::ColorStructureDescriptor** ptr = nullptr;
	int framesFetch = 0;

	ifstream listObject(listPath);
	if (!listObject.is_open()) { cout << "Invalid list path." << endl; return false; }

	string s;
	while (getline(listObject, s)) {

		ifstream ifObject(s);
		if (!ifObject.is_open()) { cout << "Invalid path in the list." << endl; ifObject.close(); continue; }

		XM::ColorStructureDescriptor** csds = nullptr;
		int numCoeff = 0;
		if (readColorStructureDs(ifObject, NULL, csds, framesFetch, numCoeff)) {
			if (obj == nullptr) {
				obj = new ReadProcessFrames(videoPath, framesFetch);
				ptr = obj->framesToCSD(numCoeff, false, false);
			}
			else if (obj->getFramesFetch() != framesFetch) {
				for (int i = 0; i < obj->getFramesFetch(); ++i) {
					delete ptr[i];
				}
				delete[] ptr;
				delete obj;
				obj = new ReadProcessFrames(videoPath, framesFetch);
				ptr = obj->framesToCSD(numCoeff, false, false);
			}

			double dist = 0.0;
			for (int i = 0; i < framesFetch; ++i) {
				dist += ptr[i]->distance(csds[i]);

				delete csds[i];
			}
			dist = dist / framesFetch;
			delete[] csds;
			cout << dist << endl; // " " << s << endl;
			ifHave = (ifHave || (dist <= 0.1));
		}
		else {}
		ifObject.close();
	}
	listObject.close();

	if (ptr != nullptr) { for (int i = 0; i < framesFetch; ++i) { delete ptr[i]; } }
	delete[] ptr;
	delete obj;

	return ifHave;
}

bool IOXml::verifyScalableColorDs(const char* videoPath, const char* listPath) {

	bool ifHave = false;

	ReadProcessFrames* obj = nullptr;
	XM::ScalableColorDescriptor** ptr = nullptr;
	int framesFetch = 0;

	ifstream listObject(listPath);
	if (!listObject.is_open()) { cout << "Invalid list path." << endl; return false; }

	string s;
	while (getline(listObject, s)) {

		ifstream ifObject(s);
		if (!ifObject.is_open()) { cout << "Invalid path in the list." << endl; ifObject.close(); continue; }

		XM::ScalableColorDescriptor** scds = nullptr;
		int numCoeff = 0, bitPlanesDiscarded = 0;
		if (readScalableColorDs(ifObject, NULL, scds, framesFetch, numCoeff, bitPlanesDiscarded)) {
			if (obj == nullptr) {
				obj = new ReadProcessFrames(videoPath, framesFetch);
				ptr = obj->framesToSCD(true, numCoeff, bitPlanesDiscarded, false, false);
			}
			else if (obj->getFramesFetch() != framesFetch) {
				for (int i = 0; i < obj->getFramesFetch(); ++i) {
					delete ptr[i];
				}
				delete[] ptr;
				delete obj;
				obj = new ReadProcessFrames(videoPath, framesFetch);
				ptr = obj->framesToSCD(true, numCoeff, bitPlanesDiscarded, false, false);
			}
			double dist = 0.0;
			for (int i = 0; i < framesFetch; ++i) {
				dist += ptr[i]->distanceNorm(scds[i]);

				delete scds[i];
			}
			dist = dist / framesFetch;
			delete[] scds;
			cout << dist << endl; // " " << s << endl;
			ifHave = (ifHave || (dist <= 0.1));
		}
		else {}
		ifObject.close();
	}
	listObject.close();

	if (ptr != nullptr) { for (int i = 0; i < framesFetch; ++i) { delete ptr[i]; } }
	delete[] ptr;
	delete obj;

	return ifHave;
}