#
##   install additional trd geometries from macro/trd
#

echo \# copy geometries
cp -v geometry/trd.v13/media.geo ../../geometry
cp -v geometry/trd.v13/trd_v13?.geo.root ../../geometry/trd
cp -v geometry/trd.v13/trd_v13?.digi.par ../../parameters/trd
echo
#ln -s ../../parameters/trd/trd_v13b.digi.par ../../parameters/trd/trd_v13c.digi.par 
#ln -s ../../parameters/trd/trd_v13b.digi.par ../../parameters/trd/trd_v13d.digi.par 
#ln -s ../../parameters/trd/trd_v13b.digi.par ../../parameters/trd/trd_v13e.digi.par 
#ln -s ../../parameters/trd/trd_v13b.digi.par ../../parameters/trd/trd_v13f.digi.par 
#ln -s ../../parameters/trd/trd_v13b.digi.par ../../parameters/trd/trd_v13g.digi.par 
#ln -s ../../parameters/trd/trd_v13b.digi.par ../../parameters/trd/trd_v13h.digi.par 
#ln -s ../../parameters/trd/trd_v13b.digi.par ../../parameters/trd/trd_v13i.digi.par 
#
#cp -v geometry/trd.v12/trd_v12[b-d,f].geo ../../geometry/trd
#cp -v geometry/trd.v12/trd_v12[b-d,f].digi.par ../../parameters/trd
#
#cp -v geometry/trd.v12/trd_v12a.geo ../../geometry/trd
#cp -v geometry/trd.v12/media.geo    ../../geometry
#cp -v geometry/trd.v11/v11?/*.geo* ../../geometry/trd
#cp -v geometry/trd.v11/*.geo       ../../geometry/trd
#echo
#echo \# copy digi.par files
#cp -v geometry/trd.v12/trd_v12a_7.5mm_1cm2.digi.par ../../parameters/trd/trd_v12a.digi.par
#cp -v ../../parameters/trd/v11a/trd_v11a_7.5mm_01cm2.digi.par ../../parameters/trd/trd_v11a.digi.par
#cp -v ../../parameters/trd/v11b/trd_v11b_7.5mm_01cm2.digi.par ../../parameters/trd/trd_v11b.digi.par
#cp -v geometry/trd.v11/v11b/trd_v11b_7.5mm_1cm2.digi.par ../../parameters/trd/trd_v11b.digi.par
#cp -v geometry/trd.v11/v11a/trd_v11a_7.5mm_1cm2.digi.par ../../parameters/trd/trd_v11a.digi.par
#cp -v geometry/trd.v11/*.digi.par ../../parameters/trd
