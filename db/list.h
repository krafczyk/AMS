/* C++ header file: Objectivity/DB DDL version 3.8.1 */

#ifndef LIST_H
#define LIST_H

#ifdef OO_DDL_TRANSLATION
#pragma ooddlout "list.ddl"
class opiDummyDeclaration;
#endif

#ifndef OO_H
#include <oo.h>
#endif
#include <ooMap.h>
#include <typedefs.h>
#line 2 "list.ddl"
#define list_h 

#ifndef LIST_REF_H
#include "list_ref.h"
#endif

class AMSEventD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSEventD);
class ooItr(AMSEventD);
class ooRef(AMSEventD);
class ooShortRef(AMSEventD);
#endif // !defined(OO_BUGGY_TEMPLATES)

class AMSgvolumeD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSgvolumeD);
class ooItr(AMSgvolumeD);
class ooRef(AMSgvolumeD);
class ooShortRef(AMSgvolumeD);
#endif // !defined(OO_BUGGY_TEMPLATES)

#line 21 "list.ddl"
class AMSEventD; 
class AMSgvolumeD; 
class AMSNode; 

class AMSEventList : public ooContObj { 




integer _listType; 
integer _nEvents; 
integer _nEventsP; 
integer _nHits[6]; 
integer _nClusters[2]; 
integer _nTracks; 
integer _nMCClusters; 
integer _nTOFMCClusters; 
integer _nAntiMCClusters; 
integer _nTOFClusters[4]; 
integer _nCTCClusters; 
integer _nCTCMCClusters; 
integer _nMCCTCClusters; 
integer _nBetas; 
integer _nCharges; 
integer _nParticles; 
integer _nmcEventg; 

class ooVString _listName; 
ooVString _Setup; 

ooVString _mapName; 
ooVString _mceventgCont; 
ooVString _trlayerCont[6]; 
ooVString _trclusterCont[2]; 
ooVString _trmcclusterCont; 
ooVString _tofmcclusterCont; 
ooVString _sclayerCont[4]; 
ooVString _ctcclusterCont; 
ooVString _ctcmcclusterCont; 
ooVString _trtrackCont; 
ooVString _betaCont; 
ooVString _chargeCont; 
ooVString _particleCont; 
ooVString _antimcclusterCont; 




public: AMSEventList(); 
AMSEventList(char *, char *); 



ooStatus AddGeometry(); 
ooStatus AddMaterial(); 
ooStatus AddTMedia(); 
ooStatus Addamsdbc(); 
ooStatus AddEvent(integer, uinteger, char *, integer, ooHandle(ooMap)); 


ooStatus AddTrCluster(char *, const integer, ooHandle(AMSEventD) &); 
ooStatus AddTrMCCluster(char *, ooHandle(AMSEventD) &); 
ooStatus AddTOFMCCluster(char *, ooHandle(AMSEventD) &); 

ooStatus AddTrRecHit(char *, const integer, ooHandle(AMSEventD) &); 
ooStatus AddTrTrack(char *, ooHandle(AMSEventD) &); 
ooStatus AddmcEventg(char *, ooHandle(AMSEventD) &); 
ooStatus AddCTCCluster(char *, ooHandle(AMSEventD) &); 
ooStatus AddCTCMCCluster(char *, ooHandle(AMSEventD) &); 
ooStatus AddAntiMCCluster(char *, ooHandle(AMSEventD) &); 

ooStatus AddTOFCluster(char *, const integer, ooHandle(AMSEventD) &); 
ooStatus AddBeta(char *, ooHandle(AMSEventD) &); 
ooStatus AddCharge(char *, ooHandle(AMSEventD) &); 
ooStatus AddParticle(char *, ooHandle(AMSEventD) &); 

ooStatus CopyGeometry(ooMode); 
ooStatus CopyMaterial(ooMode); 
ooStatus CopyTMedia(ooMode); 
ooStatus CopyEvent(char *, ooHandle(AMSEventD) &, ooMode); 
ooStatus CopyMCEvent(char *, ooHandle(AMSEventD) &, ooMode); 
ooStatus CopyMCeventg(char *, ooHandle(AMSEventD) &, ooMode); 
ooStatus CopymcEventg(char *, ooHandle(AMSEventD) &, ooMode); 
ooStatus CopyTrRecHit(char *, ooHandle(AMSEventD) &, ooMode); 
ooStatus CopyTrCluster(char *, ooHandle(AMSEventD) &, ooMode); 
ooStatus CopyTrTrack(char *, ooHandle(AMSEventD) &, ooMode); 

ooStatus CopyTrMCCluster(char *, ooHandle(AMSEventD) &, ooMode); 

ooStatus CopyTOFMCCluster(char *, ooHandle(AMSEventD) &, ooMode); 
ooStatus CopyTOFCluster(char *, ooHandle(AMSEventD) &, ooMode); 

ooStatus CopyAntiMCCluster(char *, ooHandle(AMSEventD) &, ooMode); 

ooStatus CopyCTCMCCluster(char *, ooHandle(AMSEventD) &, ooMode); 
ooStatus CopyCTCCluster(char *, ooHandle(AMSEventD) &, ooMode); 
ooStatus CopyBeta(char *, ooHandle(AMSEventD) &, ooMode); 
ooStatus CopyCharge(char *, ooHandle(AMSEventD) &, ooMode); 
ooStatus CopyParticle(char *, ooHandle(AMSEventD) &, ooMode); 


ooStatus DeleteEvent(ooHandle(AMSEventD) &); 



inline integer ListType() { return _listType; }
inline void setListType(integer listType) { _listType = listType; }
inline integer getNEvents() { return _nEvents; }
inline void incNEvents() { _nEvents ++; }
inline void decNEvents() { if (_nEvents> 0) _nEvents --; }
inline integer getNEventsP() { return _nEventsP; }
inline void incNEventsP() { _nEventsP ++; }
inline integer getNTracks() { return _nTracks; }
inline void incNTracks() { _nTracks ++; }
inline integer getNHits(integer n) { if (n < 6) {
                                            return _nHits [ n ];
                                           } else {
                                            return - 1;
                                           }
                                          }
inline void incNHits(integer n) { if (n < 6) _nHits [ n ] ++; }
inline integer getNClusters(integer n) { if (n < 2) {
                                            return _nClusters [ n ];
                                           } else {
                                            return - 1; }
                                          }
inline void incNClusters(integer n) { if (n < 2) _nClusters [ n ] ++; }

inline integer getNTOFCl(integer n) { if (n < 4) {
                                            return _nTOFClusters [ n ];
                                           } else {
                                            return - 1;
                                           }
                                          }
inline void incNTOFCl(integer n) { if (n < 4) _nTOFClusters [ n ] ++; }
inline integer getNCTCClusters() { return _nCTCClusters; }
inline void incNCTCClusters() { _nCTCClusters ++; }
inline integer getNMCCTCClusters() { return _nMCCTCClusters; }
inline void incNMCCTCClusters() { _nMCCTCClusters ++; }
inline integer getNBetas() { return _nBetas; }
inline void incNBetas() { _nBetas ++; }
inline integer getNCharges() { return _nCharges; }
inline void incNCharges() { _nCharges ++; }
inline integer getNParticles() { return _nParticles; }
inline void incNParticles() { _nParticles ++; }
inline integer getNMCClusters() { return _nMCClusters; }
inline void incNMCClusters() { _nMCClusters ++; }
inline integer getNTOFMCClusters() { return _nTOFMCClusters; }
inline void incNTOFMCClusters() { _nTOFMCClusters ++; }
inline integer getNCTCMCClusters() { return _nCTCMCClusters; }
inline void incNCTCMCClusters() { _nCTCMCClusters ++; }
inline integer getNAntiMCClusters() { return _nAntiMCClusters; }
inline void incNAntiMCClusters() { _nAntiMCClusters ++; }
inline integer getNmcEventg() { return _nmcEventg; }
inline void incNmcEventg() { _nmcEventg ++; }

ooStatus GetKeepingList(ooHandle(AMSEventList) &); 

void setsetup(char *); 
void setlistname(char *); 
void setmapname(char *); 
inline void setlisttype(integer type) { _listType = type; }


ooStatus FindMap(char *, ooMode, ooHandle(ooMap) &); 
ooStatus FindEvent(char *, ooMode, ooHandle(AMSEventD) &); 
ooStatus FindEvent(char *, ooMode, ooHandle(AMSEventD) &, ooHandle(ooMap) &); 

ooStatus FindEventByN(integer, ooMode, ooHandle(AMSEventD) &); 

ooStatus CheckContainer(char *, ooMode, ooHandle(ooContObj) &, const int); 

ooStatus CheckAllContainers(ooMode); 
ooStatus LinkHitClusterD(char *, const integer, ooHandle(AMSEventD) &); 

ooStatus LinkHitClusterM(char *, ooHandle(AMSEventD) &); 
ooStatus LinkTrackHitD(char *, ooHandle(AMSEventD) &); 
ooStatus LinkTrackHitM(char *, ooHandle(AMSEventD) &); 

void CopyByPos(ooHandle(AMSgvolumeD) &, ooMode); 
void CopyByPtr(AMSNode *); 

ooStatus PrintMap(ooMode); 
    virtual ooTypeNumber ooGetTypeN() const;
    virtual char* ooGetTypeName() const;
    ooBoolean ooIsKindOf(ooTypeNumber typeN) const;
    ooHandle(AMSEventList)& ooThis(ooHandle(AMSEventList)& objH,
			 ooMode aMode = oocNoOpen) const
      {
	opiGetObjHandle(ooDysoc(), objH, aMode);
	return objH;
      }
    ooHandle(AMSEventList) ooThis(ooMode aMode = oocNoOpen) const;
    void* operator new(size_t, const ooHandle(ooObj)& nearH = oovTopDB);
    void* operator new(size_t, const ooId& nearId);
    void* operator new(size_t, ooConstCPtr(ooObj) nearObj);
    static const ooTypeNumber opiTypeN;
    static void ooAssocRegister();
    void* operator new(size_t, const char*, uint32, uint32,
		       uint32, const ooHandle(ooObj)& nearH = oovTopDB);
    void* operator new(size_t, const char*, uint32, uint32,
		       uint32, const ooId& nearId);
    void* operator new(size_t, const char*, uint32, uint32,
		       uint32, ooConstCPtr(ooObj) nearObj);
    ooRef(AMSEventList)& ooThis(ooRef(AMSEventList)& objId, ooMode aMode = oocNoOpen) const;
    AMSEventList(ooInternalObj iobj);
#line 205 "list.ddl"
}; 

#endif /* !defined(LIST_H) */
