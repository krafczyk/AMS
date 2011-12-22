*----------------------------------------------------------
* This common should always be called after richrec_lipf.h
* since it depends on some PARAMETERs defined there
*----------------------------------------------------------

***********************************************************************
* Variables to be interfaced with C++
***********************************************************************

      integer NMAXTOFCLU
      PARAMETER(NMAXTOFCLU=100)

      integer NMAXLIPREC
      PARAMETER(NMAXLIPREC=10)

      integer NMAXMIRSECCC
      PARAMETER(NMAXMIRSECCC=10)

      integer NPMTLIP
      PARAMETER(NPMTLIP=756)

      real ztoprad_ams_c2f,
     +     ztarg_c2f,hrad_c2f,hrnaf_c2f,radtile_pitch_c2f,
     +     radtile_supthk_c2f,radix_c2f,radclarity_c2f,hpgl_c2f,
     +     ztmirgap_c2f,zbmirgap_c2f,reflec_c2f,
     +     zlgsignal_c2f,rtmir_c2f,rbmir_c2f,hmir_c2f,
     +     phimirs_c2f,refmirs_c2f,
     +     pmtwx_c2f,pmtwy_c2f,shieldw_c2f,
     +     pglix_c2f,emcxlim_c2f,emcylim_c2f,lg_top_width_c2f,
     +     lg_bot_width_c2f,lg_pitch_c2f,xbc_c2f,xbd_c2f,yef_c2f,
     +     xpc_c2f,
     +     hitsnpe_sim_ev, hitscoo_ev, hitsnpe_ev,
     +     pimp_sim,pthe_sim,pphi_sim,pmom_sim,pbeta_sim,pchg_sim,
     +     pimp_main,epimp_main,pthe_main,ethe_main,pphi_main,
     +     epphi_main,pmom_main,
     +     pimp_tof,epimp_tof,pthe_tof,ethe_tof,pphi_tof,
     +     epphi_tof,pmom_tof,
     +     edep_tof,edepd_tof,time_tof,
     +     errtime_tof,coo_tof,errcoo_tof,
     +     facthcminscanagl_c2f,facthcmaxscanagl_c2f,
     +     facthcminscannaf_c2f,facthcmaxscannaf_c2f,
     +     thcminscanagl_c2f,thcmaxscanagl_c2f,thcstepscanagl_c2f,
     +     thcminscannaf_c2f,thcmaxscannaf_c2f,thcstepscannaf_c2f,
     +     dvmaxagl_c2f,dvmaxnaf_c2f,
     +     tollinagl_c2f,tollinnaf_c2f

      integer jobc_cp_c2f,
     +        rcgeom_c2f,levgeom_c2f,levgrad_c2f,levacc_c2f,levghit_c2f,
     +        nradts_c2f,
     +        nmirsec_c2f,idmirs_c2f,ipmtstat_c2f,
     +        nbhits_ev, nbhitsmax_ntup_ev, hitspmt_ev, hitstat_ev,
     +        prad_main,iflag_tof,prad_tof,nusedclu_tof,iusedclu_tof,
     +        ntofclu,istatus_tof,ilayer_tof,
     +        ibar_tof,irecnumb,itrknumb,idispflag

      common /lipc2f/ 
                      ! [ JOB DATA ]
     +                  jobc_cp_c2f,               ! job type (0=simulation,1=real data)
                      ! [ DETECTOR DATA ]
     +                  ztoprad_ams_c2f,           ! Z coord at the radiator top in the AMS frame
     +                  rcgeom_c2f(2), 	           ! rich geometry (1)=0,flight
                                                   !               (1)=1,mixed
                                                   !               (1)=9,prototype
     +                  levgeom_c2f, 	           ! detector matrix geometry level
     +                  levgrad_c2f,	           ! radiator geometry level (0=no walls, =1,2 walls)
     +                  levacc_c2f,	           ! acceptance calculation 0/1 = ring line/ring width
     +                  levghit_c2f,	           ! exclusion of ring near track zone 0/1 = off/on
     +                  ztarg_c2f,                 ! Z top coordinate of the radiator in the RICH frame
     +                  nradts_c2f,                ! nb of radiators
     +                  hrad_c2f,	           ! main radiator height (aerogel on flight)
     +                  hrnaf_c2f,	           ! NaF radiator height
     +                  radtile_pitch_c2f,         ! tile pitch
     +                  radtile_supthk_c2f,        ! tile dead space (support)
     +                  radix_c2f(nrmax),          ! refractive index
     +                  radclarity_c2f,	           ! aerogel radiator clarity
     +                  hpgl_c2f,	           ! plexiglass foil thickness
     +                  ztmirgap_c2f,	           ! top mirror gap
     +                  zbmirgap_c2f,	           ! bottom mirror gap
     +                  reflec_c2f,	           ! mirror reflectivity
     +                  zlgsignal_c2f,	           ! efective Z distance below PMT LG
                                                   ! matrix plane for signal optimization
     +                  rtmir_c2f,	           ! top mirror radius
     +                  rbmir_c2f,	           ! bottom mirror radius
     +                  hmir_c2f,	           ! mirror height
     +                  nmirsec_c2f,               ! number of mirror sectors
     +                  idmirs_c2f(nmaxmirseccc),  ! mirror sector IDs
     +                  phimirs_c2f(nmaxmirseccc), ! mirror sector starting phis
     +                  refmirs_c2f(nmaxmirseccc), ! mirror sector reflectivities
     +                  pmtwx_c2f,                 ! PMT (LG in fact) X dimension
     +                  pmtwy_c2f,	           ! PMT (LG in fact) Y dimension
     +                  shieldw_c2f,	           ! dead space between light guides
     +                  ipmtstat_c2f(npmtlip),     ! PMT status
     +                  pglix_c2f,	           ! refractive index of foil
     +                  emcxlim_c2f,	           ! ECAL hole, X coord
     +                  emcylim_c2f,	           ! ECAL hole, Y coord
     +                  lg_top_width_c2f,          ! LG top width
     +                  lg_bot_width_c2f,          ! LG bottom width
     +                  lg_pitch_c2f,	           ! LG pitch
     +                  xbc_c2f,	           ! -
     +                  xbd_c2f,	           !  - PMT position
     +                  yef_c2f, 	           !  -
     +                  xpc_c2f,	           ! -
                      ! [ HIT DATA ]
     +                  nbhits_ev,                 ! number of hits
     +                  nbhitsmax_ntup_ev,         ! max number of hits to store in ntuple 
     +                  hitsnpe_sim_ev(nbhitsmax), ! simul signal (npe)
     +                  hitspmt_ev(nbhitsmax),     ! (pmtnumber+16*pixelnumber)*10 + gain  
     +                  hitscoo_ev(3,nbhitsmax),   ! hit coordinates
     +                  hitsnpe_ev(nbhitsmax),     ! nb photo electrons from adc   
     +                  hitstat_ev(nbhitsmax),     ! hit status
                      ! [ SIMULATION MAIN TRACK PARAMETERS ]
     +                  pimp_sim(3),               ! impact point on radiator entrance
     +                  pthe_sim,                  ! theta
     +                  pphi_sim,                  ! phi
     +                  pmom_sim,                  ! momentum
     +                  pbeta_sim,                 ! velocity
     +                  pchg_sim,                  ! charge
                      ! [ MAIN TRACK PARAMETERS ]
     +                  pimp_main(3),              ! main track: impact point on radiator entrance
     +                  epimp_main(3),             ! main track: error in impact point on radiator entrance
     +                  pthe_main,                 ! main track: theta
     +                  epthe_main,                ! main track: error in theta
     +                  pphi_main,                 ! main track: phi
     +                  epphi_main,                ! main track: error in phi
     +                  pmom_main,                 ! main track: momentum
     +                  prad_main,                 ! main track: radiator (1=aerogel, 2=NaF)
                      ! [ TOF TRACK PARAMETERS ]
     +                  iflag_tof,                 ! TOF track flag (0=failed, 1=OK)
     +                  pimp_tof(3),               ! TOF track: impact point on radiator entrance
     +                  epimp_tof(3),              ! TOF track: error in impact point on radiator entrance
     +                  pthe_tof,                  ! TOF track: theta
     +                  epthe_tof,                 ! TOF track: error in theta
     +                  pphi_tof,                  ! TOF track: phi
     +                  epphi_tof,                 ! TOF track: error in phi
     +                  pmom_tof,                  ! TOF track: momentum
     +                  prad_tof,                  ! TOF track: radiator (1=aerogel, 2=NaF)
     +                  nusedclu_tof,              ! TOF track: number of used clusters
     +                  iusedclu_tof(10),          ! TOF track: index of used clusters (first=1)
                      ! [ TOF CLUSTER DATA ]
     +                  ntofclu,                   ! number of TOF clusters
     +                  istatus_tof(nmaxtofclu),   ! TOF cluster status
     +                  ilayer_tof(nmaxtofclu),    ! TOF cluster layer (1-4)
     +                  ibar_tof(nmaxtofclu),      ! TOF cluster bar (1-14)
     +                  edep_tof(nmaxtofclu),      ! TOF cluster energy loss (MeV), anode
     +                  edepd_tof(nmaxtofclu),     ! TOF cluster energy loss (MeV), dynode
     +                  time_tof(nmaxtofclu),      ! TOF cluster time (sec)
     +                  errtime_tof(nmaxtofclu),   ! TOF cluster time error
     +                  coo_tof(3,nmaxtofclu),     ! TOF cluster coordinates (cm)
     +                  errcoo_tof(3,nmaxtofclu),  ! TOF cluster coordinate errors (cm)
                      ! [ TRACK AND RECONSTRUCTION COUNTERS ]
     +                  irecnumb,                  ! Number of LIP recs performed so far (minus one)
     +                  itrknumb,                  ! Number of tracks used by LIP so far (minus one)
                      ! [ FLAGS FOR LIP CODE ]
     +                  idispflag,                 ! LIP display flag: 0=off, 1=on
                      ! [ PARAMETERS FOR ALTERNATIVE VELOCITY ALGORITHM ]
     +                  facthcminscanagl_c2f,      ! Minimum angle factor for aerogel scan
     +                  facthcmaxscanagl_c2f,      ! Maximum angle factor for aerogel scan
     +                  facthcminscannaf_c2f,      ! Minimum angle factor for NaF scan
     +                  facthcmaxscannaf_c2f,      ! Maximum angle factor for NaF scan
     +                  thcminscanagl_c2f,         ! Minimum angle for aerogel scan
     +                  thcmaxscanagl_c2f,         ! Maximum angle for aerogel scan
     +                  thcstepscanagl_c2f,        ! Step for aerogel scan
     +                  thcminscannaf_c2f,         ! Minimum angle for NaF scan
     +                  thcmaxscannaf_c2f,         ! Maximum angle for NaF scan
     +                  thcstepscannaf_c2f,        ! Step for NaF scan
     +                  dvmaxagl_c2f,              ! Maximum distance derivative for aerogel
     +                  dvmaxnaf_c2f,              ! Maximum distance derivative for NaF
     +                  tollinagl_c2f,             ! Tolerance in linearity for aerogel
     +                  tollinnaf_c2f              ! Tolerance in linearity for NaF

      !$OMP THREADPRIVATE (/lipc2f/) 
* ===============================================================================================

      integer resb_iflag,resb_itype,resb_itrk,resb_nhit,resb_phit,
     +        resb_used,resc_iflag,resc_nmirsec
      real resb_beta,resb_thc,resb_chi2,resb_like,resb_d2like,
     +     resb_hres,resb_invchi2,resb_flatsin,resb_flatcos,
     +     resb_probkl,
     +     resc_cnpe,resc_cnpedir,resc_cnperef,resc_chg,resc_chgdir,
     +     resc_chgmir,resc_accgeom,resc_eff,
     +     resc_accmsec,resc_effmsec,resc_chgprob,
     +     resb_pimp,resb_epimp,resb_pvtx,resb_epvtx,
     +     resb_pthe,resb_epthe,resb_pphi,resb_epphi

      integer rstd_crecuhits
      real rstd_creclike,rstd_crecx0,rstd_crecy0,rstd_crectheta,
     +     rstd_crecphi,rstd_crecbeta,rstd_crecpkol

      common /lipf2c/
                     ! [ BETA RECONSTRUCTION RESULTS ]
     +                 resb_iflag(nmaxliprec),          ! flag for beta reconstruction
                                                        ! (-1=not called, 0=bad, 1=good)
     +                 resb_itype(nmaxliprec),          ! rec type, follows RICH convention
                                                        ! (2=standard, 3=TOF, 4=standalone)
     +                 resb_itrk(nmaxliprec),           ! track used (0-N=from global rec, -1=LIP-TOF, -2=standalone)
     +                 resb_beta(nmaxliprec),           ! rec velocity
     +                 resb_thc(nmaxliprec),            ! rec Cerenkov angle
     +                 resb_chi2(nmaxliprec),           ! chi2 of reconstruction
     +                 resb_like(nmaxliprec),           ! likelihood value
     +                 resb_d2like(nmaxliprec),         ! likelihood 2nd derivative
     +                 resb_nhit(nmaxliprec),           ! number of used hits
     +                 resb_phit(nhitmax,nmaxliprec),   ! pointers to used hits
     +                 resb_used(nhitmax,nmaxliprec),   ! flags of used hits
     +                 resb_hres(nhitmax,nmaxliprec),   ! hit residues (to rec pattern)
     +                 resb_invchi2(nmaxliprec),        ! sum of inv chi2 for non-assoc hits
     +                 resb_flatsin(nmaxliprec),        ! flatness (sin)
     +                 resb_flatcos(nmaxliprec),        ! flatness (cos)
     +                 resb_probkl(nmaxliprec),         ! Kolmogorov probability
                     ! [ CHARGE RECONSTRUCTION RESULTS ]
     +                 resc_iflag(nmaxliprec),          ! flag for charge reconstruction
                                                        ! (-1=not called, 0=bad, 1=good)
     +                 resc_cnpe(nmaxliprec),           ! Npe in ring (total)
     +                 resc_cnpedir(nmaxliprec),        ! Npe in ring (dir branch)
     +                 resc_cnperef(nmaxliprec),        ! Npe in ring (ref branch)
     +                 resc_chg(nmaxliprec),            ! rec charge (total)
     +                 resc_chgdir(nmaxliprec),         ! rec charge (dir branch)
     +                 resc_chgmir(nmaxliprec),         ! rec charge (ref branch)
     +                 resc_accgeom(3,nmaxliprec),      ! geom acceptance
     +                 resc_eff(6,nmaxliprec),          ! efficiency
     +                 resc_nmirsec(nmaxliprec),        ! number of mirror sectors
     +                 resc_accmsec(nmaxmirseccc,2,nmaxliprec), ! geom acceptance by mirror sector (1st,2nd ref)
     +                 resc_effmsec(nmaxmirseccc,2,nmaxliprec), ! efficiency by mirror sector (1st,2nd ref)
     +                 resc_chgprob(3,nmaxliprec),      ! charge probabilities (Z-1,Z,Z+1)
                     ! [ TRACK PARAMETERS ]
     +                 resb_pimp(3,nmaxliprec),         ! impact point at radiator top
     +                 resb_epimp(3,nmaxliprec),        ! error in impact point
     +                 resb_pvtx(3,nmaxliprec),         ! average photon emission point
     +                 resb_epvtx(3,nmaxliprec),        ! error in emission point
     +                 resb_pthe(nmaxliprec),           ! rec theta
     +                 resb_epthe(nmaxliprec),          ! error in rec theta
     +                 resb_pphi(nmaxliprec),           ! rec phi
     +                 resb_epphi(nmaxliprec),          ! error in rec phi
                     ! [ STANDALONE RECONSTRUCTION DETAILS ]
     +                 rstd_creclike(50,nmaxliprec),    ! likelihood
     +                 rstd_crecx0(50,nmaxliprec),      ! x impact point
     +                 rstd_crecy0(50,nmaxliprec),      ! y impact point
     +                 rstd_crectheta(50,nmaxliprec),   ! rec theta
     +                 rstd_crecphi(50,nmaxliprec),     ! rec phi
     +                 rstd_crecbeta(50,nmaxliprec),    ! rec beta
     +                 rstd_crecuhits(50,nmaxliprec),   ! number of used hits
     +                 rstd_crecpkol(50,nmaxliprec)     ! Kolmogorov probability
      !$OMP THREADPRIVATE (/lipf2c/)

***********************************************************************
* OBSOLETE COMMON FOR OUTPUT VARIABLES, kept for debugging
***********************************************************************

      integer nbhits_used
      real thcrec,vbetarec,vebetarec,likep,chi2beta,rprob

      common/lipvar/nbhits_used,  ! number of hits used 
     +              thcrec,       ! reconstructed Ck. angle
     +              vbetarec,     ! reconstructed velocity 
     +              vebetarec,    ! error on reconstructed velocity
     +              likep,        ! likelihood f. probability
     +              chi2beta,     ! chi2 
     +              rprob         ! kolmogorov test prob.
      !$OMP THREADPRIVATE (/lipvar/)
