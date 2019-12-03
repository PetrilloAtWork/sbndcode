////////////////////////////////////////////////////////////////////////
// Class:       RecoEfficencyFinder
// Module Type: analyzer
// File:        RecoEfficencyFinder_module.cc
//
// Generated at Thu Sep 19 03:45:10 2019 by Dominic Barker using artmod
// from cetpkgsupport v1_14_01.
////////////////////////////////////////////////////////////////////////

//Framework Includes
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "canvas/Persistency/Common/FindManyP.h"

//LArSoft Includes 
#include "larcore/Geometry/Geometry.h"
#include "nusimdata/SimulationBase/MCTruth.h"
#include "larsim/MCCheater/BackTrackerService.h"
#include "larsim/MCCheater/ParticleInventoryService.h"
#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/Shower.h"
#include "lardataobj/RecoBase/PFParticle.h"
#include "lardataobj/RecoBase/Vertex.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"
#include "larreco/RecoAlg/MCRecoUtils/RecoUtils.h"
#include "larreco/RecoAlg/MCRecoUtils/ShowerUtils.h"
#include "larcorealg/Geometry/BoxBoundedGeo.h"
#include "lardataobj/AnalysisBase/Calorimetry.h"
#include "larcoreobj/SummaryData/POTSummary.h"  
#include "nusimdata/SimulationBase/MCFlux.h"

//C++ Includes
#include <vector>
#include <iostream>

//Root Includes
#include "TSystem.h"

namespace ana {
  class RecoEfficencyFinder;
}

class ana::RecoEfficencyFinder : public art::EDAnalyzer {
public:
  explicit RecoEfficencyFinder(fhicl::ParameterSet const & p);
  // The destructor generated by the compiler is fine for classes
  // without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  RecoEfficencyFinder(RecoEfficencyFinder const &) = delete;
  RecoEfficencyFinder(RecoEfficencyFinder &&) = delete;
  RecoEfficencyFinder & operator = (RecoEfficencyFinder const &) = delete;
  RecoEfficencyFinder & operator = (RecoEfficencyFinder &&) = delete;

  // Required functions.
  void analyze(art::Event const & e) override;
  void beginJob() override;
  void beginSubRun(const art::SubRun& sr) override;
  void endJob() override;
  bool containedInAV(const TVector3 &v);
  bool isFromNuVertex(const art::Ptr<simb::MCTruth>& mc, const simb::MCParticle* &particle, float distance=5.) const;
  bool isFromNuVertex(const art::Ptr<simb::MCTruth>& mc, TVector3& position, float distance=5.) const;
  bool isShowerContainedish(const std::vector<int>& shower, std::map<int,float>& Track_Energy_map,std::map<int, const simb::MCParticle*>& trueParticles);
  bool isPrimaryHadron(const int& track, art::Ptr<simb::MCTruth>& mc_truth, std::map<int,const simb::MCParticle*>& trueParticles,int neutrino_iter,std::map<int,float>& Track_Energy_map);

private:

  //Module Labels
  art::InputTag fShowerModuleLabel;
  art::InputTag fPFParticleLabel;
  art::InputTag fGenieGenModuleLabel;
  art::InputTag fCalorimetryModuleLabel;
  art::InputTag fTrackModuleLabel;
  art::InputTag fLArGeantModuleLabel;

  //Fcl parameters 
  float fMinRecoEnergyCut;
  std::vector<geo::BoxBoundedGeo> fActiveVolume; //!< List of active volumes

  //services
  art::ServiceHandle<cheat::ParticleInventoryService> particleInventory;
  art::ServiceHandle<art::TFileService> tfs;

  //Branches
  std::vector<float> number_of_showers_per_neutrino;
  std::vector<float> vertex_recoKE_branch;
  std::vector<float> vertex_trueKE_branch;
  std::vector<float> vertex_reco_branch;
  std::vector<float> nu_reco_energy_branch;
  std::vector<float> nu_truth_energy_branch;
  std::vector<float> nu_interaction_type_branch;
  std::vector<float> nu_mode_branch;
  std::vector<float> nu_E_branch;
  std::vector<float> nu_E_numtrue_branch;
  std::vector<float> nu_distance_branch;
  std::vector<std::vector<float> > truepionE_branch;
  std::vector<std::vector<float> > trueprotonE_branch;
  std::vector<std::vector<float> > truekaonE_branch;
  std::vector<std::vector<float> > truetrackE_branch;
  std::vector<std::vector<float> > shower_energy_branch;
  std::vector<std::vector<float> > truth_pid_branch;
  std::vector<std::vector<float> > true_energy_branch;
  std::vector<std::vector<float> > shower_coversion_gap_branch;
  std::vector<std::vector<float> > shower_residual_dist_branch;
  std::vector<std::vector<float> > shower_dEdx_branch;
  std::vector<std::vector<float> > track_lengths_branch;

  std::vector<std::vector<float> > track_E_branch;
  std::vector<std::vector<float> > track_trueE_branch;
  std::vector<std::vector<float> > track_pdg_branch; 
  std::vector<std::vector<float> > track_resE_branch;

  float trueShower_num_branch;
  float numtrueVtx_branch;

  float POT;

  //TTree
  TTree* Tree;
  TTree* POTTree;

  int num_v_recoed;


};


ana::RecoEfficencyFinder::RecoEfficencyFinder(fhicl::ParameterSet const & pset)
  :
  EDAnalyzer(pset),
  fShowerModuleLabel     (pset.get<art::InputTag>("ShowerModuleLabel")),
  fPFParticleLabel       (pset.get<art::InputTag>("PFParticleLabel")),
  fGenieGenModuleLabel   (pset.get<art::InputTag>("GenieGenModuleLabel")),
  fCalorimetryModuleLabel(pset.get<art::InputTag>("CalorimetryModuleLabel")),
  fTrackModuleLabel      (pset.get<art::InputTag>("TrackModuleLabel")),
  fLArGeantModuleLabel   (pset.get<art::InputTag>("LArGeantModuleLabel")),
  fMinRecoEnergyCut      (pset.get<float>        ("MinRecoEnergyCut"))
{

  // setup active volume bounding boxes
  std::vector<fhicl::ParameterSet> AVs =				\
    pset.get<std::vector<fhicl::ParameterSet> >("ActiveVolume");
  for (auto const& AV : AVs) {
    double xmin = AV.get<double>("xmin");
    double ymin = AV.get<double>("ymin");
    double zmin = AV.get<double>("zmin");
    double xmax = AV.get<double>("xmax");
    double ymax = AV.get<double>("ymax");
    double zmax = AV.get<double>("zmax");
    fActiveVolume.emplace_back(xmin, xmax, ymin, ymax, zmin, zmax);
  }



}

void ana::RecoEfficencyFinder::beginSubRun(const art::SubRun& sr)
{
  art::Handle< sumdata::POTSummary > potListHandle;

  if(sr.getByLabel(fGenieGenModuleLabel,potListHandle))
    POT=potListHandle->totpot;
  else
    POT=0.;

  POTTree->Fill();

}


void ana::RecoEfficencyFinder::beginJob() {

  POTTree = tfs->make<TTree>("RecoEffPOTTree", "Tree Holding POT information");
      
  Tree = tfs->make<TTree>("RecoEffMetricTree", "Tree Holding all metric information");
  gInterpreter->GenerateDictionary("vector<vector<float> >","vector");

  //Initalise the branches
  Tree->Branch("number_of_showers_per_neutrino","std::vector<float>", &number_of_showers_per_neutrino, 32000, 0);
  Tree->Branch("shower_energy","std::vector<std::vector<float> >", &shower_energy_branch, 32000, 0);
  Tree->Branch("truth_pid","std::vector<std::vector<float> >", &truth_pid_branch,32000,0);
  Tree->Branch("true_energy","std::vector<std::vector<float> >",&true_energy_branch,32000,0);
  Tree->Branch("trueShower_num",&trueShower_num_branch,32000,0);
  Tree->Branch("numtrueVtx_branch",&numtrueVtx_branch,32000,0);
  Tree->Branch("vertex_recoK","std::vector<float>",&vertex_recoKE_branch,32000,0);
  Tree->Branch("vertex_trueK","std::vector<float>",&vertex_trueKE_branch,32000,0);

  Tree->Branch("true_trackE","std::vector<std::vector<float>>",&truetrackE_branch,32000,0);
  Tree->Branch("true_pionE","std::vector<std::vector<float>>",&truepionE_branch,32000,0);
  Tree->Branch("true_protonE","std::vector<std::vector<float>>",&trueprotonE_branch,32000,0);
  Tree->Branch("true_kaonE","std::vector<std::vector<float>>",&truekaonE_branch,32000,0);

  Tree->Branch("vertex_reco","std::vector<float>",&vertex_reco_branch,32000,0);
  Tree->Branch("shower_coversion_gap","std::vector<std::vector<float>>",&shower_coversion_gap_branch,32000,0);
  Tree->Branch("shower_residual_dist","std::vector<std::vector<float>>",&shower_residual_dist_branch,32000,0);
  Tree->Branch("shower_dEdx","std::vector<std::vector<float>>",&shower_dEdx_branch,32000,0);

  Tree->Branch("track_lengths","std::vector<std::vector<float>>",&track_lengths_branch,32000,0);
  Tree->Branch("track_E","std::vector<std::vector<float>>",&track_E_branch,32000,0);
  Tree->Branch("track_trueE","std::vector<std::vector<float>>",&track_trueE_branch,32000,0);
  Tree->Branch("track_pdg","std::vector<std::vector<float>>",&track_pdg_branch,32000,0);
  Tree->Branch("track_resE","std::vector<std::vector<float>>",&track_resE_branch,32000,0);

  Tree->Branch("nu_reco_energy","std::vector<float>",&nu_reco_energy_branch,32000,0);
  Tree->Branch("nu_truth_energy","std::vector<float>",&nu_truth_energy_branch,32000,0);
  Tree->Branch("nu_interaction_type","std::vector<float>",&nu_interaction_type_branch,32000,0);
  Tree->Branch("nu_mode","std::vector<float>",&nu_mode_branch,32000,0);
  Tree->Branch("nu_E","std::vector<float>",&nu_E_branch,32000,0);
  Tree->Branch("nu_E_numtrue","std::vector<float>",&nu_E_numtrue_branch,32000,0);
  
  Tree->Branch("nu_distance","std::vector<float>",&nu_distance_branch,32000,0);

    num_v_recoed = 0;

 }

void ana::RecoEfficencyFinder::analyze(art::Event const & evt){

  auto const& mcflux = evt.getValidHandle<std::vector<simb::MCFlux> >(fGenieGenModuleLabel);

  //Get the showers 
  art::Handle<std::vector<recob::Shower> > showerListHandle;
  std::vector<art::Ptr<recob::Shower> > showers;
  if(evt.getByLabel(fShowerModuleLabel,showerListHandle)){
    art::fill_ptr_vector(showers,showerListHandle);
  }

  //Get the pf particles 
  art::Handle<std::vector<recob::PFParticle> > pfpHandle;
  std::vector<art::Ptr<recob::PFParticle> > pfps;
  if(evt.getByLabel(fPFParticleLabel,pfpHandle))
    {art::fill_ptr_vector(pfps,pfpHandle);}

  // * MC truth information
  art::Handle<std::vector<simb::MCTruth> > mctruthListHandle;
  std::vector<art::Ptr<simb::MCTruth> > mclist;
  if (evt.getByLabel(fGenieGenModuleLabel,mctruthListHandle))
    art::fill_ptr_vector(mclist, mctruthListHandle);

  //Get the tracks
  art::Handle<std::vector<recob::Track> > trackListHandle;
  evt.getByLabel(fTrackModuleLabel,trackListHandle);
  if(!trackListHandle.isValid()){
    throw cet::exception("RecoEfficencyFinder") << "Track list not valid";
    return;
  }

  //Get the SimChannels so that we can find the IDEs deposited on them.
  art::Handle<std::vector<sim::SimChannel> > simChannelHandle;
  std::vector<art::Ptr<sim::SimChannel> > simchannels;
  if(evt.getByLabel(fLArGeantModuleLabel,simChannelHandle))
  {art::fill_ptr_vector(simchannels, simChannelHandle);}


  //Association between Showers and pfParticle
  art::FindManyP<recob::Shower> fmsh(pfpHandle, evt, fShowerModuleLabel);
  if(!fmsh.isValid()){
    throw cet::exception("RecoEfficencyFinder") << "Shower and PF particle association is somehow not valid. Stopping";
    return;
  }
  
  //Assocations between pfparticle and vertex.
  art::FindManyP<recob::Vertex> fmv(pfpHandle, evt, fPFParticleLabel);
  if(!fmv.isValid()){
    throw cet::exception("RecoEfficencyFinder") << "Vertex and PF particle association is somehow not valid. Stopping";
    return;
  }

  //Association between Showers and 2d Hits
  art::FindManyP<recob::Hit> fmh(showerListHandle, evt, fShowerModuleLabel);
  if(!fmh.isValid()){
    throw cet::exception("RecoEfficencyFinder") << "Hit-Recob::Shower association is somehow not valid. Stopping";
    return;
  }

  //Assn between Tracks and pfparticles
  art::FindManyP<recob::Track> fmt(pfpHandle, evt, fTrackModuleLabel);
  if(!fmt.isValid()){
    throw cet::exception("RecoEfficencyFinder") << "Track and PF particle association is somehow not valid. Stopping";
    return;
  }

  //Assn between Tracks and calo
  art::FindManyP<anab::Calorimetry> fmcal(trackListHandle, evt, fCalorimetryModuleLabel);
  if(!fmt.isValid()){
    throw cet::exception("RecoEfficencyFinder") << "Track and PF particle association is somehow not valid. Stopping";
    return;
  }

  //Assn between Tracks and hits
  art::FindManyP<recob::Hit> fmth(trackListHandle, evt, fTrackModuleLabel);
  if(!fmth.isValid()){
    throw cet::exception("RecoEfficencyFinder") << "Track and PF particle association is somehow not valid. Stopping";
    return;
  }


  //What energy do we actually reconstruct the shower. 

  //List the particles in the event
  const sim::ParticleList& particles = particleInventory->ParticleList();

  //Make a map of Track id and pdgcode
  std::map<int,int> trueParticleEnergy;
  std::map<int, const simb::MCParticle*> trueParticles;
  for (sim::ParticleList::const_iterator particleIt = particles.begin(); particleIt != particles.end(); ++particleIt){
    const simb::MCParticle *particle = particleIt->second;
    trueParticleEnergy[particle->TrackId()] = 0;
    trueParticles[particle->TrackId()] = particle;

    //    std::cout << "particle id: " << particle->TrackId() << " pdg: " << particle->PdgCode() << " Mother: " << particle->Mother() << " E: " << particle->E() << std::endl;

  }


  //Get the true showers. This is a map of the mother id with the down stream mothers 
  std::map<int,std::vector<int> > trueShowerCandidates = ShowerUtils::FindTrueShowerIDs(trueParticles);
  std::map<int,float> MCTrack_Energy_map = RecoUtils::TrueEnergyDepositedFromMCTracks(simchannels);

  std::map<int,std::vector<int> > trueShowers;
  //Remove showers candiates  which are not neutrino ones.
  int numVtx=0;
  for(auto const& trueShowerCandidate: trueShowerCandidates){
    
    //Assume anything that comes from the vertex is a shower. 
    for(auto const& mc: mclist){

      //Only deal with neutrinos 
      if(mc->Origin() != simb::kBeamNeutrino){continue;} 

      if(!isFromNuVertex(mc,trueParticles[trueShowerCandidate.first])){continue;}
      
      //Make sure that the shower started is contained
      if(!isShowerContainedish(trueShowerCandidate.second,MCTrack_Energy_map,trueParticles)){continue;}

      //Make sure we are not a silly photon below 10 MeV 
      if(MCTrack_Energy_map[trueShowerCandidate.first]  < 10){continue;}

      trueShowers[trueShowerCandidate.first] = trueShowerCandidate.second;
    }
  }
  trueShowerCandidates.clear();
  
  trueShower_num_branch = trueShowers.size();

  //For the time being Lets not continue without a neutrino vertex in the AV.
  for(auto const& mc: mclist){

    //Only deal with neutrinos 
    if(mc->Origin() != simb::kBeamNeutrino){continue;} 

    //Count the number of vertices that can be reconstructed.
    const TVector3 nuVtx = mc->GetNeutrino().Nu().Trajectory().Position(0).Vect(); 

    if(containedInAV(nuVtx)){
      nu_E_numtrue_branch.push_back(mc->GetNeutrino().Nu().E());
      ++numVtx;
    }
  }

  if(numVtx == 0){return;}

  numtrueVtx_branch = numVtx;

  //Loop over fpf and find the neutrinos
  std::vector<art::Ptr<recob::PFParticle> > neutrinos;
  for(auto const& pfp: pfps){
    if(TMath::Abs(pfp->PdgCode()) == 12 || TMath::Abs(pfp->PdgCode()) == 14){
      neutrinos.push_back(pfp);
    }
  }

  //Make the pfp map
  std::map<int, art::Ptr<recob::PFParticle> > pfp_map;
  for(auto const& pfp: pfps){
      pfp_map[pfp->Self()] = pfp;
  }


  int neutrino_iter=0;

  number_of_showers_per_neutrino.resize(neutrinos.size(),-999);
  vertex_recoKE_branch.resize(neutrinos.size(),-999); 
  vertex_trueKE_branch.resize(neutrinos.size(),-999); 
  vertex_reco_branch.resize(neutrinos.size(),-999);
  nu_reco_energy_branch.resize(neutrinos.size(),-999);
  nu_distance_branch.resize(neutrinos.size(),-999);
  shower_energy_branch.resize(neutrinos.size());
  truth_pid_branch.resize(neutrinos.size()); 
  true_energy_branch.resize(neutrinos.size()); 
  shower_coversion_gap_branch.resize(neutrinos.size());
  shower_residual_dist_branch.resize(neutrinos.size());
  shower_dEdx_branch.resize(neutrinos.size());
  track_lengths_branch.resize(neutrinos.size());   
  nu_truth_energy_branch.resize(neutrinos.size()); 
  nu_interaction_type_branch.resize(neutrinos.size());
  nu_mode_branch.resize(neutrinos.size());
  nu_E_branch.resize(neutrinos.size());
  track_E_branch.resize(neutrinos.size());
  track_trueE_branch.resize(neutrinos.size());
  track_pdg_branch.resize(neutrinos.size());
  truepionE_branch.resize(neutrinos.size());
  trueprotonE_branch.resize(neutrinos.size());
  truekaonE_branch.resize(neutrinos.size());
  truetrackE_branch.resize(neutrinos.size());
  track_resE_branch.resize(neutrinos.size());

  for(auto const neutrino: neutrinos){

    std::vector<art::Ptr<recob::Shower> > neutrino_showers; 
      
    const std::vector<long unsigned int> daughters = neutrino->Daughters();

    for(auto const& daughter: daughters){
      std::cout << "daughter id: " << daughter << " pdgcode: " << pfp_map[daughter]->PdgCode() << std::endl;

      //is the daughter a shower.
      if(pfp_map[daughter]->PdgCode() != 11){continue;}

      //Do we have a corresponding shower particle.
      std::vector<art::Ptr<recob::Shower> > shower = fmsh.at(pfp_map[daughter].key());

      //Did we succeed at characterising the shower particle?
      if(shower.size() == 0){continue;}
	

      //If we have two then our charactisation did a silly.
      if(shower.size() != 1){
  	throw cet::exception("RecoEfficencyFinder") << "we have too many recob showers for pfparticles";
  	return;
     }
     
      //Then we have a shower
      std::cout << "pushing back shower" << std::endl;
      neutrino_showers.push_back(shower[0]);

    }
    std::cout << " neutrino_showers.size(): " << neutrino_showers.size() << std::endl;

    //Order the showers with regards to their energy
    std::sort(neutrino_showers.begin(), neutrino_showers.end(),[](const art::Ptr<recob::Shower> & a, const art::Ptr<recob::Shower> & b)
	      { 
		return a->Energy().at(a->best_plane()) > b->Energy().at(b->best_plane()); 
	      });

    //Fill the information about the showers
    for(auto const& shower: neutrino_showers){

      //Check the truth out 
      const int ShowerBest_Plane = shower->best_plane();
      std::pair<int,double> ShowerTrackInfo = ShowerUtils::TrueParticleIDFromTrueChain(trueShowers,fmh.at(shower.key()),ShowerBest_Plane);
      truth_pid_branch.at(neutrino_iter).push_back(ShowerTrackInfo.first);

      float true_energy = 0;
      for(auto const& shower_element: trueShowers[ShowerTrackInfo.first]){
	true_energy += MCTrack_Energy_map[shower_element];
      }

      true_energy_branch.at(neutrino_iter).push_back(true_energy);

      //Lets also fill the dEdx.
      shower_dEdx_branch.at(neutrino_iter).push_back(shower->dEdx().at(ShowerBest_Plane));
      //Apply reconstruction energy cut. 
      const std::vector<double> ShowerEnergyPlanes = shower->Energy(); //MeV
      //      auto const  max_shower_energy = *max_element(std::begin(ShowerEnergyPlanes), std::end(ShowerEnergyPlanes));
      float max_shower_energy = ShowerEnergyPlanes[0];
      if(ShowerBest_Plane != -999){
	if((int) ShowerEnergyPlanes.size() < ShowerBest_Plane+1){
	  max_shower_energy = ShowerEnergyPlanes[ShowerBest_Plane];
	}
      }

      
      if(max_shower_energy > fMinRecoEnergyCut){
  	shower_energy_branch.at(neutrino_iter).push_back(max_shower_energy);
      }
    }

    //Add the number of showers to the true.
    number_of_showers_per_neutrino.at(neutrino_iter) = neutrino_showers.size();
    
    //How well do we reconstruct the vertex. 

    //Define the vertex as being correct if within 5cm 
    std::vector<art::Ptr<recob::Vertex> > vertex = fmv.at(neutrino.key());

    if(vertex.size() > 1){
      throw cet::exception("RecoEfficencyFinder") << "we have too many recob vertex for pfparticles";
      return;
    }

    double vtx_xyz[3] = {-999,-999,-999};
    vertex[0]->XYZ(vtx_xyz);
     
    TVector3 vertex_position = {vtx_xyz[0],vtx_xyz[1],vtx_xyz[2]};

    //Find the corresponding vertex. Hopefully there is just one event but pileup is ~few% 
    bool isReconstructed = false;
    
    if(mclist.size() < 1){
      throw cet::exception("RecoEfficencyFinder") << "why no truth :(";
      return;
    }

    art::Ptr<simb::MCTruth> mc_truth = mclist[0];
    int mc_truth_iter = 0;
    for(auto const& mc: mclist){
      ++mc_truth_iter;
      isReconstructed = isFromNuVertex(mc,vertex_position);
      if(isReconstructed){
	++num_v_recoed;
  	mc_truth = mc;
  	break;
      }
    }
    --mc_truth_iter;

    nu_distance_branch.at(neutrino_iter) = mcflux->at(mc_truth_iter).fdk2gen + mcflux->at(mc_truth_iter).fgen2vtx;
    nu_interaction_type_branch.at(neutrino_iter) = mc_truth->GetNeutrino().InteractionType();
    nu_mode_branch.at(neutrino_iter)             = mc_truth->GetNeutrino().Mode();
    nu_E_branch.at(neutrino_iter)                = mc_truth->GetNeutrino().Nu().E();

    //Get the true vertex KE
    float True_Vertex_Energy = 0;
    for(auto const& track: MCTrack_Energy_map){
      if(isPrimaryHadron(track.first,mc_truth,trueParticles,neutrino_iter,MCTrack_Energy_map)){
  	True_Vertex_Energy += track.second;
      }
    }
    vertex_trueKE_branch.at(neutrino_iter) = True_Vertex_Energy;

    if(isReconstructed){
      vertex_reco_branch.at(neutrino_iter) = 1;
    }

    //Loop over hadron daughter pfps and calculate the energy
    float Vertex_KE = 0;
    for(auto const& daughter: daughters){
      
      if(pfp_map[daughter]->PdgCode() == 11){continue;}

      art::Ptr<recob::PFParticle> daughter_pfp = pfp_map[daughter];

      //Get the track info 
      std::vector<art::Ptr<recob::Track> > daughter_track = fmt.at(daughter_pfp.key());
      if(daughter_track.size() == 0){continue;}
      if(daughter_track.size() != 1){
	throw cet::exception("RecoEfficencyFinder") << "we have too many recob tracks for pfparticles";
	return;
      }

      //Add the length 
      track_lengths_branch.at(neutrino_iter).push_back(daughter_track[0]->Length());

      //Get the calorimetry info
      std::vector<art::Ptr<anab::Calorimetry> > daughter_calo = fmcal.at(daughter_track[0].key());
      if(daughter_calo.size() != 3){continue;}
      // if(daughter_calo.size() != 1){
      // 	throw cet::exception("RecoEfficencyFinder") << "we have too many calos for tracks" << daughter_calo.size() << std::endl;€;
      //   return;
      // }
      Vertex_KE += daughter_calo[2]->KineticEnergy(); 
    }

    vertex_recoKE_branch.at(neutrino_iter) = Vertex_KE;
  
    //Get the conversion gap 
    float min_coversion_gap = 999;
    int min_convgap_key = 0;
    int convgap_key = 0;
    for(auto const& neutrino_shower: neutrino_showers){

      //Get the conversion gap 
      TVector3 ShowerStart     = neutrino_shower->ShowerStart();//cm 
      double   conversion_gap  = (ShowerStart - vertex_position).Mag();
      shower_coversion_gap_branch.at(neutrino_iter).push_back(conversion_gap);
      
      //Get the closest shower to the vertex.
      if(conversion_gap < min_coversion_gap){
	min_convgap_key = convgap_key;
      }
      ++convgap_key;
    }

    //Calculate the residual of the closest shower
    for(auto const& neutrino_shower: neutrino_showers){
      TVector3 ShowerStart     = neutrino_shower->ShowerStart();//cm 
      TVector3 conversion_vec  = ShowerStart - vertex_position;
      
      TVector3 min_conversion_vec = neutrino_showers[min_convgap_key]->ShowerStart() - vertex_position;

      double len  = conversion_vec.Dot(min_conversion_vec);
      double perp = (conversion_vec - len*min_conversion_vec).Mag();
      std::cout << "len: " << len << " perp: " << perp << std::endl;
      shower_residual_dist_branch.at(neutrino_iter).push_back(perp);
    }

    //Get the total reco energy.
    float Nu_reco_Energy = 0;
    float max_s_energy = -999;
    for(auto const& pfp: pfp_map){
      art::Ptr<recob::PFParticle> parent   = pfp.second;
      art::Ptr<recob::PFParticle> temp_pfp = pfp.second;
      
      //Get the primary particle
      while(!parent->IsPrimary()){
  	temp_pfp = parent;
  	if(pfp_map.find(parent->Self()) == pfp_map.end()){parent = temp_pfp; break;}
  	parent = pfp_map[parent->Parent()];
      }

      //Check we come from the neutrino
      if(parent->Self() != neutrino->Self()){continue;}

      //Get the energy from tracks
      if(pfp.second->PdgCode() == 13){

  	//Get the track info 
  	std::vector<art::Ptr<recob::Track> > track = fmt.at(pfp.second.key());
  	if(track.size() == 0){continue;}
  	if(track.size() != 1){
  	  throw cet::exception("RecoEfficencyFinder") << "we have too many recob tracks for pfparticles";
  	  return;
  	}

  	//Get the calorimetry info
  	std::vector<art::Ptr<anab::Calorimetry> > calo = fmcal.at(track[0].key());
  	if(calo.size() != 3){continue;}
	//  	if(calo.size() != 1){
  	//  throw cet::exception("RecoEfficencyFinder") << "we have too many calos for tracks";
  	//  return;
  	//}
	float TrackEnergy = calo[2]->KineticEnergy();

  	Nu_reco_Energy += TrackEnergy;
      
	std::vector<art::Ptr<recob::Hit> > track_hits = fmth.at(track[0].key());

	//Get the true track id
	int truetrack_id = RecoUtils::TrueParticleIDFromTotalRecoHits(track_hits);
	if(MCTrack_Energy_map.find(truetrack_id) == MCTrack_Energy_map.end()){continue;}
        if(trueParticles.find(truetrack_id) == trueParticles.end()){continue;} 
	track_E_branch.at(neutrino_iter).push_back(TrackEnergy);
	track_trueE_branch.at(neutrino_iter).push_back(MCTrack_Energy_map[truetrack_id]);
	track_pdg_branch.at(neutrino_iter).push_back(trueParticles[truetrack_id]->PdgCode());
	track_resE_branch.at(neutrino_iter).push_back((MCTrack_Energy_map[truetrack_id]-TrackEnergy)/MCTrack_Energy_map[truetrack_id]);
      
      }

      //is the daughter a shower.
      if(pfp.second->PdgCode() == 11){

  	//Do we have a corresponding shower particle.
  	std::vector<art::Ptr<recob::Shower> > shower = fmsh.at(pfp.second.key());
	
  	//Did we succeed at characterising the shower particle?
  	if(shower.size() == 0){continue;}
	
	
  	//If we have two then our charactisation did a silly.
  	if(shower.size() != 1){
  	  throw cet::exception("RecoEfficencyFinder") << "we have too many recob showers for pfparticles";
  	  return;
  	}

  	//Apply reconstruction energy cut. 
	const int ShowerBest_Plane = shower[0]->best_plane();
  	const std::vector<double> ShowerEnergyPlanes = shower[0]->Energy(); //MeV
	//  	auto const  max_shower_energy = *max_element(std::begin(ShowerEnergyPlanes), std::end(ShowerEnergyPlanes));
	float max_shower_energy = ShowerEnergyPlanes[0];
	if(ShowerBest_Plane != -999){
	  if((int) ShowerEnergyPlanes.size() < ShowerBest_Plane+1){
	    max_shower_energy = ShowerEnergyPlanes[ShowerBest_Plane];
	  }
	}

  	if(pfp.second->Parent() == neutrino->Self()){
  	  if(max_shower_energy > max_s_energy){
  	    max_s_energy = max_shower_energy;
  	    //Then we have a shower
  	  }
  	}
  	else{
  	  Nu_reco_Energy += max_shower_energy;
  	}
      }
    }

    Nu_reco_Energy += max_s_energy;
    nu_reco_energy_branch.at(neutrino_iter) = Nu_reco_Energy;

    //Get the truth energy
    nu_truth_energy_branch.at(neutrino_iter) = mc_truth->GetNeutrino().Nu().E();
    ++neutrino_iter;
  }


  Tree->Fill();

  number_of_showers_per_neutrino.clear();
  shower_energy_branch.clear();
  truth_pid_branch.clear();
  true_energy_branch.clear();
  vertex_recoKE_branch.clear();
  vertex_trueKE_branch.clear();
  vertex_reco_branch.clear();
  track_lengths_branch.clear();
  shower_coversion_gap_branch.clear();
  shower_residual_dist_branch.clear();
  shower_dEdx_branch.clear();
  nu_reco_energy_branch.clear();
  nu_truth_energy_branch.clear();
  nu_interaction_type_branch.clear();
  nu_mode_branch.clear();
  nu_E_branch.clear(); 
  nu_E_numtrue_branch.clear();
  nu_distance_branch.clear();
  track_E_branch.clear();
  track_trueE_branch.clear();
  track_pdg_branch.clear();
  truetrackE_branch.clear();
  truepionE_branch.clear();
  trueprotonE_branch.clear();
  truekaonE_branch.clear();
  track_resE_branch.clear();
}

void ana::RecoEfficencyFinder::endJob(){

  std::cout << "num_v_recoed: " << num_v_recoed << std::endl;
}

bool ana::RecoEfficencyFinder::isFromNuVertex(const art::Ptr<simb::MCTruth>& mc, const simb::MCParticle* &particle, float distance) const{
  
  const TLorentzVector nuVtx     = mc->GetNeutrino().Nu().Trajectory().Position(0);
  const TLorentzVector partstart = particle->Position();
  return TMath::Abs((partstart - nuVtx).Mag()) < distance;
}

bool ana::RecoEfficencyFinder::isFromNuVertex(const art::Ptr<simb::MCTruth>& mc, TVector3& position, float distance) const{
  
  const TVector3 nuVtx     = mc->GetNeutrino().Nu().Trajectory().Position(0).Vect();
  return TMath::Abs((position - nuVtx).Mag()) < distance;
}

//Check if the point is in the Active volume.
bool ana::RecoEfficencyFinder::containedInAV(const TVector3 &v) {
  for (auto const& AV: fActiveVolume) {
    if (AV.ContainsPosition(v)) return true;
  }
  return false;
}

bool ana::RecoEfficencyFinder::isShowerContainedish(const std::vector<int>& shower, std::map<int,float>& Track_Energy_map, std::map<int, const simb::MCParticle*>& trueParticles){

  //Add up the energy of the shower
  float Energy = 0;
  for(auto const& shower_part: shower){
    Energy += trueParticles[shower_part]->E();
  }
  
  //Add up the deposited energy of the shower 
  float deposited_Energy = 0;
  for(auto const& shower_part: shower){
    deposited_Energy += Track_Energy_map[shower_part];
  }

  //If more than 90% of the shower has been identified on the collection plane it is contained.
  if(deposited_Energy/Energy > 0.9){return true;}
  
  return false;

}


bool ana::RecoEfficencyFinder::isPrimaryHadron(const int& track, art::Ptr<simb::MCTruth>& mc_truth, std::map<int,const simb::MCParticle*>& trueParticles,int neutrino_iter,std::map<int,float>& Track_Energy_map){

  if(trueParticles.find(track) == trueParticles.end()){return false;}

  //Check that it is from the vertex. 
  if(!isFromNuVertex(mc_truth,trueParticles[track])){
    return false;
  }

  truetrackE_branch.at(neutrino_iter).push_back(Track_Energy_map[track]);

  //Check if its a hadron 
  if(TMath::Abs(trueParticles[track]->PdgCode()) != 2212 &&
     TMath::Abs(trueParticles[track]->PdgCode()) != 211  &&
     TMath::Abs(trueParticles[track]->PdgCode()) != 321)
    {return false;}
  

  if(TMath::Abs(trueParticles[track]->PdgCode()) == 211){truepionE_branch.at(neutrino_iter).push_back(Track_Energy_map[track]);}
  if(TMath::Abs(trueParticles[track]->PdgCode()) == 2212){trueprotonE_branch.at(neutrino_iter).push_back(Track_Energy_map[track]);}
  if(TMath::Abs(trueParticles[track]->PdgCode()) == 321){truekaonE_branch.at(neutrino_iter).push_back(Track_Energy_map[track]);}

  return true;
}




DEFINE_ART_MODULE(ana::RecoEfficencyFinder)