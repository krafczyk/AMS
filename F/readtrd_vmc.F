#         include "trdf.h"
         OPEN (21,FILE='~/DESY1.DAT',
     +          FORM='UNFORMATTED',STATUS='OLD')
         READ (21) AC,ZC,ROC,NWE,WE,ANL,GAME
         CLOSE(21)
         OPEN (21,FILE='~/DESY2.DAT',
     +   FORM='UNFORMATTED',STATUS='OLD')
         READ (21)TDATA,WT,AMUTT,FOIL,GAS,GINWM,DNM,WTR
         CLOSE(21)
         OPEN (21,FILE='~/DESY3.DAT',
     +           FORM='UNFORMATTED',STATUS='OLD')
         READ (21) YTSYN,FTSYN
         CLOSE(21)
         open(21,file='~/trd.aachen.dat',form='formatted')
         WRITE (21,*) AC,ZC,ROC,NWE,WE,ANL,GAME
         WRITE (21,*)TDATA,WT,AMUTT,FOIL,GAS,GINWM,DNM,WTR
         WRITE (21,*) YTSYN,FTSYN
         close(21)
         end
