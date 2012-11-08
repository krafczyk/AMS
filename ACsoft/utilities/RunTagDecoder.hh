#include <Rtypes.h>

namespace Utilities {

/** Decoder class for AMS run tags
 *
 * Decoder for AMS run tags based on Bastian's documentation of the run tags.
 *
 * \li \c 0xF*** nominal run
 * \li \c 0xD*** debug run
 * \li \c 0xED** TRD refill run
 *
 * The last 3 (for nominal and debug runs) or 2 (for TRD refill runs) places define
 * the configuration number or refill number respectively.
 *
 * A list of AMS run tags used can be found under:
 * https://amsvobox04.cern.ch/elog/REF/2
 *
 * @see AC::EventHeader::RunTag
 */
class RunTagDecoder {
public:
  /** Default constructor
   * @param[in] RunTag AMS run tag
   */
  RunTagDecoder( const UShort_t& RunTag = 0x0);
  
  /** Check if run has nominal tag
   *  @return true if nominal run, false else
   */
  bool IsNominal() const { return fNominal; }
  
  /** Check if run has debug tag
   *  @return true if debug run, false else
   */
  bool IsDebug() const { return fDebug; }
  
  /** Check if run has TRD refill tag
   *  @return true if TRD refill run, false else
   */
  bool IsTRDRefill() const { return fTRDRefill; }
  
  /** Check if run has undefined tag
   *  @return true if nominal run, false else
   */
  bool IsUndefined() const { return fUndefined; }
  
  /** Get encoded number
   * 
   *  Encoded number represents
   *  \li configuration number for nominal and debug runs
   *  \li refill number for TRD refill runs
   *  \li complete run tag for undefined tags
   *
   *  @return encoded number
   */
  const UShort_t& GetNumber() { return fNumber; }
  
  /** static helper function
   *
   *  equivalent to:
   *  \code{.cpp}
   *  RunTagDecoder decoder(RunTag);
   *  return decoder.IsNominal();
   *  \endcode
   *
   *  \see IsNominal()
   */
  static bool IsNominal( const UShort_t& RunTag );
  
  /** static helper function
   *
   *  equivalent to:
   *  \code{.cpp}
   *  RunTagDecoder decoder(RunTag);
   *  return decoder.IsDebug();
   *  \endcode
   *
   *  \see IsDebug()
   */
  static bool IsDebug( const UShort_t& RunTag );
  
  /** static helper function
   *
   *  equivalent to:
   *  \code{.cpp}
   *  RunTagDecoder decoder(RunTag);
   *  return decoder.IsTRDRefill();
   *  \endcode
   *
   *  \see IsTRDRefill()
   */
  static bool IsTRDRefill( const UShort_t& RunTag );
  
  /** static helper function
   *
   *  equivalent to:
   *  \code{.cpp}
   *  RunTagDecoder decoder(RunTag);
   *  return decoder.IsUndefined();
   *  \endcode
   *
   *  \see IsUndefined()
   */
  static bool IsUndefined( const UShort_t& RunTag );
  
private:
  bool fNominal;
  bool fDebug;
  bool fTRDRefill;
  bool fUndefined;
  UShort_t fNumber;
};

}
