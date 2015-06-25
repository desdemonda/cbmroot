# #!
# run simulation and tracks reconstruction

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

rm -rf *

PID=""
for i in {0..74}
do
  CURDIR="Signal"$i
  mkdir $CURDIR
  cd $CURDIR
  echo `pwd`

  cp -rf $MAINDIR/Signal/* .
  
  bash runsim.sh $i $NEVENTS >& runsim.log &
  PID=$PID" "$!
  
  sleep 0.1
  cd ../
done
wait $PID

cd $MAINDIR

echo -e "\007"

