export SCRIPT=yes

draw_litqa() 
{
	export LMVM_LITQA_FILE_NAME=${2}
	export LMVM_MAIN_DIR=${1}/${3}
	root -b -l -q draw_litqa.C
}

draw_ana()
{
	export LMVM_ANA_FILE_NAME=${2}
	export LMVM_MAIN_DIR=${1}/${3}
	root -b -l -q draw_analysis.C
}

draw_ana_all()
{
	export LMVM_ANA_FILE_NAME=${2}
	export LMVM_MAIN_DIR=${1}
	root -b -l -q draw_analysis_all.C
}

draw_all()
{
	main_dir=${1}
	ana_file=analysis${2}
	litqa_file=litqa${2}
	echo "main_dir:" ${main_dir}
	echo "ana_file:" ${ana_file}
	echo "litqa_file:" ${litqa_file}
	
	echo "Draw LITQA"
	draw_litqa ${main_dir} ${litqa_file} rho0/
	#draw_litqa ${main_dir} ${litqa_file} omegaepem/
	#draw_litqa ${main_dir} ${litqa_file} phi/
	#draw_litqa ${main_dir} ${litqa_file} omegadalitz/
	
	echo "Draw ANALYSIS"
	draw_ana ${main_dir} ${ana_file} rho0/
	draw_ana ${main_dir} ${ana_file} omegaepem/
	draw_ana ${main_dir} ${ana_file} phi/
	draw_ana ${main_dir} ${ana_file} omegadalitz/
	
	echo "Draw ANALYSIS ALL"
	draw_ana_all ${main_dir} ${ana_file}
}

#draw_all /Users/slebedev/Development/cbm/data/lmvm/apr15/rich_pmts/25gev/stsv13d/richv08a/trd10/tofv13/1.0field/nomvd/ .auau.25gev.centr.all.root
#draw_all /Users/slebedev/Development/cbm/data/lmvm/apr15/rich_pmts/25gev/stsv13d/richv08a_01/trd10/tofv13/1.0field/nomvd/ .auau.25gev.centr.all.root
#draw_all /Users/slebedev/Development/cbm/data/lmvm/apr15/rich_pmts/25gev/stsv13d/richv08a_02/trd10/tofv13/1.0field/nomvd/ .auau.25gev.centr.all.root
#draw_all /Users/slebedev/Development/cbm/data/lmvm/apr15/rich_pmts/25gev/stsv13d/richv08a_03/trd10/tofv13/1.0field/nomvd/ .auau.25gev.centr.all.root
#draw_all /Users/slebedev/Development/cbm/data/lmvm/apr15/rich_pmts/25gev/stsv13d/richv08a_04/trd10/tofv13/1.0field/nomvd/ .auau.25gev.centr.all.root
#draw_all /Users/slebedev/Development/cbm/data/lmvm/apr15/rich_pmts/25gev/stsv13d/richv08a_05/trd10/tofv13/1.0field/nomvd/ .auau.25gev.centr.all.root
#draw_all /Users/slebedev/Development/cbm/data/lmvm/apr15/rich_pmts/25gev/stsv13d/richv08a_06/trd10/tofv13/1.0field/nomvd/ .auau.25gev.centr.all.root
#draw_all /Users/slebedev/Development/cbm/data/lmvm/apr15/rich_pmts/25gev/stsv13d/richv08a_07/trd10/tofv13/1.0field/nomvd/ .auau.25gev.centr.all.root

draw_all /Users/slebedev/Development/cbm/data/lmvm/mar15_rtcut/8gev/stsv13d/richv14a_1e/notrd/tofv13/1.0field/nomvd/ .auau.8gev.centr.all.root
draw_all /Users/slebedev/Development/cbm/data/lmvm/mar15_trdclustering/8gev/stsv13d/richv14a_1e/trd4/tofv13/1.0field/nomvd/ .trdclustering.auau.8gev.centr.all.root 

#draw_all /Users/slebedev/Development/cbm/data/lmvm/mar15/8gev/stsv13d/richv14a_1e/trd4/tofv13/1.0field/nomvd/ .auau.8gev.centr.all.root
#draw_all /Users/slebedev/Development/cbm/data/lmvm/mar15/25gev/stsv13d/richv14a_3e/trd10/tofv13/1.0field/nomvd/ .auau.25gev.centr.all.root 

#draw_all /Users/slebedev/Development/cbm/data/lmvm/mar15/3.5gev/stsv13d/richv14a_1e/trd4/tofv13/0.5field/nomvd/ .auau.3.5gev.centr.all.root
#draw_all /Users/slebedev/Development/cbm/data/lmvm/mar15/3.5gev/stsv13d/richv14a_1e/trd4/tofv13/0.7field/nomvd/ .auau.3.5gev.centr.all.root
#draw_all /Users/slebedev/Development/cbm/data/lmvm/mar15/3.5gev/stsv13d/richv14a_1e/trd4/tofv13/1.0field/nomvd/ .auau.3.5gev.centr.all.root
#draw_all /Users/slebedev/Development/cbm/data/lmvm/mar15/3.5gev/stsv13d/richv14a_1e/notrd/tofv13/0.5field/nomvd/ .auau.3.5gev.centr.all.root
#draw_all /Users/slebedev/Development/cbm/data/lmvm/mar15/3.5gev/stsv13d/richv14a_1e/notrd/tofv13/0.7field/nomvd/ .auau.3.5gev.centr.all.root
#draw_all /Users/slebedev/Development/cbm/data/lmvm/mar15/3.5gev/stsv13d/richv14a_1e/notrd/tofv13/1.0field/nomvd/ .auau.3.5gev.centr.all.root

export SCRIPT=no