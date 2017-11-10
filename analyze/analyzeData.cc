#include <iostream>
#include <fstream>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include "Event.h"
#include <TMVA/Reader.h>
#include <sstream>
#include <string>

using namespace std;

template<class T>
std::string toString(const T &value) {
 std::ostringstream os;
 os << value;
 return os.str();
}  

int main() {
	ofstream fout;
	fout.open("stats_ps5.txt");
	
	string signame ("hSig");
	string bkgname ("hBkg");
	
  int arr [12] {1, 2, 5, 10, 20, 50, 100, 200, 500, 1000, 10000, 50000};
  //int arr [2] {200,500};
  TFile* histFile = new TFile("analysis.root", "RECREATE");
  for (int k = 0; k<12; k++){
	
	double count1 = 0.0;
	double count2 = 0.0;	
	
	std::string iterations = toString(arr [k]);
	// TFile* histFile = new TFile("analysis_BDT" + iterations + ".root", "RECREATE");
	// Set up an output file and book some histograms	  
	  
	// string signal = signame + iterations;
	// string background = bkgname + iterations;	  
	  
	TH1D* hSig = new TH1D("hSig", "Fisher, signal", 100, -1.0, 1.0);
	TH1D* hBkg = new TH1D("hBkg", "Fisher, background", 100, -1.0, 1.0);
	
	// Set up the TMVA Reader object.
	// The names in AddVariable must be same as in the input (weight) file.
	
	  TMVA::Reader* reader = new TMVA::Reader();
	  float x, y, z;                       // TMVA needs float, not double
	  reader->AddVariable("x", &x);  
	  reader->AddVariable("y", &y);
	  reader->AddVariable("z", &z);
	  std::string dir    = "../train/weights/";
	  std::string prefix = "tmvaTest";
	  //reader->BookMVA("Fisher", dir + prefix + "_Fisher.weights.xml");
	  reader->BookMVA("BDT" + iterations, dir + prefix + "_BDT" + iterations + ".weights.xml");

	
	// Open input file, get the TTrees, put into a vector
	
	  TFile* inputFile = new TFile("../generate/testData.root");
	  inputFile->ls();
	  TTree* sig = dynamic_cast<TTree*>(inputFile->Get("sig"));
	  TTree* bkg = dynamic_cast<TTree*>(inputFile->Get("bkg"));
	  std::vector<TTree*> treeVec;
	  treeVec.push_back(sig);
	  treeVec.push_back(bkg);
	
	// Loop over TTrees
	
	  int nSigAccFish = 0;
	  int nBkgAccFish = 0;
	  int nSig, nBkg;
	  int count = 0;
	  const double tCutFisher = 0.0;
	  const double tCutMLP = 0.5;
	  for (int i=0; i<treeVec.size(); i++){
	
	    treeVec[i]->Print();
	    Event evt;
	    treeVec[i]->SetBranchAddress("evt", &evt);
	    int numEntries = treeVec[i]->GetEntries();
	    if ( i == 0 ) { nSig = numEntries; }
	    if ( i == 1 ) { nBkg = numEntries; }
	    std::cout << "number of entries = " << numEntries << std::endl;
	
	// Loop over events.  The test statistic is identified by the first 
	// argument used above in BookMVA (below, e.g., "Fisher").
	
	    for (int j=0; j<numEntries; j++){
	      treeVec[i]->GetEntry(j);                // sets evt
	      x = evt.x;                              // set variables of reader
	      y = evt.y;
	      z = evt.z;
	
	      double tFisher = reader->EvaluateMVA("BDT" + iterations);
	      if ( i == 0 ){                       // signal
	        hSig->Fill(tFisher);
			  if ( tFisher < 0 ){	        
	        	count1 += 1.0;
	        }
	      }
	      else if ( i == 1 ){                  // background
	        hBkg->Fill(tFisher);
	        if ( tFisher > 0){
	        	count2 +=1.0;
	        }
	      }
	
	      // ADD YOUR CODE HERE TO INCLUDE OTHER CLASSIFIERS SUCH AS MLP
	      // AND COUNT THE NUMBERS OF SELECTED EVENTS (COMPARE CLASSIFIER
	      // VALUE TFISHER, ETC., TO TCUT).
	
	
	
	
	    }
		hSig->Draw();	
	  }
	  fout << iterations << " iterations " << count1/10000.0 << " signal " << count2/10000.0 << " bkg " << endl;
  }
  fout.close();


  histFile->Write();
  histFile->Close();
  return 0;

}
