#!/bin/sh

MAINDIR=`pwd`
rm -rf CbmKFParticleFinderQA.root

if [ -z $1 ]
then
  WORKDIR=$MAINDIR"/data"
else
  WORKDIR=$1
fi

START=0

NDIRS=74
for (( i=$START; i<=$NDIRS; i++ ))
do
  TMPHISTOSNAMES[i]=""
done

ONE=1
NTMPFILES=0
NDIRSINTMPFILE=0
MAXDIRSINTMPFILE=7
for (( i=$START; i<=$NDIRS; i++ ))
do
  TMPHISTOSNAMES[NTMPFILES]=${TMPHISTOSNAMES[NTMPFILES]}" "$WORKDIR"/Signal"$i"/CbmKFParticleFinderQA.root"
  
  NDIRSINTMPFILE=`expr $NDIRSINTMPFILE + $ONE`
  if [ $NDIRSINTMPFILE -eq $MAXDIRSINTMPFILE ]
  then
    NTMPFILES=`expr $NTMPFILES + $ONE`
    NDIRSINTMPFILE=0
  fi
done

PID=""
for (( i=$START; i<=$NTMPFILES; i++ ))
do
  TMPFILENAME[i]="CbmKFParticleFinderQATmp"$i".root"
  hadd ${TMPFILENAME[i]} ${TMPHISTOSNAMES[i]} &> collog$i.log &
  PID=$PID" "$!
done
wait $PID

HISTOSNAMES=""
for (( i=$START; i<=$NTMPFILES; i++ ))
do
  HISTOSNAMES=$HISTOSNAMES" "${TMPFILENAME[i]}
done

echo $HISTOSNAMES
hadd CbmKFParticleFinderQA.root $HISTOSNAMES &> collog.log &
PID=$!
wait $PID

rm -rf CbmKFParticleFinderQATmp*
rm -rf collog*
