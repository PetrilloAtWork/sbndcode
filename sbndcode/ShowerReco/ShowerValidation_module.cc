// Framework includes                                                                
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/View.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/Optional/TFileDirectory.h"
#include "canvas/Persistency/Common/FindMany.h"
#include "canvas/Persistency/Common/FindOneP.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

//Larsoft includes 
#include "larcore/Geometry/Geometry.h"
#include "nusimdata/SimulationBase/MCTruth.h"
#include "nusimdata/SimulationBase/MCFlux.h"
#include "lardataobj/Simulation/SimChannel.h"
#include "lardataobj/Simulation/AuxDetSimChannel.h"
#include "lardataobj/AnalysisBase/Calorimetry.h"
#include "lardataobj/AnalysisBase/ParticleID.h"
#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RawData/BeamInfo.h"
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"
#include "larcoreobj/SummaryData/POTSummary.h"
#include "larsim/MCCheater/BackTrackerService.h"
#include "larsim/MCCheater/ParticleInventoryService.h"
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/Cluster.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/EndPoint2D.h"
#include "lardataobj/RecoBase/Vertex.h"
#include "lardataobj/RecoBase/Shower.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"
#include "lardataobj/AnalysisBase/CosmicTag.h"
#include "lardataobj/AnalysisBase/FlashMatch.h"
#include "sbndcode/RecoUtils/RecoUtils.h"

//Root Includes
#include "TFile.h"
#include "TMath.h"
#include "TTree.h"
#include "TGraph.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TH1D.h"

//C++ Includes 
#include <vector>
#include <iostream>


namespace ana {
  class ShowerValidation;
}

class ana::ShowerValidation : public art::EDAnalyzer {
public:

  ShowerValidation(const fhicl::ParameterSet& pset);

  void analyze(const art::Event& evt);
  void endJob();
  void beginJob();
private:

  std::string fGenieGenModuleLabel;
  std::string fLArGeantModuleLabel;
  std::string fHitsModuleLabel;
  std::string fTrackModuleLabel;
  std::string fShowerModuleLabel;
  
  TH1F* ShowerDirection_X_Hist;
  TH1F* ShowerDirection_Y_Hist;
  TH1F* ShowerDirection_Z_Hist;
  TH1F* ShowerStart_X_Hist;
  TH1F* ShowerStart_Y_Hist;
  TH1F* ShowerStart_Z_Hist;
  TH1F* ShowerLength_Hist;
  TH1F* ShowerEnergy_Hist;
  TH1F* ShowerdEdx_Hist;
  TH1F* EventSeggy_Hist;
  TH1F* ShowerCompleteness_Hist;
  TH1F* ShowerPurity_Hist;

  art::ServiceHandle<geo::Geometry> geom;
  art::ServiceHandle<art::TFileService> tfs;

};

ana::ShowerValidation::ShowerValidation(const fhicl::ParameterSet& pset) : EDAnalyzer(pset){

  fGenieGenModuleLabel = pset.get<std::string>("GenieGenModuleLabel");
  fLArGeantModuleLabel = pset.get<std::string>("LArGeantModuleLabel"); 
  fHitsModuleLabel     = pset.get<std::string>("HitsModuleLabel");
  fTrackModuleLabel    = pset.get<std::string>("TrackModuleLabel");
  fShowerModuleLabel   = pset.get<std::string>("ShowerModuleLabel");

  TFile output_file("showervalidationGraphs.root","RECREATE");

  ShowerDirection_X_Hist  = tfs->make<TH1F>("ShowerDirectionX", "ShowerDirectionX", 100, -10, 10);
  ShowerDirection_Y_Hist  = tfs->make<TH1F>("ShowerDirectionY", "ShowerDirectionY", 100, -10, 10);
  ShowerDirection_Z_Hist  = tfs->make<TH1F>("ShowerDirectionZ", "ShowerDirectionZ", 100, -10, 10);

  ShowerStart_X_Hist  = tfs->make<TH1F>("ShowerStartX", "ShowerStartX", 400, -200, 200);
  ShowerStart_Y_Hist  = tfs->make<TH1F>("ShowerStartY", "ShowerStartY", 400, -200, 200);
  ShowerStart_Z_Hist  = tfs->make<TH1F>("ShowerStartZ", "ShowerStartZ", 600, -100, 100);
  
  ShowerLength_Hist        = tfs->make<TH1F>("ShowerLength_Hist", "ShowerLength_Hist", 200, -20, 20);
  ShowerEnergy_Hist        = tfs->make<TH1F>("ShowerEnergy_Hist", "ShowerEnergy_Hist", 100, -5, 5);
  ShowerdEdx_Hist          = tfs->make<TH1F>("ShowerdEdx_Hist", "ShowerdEdx_Hist", 100, -5, 5);
  EventSeggy_Hist          = tfs->make<TH1F>("ShowerSeggy_Hist", "ShowerSeggy_Hist", 200, -10, 10);
  ShowerCompleteness_Hist  = tfs->make<TH1F>("ShowerCompleteness_Hist", "ShowerCompleteness_Hist", 10, 0, 1);
  ShowerPurity_Hist        = tfs->make<TH1F>("ShowerPurity_Hist", "ShowerPurity_Hist", 10, 0, 1);

}



void ana::ShowerValidation::beginJob() {

}


void ana::ShowerValidation::analyze(const art::Event& evt) {

  //Service handles 
  art::ServiceHandle<cheat::BackTrackerService> backtracker;
  art::ServiceHandle<cheat::ParticleInventoryService> particleInventory;
  //  auto const* detprop = lar::providerFrom<detinfo::DetectorPropertiesService>();

  //Getting  MC truth information
  art::Handle< std::vector<simb::MCTruth> > mctruthListHandle;
  std::vector<art::Ptr<simb::MCTruth> > mclist;
  if(evt.getByLabel(fGenieGenModuleLabel,mctruthListHandle))
    {art::fill_ptr_vector(mclist, mctruthListHandle);}
  
  //Getting the SimWire Information
  //Get the SimChannels so that we can find the IDEs deposited on them.                                                                          
  art::Handle<std::vector<sim::SimChannel> > simChannelHandle;
  std::vector<art::Ptr<sim::SimChannel> > simchannels;
  if(evt.getByLabel(fLArGeantModuleLabel,simChannelHandle))
    {art::fill_ptr_vector(simchannels, simChannelHandle);}


  // Getting the Hit Information 
  art::Handle<std::vector<recob::Hit> > hitListHandle;
  std::vector<art::Ptr<recob::Hit> > hits;
  if(evt.getByLabel(fHitsModuleLabel,hitListHandle))
    {art::fill_ptr_vector(hits, hitListHandle);}

  //Getting the Shower Information
  art::Handle<std::vector<recob::Shower> > showerListHandle;
  std::vector<art::Ptr<recob::Shower> > showers;
  if(evt.getByLabel(fShowerModuleLabel,showerListHandle))
    {art::fill_ptr_vector(showers,showerListHandle);}
    
  //Association between Showers and 2d Hits
  art::FindManyP<recob::Hit>  fmh(showerListHandle,   evt, fShowerModuleLabel);

  //List the particles in the event
  const sim::ParticleList& particles = particleInventory->ParticleList();

  //Loop over the particles 
  std::map<int,const simb::MCParticle*> trueParticles;
  std::map<int,const simb::MCParticle*> trueInitialParticles;
  std::map<int,float> trueParticleEnergy;
  int num_of_showers_viaEcut = 0;
  int num_of_showers_viaDensitycut = 0;

  for (sim::ParticleList::const_iterator particleIt = particles.begin(); particleIt != particles.end(); ++particleIt){
    const simb::MCParticle *particle = particleIt->second;
    trueParticleEnergy[particle->TrackId()] = 0;
    trueParticles[particle->TrackId()] = particle;
    std::cout << "Particle TrackID: " << particle->TrackId() << " PdgCode: " << particle->PdgCode() << " Mother: " << particle->Mother() << "Energy: "  << particle->E() << std::endl;

    //Particles with mother 0 are the initial particles (neutrino events this is the particles generated after the interaction. Keep note of these. 
    if(particle->Mother() == 0){trueInitialParticles[particle->TrackId()] = particle;}

    //I've read that pair production starts to dominate at around ~100 MeV so to find how many showers we expect loop over the mother particle. Pi0=143.97 MeV min gammas = 71.985 MeV which is greater than that from electrons at ~100MeV so pi0 should always shower? So cut on anything below 100MeV in energy.

    //It ain't a shower I'm interested in if it didn't start with a pi0 or electron...probably.
    int pdgcode = particle->PdgCode();
    if(pdgcode == 11 || pdgcode == 22){

    if(particle->E() > 0.1){
      ++num_of_showers_viaEcut;
    }

      
      //using the RecoUtil function calculate the number of hits that see a charge deposition from the track.
      std::map<geo::PlaneID,int> Hit_num_map = RecoUtils::NumberofHitsThatContainEnergyDepositedByTrack(particle->TrackId(), hits);
      std::map<geo::PlaneID,int> Wire_num_map = RecoUtils::NumberofMCWiresHit(particle->TrackId(),simchannels);

      //Compare hit density on the collection plane;
      for(std::map<geo::PlaneID,int>::iterator Hitnum_iter=Hit_num_map.begin(); Hitnum_iter!=Hit_num_map.end(); ++Hitnum_iter){
	if(Wire_num_map[Hitnum_iter->first] == 0){continue;} 
    	double Hit_num = (Hitnum_iter->second);
    	double Wire_num = Wire_num_map[Hitnum_iter->first];
	std::cout << "Hit_num: " << Hit_num << " Wire_num: " << Wire_num << std::endl;
    	double Hit_Density = Hit_num/Wire_num;
	if(Hit_Density > 1){++num_of_showers_viaDensitycut; break;} 
      }
    }
  }

  std::cout << "num_of_showers_viaDensitycut: " << num_of_showers_viaDensitycut << " num_of_showers_viaEcut: " << num_of_showers_viaEcut << std::endl;

  //Loop over hits associated with the shower add up the IDEs energy for each of the "track ID" and find the purity and compare other properites.
  std::vector< art::Ptr<recob::Hit> > showerhits; //hits in the shower 
  
  //Loop over the showers in the event
  for(unsigned int shower_iter = 0; shower_iter < showers.size(); ++shower_iter){

    //Get the shower 
    art::Ptr<recob::Shower>& shower = showers.at(shower_iter);

    //Get the hits vector from the shower                                                          
    showerhits = fmh.at(shower.key());
    if(showerhits.size() == 0) {continue;}

    //Function from RecoUtils, finds the most probable track ID associated with the set of hits from there true energy depositons. 
    int ShowerTrackID = RecoUtils::TrueParticleIDFromTotalTrueEnergy(showerhits);
    std::cout << "True Shower ID: " <<  ShowerTrackID << std::endl;


    //For all the particles that orginate from the showering particle add up the energy.
    double TrueEnergyDep_FromShower = 0; 
    for (sim::ParticleList::const_iterator particleIt = particles.begin(); particleIt != particles.end(); ++particleIt){
      const simb::MCParticle *particle = particleIt->second;
      std::cout << "Test 0.5" << std::endl;
      if(particle->TrackId() == ShowerTrackID){TrueEnergyDep_FromShower += RecoUtils::TrueEnergyDepositedFromMCTrack(particle->TrackId(), simchannels); continue;}

      int motherID = particle->Mother();
      while(motherID != ShowerTrackID || motherID != 0){
	std::cout << "Test 0.52" << std::endl;
	std::cout << "motherID: " << motherID << std::endl; 
	const simb::MCParticle * particle_prev =  trueParticles[motherID]; 
	
	//Function from RecoUtils, adds up all the energy deposited from that Track ID.
	//	double TrueEnergyDep_Fromtrack = RecoUtils::TrueEnergyDepositedFromMCTrack(ShowerTrackID, simchannels);
	std::cout << "Test 0.53" << std::endl;
	motherID = particle_prev->Mother();
	std::cout << "Test 0.54" << std::endl;
	if(motherID == ShowerTrackID){TrueEnergyDep_FromShower += RecoUtils::TrueEnergyDepositedFromMCTrack(particle->TrackId(), simchannels);}
      }
    }

    //Energy deposited within set of hits assigned to the shower by reconstruction from the true track id.
    double TrueEnergyDep_Fromtrack_WithinShower = 0;

    //Energy deposited within the set of Hits associated to the shower.
    double TrueEnergyDep_WithinShower = 0; 

    //Loop over the hits and find the IDEs 
    for(std::vector< art::Ptr<recob::Hit> >::iterator hitIt=showerhits.begin(); hitIt!=showerhits.end(); ++hitIt){

      //Split the Hit into its IDE for each track it associates with.                               
      std::vector<sim::TrackIDE> trackIDEs = backtracker->HitToTrackIDEs((*hitIt));

      for (unsigned int idIt = 0; idIt < trackIDEs.size(); ++idIt){ 
  	//Find the true total energy deposited in a set of hits. 
  	TrueEnergyDep_WithinShower += trackIDEs.at(idIt).energy;
	
  	//Add up the contribution from the shower trackID. 
  	if(trackIDEs.at(idIt).trackID == ShowerTrackID){
  	  TrueEnergyDep_Fromtrack_WithinShower += trackIDEs.at(idIt).energy;
  	}
      }
    }//Hit Loop
    
    std::cout << "Test 1" << std::endl;

    double completeness =  TrueEnergyDep_Fromtrack_WithinShower/TrueEnergyDep_FromShower;
    double purity       =  TrueEnergyDep_Fromtrack_WithinShower/TrueEnergyDep_WithinShower;
  
    //Add to the respective hitograms.
    ShowerCompleteness_Hist->Fill(completeness);
    ShowerPurity_Hist->Fill(purity);

    std::cout << "Test 1.1" << std::endl;

    //Find the MCParticle this shower associates to
    const simb::MCParticle* MCShowerParticle = trueInitialParticles.at(ShowerTrackID);
    
    //Find the Energy of the particle: 
    //double Energy = MCShowerParticle->E();

    //Get the number of Traj points to loop over                                                    
    unsigned int TrajPoints = MCShowerParticle->NumberTrajectoryPoints();

    //Find the start and end points of the initial particle in order to compare the track length. 
    const TLorentzVector PositionTrajStart =  MCShowerParticle->Position(0);
    const TLorentzVector PositionTrajEnd   =  MCShowerParticle->Position(TrajPoints-1);

    std::cout << "Test 1.2" << std::endl;

    //The three vecotor for track length is the shower direction 
    TVector3  TrueShowerDirection = (PositionTrajEnd - PositionTrajStart).Vect();
    
    //Initial track lentgh of the shower.
    double TrueTrackLength = TrueShowerDirection.Mag();

    std::cout << "Test 1.3" << std::endl;

    //Get the information for the shower  
    const int ShowerBest_Plane                       = shower->best_plane(); 
    const TVector3 & ShowerDirection                 = shower->Direction();
    const TVector3 & ShowerStart                     = shower->ShowerStart();
    const double   & ShowerTrackLength               = shower->Length();       
    const std::vector< double > & ShowerEnergyPlanes = shower->Energy();
    const std::vector< double > & ShowerdEdX_vec     = shower->dEdx(); 

    std::cout << "Test 2" << std::endl;

    //Get the Fraction off the true value and fill the histograms.
    ShowerDirection_X_Hist->Fill((TrueShowerDirection.X()-ShowerDirection.X())/TrueShowerDirection.X());
    ShowerDirection_Y_Hist->Fill((TrueShowerDirection.Y()-ShowerDirection.Y())/TrueShowerDirection.Y());
    ShowerDirection_Z_Hist->Fill((TrueShowerDirection.Z()-ShowerDirection.Z())/TrueShowerDirection.Z());

    ShowerStart_X_Hist->Fill((PositionTrajStart.X()-ShowerStart.X())/PositionTrajStart.X());
    ShowerStart_Y_Hist->Fill((PositionTrajStart.Y()-ShowerStart.Y())/PositionTrajStart.Y());
    ShowerStart_Z_Hist->Fill((PositionTrajStart.Z()-ShowerStart.Z())/PositionTrajStart.Z());

    ShowerLength_Hist->Fill((TrueTrackLength-ShowerTrackLength)/TrueTrackLength);
    ShowerEnergy_Hist->Fill((TrueEnergyDep_FromShower-ShowerEnergyPlanes[ShowerBest_Plane])/TrueEnergyDep_FromShower);
    
    for(std::vector< double >::const_iterator dEdx=ShowerdEdX_vec.begin();dEdx!=ShowerdEdX_vec.end(); ++dEdx){
      ShowerdEdx_Hist->Fill((*dEdx));
    }

  }//Shower Loop 
  
  //Whats the segementyness of the event. 
  EventSeggy_Hist->Fill(num_of_showers_viaDensitycut - showers.size());

  std::cout << "Test 3" << std::endl;

  // //This is for Rhiannon 
  // for(std::vector<art::Ptr<simb::MCTruth> >::const_iterator genielist = mclist.begin(); genielist != mclist.end(); ++genielist){ 
  //   std::cout << "Origin: " << (*genielist)->Origin() << std::endl;
  //   int num_genieparticles = (*genielist)->NParticles();
    
  //   simb::MCNeutrino  neutrino = (*genielist)->GetNeutrino();
  //   std::cout << "Neutrino interaction: " << neutrino.InteractionType()<< std::endl;
    
    
  //   for(int i=0; i<num_genieparticles; ++i){
  //     const simb::MCParticle & particle = (*genielist)->GetParticle(i);
  //     std::cout << "Particle TrackID: " << particle.TrackId() << " PdgCode: " << particle.PdgCode() << " Mother: " << particle.Mother() << std::endl;
  //   }
  // }

 
  return; 
  }



void ana::ShowerValidation::endJob() {

}




DEFINE_ART_MODULE(ana::ShowerValidation)