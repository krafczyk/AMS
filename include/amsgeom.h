extern "C" void mtx_(geant nrm[][3],geant vect[]);
extern "C" void mtx2_(number nrm[][3],geant  xnrm[][3]);
#define MTX mtx_
#define MTX2 mtx2_
namespace amsgeom{
extern void tkgeom(AMSgvolume &);
extern void tkgeom02(AMSgvolume &);
extern void magnetgeom(AMSgvolume &);
extern void magnetgeom02(AMSgvolume &);
extern void magnetgeom02r(AMSgvolume &);
extern void magnetgeom02Test(AMSgvolume &);
extern void tofgeom(AMSgvolume &);
extern void tofgeom01(AMSgvolume &);
extern void tofgeom02(AMSgvolume &);
extern void antigeom(AMSgvolume &);
extern void antigeom01(AMSgvolume &);
extern void antigeom02(AMSgvolume &);
extern void antigeom002(AMSgvolume &);
#ifdef __G4AMS__
 extern void antigeom02g4(AMSgvolume &);
extern void testg4geom(AMSgvolume &);
#endif
extern void pshgeom(AMSgvolume &);
extern void pshgeom02(AMSgvolume &);
extern void ctcgeom(AMSgvolume &);
extern void ctcgeomE(AMSgvolume &, integer iflag);
extern void ctcgeomAG(AMSgvolume& );
extern void ctcgeomAGPlus(AMSgvolume& );
extern void richgeom02(AMSgvolume &);
extern void ecalgeom02(AMSgvolume &);
extern void trdgeom02(AMSgvolume &);
extern void srdgeom02(AMSgvolume &);
extern void Put_rad(AMSgvolume * , int);
};
