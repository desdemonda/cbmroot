#!/bin/bash
#
##   semi-automated script installing FairSoft, FairRoot and CbmRoot
#

# 13.03.2015
# by David Emschermann

#-------------------------------------

# put your desired variants here:
export FSOFTVER=dec14
export FROOTVER=v-14.11
export NUMOFCPU=12
export CBMSRCDIR=`pwd`

#-------------------------------------
# usage:
# svn co https://subversion.gsi.de/cbmsoft/cbmroot/trunk $CBMSRCDIR
# cd $CBMSRCDIR
# ./autoinstall_framework.sh


#
##   FairSoft
#

cd ..
git clone https://github.com/FairRootGroup/FairSoft fairsoft_$FSOFTVER
cd fairsoft_$FSOFTVER/
git tag -l
git checkout -b $FSOFTVER $FSOFTVER
#emacs -nw automatic.conf
#./configure.sh automatic.conf
sed s/compiler=/compiler=gcc/ automatic.conf > automatic_gcc.conf
./configure.sh automatic_gcc.conf

echo done installing FairSoft


#
##   FairRoot
#

cd ..
echo "SIMPATH	 : $SIMPATH"
cd fairsoft_$FSOFTVER/installation/
export SIMPATH=`pwd`
echo "SIMPATH	 : $SIMPATH"
cd ../..

git clone https://github.com/FairRootGroup/FairRoot.git fairroot_src
cd fairroot_src
git tag -l
git checkout -b $FROOTVER $FROOTVER
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=../../fairroot_$FROOTVER-fairsoft_$FSOFTVER
nice make install -j$NUMOFCPU
#nice make install -j4

cd ../..
cd fairroot_$FROOTVER-fairsoft_$FSOFTVER
export FAIRROOTPATH=`pwd`
echo "FAIRROOTPATH: $FAIRROOTPATH"
cd ..

echo done installing FairRoot


#
##   CbmRoot
#

echo "SIMPATH	 : $SIMPATH"
echo "FAIRROOTPATH: $FAIRROOTPATH"

svn co https://subversion.gsi.de/cbmsoft/cbmroot/fieldmaps fieldmaps

#svn co https://subversion.gsi.de/cbmsoft/cbmroot/trunk $CBMSRCDIR
cd $CBMSRCDIR
mkdir build
cd build
cmake ..
nice make -j$NUMOFCPU
cd ..

cd input
ln -s ../../fieldmaps/* .
cd ..

. build/config.sh

echo done installing CbmRoot


#####################################################################################
#####################################################################################
