#!/bin/bash

export RANSEED=`expr $3`
export filenr=`expr $1 - 1`

echo "Start of full sts simulation"
echo "-----------------------------------------------------------------------"
root -l -b -q "sim.C($filenr,$2,0,$RANSEED)" || exit 11
root -l -b -q "rec.C($filenr,$2,0)" || exit 12
root -l -b -q "eff.C($filenr,$2,3122)" || exit 13
#root -l -b -q "sim.C($filenr,$2,3312,$RANSEED)" || exit 14
#root -l -b -q "rec.C($filenr,$2,3312)" || exit 15
#root -l -b -q "eff.C($filenr,$2,3312)" || exit 13
#root -l -b -q "sim.C($filenr,$2,3334,$RANSEED)" || exit 17
#root -l -b -q "rec.C($filenr,$2,3334)" || exit 18
#root -l -b -q "eff.C($filenr,$2,3334)" || exit 19
#root -l -b -q "ana_la.C($filenr,$2)" || exit 20
#root -l -b -q "create_histo_la.C($filenr)" || exit 21
#root -l -b -q "sela.C($filenr,$2,3312)" || exit 22
#root -l -b -q "ana_cascade.C($filenr,$2,3312)" || exit 23
#root -l -b -q "mix_cascade.C($filenr,$2,3312)" || exit 24
#root -l -b -q "create_histo_xi.C($filenr)" || exit 21
#root -l -b -q "sela.C($filenr,$2,3334)" || exit 25
#root -l -b -q "ana_cascade.C($filenr,$2,3334)" || exit 26
#root -l -b -q "mix_cascade.C($filenr,$2,3334)" || exit 27
#root -l -b -q "create_histo_om.C($filenr)" || exit 21
echo "-----------------------------------------------------------------------"
echo "Chain finished successfully"
