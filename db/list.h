/* C++ header file: Objectivity/DB DDL version 4.0.2 */

#ifndef LIST_H
#define LIST_H

#ifdef OO_DDL_TRANSLATION
#pragma ooddlout "list.ddl"
class opiDummyDeclaration;
#endif

#ifndef OO_H
#include <oo.h>
#endif
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

class AMSmceventD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSmceventD);
class ooItr(AMSmceventD);
class ooRef(AMSmceventD);
class ooShortRef(AMSmceventD);
#endif // !defined(OO_BUGGY_TEMPLATES)

class AMSgvolumeD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSgvolumeD);
class ooItr(AMSgvolumeD);
class ooRef(AMSgvolumeD);
class ooShortRef(AMSgvolumeD);
#endif // !defined(OO_BUGGY_TEMPLATES)

#line 23 "list.ddl"
class AMSEventD; 
class AMSmceventD; 
class AMSgvolumeD; 
class AMSNode; 

class AMSEventList : public ooContObj { 



integer _listType; 
integer _eventType; 
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
integer _nBetas; 
integer _nCharges; 
integer _nParticles; 
integer _nmcEventg; 

class ooVString _listName; 
ooVString _Setup; 
#line 57
public: AMSEventList(); 
AMSEventList(char *, char *); 


ooStatus AddGeometry(ooHandle(ooDBObj) &); 
ooStatus AddMaterial(ooHandle(ooDBObj) &); 
ooStatus AddTMedia(ooHandle(ooDBObj) &); 
ooStatus Addamsdbc(ooHandle(ooDBObj) &); 
ooStatus AddTDV(ooHandle(ooDBObj) &); 
ooStatus AddEvent(integer, uinteger, integer); 
ooStatus AddMCEvent(integer, uinteger, integer); 
ooStatus AddDummyMCEvent(integer, uinteger, integer); 
ooStatus AddTrCluster(const integer, ooHandle(AMSEventD) &); 
ooStatus AddTrMCCluster(ooHandle(AMSEventD) &); 
ooStatus AddTOFMCCluster(ooHandle(AMSEventD) &); 
ooStatus AddTrRecHit(const integer, ooHandle(AMSEventD) &); 
ooStatus AddTrTrack(ooHandle(AMSEventD) &); 
ooStatus AddmcEventg(ooHandle(AMSEventD) &); 
ooStatus AddCTCCluster(ooHandle(AMSEventD) &); 
ooStatus AddCTCMCCluster(ooHandle(AMSEventD) &); 
ooStatus AddAntiMCCluster(ooHandle(AMSEventD) &); 
ooStatus AddTOFCluster(const integer, ooHandle(AMSEventD) &); 
ooStatus AddBeta(ooHandle(AMSEventD) &); 
ooStatus AddCharge(ooHandle(AMSEventD) &); 
ooStatus AddParticle(ooHandle(AMSEventD) &); 


ooStatus CopyGeometry(ooMode, ooHandle(ooDBObj) &); 
ooStatus CopyMaterial(ooMode, ooHandle(ooDBObj) &); 
ooStatus CopyTMedia(ooMode, ooHandle(ooDBObj) &); 
ooStatus CopyTDV(time_t, ooMode, ooHandle(ooDBObj) &); 
ooStatus CopyEventHeader(ooHandle(AMSEventD) &, ooMode); 
ooStatus CopyEvent(ooHandle(AMSEventD) &, ooMode); 
ooStatus CopyMCEvent(ooHandle(AMSEventD) &, ooMode); 
ooStatus CopyMCEventD(ooHandle(AMSmceventD) &, ooMode); 
ooStatus CopyMCeventg(ooHandle(AMSEventD) &, ooMode); 
ooStatus CopymcEventg(ooHandle(AMSEventD) &, ooMode); 
ooStatus CopyTrRecHit(ooHandle(AMSEventD) &, ooMode); 
ooStatus CopyTrCluster(ooHandle(AMSEventD) &, ooMode); 
ooStatus CopyTrTrack(ooHandle(AMSEventD) &, ooMode); 
ooStatus CopyTrMCCluster(ooHandle(AMSEventD) &, ooMode); 
ooStatus CopyTOFMCCluster(ooHandle(AMSEventD) &, ooMode); 
ooStatus CopyTOFCluster(ooHandle(AMSEventD) &, ooMode); 
ooStatus CopyAntiMCCluster(ooHandle(AMSEventD) &, ooMode); 
ooStatus CopyCTCMCCluster(ooHandle(AMSEventD) &, ooMode); 
ooStatus CopyCTCCluster(ooHandle(AMSEventD) &, ooMode); 
ooStatus CopyBeta(ooHandle(AMSEventD) &, ooMode); 
ooStatus CopyCharge(ooHandle(AMSEventD) &, ooMode); 
ooStatus CopyParticle(ooHandle(AMSEventD) &, ooMode); 

ooStatus LinkHitClusterD(const integer, ooHandle(AMSEventD) &); 
ooStatus LinkTrackHitD(ooHandle(AMSEventD) &); 

void CopyByPos(ooHandle(AMSgvolumeD) &, ooMode); 
void CopyByPtr(AMSNode *); 
#line 117
inline integer ListType() { return _listType; }
inline integer EventType() { return _eventType; }
inline const char *ListName() { return _listName; }
inline const char *getsetup() { return _Setup; }
inline void setListType(integer listType) { _listType = listType; }
inline void setEventType(integer eventType) { _eventType = eventType; }
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

void setsetup(char *); 
void setlistname(char *); 
void SetContainersNames(); 
inline void setlisttype(integer type) { _listType = type; }


ooStatus FindEvent(integer, uinteger, ooMode, ooHandle(AMSEventD) &); 

ooStatus FindEvent(integer, uinteger, ooMode, ooHandle(AMSmceventD) &); 


ooStatus CheckContainer(char *, ooMode, ooHandle(ooContObj) &); 

ooStatus CheckContainer(char *, ooMode, ooHandle(ooContObj) &, ooHandle(ooDBObj) &); 

ooStatus CheckAllContainers(ooMode); 
ooBoolean CheckListSstring(char *); 


ooStatus PrintListStatistics(char *); 
ooStatus PrintListStatistics_mc(char *); 


ooStatus DeleteEventList(); 
ooStatus DeleteAllContainers(); 
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
    void *operator new(size_t, const ooHandle(ooObj) &nearH = oovTopDB);
    void *operator new(size_t, const ooId& nearId);
    void *operator new(size_t, ooConstCPtr(ooObj) nearObj);
    void *operator new(size_t,
		       const ooHandle(ooObj) &nearH,
		       const char *typeName);
#ifdef OO_ODMG
    void *operator new(size_t  size,
		       d_Database *odmg_db)
      {
	return opiODMGDatabaseNew(size, ooTypeN(AMSEventList), odmg_db);
      }	
    void *operator new(size_t  size,
		       d_Database *odmg_db,
		       const char *)
      {
	return opiODMGDatabaseNew(size, ooTypeN(AMSEventList), odmg_db);
      }	
#endif
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
#line 196 "list.ddl"
}; 

#endif /* !defined(LIST_H) */
