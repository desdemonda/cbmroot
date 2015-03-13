# #!
# run KF Particle Finder

MAINDIR=`pwd`

if [ -z $1 ]
then
  NEVENTS=1000
else
  NEVENTS=$1
fi

if [ -z $2 ]
then
  WORKDIR=$MAINDIR"/data"
else
  WORKDIR=$2
fi
cd $WORKDIR

PID=""
for i in {0..72}
do
  CURDIR="Signal"$i
  cd $CURDIR
  echo `pwd`

  cp -rf $MAINDIR/Signal/* .
  
  bash runphys.sh $NEVENTS >& runphys.log &
  PID=$PID" "$!

  cd ../
done
wait $PID

cd $MAINDIR

echo -e "\007"

