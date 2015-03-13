#
# create TRD geometry from patch file
#
# diff -c Create_TRD_Geometry_v15b_3e.C Create_TRD_Geometry_v15b_1h.C > b3etob1h.patch
#
# cp Create_TRD_Geometry_v13p4.C Create_TRD_Geometry_v13p1.C
# patch Create_TRD_Geometry_v13p1.C p4top1.patch
# root -l Create_TRD_Geometry_v13p1.C
#
SOURCE=Create_TRD_Geometry_v15b_3e.C
TARGET=Create_TRD_Geometry_v15b_1h.C
PATCH=b3etob1h.patch
#
echo 
echo cp $SOURCE $TARGET
echo patch $TARGET $PATCH
echo root -l $TARGET
echo 
#
cp $SOURCE $TARGET
patch $TARGET $PATCH
root -l $TARGET


