#!/bin/tcsh

# Prepare an slc6 computer to build the AMS components,
# currently for 64-bit compilation, except amsprodserver

setenv CXXVERS `g++ -dumpversion | tr -d "."`
if ( "$CXXVERS" < "440" || "$CXXVERS" >= "450") then
  echo "We are not on SLC6 computer"
  exit
endif

ln -s /afs/cern.ch/ams/Offline /Offline
ln -s /afs/cern.ch/asis /asis
ln -s /afs/cern.ch/asis/i386_redhat72/cern /cern
ln -s /afs/cern.ch/ams/Offline/intel /opt/intel

wget ftp://fr2.rpmfind.net/linux/centos/6.3/os/i386/Packages/glib2-static-2.22.5-7.el6.i686.rpm
yum install -y glib2-static-2.22.5-7.el6.i686.rpm
rm -f glib2-static-2.22.5-7.el6.i686.rpm

yum install -y glib2-devel-2.22.5-7.el6.x86_64
yum install -y glib2-devel-2.22.5-7.el6.i686
yum install -y glib2-static-2.22.5-7.el6.x86_64
yum install -y glibc-devel-2.12-1.80.el6_3.3.i686
yum install -y glibc-static-2.12-1.80.el6_3.3.x86_64
yum install -y glibc-static-2.12-1.80.el6_3.3.i686
yum install -y ORBit2-2.14.17-3.1.el6.i686 
yum install -y ORBit2-devel-2.14.17-3.1.el6.i686
yum install -y libstdc++-devel-4.4.6-4.el6.x86_64
yum install -y libstdc++-devel-4.4.6-4.el6.i686
yum install -y compat-libstdc++-33-3.2.3-69.el6.x86_64
yum install -y compat-libstdc++-33-3.2.3-69.el6.i686

yum install -y libXpm-devel-3.5.8-2.el6.x86_64
yum install -y fontconfig-devel-2.8.0-3.el6.x86_64
yum install -y libjpeg-devel-6b-46.el6.x86_64
yum install -y libjpeg-static-6b-46.el6.x86_64
yum install -y libpng-devel-2:1.2.49-1.el6_2.x86_64
yum install -y libpng-static-1.2.49-1.el6_2.x86_64
yum install -y libtiff-3.9.4-6.el6_3.x86_64
yum install -y libtiff-static-3.9.4-6.el6_3.x86_64
yum install -y openssl-devel-1.0.0-20.el6_2.5.x86_64
yum install -y openssl-static-1.0.0-20.el6_2.5.x86_64
yum install -y expat-devel-2.0.1-11.el6_2.x86_64
yum install -y ncurses-static-5.7-3.20090208.el6.x86_64
 
