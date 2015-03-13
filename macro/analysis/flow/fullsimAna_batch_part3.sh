#$ -wd /tmp 
#$ -j y

#$ -N thirdPass
#$ -hold_jid merge2

cbmroot_config_path=/hera/cbm/users/sseddiki/cbmroot/trunk_181113/build/config.sh
macro_dir=/hera/cbm/users/sseddiki/cbmroot/trunk_181113/macro

# setup the run environment
source ${cbmroot_config_path}

echo "Third pass of event plane simulation"
echo "-----------------------------------------------------------------------"

## flow ana

root -l -b -q "${macro_dir}/analysis/flow/anaFlow.C(0, $1, $2, $3, 1, 0, 20., 30.)" || exit 5
root -l -b -q "${macro_dir}/analysis/flow/anaFlow.C(1, $1, $2, $3, 1, 0, 20., 30.)" || exit 6

root -l -b -q "${macro_dir}/analysis/flow/anaFlow.C(5, $1, $2, $3, 1, 0, 20., 30.)" || exit 7
root -l -b -q "${macro_dir}/analysis/flow/anaFlow.C(5, $1, $2, $3, 1, 1, 20., 30.)" || exit 8
root -l -b -q "${macro_dir}/analysis/flow/anaFlow.C(5, $1, $2, $3, 2, 0, 20., 30.)" || exit 9
root -l -b -q "${macro_dir}/analysis/flow/anaFlow.C(5, $1, $2, $3, 2, 1, 20., 30.)" || exit 10

echo "-----------------------------------------------------------------------"
echo "Third pass finished successfully"