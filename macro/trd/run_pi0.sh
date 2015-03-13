#!/bin/bash
# NEVENTS can be between 1 and 500
NEVENTS=500
# NURQMD can be between 0 and 199
NURQMD=3
NTHREADS=0
MAXTHREADS=$(($NURQMD))
for (( i = 0; i <= $NURQMD; i++ )) do
  NTHREADS=$(($NTHREADS+1))
  echo "nice root -l -b -q run_sim_pi0.C'('${NEVENTS},${i}')' &" > mytemp
  echo "nice root -l -b -q run_sim_pi0.C'('${NEVENTS},${i}')' &"
  . mytemp &> data/logFile.${NEVENTS}.${i}eventsSim.txt
  sleep 5
  if [ "$NTHREADS" -ge "$MAXTHREADS" ]; then
	wait ${!}
	NTHREADS=0
  fi
done
for (( i = 0; i <= $NURQMD; i++ )) do
  NTHREADS=$(($NTHREADS+1))
  echo "nice root -l -b -q run_reco_pi0.C'('${NEVENTS},${i}')' &" > mytemp
  echo "nice root -l -b -q run_reco_pi0.C'('${NEVENTS},${i}')' &"
  . mytemp &> data/logFile.${NEVENTS}.${i}eventsReco.txt
#/dev/null
  sleep 5
  if [ "$NTHREADS" -ge "$MAXTHREADS" ]; then
	wait ${!}
	NTHREADS=0
  fi
done
rm mytemp