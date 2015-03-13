#
# create TRD geometry from patch file
#
# cp Create_TRD_Geometry_v13p4.C Create_TRD_Geometry_v13p1.C
# patch Create_TRD_Geometry_v13p1.C p4top1.patch
# root -l Create_TRD_Geometry_v13p1.C
#
SOURCE=Create_TRD_Geometry_v13p_3e.C
TARGET=Create_TRD_Geometry_v13p_1e.C
PATCH=p3etop1e.patch
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


