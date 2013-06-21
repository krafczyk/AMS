#ifndef TRACKFACTORY_HH
#define TRACKFACTORY_HH

namespace ACsoft {

namespace AC {
  class Event;
  class TrackerTrackFit;
}

namespace Analysis {

class SplineTrack;

/** Create tracks for analysis from raw event.
  *
  */
class TrackFactory
{
public:

  TrackFactory();

  SplineTrack* CreateSplineTrackFrom( const AC::TrackerTrackFit& );
};

}

}

#endif // TRACKFACTORY_HH
