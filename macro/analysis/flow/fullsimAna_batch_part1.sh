#$ -wd /tmp 
#$ -j y

#$ -N firstPass

cbmroot_config_path=/hera/cbm/users/sseddiki/cbmroot/trunk_181113/build/config.sh
macro_dir=/hera/cbm/users/sseddiki/cbmroot/trunk_181113/macro

## setup the run environment
source ${cbmroot_config_path}

echo "Start of full event plane simulation"
echo "-----------------------------------------------------------------------"

root -l -b -q "${macro_dir}/run/run_sim.C($1, $2, $3)" || exit 1      
root -l -b -q "${macro_dir}/run/run_reco.C($1, $2, $3)" || exit 2     
root -l -b -q "${macro_dir}/analysis/flow/eventPlane.C($1, $2, $3, 0, 0)" || exit 3

echo "-----------------------------------------------------------------------"
echo "First pass finished successfully"