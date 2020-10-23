#include <map>
#include <string>

#include "TH1.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"

class WWlep : public edm::EDAnalyzer {

public:
  explicit WWlep(const edm::ParameterSet&);
  ~WWlep();
  
private:

  virtual void beginJob() ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;
  
  // simple map to contain all histograms; 
  // histograms are booked in the beginJob() 
  // method
  std::map<std::string,TH1F*> histContainer_; 

  // input tags  
  //edm::InputTag elecSrc_;
  //edm::InputTag muonSrc_;
  edm::EDGetTokenT<edm::View<pat::Electron>> elecSrc_;
  edm::EDGetTokenT<edm::View<pat::Muon>> muonSrc_;
};


WWlep::WWlep(const edm::ParameterSet& iConfig):

  histContainer_(),
  //elecSrc_(iConfig.getUntrackedParameter<edm::InputTag>("electronSrc")),
  //muonSrc_(iConfig.getUntrackedParameter<edm::InputTag>("muonSrc")),
  elecSrc_(consumes<edm::View<pat::Electron>>(iConfig.getUntrackedParameter<edm::InputTag>("electronSrc"))),
  muonSrc_(consumes<edm::View<pat::Muon>>(iConfig.getUntrackedParameter<edm::InputTag>("muonSrc")))
{
}

WWlep::~WWlep()
{
}

void
WWlep::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  // get electron collection
  edm::Handle<edm::View<pat::Electron> > electrons;
  //iEvent.getByLabel(elecSrc_,electrons);
  iEvent.getByToken(elecSrc_,electrons);

  // get muon collection
  edm::Handle<edm::View<pat::Muon> > muons;
  //iEvent.getByLabel(muonSrc_,muons);
  iEvent.getByToken(muonSrc_,muons);

    
  // loop over muons
  for(edm::View<pat::Muon>::const_iterator muon1=muons->begin(); muon1!=muons->end(); ++muon1){
   histContainer_["muonPt"]->Fill(muon1->pt());
   histContainer_["muonEta"]->Fill(muon1->eta());
   histContainer_["muonPhi"]->Fill(muon1->phi());

  if( muon1->pt()>20 && fabs(muon1->eta())<2.1 ){
	    for(edm::View<pat::Muon>::const_iterator muon2=muons->begin(); muon2!=muons->end(); ++muon2){
	      if(muon2>muon1){ // prevent double conting
		if( muon1->charge()*muon2->charge()<0 ){ // check only muon pairs of unequal charge 
		  if( muon2->pt()>20 && fabs(muon2->eta())<2.1 ){
		    histContainer_["mumuMass"]->Fill( (muon1->p4()+muon2->p4()).mass() );
		  }
		}
	      }
	    }
	  }




  }

  // loop over electrons
  for(edm::View<pat::Electron>::const_iterator electron=electrons->begin(); electron!=electrons->end(); ++electron){
   histContainer_["elePt"]->Fill(electron->pt());
   histContainer_["eleEta"]->Fill(electron->eta());
   histContainer_["elePhi"]->Fill(electron->phi());
  }


  // Multiplicity
  histContainer_["eleMult" ]->Fill(electrons->size());
  histContainer_["muonMult"]->Fill(muons->size() );
}

void 
WWlep::beginJob()
{
  // register to the TFileService
  edm::Service<TFileService> fs;


  histContainer_["mumuMass"]=fs->make<TH1F>("mumuMass", "mass",    90,   30., 120.);
  
  // book histograms for Multiplicity:

  histContainer_["eleMult"]=fs->make<TH1F>("eleMult",   "electron multiplicity", 100, 0,  50);
  histContainer_["muonMult"]=fs->make<TH1F>("muonMult",   "muon multiplicity",     100, 0,  50);

  // book histograms for Pt:

  histContainer_["elePt"]=fs->make<TH1F>("elePt",   "electron Pt", 100, 0,  200);
  histContainer_["muonPt"]=fs->make<TH1F>("muonPt",   "muon Pt", 100, 0, 200);

 // book histograms for Eta:
  histContainer_["eleEta"]=fs->make<TH1F>("eleEta",   "electron Eta",100, -5,  5);
  histContainer_["muonEta"]=fs->make<TH1F>("muonEta",   "muon Eta",  100, -5,  5);


 // book histograms for Phi:
  histContainer_["elePhi"]=fs->make<TH1F>("elePhi",   "electron Phi", 100, -3.5, 3.5);
  histContainer_["muonPhi"]=fs->make<TH1F>("muonPhi",   "muon Phi",     100, -3.5, 3.5);
    
}

void 
WWlep::endJob() 
{
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(WWlep);

