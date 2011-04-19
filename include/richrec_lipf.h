
*********************************
*** PARAMETERS
*********************************
      integer nbhitsmax, nhitmax
*      PARAMETER (nbhitsmax = 11000,   ! nb of data hits to be read
      PARAMETER (nbhitsmax = 1000,   ! nb of data hits to be read (changed 13-Jan-2011)
     +           NHITMAX   = 1000     ! nb of data hits to be stored on reconstruction
     +          )


******************************
*** CODE FROM constant.inc ***
******************************

      real pi,twopi,piby2,degrad,raddeg,
     +     clight,big,emass,emmu,pmass,avo

      PARAMETER (PI=3.14159265358979324)
      PARAMETER (TWOPI=6.28318530717958648)
      PARAMETER (PIBY2=1.57079632679489662)
      PARAMETER (DEGRAD=0.0174532925199432958)
      PARAMETER (RADDEG=57.2957795130823209)
      PARAMETER (CLIGHT=29979245800.)
      PARAMETER (BIG=10000000000.)
      PARAMETER (EMASS=0.00051099906)
      PARAMETER (EMMU=0.105658389)
      PARAMETER (PMASS=0.93827231)
      PARAMETER (AVO=0.60221367)


*****************************
*** CODE FROM richsim.inc ***
*****************************

* --- simulation variables
      real cerang,pbeta,pchg

      common /richsimc/ cerang,pbeta,pchg
      !$OMP THREADPRIVATE (/richsimc/)
* --- nb counted photons
        integer nphb_nscat,nphb_nscat_ref,nphb_nscat_nref
        common /richphotc/
     +  nphb_nscat,     !nb photons at base (non-scatt and from primary)
     +  nphb_nscat_ref, !nb photons at base reflected
     +  nphb_nscat_nref !nb photons at base non-reflected
      !$OMP THREADPRIVATE (/richphotc/)

* --- ratios
        real rat_nphb_nscat_ref,
     +       rat_nphb_nscat_nref,
     +       xphnint,xphnint_frac

        common/richphotr/
     +  rat_nphb_nscat_ref,
     +  rat_nphb_nscat_nref,
     +  xphnint,xphnint_frac
      !$OMP THREADPRIVATE (/richphotr/)

*****************************
*** CODE FROM richgen.inc ***
*****************************

* --- statistics                          
        integer icevt,itevt

        common /richwrkc/ icevt,        ! number of events read
     +                    itevt         ! event number (nb trials)

      !$OMP THREADPRIVATE (/richwrkc/)
*****************************
*** CODE FROM richcrd.inc ***
*****************************

        integer
     +          nbrevt, ldebug,
     +          firste, brackb,
     +          rcgeom, betmet,
     +          jobc_cp,ievnumb,irun
        real
     +          chi2htcrd, chi2ttcrd, zphini,
     +          lbdcut, phecut, radint,
     +          recbet, cerlim,
     +          reflec


        common /cardc/
     +                  nbrevt,         !nb events to read
     +                  ldebug,         !debugging flag
     +                  firste,         !event to read
     +                  brackb,         !nb of bins for bracketing
     +                  rcgeom(2),      !rich geometry (1)=0,flight
                                        !              (1)=1,MIXED
                                        !              (1)=9,PROTOTYPE
     +                  betmet,         !
     +                  jobc_cp,        !kind of data
     +                  ievnumb,        !event number
     +                  irun,           !run id
     +                  chi2htcrd,      !chi2/hit
     +                  chi2ttcrd,      !chi2/event
     +                  zphini,         !Z fraction of emission point
     +                  lbdcut,         !wavelength cut
     +                  phecut,         !pe/hit
     +                  radint,         !radiator interaction length
     +                  recbet(4),      !beta rec option
     +                  cerlim(2),      !limits for bracketing
     +                  reflec          !mirror reflectivity

      !$OMP THREADPRIVATE (/cardc/)
*****************************
*** CODE FROM richgeo.inc ***
*****************************

        integer nrmax
        PARAMETER(NRMAX=2)
	real RTMIR, RBMIR, HMIR, VTMIR, HRAD, RADTILE_PITCH,
     +       RADTILE_SUPTHK, RADTILE_CLART, HPGL, ZPMTDET,
     +       HRNAF, ZTMIRGAP, ZBMIRGAP, ZLGSIGNAL, RADCLARITY,
     +       PGLIX,RADIX,TGMIR,D0MIR,EMCXLIM,EMCYLIM,JUMP,
     +       DMECX,DMECY,DX0,ZTARG

	integer nmaxmirsec
	PARAMETER(NMAXMIRSEC=10)
	integer nmirsec
	integer idmirs(nmaxmirsec)
	real phimirs(nmaxmirsec)
	real refmirs(nmaxmirsec)

        integer NRADTS


! distances and coordinates of some pmts in the matrix (22/7/02)


	common /patgeomc/ RTMIR,         !Top mirror radius
     +                    RBMIR,         !bottom mirror radius
     +                    HMIR,          !mirror height (plutot expansion volume!!!!)
     +                    HRNAF,         !NAF radiator height
     +                    HRAD,          !MAIN radiator height (aerogel on flight)
     +                    RADTILE_PITCH, !tile pitch
     +                    RADTILE_SUPTHK,!tile dead space (support)
     +                    HPGL,          !plexiglass foil thickness
     +                    ZPMTDET,       !Z coordinate of PMT detector
     +                    NRADTS,        !nb of radiators
     +                    PGLIX,         !refractive index of foil
     +                    RADIX(NRMAX),  !refractive index
     +                    RADCLARITY,    !radiator agl clarity
     +                    TGMIR,         !tangent gamma for mirror
     +                    D0MIR,         !d0
     +                    VTMIR(3),      !mirror vertex
     +                    EMCXLIM,       !EMC HOLE (X coord)
     +                    EMCYLIM,       !EMC HOLE (Y coord)
     +                    JUMP,          !DISTANCIA ENTRE MODULOS 3 E 4
     +                    DMECX,         !DISTANCIA CANTOS E CENTRAIS(X)
     +                    DMECY,         !DISTANCIA CANTOS E CENTRAIS(Y)
     +                    DX0,           !DISTANCIA ENTRE X=0 E INICIO DO MODULO 3
     +                    ZTMIRGAP,      !top mirror gap
     +                    ZBMIRGAP,      !bottom mirror gap
     +                    ZLGSIGNAL,     !z efective distance below pmt lg matrix plane for
                                         !signal optimization
     +                    ZTARG,         !Z top coordinate of the radiator in the RICH frame
     +                    NMIRSEC,       !number of mirror sectors
     +                    IDMIRS,        !id of each mirror sector (number 1-N)
     +                    PHIMIRS,       !starting phi of each mirror sector
                                         !(should be in ascending order except for first sector
                                         ! which includes phi=0, each sector ends on following phi,
                                         ! last sector ends on first phi)
     +                    REFMIRS        !reflectivity of each mirror sector

! detection matrix
      !$OMP THREADPRIVATE (/patgeomc/)

        REAL PMTWX, PMTWY, SHIELDW, LG_PITCH, LG_TOP_WIDTH, LG_BOT_WIDTH


        common /pmtgenc/  PMTWX,         !pmt(lguide in fact) x dimension
     +                    PMTWY,         !pmt                 y dimension
     +                    SHIELDW,       !dead space between Light guides
     +                    LG_PITCH,      !Light Guide Pitch (37mm)
     +                    LG_TOP_WIDTH,  !Light Guide Top Width (34mm)
     +                    LG_BOT_WIDTH   !Light Guide Bottom Width

       !$OMP THREADPRIVATE (/pmtgenc/)

        REAL XPC,XBC,XBD,YEF

        COMMON /PMTPOS/XPC,XBC,XBD,YEF
        !$OMP THREADPRIVATE (/PMTPOS/)

        INTEGER LEVGEOM, LEVGRAD, LEVACC, LEVGHIT


        COMMON /CLEVEL/ LEVGEOM,         !detector matrix geometry level
     +                  LEVGRAD,         !radiator geometry level (0=no walls, =1,2 walls)
     +                  LEVACC,		 !acceptance calculation 0/1 = ring line/ring width
     +                  LEVGHIT 	 !exclusion of ring near track zone 0/1 = off/on

       !$OMP THREADPRIVATE (/CLEVEL/)

        real XCRAD,YCRAD,XDRAD,YDRAD,XCPMM,YCPMM,
     &       DIMXPROT,DIMYPROT,XLIMPROT,YLIMPROT,
     +       MIRPHILD,MIRPHILG


        common /patprotoc/
     +                    XCRAD,        !x-coord of the center of radiator
     +                    YCRAD,        !y-coord of the center of radiator
     +                    XDRAD,        !x-dimension of radiator
     +                    YDRAD,        !y-dimension of radiator
     +                    XCPMM,        !x-coord of the center of PMT matrix
     +                    YCPMM,        !y-coord of the center of PMT matrix
     +                    DIMXPROT,     !x prototype dimension
     +                    DIMYPROT,     !y prototype dimension
     +                    XLIMPROT,     !x coord-max for prototype
     +                    YLIMPROT,      !y coord-max for prototype
     +                    MIRPHILD(2),   !mirror phi limits for DRAWING
     +                    MIRPHILG(2)   !mirror phi limits for GEOMETRY check

        !$OMP THREADPRIVATE (/patprotoc/)

        INTEGER NBPMTX,NBPMTY
        PARAMETER(NBPMTX=10,NBPMTY=11)
        INTEGER iflpmt,INBPMT,IEXPMT,NEXPMT,NONPMT,IONPMT
        INTEGER NBPMTLIP
        PARAMETER(NBPMTLIP=756)
        INTEGER ipmtstat

        common /pmtprotoc/
     +                    iflpmt(nbpmtx,nbpmty),!pmt status flag (=0/=1,2)
     +                    inbpmt(nbpmtx,nbpmty),!pmt id
     +                    iexpmt(110),          !pmt id of non-existing pmts
     +                    nexpmt,               !nb of non-working pmts
     +                    nonpmt,
     +                    ionpmt(110),
     +                    ipmtstat(nbpmtlip)
       !$OMP THREADPRIVATE (/pmtprotoc/)

*****************************
*** CODE FROM richdat.inc ***
*****************************

        integer nbhits, hitsadc, hitsnpe_sim,
     +          hitspmt, hitstat, nbhitsmax_ntup
        real hitscoo, hitsnpe
*

	common /richdatc/
     +         nbhits,                ! number of hits
     +         nbhitsmax_ntup,        ! max number of hits in ntuple
     +         hitsadc(nbhitsmax),    ! adc counts
     +         hitsnpe_sim(nbhitsmax),! simul signal (npe)
     +         hitspmt(nbhitsmax),    ! (pmtnumber+16*pixelnumber)*10 + gain
     +         hitscoo(3,nbhitsmax),  ! coordinates
     +         hitsnpe(nbhitsmax),    ! nb photo electrons from adc
     +         hitstat(nbhitsmax)     ! hit status flag:
                                      !    0 = good
                                      !    1 = TB2002 bad
       !$OMP THREADPRIVATE (/richdatc/)
*****************************
*** CODE FROM richtrk.inc ***
*****************************

* --------------------------------------
* PARTICLE COORDINATES IN RICH FRAME
* --------------------------------------

      real pimp, epimp, pmom, pthe, epthe, pphi, epphi, pchi2red,
     +     pcoopmt, pcoopmtradius,
     +     pcoopmtopt, pcoopmtoptradius

C ... Track information (be used in reconstructions)
*     ----------------------------------


      common /richparc/ pimp(3),        ! impact point @radiator top
     +                  epimp(3),       ! error in impact point @radiator top
     +                  pmom,           ! particle momentum
     +                  pthe,           ! particle theta
     +                  epthe,          ! error in particle theta
     +                  pphi,           ! particle phi
     +                  epphi,          ! error in particle phi
     +                  pchi2red(2),    ! particle chi2(x,y)
     +                  pcoopmt(3),     ! particle point extrapol@PMT
     +                  pcoopmtradius,  ! part point extrap@PMT radius
     +                  pcoopmtopt(3),  ! particle point extrapol@PMT
                                        ! to optimized depth (1.8cm below RP)
     +                  pcoopmtoptradius ! part point extrap@PMT radius

       !$OMP THREADPRIVATE (/richparc/)
C ... Particle impact point hint in the PMT matrix to be used by 5par rec
*     -------------------------------------------------------------------
      real pcoopmtopthint
      integer iflgrec5par


      common /rec5parc/
     +                  iflgrec5par,       ! flag =0, no hint established
                                           !          in this common
     +                  pcoopmtopthint(3)  ! particle point hint @PMT from
                                           ! LG signals
       !$OMP THREADPRIVATE (/rec5parc/)

*****************************
*** CODE FROM richrec.inc ***
*****************************



* -------------------------------------------------------------------------
* --- REC working variables
* -------------------------------------------------------------------------

        integer ihit,nmirefhit,msechit
        real curhit,hypthc

        common /cerwrkc1/
     +         ihit,       ! hit number
     +         curhit(3),  ! coord of current hit
     +         hypthc,     ! cerenkov angle being tested
     +         nmirefhit,  ! nb of mirror reflections of current hit
     +         msechit     ! mirror sector of reflected hit

       !$OMP THREADPRIVATE (/cerwrkc1/)
        real pcervtx,refindex,clari
        character chradid*3


        common /cerwrkc2/
     +         pcervtx(3), ! cerenkov vertex
     +         refindex,   ! current refractive index
     +         chradid     ! current radiator name

       !$OMP THREADPRIVATE (/cerwrkc2/)
        real cangrecup, cangreclow, cang1, cang2
        INTEGER nhitscan, iphihintkind, iphihintflag


        common /cerlimc/
     +         cangrecup,   ! thetac search upper limit
     +         cangreclow,  ! thetac search lower limit
     +         cang1,       ! lower limit for thetac
     +         cang2,       ! upper limit for thetac
     +         nhitscan,    ! number of hits taken for thc minimization
     +         iphihintkind,! hit phi finding: = 0 phi scanning for all thcs
                            !                  = 1 phi scanning at thcmax (once)
                            !                  = 2 phi geometrical at thcmax (once)
     +         iphihintflag ! hit phi finding flag: = 0 no phi hint scanned yet
                            !                       = 1 phi hint scanned
       !$OMP THREADPRIVATE (/cerlimc/)
* --------------------------------------------------------------------------
* --- variables for cerenkov angle reconstruction (per event)
* --------------------------------------------------------------------------
      integer ntherecmax
      PARAMETER (NTHERECMAX=10)

* --- auxiliary
      integer nbhits_nass, ip_nass


      common /richauxc/ nbhits_nass,      ! nb hits non-associated to particle
     +                  ip_nass(nhitmax)  ! pointers to non-assoc to particle hits

      !$OMP THREADPRIVATE (/richauxc/)
* --- fit variables
      real chi2hit_fit,phihit_fit,chi2rec_fit
      integer nbushits_fit,ipushits_fit,nbminshit_fit,ireflechit_fit,
     +        imsechit_fit


      common /richfitc/
     +                    chi2hit_fit(nhitmax),
     +                    phihit_fit(nhitmax),
     +                    chi2rec_fit,
     +                    nbushits_fit,
     +                    ipushits_fit(nhitmax),
     +                    nbminshit_fit(nhitmax),
     +                    ireflechit_fit(nhitmax),
     +                    imsechit_fit(nhitmax)

       !$OMP THREADPRIVATE (/richfitc/)
* --- beta reconstruction: final results

      integer ipthetac,ntherec,nbushits,
     +        ipushits,nbminshit,iflghit,ireflechit,imsechit

      real betarec, cangrec, chi2rec, likerec
     +     chi2hit, phihit,  cangrecdif, pkolmog, flatevt

* --- charge probability

      real chgtest1,chgtest2,chgtest3
      real chgprob1,chgprob2,chgprob3

* --- likelihood value

      real resvlike


      common /richbetc/
     +         ipthetac,                       ! pointer to best fit
     +         ntherec,                        ! nb of rec cerenkov thetas
     +         nbushits(ntherecmax),           ! nb hits associated to pattern (flagged 2)
     +         ipushits(ntherecmax,nhitmax),   ! pointer to used hits
     +         nbminshit(ntherecmax,nhitmax),  ! nb of phi minimums per hit
     +         iflghit(nhitmax),               ! hit flag
                                               ! =0 not associated
                                               ! =1 assoc to track
                                               ! =2 used on 1st rec
                                               ! =3 far from thcmax pattern
     +         ireflechit(ntherecmax,nhitmax), ! (=0 nonreflect,=1 reflect)
     +         imsechit(ntherecmax,nhitmax),   ! mirror sector, =0 if nonreflect
     +         betarec,                        ! beta reconstructed
     +         cangrec(ntherecmax),            ! cerenkov angle reconstr
     +         chi2rec(ntherecmax),            ! reduced chi2
     +         likerec(ntherecmax),            ! lorentzian event prob
     +         chi2hit(ntherecmax,nhitmax),    ! chi2 of all hits
     +         phihit(ntherecmax,nhitmax),     ! hit phi from chosen minimum
     +         cangrecdif(ntherecmax)          ! quality of minimum

       !$OMP THREADPRIVATE (/richbetc/)

      common /richbetauxc/
     +         pkolmog(ntherecmax),            ! kolmogorov estimator for the
                                               ! azimuthal hit uniformity
     +         flatevt(2)                      ! event flatness (sin,cos)

       !$OMP THREADPRIVATE (/richbetauxc/)

      common /chgprobres/
     +         chgtest1,chgtest2,chgtest3,     ! test charge values
     +         chgprob1,chgprob2,chgprob3      ! test charge probabilities

       !$OMP THREADPRIVATE (/chgprobres/)


      common /likeres/
     +         resvlike                        ! likelihood function value at minimum

       !$OMP THREADPRIVATE (/likeres/)
* -----------------------------------------------------------------------
* --- variables for cerenkov charge reconstruction (per event)
* -----------------------------------------------------------------------

      integer nmaxmirsecc
      parameter(NMAXMIRSECC=10)

      integer iflagchg
      real chg_nphe, chg_nphe_sim, chg_nphe_dir, chg_nphe_ref,
     +     chgrec,   chgrec_dir,   chgrec_ref,   chgsim,
     +     chgrec_gap, chgrec_gapdir, chgrec_gapref


      common /richchgc/
     +        iflagchg,         ! charge rec flag (0/1)
     +        chgsim,           ! simulated charge
     +        chg_nphe,         ! total nb of pe assoc
     +        chg_nphe_sim,     ! total nb of pe assoc (simulation)
     +        chg_nphe_dir,     ! tot nb of pe assoc (direct branch)
     +        chg_nphe_ref,     ! tot nb of pe assoc (relected branch)
     +        chgrec,           ! reconstructed charge based on full pattern
     +        chgrec_dir,       ! reconstructed charge based on direct pattern
     +        chgrec_ref,       ! reconstructed charge based on reflected pattern
     +        chgrec_gap,       ! reconstructed charge based on full pattern accounting pmt gaps
     +        chgrec_gapdir,    !
     +        chgrec_gapref     !

      !$OMP THREADPRIVATE (/richchgc/)
      integer nphirmax
      parameter(NPHIRMAX=500)

      integer iflagacc
      real richacc_inv, richacc_dir, richacc_mir0, richacc_mir1,
     +     richacc_hol, richacc_vis, richacc_gap, richacc_gapdir,
     +     richacc_gapref, richacc_msec0, richacc_msec1,
     +     richacc_gapmsec


       common /richaccg/
     +        iflagacc,         ! flag=0/1	
     +        richacc_inv,      ! pattern acceptance INVISIBLE
     +        richacc_dir,      ! pattern acceptance DIRECT to PMT
     +        richacc_mir0,     ! pattern acceptance to MIRROR
     +        richacc_mir1,     ! pattern acceptance to MIRROR
     +        richacc_hol,      ! pattern acceptance to HOLE
     +        richacc_vis,      ! pattern acceptance VISIBLE
     +        richacc_gap,      ! detection efficiency due to gaps (geometrical)
     +        richacc_gapdir,   ! direct detection efficiency due to gaps
     +        richacc_gapref,   ! reflected detection efficiency due to gaps
     +        richacc_msec0(nmaxmirsecc),  ! pattern acceptance by MIRROR SECTOR (1st ref)
     +        richacc_msec1(nmaxmirsecc),  ! pattern acceptance by MIRROR SECTOR (2nd ref)
     +        richacc_gapmsec(nmaxmirsecc)  ! gap efficiency by MIRROR SECTOR

       !$OMP THREADPRIVATE (/richaccg/)
       real richeff_rad, richeff_lg, richeff_dir, richeff_1rf,
     +      richeff_2rf, richeff_tot,richeff_pni, richeff_avz,
     +      richeff_tot_gap, richeff_dir_gap, richeff_ref_gap,
     +      richeff_msec0, richeff_msec1, richeff_msec_gap


       common /richeffc/
     +        richeff_rad,      ! Eff(nint)*Eff(geom)
     +        richeff_lg,       ! LG/PMT efficiency	
     +        richeff_dir,      ! efficiency direct branch
     +        richeff_1rf,      ! efficiency 1rf branch
     +        richeff_2rf,      ! efficiency 2rf branch
     +        richeff_tot,      ! overall efficiency
     +        richeff_pni,      ! probab. of not interacting (rad)
     +        richeff_avz,      ! average photon distance
     +        richeff_tot_gap,  ! overall efficiency with pmt gap effect included
     +        richeff_dir_gap,  !
     +        richeff_ref_gap,  !
     +        richeff_msec0(nmaxmirsecc),  ! efficiency for each mirror sector (1st ref)
     +        richeff_msec1(nmaxmirsecc),  ! efficiency for each mirror sector (2nd ref)
     +        richeff_msec_gap(nmaxmirsecc)  ! eff by mir sect with gap effect

        !$OMP THREADPRIVATE (/richeffc/)
	real a_beta, b_beta


	common /richbetawidthc/
     +        a_beta,           ! statistical uncertainty for beta
     +        b_beta            ! systematic uncertainty for beta
        !$OMP THREADPRIVATE (/richbetawidthc/)

****************************
*** CODE FROM matrix.inc ***
****************************

      integer irotflg
      real figrotm(3,3)

      common /matrixf/ irotflg
      !$OMP THREADPRIVATE (/matrixf/)

      common /matrixc/ figrotm
      !$OMP THREADPRIVATE (/matrixc/)
C      data irotflg /0/


*******************************
*** CODE FROM liprecpar.inc ***
*******************************

* RICH METHOD FOR VELOCITY AND CHARGE RECONSTRUCTION: PARAMETERS (LIP)

      real
     +     fbackgr, fdistan, f1norm, f1sigma2, f2sigma2,
     +     chi2htcut, nsigcut,
     +     thcstep, phistep,
     +     trhitass2, hitresmax,
     +     chgwin


      common /liprecparc/
                  !-----[LIP RECONSTRUCTION: LIKELIHOOD]
     + FBACKGR,   !photon background fraction
     + FDISTAN,   !effective detector size for background probability
     + F1NORMG,   !signal gaussian normalization
     + F1SIGMA2,  !1st signal gaussian width (squared)
     + F2SIGMA2,  !2nd signal gaussian width (squared)
                  !-----[LIP RECONSTRUCTION: HIT ASSOCIATION]
     + CHI2HTCUT, !chi2 cut for photon pattern hit association
     + NSIGCUT,   !nb of signal sigmas to count fitted hits
                  !-----[LIP RECONSTRUCTION: SCANNING FUNCTION STEPS]
     + PHISTEP,   !phi step for hit chi2 scanning and minimum finding
     + THCSTEP,   !thetac step for scanning and minimum finding
                  !-----[LIP RECONSTRUCTION: HIT ASSOCIATION]
     + TRHITASS2, !Track hit association distance (squared)
     + HITRESMAX, !Residue cut to reject hits used in reconstruction
     + CHGWIN     !Charge window association width (cm)

      !$OMP THREADPRIVATE (/liprecparc/)
*********************************
*** CODE FROM richrec5par.inc ***
*********************************

	real vparin,vparer

	common /richrec5parc/ vparin(5), !initial hint for 5 parameters fit (to be used
     +                                   !in the likelihood funtion fliketot5parxypart.F)
     +                                   !vparin(1)=thetac
     +                                   !vparin(2)=x coord at matrix, correct LG depth
     +                                   !vparin(3)=y coord at matrix, correct LG depth
     +                                   !vparin(4)=x coord at top of radiator
     +                                   !vparin(5)=y coord at top of radiator
     +                                   !
     +	                      vparer(5)  !input errors associated to the 5 parameters fit
                                         !
                                         !vparer(1)=error in thetac error
                                         !vparer(2)=error in x coord at matrix, correct LG depth
                                         !vparer(3)=error in y coord at matrix, correct LG depth
                                         !vparer(4)=error in x coord at top of radiator
                                         !vparer(5)=error in y coord at top of radiator
       !$OMP THREADPRIVATE (/richrec5parc/)
*******************************************
*** NEW CODE: TOF reconstruction result ***
*******************************************

      integer iflag_rectof,nuhits_rectof
      real beta_rectof,thc_rectof,
     +     chi2_rectof,phit_rectof,used_rectof,hres_rectof,
     +     invchi2_rectof,pimp_rectof,epimp_rectof,
     +     pvtx_rectof,epvtx_rectof,
     +     pthe_rectof,epthe_rectof,pphi_rectof,epphi_rectof


      common /rtofdata/ iflag_rectof,            ! flag for TOF reconstruction
                                                 ! (0=bad, 1=good)
     +                  beta_rectof,             ! rec velocity
     +                  thc_rectof,              ! rec Cerenkov angle
     +                  chi2_rectof,             ! chi2 of reconstruction
     +                  nuhits_rectof,           ! number of used hits
     +                  phit_rectof(nhitmax),    ! pointers to used hits
     +                  used_rectof(nhitmax),    ! flags of used hits
     +                  hres_rectof(nhitmax),    ! hit residues (to rec pattern)
     +                  invchi2_rectof,          ! sum of inv chi2 for non-assoc hits
     +                  pimp_rectof(3),          ! impact point at radiator top
     +                  epimp_rectof(3),         ! error in impact point at radiator top
     +                  pvtx_rectof(3),          ! average photon emission point
     +                  epvtx_rectof(3),         ! error in average photon emission point
     +                  pthe_rectof,             ! rec theta
     +                  epthe_rectof,            ! error in rec theta
     +                  pphi_rectof,             ! rec phi
     +                  epphi_rectof             ! error in rec phi

       !$OMP THREADPRIVATE (/rtofdata/)
**************************************************
*** NEW CODE: standalone reconstruction result ***
**************************************************

      integer iflag_recstd,nuhits_recstd
      real beta_recstd,thc_recstd,
     +     chi2_recstd,phit_recstd,used_recstd,hres_recstd,
     +     invchi2_recstd,pimp_recstd,epimp_recstd,
     +     pvtx_recstd,epvtx_recstd,
     +     pthe_recstd,epthe_recstd,pphi_recstd,epphi_recstd


      common /rstddata/ iflag_recstd,            ! flag for standalone reconstruction
                                                 ! (0=bad, 1=good)
     +                  beta_recstd,             ! rec velocity
     +                  thc_recstd,              ! rec Cerenkov angle
     +                  chi2_recstd,             ! chi2 of reconstruction
     +                  nuhits_recstd,           ! number of used hits
     +                  phit_recstd(nhitmax),    ! pointers to used hits
     +                  used_recstd(nhitmax),    ! flags of used hits: 10=assoc to rec (direct); 11=assoc to rec (reflec);
     +                  hres_recstd(nhitmax),    ! hit residues (to rec pattern)
     +                  invchi2_recstd,          ! sum of inv chi2 for non-assoc hits
     +                  pimp_recstd(3),          ! impact point at radiator top
     +                  epimp_recstd(3),         ! error in impact point at radiator top
     +                  pvtx_recstd(3),          ! average photon emission point
     +                  epvtx_recstd(3),         ! error in average photon emission point
     +                  pthe_recstd,             ! rec theta
     +                  epthe_recstd,            ! error in rec theta
     +                  pphi_recstd,             ! rec phi
     +                  epphi_recstd             ! error in rec phi


       !$OMP THREADPRIVATE (/rstddata/)
********************************************
*** NEW CODE: standalone grid parameters ***
********************************************

       integer ngridtop,ngridbot,ngridthc,ntestcand
      real dmtop,steptop,stepbot,fracminthc,herrthc,herrtop,herrbot


      common /stdgridpar/ ngridtop,      ! x/y points at radiator to test
     +                    ngridbot,      ! x/y points at matrix to test
     +                    ngridthc,      ! Cerenkov angles to test
     +                    ntestcand,     ! number of candidates to minimize
     +                    dmtop,         ! max x/y distance btw top and bottom
     +                    steptop,       ! x/y step for points at radiator
     +                    stepbot,       ! x/y step for points at matrix
     +                    fracminthc,    ! min Cerenkov angle as fraction of max
     +                    herrthc,       ! initial perturbation in Cerenkov angle
     +                    herrtop,       ! initial perturbation in radiator x/y coords
     +                    herrbot        ! initial perturbation in matrix x/y coords

       !$OMP THREADPRIVATE (/stdgridpar/)
*****************************************
*** NEW CODE: standalone hint details ***
*****************************************

      integer ncandmax
      PARAMETER(NCANDMAX=2000)
      integer candstatus(ncandmax),canduhits(ncandmax),
     +     candrecuhits(ncandmax)
      real candlike(ncandmax),candx0v(ncandmax),candy0v(ncandmax),
     +     candx0m(ncandmax),candy0m(ncandmax),candthc(ncandmax),
     +     candpkol(ncandmax),candreclike(ncandmax),
     +     candrecx0v(ncandmax),candrecy0v(ncandmax),
     +     candrecx0m(ncandmax),candrecy0m(ncandmax),
     +     candrecthc(ncandmax),candrecbeta(ncandmax),
     +     candrecpkol(ncandmax)
      integer candrkhint(ncandmax),candrkfit(ncandmax)


      common /stdhint/ candstatus,      ! candidate status (0=unused,1=hint,2=hint+fit)
     +                 canduhits,       ! candidate hint used hits
     +                 candrecuhits,    ! candidate fit used hits
     +                 candlike,        ! candidate hint likelihood
     +                 candx0v,         ! candidate hint x at radiator
     +                 candy0v,         ! candidate hint y at radiator
     +                 candx0m,         ! candidate hint x at matrix
     +                 candy0m,         ! candidate hint y at matrix
     +                 candthc,         ! candidate hint Cerenkov angle
     +                 candpkol,        ! candidate hint Kolmogorov probability
     +                 candreclike,     ! candidate fit likelihood
     +                 candrecx0v,      ! candidate fit x at radiator
     +                 candrecy0v,      ! candidate fit y at radiator
     +                 candrecx0m,      ! candidate fit x at matrix
     +                 candrecy0m,      ! candidate fit y at matrix
     +                 candrecthc,      ! candidate fit Cerenkov angle
     +                 candrecbeta,     ! candidate fit velocity
     +                 candrecpkol,     ! candidate fit Kolmogorov probability
     +                 candrkhint,      ! rank of pointers to candidate hints
     +                 candrkfit        ! rank of pointers to candidate fits

       !$OMP THREADPRIVATE (/stdhint/)
**************************************************************
*** NEW CODE: standalone reconstruction details for saving ***
**************************************************************

      real creclike(50)
      real crecx0(50),crecy0(50)
      real crectheta(50),crecphi(50)
      real crecbeta(50)
      real crecuhits(50)
      real crecpkol(50)


      common /rstddet/ creclike,       ! likelihood		
     +                 crecx0,	       ! x impact point	
     +                 crecy0,	       ! y impact point	
     +                 crectheta,      ! rec theta		
     +                 crecphi,	       ! rec phi		
     +                 crecbeta,       ! rec beta		
     +                 crecuhits,      ! number of used hits	
     +                 crecpkol	       ! Kolmogorov probability


      !$OMP THREADPRIVATE (/rstddet/)
