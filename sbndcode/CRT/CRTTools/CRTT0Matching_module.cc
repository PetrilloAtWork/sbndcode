/////////////////////////////////////////////////////////////////////////////
/// Class:       CRTT0Matching
/// Module Type: producer
/// File:        CRTT0Matching_module.cc
///
/// Author:         Thomas Brooks
/// E-mail address: tbrooks@fnal.gov
///
/// Modified from CRTT0Matching by Thomas Warburton.
/////////////////////////////////////////////////////////////////////////////

// sbndcode includes
#include "sbnobj/Common/CRT/CRTHit.hh"
#include "sbndcode/CRT/CRTUtils/CRTT0MatchAlg.h"

// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "canvas/Persistency/Common/FindManyP.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Event.h" 
#include "canvas/Persistency/Common/Ptr.h" 
#include "canvas/Persistency/Common/PtrVector.h" 
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art_root_io/TFileService.h"
#include "art_root_io/TFileDirectory.h"

#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <memory>
#include <iostream>
#include <map>
#include <iterator>
#include <algorithm>

// LArSoft
#include "larcore/Geometry/Geometry.h"
#include "larcorealg/Geometry/GeometryCore.h"
#include "larcorealg/Geometry/PlaneGeo.h"
#include "larcorealg/Geometry/WireGeo.h"
#include "lardataobj/AnalysisBase/T0.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/Track.h"
#include "lardata/Utilities/AssociationUtil.h"
#include "lardata/DetectorInfoServices/LArPropertiesService.h"
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h"
#include "lardataobj/RawData/ExternalTrigger.h"
#include "larcoreobj/SimpleTypesAndConstants/PhysicalConstants.h"
#include "lardataobj/AnalysisBase/ParticleID.h"
#include "larsim/MCCheater/BackTrackerService.h"

// ROOT
#include "TVector3.h"


namespace sbnd {
  
  class CRTT0Matching : public art::EDProducer {
  public:

    explicit CRTT0Matching(fhicl::ParameterSet const & p);

    // The destructor generated by the compiler is fine for classes
    // without bare pointers or other resource use.

    // Plugins should not be copied or assigned.
    CRTT0Matching(CRTT0Matching const &) = delete;
    CRTT0Matching(CRTT0Matching &&) = delete;
    CRTT0Matching & operator = (CRTT0Matching const &) = delete; 
    CRTT0Matching & operator = (CRTT0Matching &&) = delete;

    // Required functions.
    void produce(art::Event & e) override;

    // Selected optional functions.
    void beginJob() override;

    void endJob() override;

    void reconfigure(fhicl::ParameterSet const & p);

  private:

    // Params got from fcl file.......
    art::InputTag fTpcTrackModuleLabel; ///< name of track producer
    art::InputTag fCrtHitModuleLabel;   ///< name of crt producer

    CRTT0MatchAlg t0Alg;

  }; // class CRTT0Matching


  CRTT0Matching::CRTT0Matching(fhicl::ParameterSet const & p)
    : EDProducer(p), t0Alg(p.get<fhicl::ParameterSet>("T0Alg"))
  // Initialize member data here, if know don't want to reconfigure on the fly
  {

    // Call appropriate produces<>() functions here.
    produces< std::vector<anab::T0>               >();
    produces< art::Assns<recob::Track , anab::T0> >();
    
    reconfigure(p);

  } // CRTT0Matching()


  void CRTT0Matching::reconfigure(fhicl::ParameterSet const & p)
  {

    fTpcTrackModuleLabel = (p.get<art::InputTag> ("TpcTrackModuleLabel"));
    fCrtHitModuleLabel   = (p.get<art::InputTag> ("CrtHitModuleLabel")); 

  } // CRTT0Matching::reconfigure()


  void CRTT0Matching::beginJob()
  {

  } // CRTT0Matching::beginJob()

  void CRTT0Matching::produce(art::Event & event)
  {

    // Create anab::T0 objects and make association with recob::Track
    std::unique_ptr< std::vector<anab::T0> > T0col( new std::vector<anab::T0>);
    std::unique_ptr< art::Assns<recob::Track, anab::T0> > Trackassn( new art::Assns<recob::Track, anab::T0>);

    // Retrieve CRT hit list
    art::Handle<std::vector<crt::CRTHit>> crtListHandle;
    std::vector<art::Ptr<crt::CRTHit>> crtList;
    if(event.getByLabel(fCrtHitModuleLabel, crtListHandle))
      art::fill_ptr_vector(crtList, crtListHandle);

    std::vector<crt::CRTHit> crtHits;
    for (auto const& crtHit : crtList){
      crtHits.push_back(*crtHit);
    }

    // Retrieve track list
    art::Handle< std::vector<recob::Track> > trackListHandle;
    std::vector<art::Ptr<recob::Track> > trackList;
    if (event.getByLabel(fTpcTrackModuleLabel,trackListHandle))
      art::fill_ptr_vector(trackList, trackListHandle);   

    mf::LogInfo("CRTT0Matching")
      <<"Number of reconstructed tracks = "<<trackList.size()<<"\n"
      <<"Number of CRT hits = "<<crtList.size();
   
    if (trackListHandle.isValid() && crtListHandle.isValid() ){
      
      // Loop over all the reconstructed tracks 
      for(size_t track_i = 0; track_i < trackList.size(); track_i++) {

        // Get the closest matched time
        std::pair<double, double> matchedTime = t0Alg.T0AndDCAFromCRTHits(*trackList[track_i], crtHits, event);
        if(matchedTime.first != -99999){
          mf::LogInfo("CRTT0Matching")
            <<"Matched time = "<<matchedTime.first<<" [us] to track "<<trackList[track_i]->ID()<<" with DCA = "<<matchedTime.second;
          T0col->push_back(anab::T0(matchedTime.first * 1e3, 0, trackList[track_i]->ID(), (*T0col).size(), matchedTime.second));
          util::CreateAssn(*this, event, *T0col, trackList[track_i], *Trackassn);
        }

      } // Loop over tracks  

    } // Validity check
   
    event.put(std::move(T0col));
    event.put(std::move(Trackassn));
    
  } // CRTT0Matching::produce()


  void CRTT0Matching::endJob()
  {

  } // CRTT0Matching::endJob()


  DEFINE_ART_MODULE(CRTT0Matching)

} // sbnd namespace

namespace {

}
