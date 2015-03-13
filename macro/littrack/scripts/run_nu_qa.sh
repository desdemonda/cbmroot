

export MAIN_DIR=/lustre/cbm/user/andrey/events/nu/25gev/

for I in 0 1; do
  for J in 0 1 2 3 4 5 6 7 8 9; do
    for K in 0 1 2 3 4 5 6 7 8 9 ; do
      export XXXX=0${I}${J}${K}
      bsub -q batch -J mc.${XXXX}.run -o ${MAIN_DIR}/log/${XXXX}.log -N sh ./nu_qa.sh 
   done
  done
done

#export MAIN_DIR=/Users/andrey/Development/cbm/d/events/nu/25gev/
#export XXXX=0000
#xterm -hold -e ". ./nu_qa.sh"&