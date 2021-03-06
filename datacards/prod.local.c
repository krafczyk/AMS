//  $Id: prod.local.c,v 1.2 2001/01/22 17:32:26 choutko Exp $
[ -z "$UNAME" ]      && UNAME=`(uname) 2>/dev/null`
[ -z "$UNAME" -a -d /sys/node_data ] && UNAME="DomainOS"
[ -z "$UNAME" ]      && (echo could not determine hosttype ; exit)

if [ "$UNAME" = "AIX" ] ; then
   MACHINE="aix"
elif [ "$UNAME" = "OSF1" ] ; then
    MACHINE="osf1"
elif [ "$UNAME" = "HP-UX" ] ; then
    MACHINE="hpux"
elif [ "$UNAME" = "IRIX" ] ; then
    MACHINE="irix"
elif [ "$UNAME" = "Linux" ] ; then
    MACHINE="linux"
else
echo $UNAME is not supported yet

fi
RUN=$1
EVENT=$2
cp $Offline/vdev/exe/$MACHINE/gbatch.exe $Offline/logs.local/gbatch.exe.c
$Offline/logs.local/gbatch.exe.c <<!
LIST
KINE 14 
MULS 1
C MSEL 1=0
LOSS 1 
CUTS 1=0.0005 2=0.0005 3=0.001 4=0.001 5=0.001
HADR 1
TRIG 100000000
DEBUG -1 10 1000
C    tmaxf  smax   fieldm  cconv   maxcal  nitm    xtol iver ialg   steps 
TKFIT 0.2    50.   2.     5.e-4     8  1000     1.e-2   0     1    0.03 
C npars ims 
  5     1   
C          Nparticles 
IOPA 1=3 2='$Offline/amsdatantuple.local/' 43=5012 
45='AMSParticle' 126=100000 44=1.1
C RNDM 197683093 1356123257
RNDM  1310381404  1978925784
C      Simul Debug    CpuLim  Read Write Jobname
AMSJOB 1=10011   2=0      3=6.   4=1  5=0 6='v203a'  87=1
88='EventStatusTable'
C Special HP-UX
TIME 3=10000000
DAQC 1=1 2='$Offline/RunsDir.local/runs.c/'
TRCALIB 9=2 13=1
TERM 1=1234567890
L3REC  11=1 12=2 
SELECT $RUN $EVENT
ESTA  10=1111
C LVL1 3=1
END


!

