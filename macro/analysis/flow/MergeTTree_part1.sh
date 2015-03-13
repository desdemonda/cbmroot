#$ -wd /tmp 
#$ -j y

#$ -N merge1
#$ -hold_jid firstPass

cbmroot_config_path=/hera/cbm/users/sseddiki/cbmroot/trunk_181113/build/config.sh
macro_dir=/hera/cbm/users/sseddiki/cbmroot/trunk_181113/macro

# setup the run environment
source ${cbmroot_config_path}

echo "Merging of event plane output files "
echo "-----------------------------------------------------------------------"

root -l -b -q "${macro_dir}/analysis/flow/MergeTTree.C($1, $2, $3, $4)" || exit 11

echo "-----------------------------------------------------------------------"
echo "Merging finished successfully"