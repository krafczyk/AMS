//  $Id: eventTag.h,v 1.5 2001/01/22 17:32:29 choutko Exp $
/* C++ header file: Objectivity/DB DDL version 4.0.2 */

#ifndef EVENT_TAG_H
#define EVENT_TAG_H

#ifdef OO_DDL_TRANSLATION
#pragma ooddlout "eventTag.ddl"
class opiDummyDeclaration;
#endif

#ifndef OO_H
#include <oo.h>
#endif
#include <typedefs.h>
#include <cern.h>
#include <dbevent_ref.h>
#include <dbevent.h>

#ifndef EVENT_TAG_REF_H
#include "eventTag_ref.h"
#endif

class AMSraweventD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSraweventD);
class ooItr(AMSraweventD);
class ooRef(AMSraweventD);
class ooShortRef(AMSraweventD);
#endif // !defined(OO_BUGGY_TEMPLATES)

class AMSeventD;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSeventD);
class ooItr(AMSeventD);
class ooRef(AMSeventD);
class ooShortRef(AMSeventD);
#endif // !defined(OO_BUGGY_TEMPLATES)

class AMSmcevent;
#ifndef OO_BUGGY_TEMPLATES
class ooHandle(AMSmcevent);
class ooItr(AMSmcevent);
class ooRef(AMSmcevent);
class ooShortRef(AMSmcevent);
#endif // !defined(OO_BUGGY_TEMPLATES)

#line 11 "eventTag.ddl"
class AMSeventD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSeventD <recevent_ref.h>
#else
#include <recevent_ref.h>
#endif
class opiDummyDeclaration;

class AMSmcevent; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSmcevent <mcevent_ref.h>
#else
#include <mcevent_ref.h>
#endif
class opiDummyDeclaration;

class AMSraweventD; 
#ifdef OO_DDL_TRANSLATION
#pragma ooclassref AMSraweventD <raweventD_ref.h>
#else
#include <raweventD_ref.h>
#endif
class opiDummyDeclaration;



class AMSEventTag : public dbEvent { 
#line 29
uinteger _RunType; 

number _NorthPolePhi; 
number _StationTheta; 
number _StationPhi; 
#line 38 "eventTag.ddl"
public: 
#ifdef OO_DDL_TRANSLATION
#line 38 "eventTag.ddl"
    ooRef(AMSraweventD) itsRawEvent : delete(propagate);
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for unidirectional association link: itsRawEvent */
#if !defined(__DECCXX) || !defined(OO_VMS)
    ooHandle(AMSraweventD) itsRawEvent(ooMode mode = oocNoOpen) const;
#endif
    ooHandle(AMSraweventD)& itsRawEvent(ooHandle(AMSraweventD)& objH,
		       ooMode mode = oocNoOpen) const;
    ooStatus set_itsRawEvent(const ooHandle(AMSraweventD)& objH);
    ooStatus del_itsRawEvent()
      {
	return opiDelUssoc_1(ooDysoc(), ooAssocN(AMSEventTag,itsRawEvent));
      }
    ooBoolean exist_itsRawEvent(const ooHandle(AMSraweventD)& objH) const;
    ooBoolean exist_itsRawEvent() const
      {
        return this->exist_itsRawEvent((ooHandle(AMSraweventD)&) (oocNullHandle));
      }
    static ooAssocNumber itsRawEvent_ooAssocN;
    ooRef(AMSraweventD)& itsRawEvent(ooRef(AMSraweventD)& objId, ooMode mode = oocNoOpen) const;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 39 "eventTag.ddl"
    ooRef(AMSeventD) itsRecEvent : delete(propagate);
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for unidirectional association link: itsRecEvent */
#if !defined(__DECCXX) || !defined(OO_VMS)
    ooHandle(AMSeventD) itsRecEvent(ooMode mode = oocNoOpen) const;
#endif
    ooHandle(AMSeventD)& itsRecEvent(ooHandle(AMSeventD)& objH,
		       ooMode mode = oocNoOpen) const;
    ooStatus set_itsRecEvent(const ooHandle(AMSeventD)& objH);
    ooStatus del_itsRecEvent()
      {
	return opiDelUssoc_1(ooDysoc(), ooAssocN(AMSEventTag,itsRecEvent));
      }
    ooBoolean exist_itsRecEvent(const ooHandle(AMSeventD)& objH) const;
    ooBoolean exist_itsRecEvent() const
      {
        return this->exist_itsRecEvent((ooHandle(AMSeventD)&) (oocNullHandle));
      }
    static ooAssocNumber itsRecEvent_ooAssocN;
    ooRef(AMSeventD)& itsRecEvent(ooRef(AMSeventD)& objId, ooMode mode = oocNoOpen) const;
#endif /* !defined(OO_DDL_TRANSLATION) */
#ifdef OO_DDL_TRANSLATION
#line 40 "eventTag.ddl"
    ooRef(AMSmcevent) itsMCEvent : delete(propagate);
#else /* !defined(OO_DDL_TRANSLATION) */
    /* Methods for unidirectional association link: itsMCEvent */
#if !defined(__DECCXX) || !defined(OO_VMS)
    ooHandle(AMSmcevent) itsMCEvent(ooMode mode = oocNoOpen) const;
#endif
    ooHandle(AMSmcevent)& itsMCEvent(ooHandle(AMSmcevent)& objH,
		       ooMode mode = oocNoOpen) const;
    ooStatus set_itsMCEvent(const ooHandle(AMSmcevent)& objH);
    ooStatus del_itsMCEvent()
      {
	return opiDelUssoc_1(ooDysoc(), ooAssocN(AMSEventTag,itsMCEvent));
      }
    ooBoolean exist_itsMCEvent(const ooHandle(AMSmcevent)& objH) const;
    ooBoolean exist_itsMCEvent() const
      {
        return this->exist_itsMCEvent((ooHandle(AMSmcevent)&) (oocNullHandle));
      }
    static ooAssocNumber itsMCEvent_ooAssocN;
    ooRef(AMSmcevent)& itsMCEvent(ooRef(AMSmcevent)& objId, ooMode mode = oocNoOpen) const;
#endif /* !defined(OO_DDL_TRANSLATION) */
#line 44 "eventTag.ddl"
inline AMSEventTag() { }
AMSEventTag(uinteger, uinteger); 
AMSEventTag(uinteger, uinteger, uinteger, time_t); 
#line 52
inline uinteger runType() const { return _RunType; }
inline uinteger run() { return Run (); }

inline void GetGeographicCoo(number &pole, number &theta, number &phi) {
                      pole = _NorthPolePhi; theta = _StationTheta; phi = _StationPhi; }


inline void SetGeographicCoo(number pole, number theta, number phi) {
                      _NorthPolePhi = pole;
                      _StationTheta = theta;
                     _StationPhi = phi; }
    virtual ooTypeNumber ooGetTypeN() const;
    virtual char* ooGetTypeName() const;
    ooBoolean ooIsKindOf(ooTypeNumber typeN) const;
    ooHandle(AMSEventTag)& ooThis(ooHandle(AMSEventTag)& objH,
			 ooMode aMode = oocNoOpen) const
      {
	opiGetObjHandle(ooDysoc(), objH, aMode);
	return objH;
      }
    ooHandle(AMSEventTag) ooThis(ooMode aMode = oocNoOpen) const;
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
	return opiODMGDatabaseNew(size, ooTypeN(AMSEventTag), odmg_db);
      }	
    void *operator new(size_t  size,
		       d_Database *odmg_db,
		       const char *)
      {
	return opiODMGDatabaseNew(size, ooTypeN(AMSEventTag), odmg_db);
      }	
#endif
    static const ooTypeNumber opiTypeN;
    static void ooAssocRegister();
    ooRef(AMSEventTag)& ooThis(ooRef(AMSEventTag)& objId, ooMode aMode = oocNoOpen) const;
    AMSEventTag(ooInternalObj iobj);
#line 63 "eventTag.ddl"
}; 

#endif /* !defined(EVENT_TAG_H) */
