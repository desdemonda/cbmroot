cd /hera/cbm/users/sseddiki/cbmroot/trunk_181113/macro/analysis/flow

# --------------------------
# Particle transport + reconstruction + event plane calculation (event-wise parallellization)
for ((i=0; $i<=$1; i++));do
qsub -l mem=3G,ct=00:10:00 -e /hera/cbm/users/sseddiki/batch_out/ ./fullsimAna_batch_part1.sh 10 200 $i
done

# --------------------------
# Merging of produced files containing (non-corrected) event planes -> to estimate correction factors for event plane flattening method
qsub -l mem=3G,ct=00:10:00 -e /hera/cbm/users/sseddiki/batch_out/ ./MergeTTree_part1.sh 0 10 0 0

# --------------------------
# Event plane calculation, including the flattening method, e.g. Q-recentering (event-wise parallellization)
for ((i=0; $i<=$1; i++));do
qsub -l mem=3G,ct=00:10:00 -e /hera/cbm/users/sseddiki/batch_out/ ./fullsimAna_batch_part2.sh 10 200 $i
done

# --------------------------
# Merging of produced files containing corrected event planes
qsub -l mem=3G,ct=00:10:00 -e /hera/cbm/users/sseddiki/batch_out/ ./MergeTTree_part2.sh 0 10 1 0

# --------------------------
# Particle flow calculation (event-wise parallellization)
for ((i=0; $i<=$1; i++));do
qsub -l mem=5G,ct=00:10:00 -e /hera/cbm/users/sseddiki/batch_out/ ./fullsimAna_batch_part3.sh 10 200 $i
done