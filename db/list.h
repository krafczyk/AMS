//  $Id: list.h,v 1.8 2001/01/22 17:32:29 choutko Exp $
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

#line 12 "list.ddl"
class EventList : public ooContObj { 



integer _nEvents; 

class ooVString _listName; 
ooVString _setupName; 




public: EventList(); 
EventList(char *); 
EventList(char *, char *); 
inline virtual ~EventList() { }


inline const char *ListName() { return _listName; }
inline const char *SetupName() { return _setupName; }
inline integer Events() { return _nEvents; }
inline integer getNEvents() { return _nEvents; }
inline void incNEvents() { _nEvents ++; }
inline void decNEvents() { if (_nEvents> 0) _nEvents --; }

inline void resetCounters() { _nEvents = 0; }
void setlistname(char *); 
void setsetupname(char *); 

ooBoolean CheckListSstring(char *); 

ooStatus CheckContainer(char *, ooMode, ooHandle(ooContObj) &); 

void printListHeader(); 
ooStatus PrintListStatistics(char *); 
    virtual ooTypeNumber ooGetTypeN() const;
    virtual char* ooGetTypeName() const;
    ooBoolean ooIsKindOf(ooTypeNumber typeN) const;
    ooHandle(EventList)& ooThis(ooHandle(EventList)& objH,
			 ooMode aMode = oocNoOpen) const
      {
	opiGetObjHandle(ooDysoc(), objH, aMode);
	return objH;
      }
    ooHandle(EventList) ooThis(ooMode aMode = oocNoOpen) const;
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
	return opiODMGDatabaseNew(size, ooTypeN(EventList), odmg_db);
      }	
    void *operator new(size_t  size,
		       d_Database *odmg_db,
		       const char *)
      {
	return opiODMGDatabaseNew(size, ooTypeN(EventList), odmg_db);
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
    ooRef(EventList)& ooThis(ooRef(EventList)& objId, ooMode aMode = oocNoOpen) const;
    EventList(ooInternalObj iobj);
#line 47 "list.ddl"
}; 

#endif /* !defined(LIST_H) */
