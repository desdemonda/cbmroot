#$ -wd /tmp 
#$ -j y
# Number of subtasks to start
#$ -t 1-51

X=$((${SGE_TASK_ID} - 1))
XXX=$(printf "%03d" "$X")

#energy=15gev

#indir=/hera/cbm/users/vassilie/mc/nov12/tof_10m_new_L1/urqmd/auau/${energy}/centr
#outdir=/hera/cbm/users/vassilie/mc/nov12/tof_10m_new_L1/urqmd/auau/${energy}/centr
#indir=/hera/cbm/users/vassilie/sts/nov12/urqmd/auau/${energy}/centr
#outdir=/hera/cbm/users/vassilie/sts/nov12/urqmd/auau/${energy}/centr
#indir=/hera/cbm/users/vassilie/mc/sts_v12b_rich_tof/urqmd/auau/${energy}/${centrality}
#outdir=/hera/cbm/users/vassilie/mc/sts_v12b_rich_tof/urqmd/auau/${energy}/${centrality}
indir=/hera/cbm/users/vassilie/mc/sts_v13c_rich_tof/urqmd/auau/${energy}/${centrality}
outdir=/hera/cbm/users/vassilie/mc/sts_v13c_rich_tof/urqmd/auau/${energy}/${centrality}


#source /cvmfs/fairroot.gsi.de/cbm/cbmroot/release/nov12/build/config.sh
source /hera/cbm/users/andrey/cbmroot/trunk/build/config.sh

#for i in {0..50..1}
#do
 # XXX=$(printf "%03d" "$i")
  outfile=${outdir}/analysis.auau.${energy}.${centrality}.temp-all.${XXX}.root
  infiles=${indir}/analysis.auau.${energy}.${centrality}.${XXX}*.root
#  outfile=${outdir}/qa.auau.${energy}.${centrality}.temp-all.${XXX}.root
#  infiles=${indir}/qa.auau.${energy}.${centrality}.${XXX}*.root
  echo "Adding histograms ${XXX}: input=${infiles}, output=${outfile}"
  hadd  -T -f -k ${outfile} ${infiles}
#done

#outfile=${dir}/analysis.auau.8gev.centr.all.root
#infiles=${dir}/analysis.auau.8gev.centr.temp-all.*.root
#hadd  -T -f -k -v 0 ${outfile} ${infiles}

#rm -rf ${infiles} 

cp -v ${SGE_STDOUT_PATH} ${outdir}/log/${JOB_ID}.${SGE_TASK_ID}.log
  