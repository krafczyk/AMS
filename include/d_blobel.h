//================================ d_blobel ===================================
//
// Interfaces for blobel routines in double precision !!!!!!!!
//
#define DSIMDEB(A1) CCALLSFSUB1(DSIMDEB,dsimdeb,INT,A1);
#define DSIMDIM(A1,A2) CCALLSFSUB2(DSIMDIM,dsimdim,INT,INT,A1,A2);
#define DERRPRP(A1,A2,A3,A4,A5) CCALLSFSUB5(DERRPRP,derrprp,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,PINT,A1,A2,A3,A4,A5);
#define DAPLCON(A1,A2,A3,A4,A5) CCALLSFSUB5(DAPLCON,daplcon,DOUBLEV,DOUBLEV,DOUBLEV,PINT,DOUBLE,A1,A2,A3,A4,A5);

#define DSMTOS(A1,A2,A3,A4,A5) CCALLSFSUB5(DSMTOS,dsmtos,DOUBLEV,INT,DOUBLEV,INT,INT,A1,A2,A3,A4,A5);

#define DVALLIN(A1,A2,A3) CCALLSFSUB3(DVALLIN,dvallin,INT,DOUBLEV,INT,A1,A2,A3);
#define DVALLEY(A1,A2,A3) CCALLSFSUB3(DVALLEY,dvalley,DOUBLE,DOUBLEV,PINT,A1,A2,A3);

//=============================================================================

