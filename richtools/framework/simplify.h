#ifndef __SIMPLIFY__
#define __SIMPLIFY__

/// MACROS TO MAKE WRITING CLEARER
/// Notice that no colon is needed after callling them. This possibilites to
/// write very nice things (phrases indeed).
#define CUTS_NEW_EVENT {SELECT("ALL",true) initCuts();}
#define SELECT(_name,_condition) {if(!select(_name,(_condition))) return false;}
#define MASK(_name,_condition) {select(#_name,(_condition),true);}
#define CHECK(_name) (cuts_passed.find(#_name)!=cuts_passed.end() && cuts_passed[#_name])
#define CUTS_CLEAR {resetCuts();}
#define CUTS_REPORT {reportCuts();}



// Loop on AMSEvent containers classes
#define FOR_EACH(_class){for(int _count=0;_count<n##_class ();_count++){_class##R & _ANONYMOUS_=_class (_count);
#define AS(_variable)  typeof(_ANONYMOUS_) _variable=_ANONYMOUS_;
#define END };}

// Execute block every _counter events
#define EVERY(_counter)  {\
static int _ANONYMOUS_=-1;\
_ANONYMOUS_++;\
if(_ANONYMOUS_==(_counter)-1)_ANONYMOUS_=0;\
if(_ANONYMOUS_==0){


#define NEW_ANALYSIS(_name) namespace Framework{\
class _name: public AnalysisFramework{ \
  public:\
    _name(){};


// foreach for stl containers
#define foreach(container,var)				     \
  for(typeof((container).begin()) var = (container).begin(); \
      var != (container).end(); \
      ++var)


// Special for maps 
#define _ci(container) container ## iter
#define foreach_value(container,var)				       \
  for (typeof((container).begin()) _ci(container) = container.begin(); \
       _ci(container) != container.end(); ) \
    for (typeof(_ci(container)->second)* var = &_ci(container)->second; \
	 _ci(container) != container.end(); \
	 (++_ci(container) != container.end()) ? \
	   (var = &_ci(container)->second) : var)

#define foreach_key(container,var)					       \
  for (typeof((container).begin()) _ci(container) = container.begin(); \
       _ci(container) != container.end(); ) \
    for (typeof(_ci(container)->first)* var = &_ci(container)->first; \
	 _ci(container) != container.end(); \
	 (++_ci(container) != container.end()) ? \
	   (var = &_ci(container)->first) : var)



#endif
