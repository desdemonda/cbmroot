#$ -wd /tmp 
#$ -j y

#$ -N secondPass
#$ -hold_jid merge1

cbmroot_config_path=/hera/cbm/users/sseddiki/cbmroot/trunk_181113/build/config.sh
macro_dir=/hera/cbm/users/sseddiki/cbmroot/trunk_181113/macro

# setup the run environment
source ${cbmroot_config_path}

echo "Second pass of event plane simulation"
echo "-----------------------------------------------------------------------"

root -l -b -q "${macro_dir}/analysis/flow/eventPlane.C($1, $2, $3, 1, 0)" || exit 4

echo "-----------------------------------------------------------------------"
echo "Second pass finished successfully"