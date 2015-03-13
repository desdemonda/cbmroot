#!/bin/bash
#
# create for a list of geometries in root format
# the corresponding trd digitization parameters
#

# as defined in create_geometry_file_root.C,
# trd_v13q.root file is expected in:
directory=geometry
#directory=geometry/trd.v13

for file in $directory/*.geo.root; do

 echo "Creating trd parameters for $file"
 fileName=$(basename $file)
# echo "fileName : $fileName"
 fileNameNoExt=${fileName%.*.*}
# echo "fileNoExt: $fileNameNoExt"

# prepare geometry file
 root -l -b -q create_geometry_file_root.C\(\"$fileNameNoExt\"\)
# extract digi parameters
 root -l -b -q create_digipar_root.C\(\"$fileNameNoExt\"\)

# compile, if needed
 if [ ! -x ./cut_digipar_lf ] ; then 
  echo "compiling cut_digipar_lf.c"
  echo g++ -o cut_digipar_lf cut_digipar_lf.c
  g++ -o cut_digipar_lf cut_digipar_lf.c
  echo done
 fi

# cut trailing 0x0a
 if [ -x ./cut_digipar_lf ] ; then 
  echo ""
  echo ">> Please remember to delete the last line in all the created parameter files. <<"
  echo "./cut_digipar_lf $fileNameNoExt.digi.par.long $fileNameNoExt.digi.par"
  echo ""
  ./cut_digipar_lf $fileNameNoExt.digi.par.long $fileNameNoExt.digi.par
  cp $fileNameNoExt.digi.par $directory
 fi

done

#[ -x digipar_cut_lf ] echo digipar_cut_lf digipar_cut_lf.cut
# # fileName=$(basename $file)
# # fileNameNoExt=${fileName%.*}
# # echo mv $fileNameNoExt.digi.par $directory
# # echo mv geofile_$fileNameNoExt.root $directory

# echo ""
# echo ">> Please remember to delete the last line in all the created parameter files. <<"
# echo "cut_digipar_lf file.digi.par.long file.digi.par"
# echo ""
