#ifndef CoordinateSystems_hh
#define CoordinateSystems_hh

namespace ACsoft {

namespace AC {
  class EventHeader;
}

namespace Utilities {

  /** A convenience wrapper for coordinate system transformations.
   *
   * This class acts as an interface to the gbatch "FrameTrans" code which is
   * located under utilities/third_party.
   *
   * It provides function which extract relevant information from ACQt
   * structures and relays them to the FrameTrans functions.
   *
   */

  class CoordinateSystems
  {

  public:

    /// Obtain pointer to singleton class instance.
    static CoordinateSystems* instance() {
      static CoordinateSystems* fInstance = 0;
      if( !fInstance )
        fInstance = new CoordinateSystems;
      return fInstance;
    }

    /** Convert direction given by two angles in AMS reference frame into galactic coordinates
     * @param[in] eventHeader AC::EventHeader from which this function takes the ISS position and velocity, the yaw,pitch,roll angles and the current UTCtime
     * @param[in] theta The theta angle in the AMS frame of the point whose galactic coordinates should be calculated / rad
     * @param[in] phi The phi angle in the AMS frame of the point whose galactic coordinates should be calculated / rad
     * @param[out] galacticLongitude galactic longitude of the input point / degree
     * @param[out] galacticLatitude galactic latitude of the input point / degree
     */
    void GetGalacticCoordinates(const AC::EventHeader& eventHeader, double theta, double phi, double& galacticLongitude, double& galacticLatitude) const;

    /** Convert direction given by two angles in AMS reference frame into right ascension and declination in the equatorial frame
     * @param[in] eventHeader AC::EventHeader from which this function takes the ISS position and velocity, the yaw,pitch,roll angles and the current UTCtime
     * @param[in] theta The theta angle in the AMS frame of the point whose galactic coordinates should be calculated / rad
     * @param[in] phi The phi angle in the AMS frame of the point whose galactic coordinates should be calculated / rad
     * @param[out] rightAscension right ascension in equatorial coordinates of the input point / degree
     * @param[out] declination declination in equatorial coordinates of the input point / degree
     */
    void GetEquatorialCoordinates(const AC::EventHeader& eventHeader, double theta, double phi, double& rightAscension, double& declination) const;

    /** Convert Galactic coordinates to Equatorial
      * @param[in] l galactic longitude of input point / degree
      * @param[in] b galactic latitude of input point / degree
      * @param[out] rightAscension right ascension in equatorial coordinates of the input point / degree
      * @param[out] declination declination in equatorial coordinates of the input point / degree
     */
    void GalacticToEquatorial(double l, double b, double& rightAscension, double& declination) const;

    /** Convert Equatorial coordinates to Galactic
      * @param[in] rightAscension right ascension in equatorial coordinates of the input point / degree
      * @param[in] declination declination in equatorial coordinates of the input point / degree
      * @param[out] l galactic longitude of input point / degree
      * @param[out] b galactic latitude of input point / degree
     */
    void EquatorialToGalactic(double rightAscension, double declination, double& l, double& b) const;

  private:

    CoordinateSystems();
    ~CoordinateSystems();

  };

}

}

#endif /* CoordinateSystems_hh */
