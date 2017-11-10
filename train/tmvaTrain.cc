#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <TFile.h>
#include <TString.h>
#include <TMVA/Factory.h>

template<class T>
std::string toString(const T &value) {
 std::ostringstream os;
 os << value;
 return os.str();
}  

int main() {  

  int arr [12] {1, 2, 5, 10, 20, 50, 100, 200, 500, 1000, 10000, 50000};
  //int arr [2] {200,500};
  
// Create ouput file, factory object and open the input file

  TFile* outputFile = TFile::Open( "TMVA.root", "RECREATE" );
  TMVA::Factory* factory = new TMVA::Factory("tmvaTest", outputFile, "");
  TFile* trainingFile = new TFile("../generate/trainingData.root");
  TFile* testFile = new TFile("../generate/testData.root");

// get the TTree objects from the input files

  TTree* sigTrain = (TTree*)trainingFile->Get("sig");
  TTree* bkgTrain = (TTree*)trainingFile->Get("bkg");
  int nSigTrain = sigTrain->GetEntries();
  int nBkgTrain = bkgTrain->GetEntries();

  TTree* sigTest = (TTree*)testFile->Get("sig");
  TTree* bkgTest = (TTree*)testFile->Get("bkg");
  int nSigTest = sigTest->GetEntries();
  int nBkgTest = bkgTest->GetEntries();

// global event weights (see below for setting event-wise weights)

  double sigWeight = 1.0;
  double bkgWeight = 1.0;
  factory->AddSignalTree(sigTrain, sigWeight, TMVA::Types::kTraining);
  factory->AddBackgroundTree(bkgTrain, bkgWeight, TMVA::Types::kTraining);
  factory->AddSignalTree(sigTest, sigWeight, TMVA::Types::kTesting);
  factory->AddBackgroundTree(bkgTest, bkgWeight, TMVA::Types::kTesting);
   
// Define the input variables that shall be used for the MVA training
// (the variables used in the expression must exist in the original TTree).

  factory->AddVariable("x", 'F');
  factory->AddVariable("y", 'F');
  factory->AddVariable("z", 'F');

// Book MVA methods (see TMVA manual).  

  // factory->BookMethod(TMVA::Types::kFisher, "Fisher", "H:!V:Fisher");  
  // factory->BookMethod(TMVA::Types::kMLP, "MLP", "H:!V:HiddenLayers=3");  
  
	 
  for (int k = 0; k<12; k++){
  	  std::string iterations = toString(arr [k]);
	  factory->BookMethod(TMVA::Types::kBDT, "BDT" + iterations, "NTrees=" + iterations + ":BoostType=AdaBoost");	
  }
  
// Train, test and evaluate all methods

  factory->TrainAllMethods();
  factory->TestAllMethods();
  factory->EvaluateAllMethods();    

// Save the output and finish up

  outputFile->Close();
  std::cout << "==> wrote root file TMVA.root" << std::endl;
  std::cout << "==> TMVAnalysis is done!" << std::endl; 

  delete factory;
  return 0;

}
