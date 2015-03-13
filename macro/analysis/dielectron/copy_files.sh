#####!/bin/sh

mainDirFrom=/hera/cbm/users/slebedev/mc/dielectron/feb15/
mainDirTo=/u/slebedev/Baykal/copytemp/feb15_new2/

copy_func() {
   dirFrom=${1}
   dirTo=${2}
   filename=${3}
   meson=${4}
   mkdir -p ${dirTo}/${meson}/
   cp ${dirFrom}/${meson}/${filename} ${dirTo}/${meson}/
}

copy_all_mesons() {
  copy_func ${1} ${2} ${3} rho0
  copy_func ${1} ${2} ${3} omegaepem
  copy_func ${1} ${2} ${3} omegadalitz
  copy_func ${1} ${2} ${3} phi 
  #copy_func ${1} ${2} ${3} urqmd
}

copy_all_mesons_ana_litqa() {
	subPath=${1}
	fileNameAna=analysis${2}
	copy_all_mesons ${mainDirFrom}/${subPath} ${mainDirTo}/${subPath} ${fileNameAna}
	
	fileNameLitqa=litqa${2}
	copy_all_mesons ${mainDirFrom}/${subPath} ${mainDirTo}/${subPath} ${fileNameLitqa}
}

#copy_all_mesons_ana_litqa /8gev/stsv13d/richv14a/trd4/tofv13/1.0field/nomvd/ .auau.8gev.centr.all.root
#copy_all_mesons_ana_litqa /25gev/stsv13d/richv14a/trd10/tofv13/1.0field/nomvd/ .auau.25gev.centr.all.root 
#copy_all_mesons_ana_litqa /8gev/stsv13d/richv14a_bepipe/trd4/tofv13/1.0field/nomvd/ .auau.8gev.centr.all.root
#copy_all_mesons_ana_litqa /25gev/stsv13d/richv14a_bepipe/trd10/tofv13/1.0field/nomvd/ .auau.25gev.centr.all.root
#copy_all_mesons_ana_litqa /8gev/stsv13d/richv14a/trd4/tofv13/0.7field/nomvd/ .auau.8gev.centr.all.root
#copy_all_mesons_ana_litqa /8gev/stsv14_2cm/richv14a/trd4/tofv13/1.0field/nomvd/ .auau.8gev.centr.all.root
#copy_all_mesons_ana_litqa /25gev/stsv14_2cm/richv14a/trd10/tofv13/1.0field/nomvd/ .auau.25gev.centr.all.root
#copy_all_mesons_ana_litqa /8gev/stsv14_4cm/richv14a/trd4/tofv13/1.0field/nomvd/ .auau.8gev.centr.all.root
#copy_all_mesons_ana_litqa /25gev/stsv14_4cm/richv14a/trd10/tofv13/1.0field/nomvd/ .auau.25gev.centr.all.root

copy_all_mesons_ana_litqa /8gev/stsv13d/richv14a_1e/trd4/tofv13/1.0field/nomvd/ .auau.8gev.centr.all.root
copy_all_mesons_ana_litqa /25gev/stsv13d/richv14a_3e/trd10/tofv13/1.0field/nomvd/ .auau.25gev.centr.all.root

copy_all_mesons_ana_litqa /3.5gev/stsv13d/richv14a_1e/trd4/tofv13/1.0field/nomvd/ .auau.3.5gev.centr.all.root
copy_all_mesons_ana_litqa /3.5gev/stsv13d/richv14a_1e/trd4/tofv13/0.7field/nomvd/ .auau.3.5gev.centr.all.root
copy_all_mesons_ana_litqa /3.5gev/stsv13d/richv14a_1e/trd4/tofv13/0.5field/nomvd/ .auau.3.5gev.centr.all.root
copy_all_mesons_ana_litqa /3.5gev/stsv13d/richv14a_1e/notrd/tofv13/1.0field/nomvd/ .auau.3.5gev.centr.all.root
copy_all_mesons_ana_litqa /3.5gev/stsv13d/richv14a_1e/notrd/tofv13/0.7field/nomvd/ .auau.3.5gev.centr.all.root
copy_all_mesons_ana_litqa /3.5gev/stsv13d/richv14a_1e/notrd/tofv13/0.5field/nomvd/ .auau.3.5gev.centr.all.root



