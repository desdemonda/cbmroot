#
# create TRD geometry from patch file
#
SOURCE=Create_TRD_Geometry_v13p_3e.C
TARGET=Create_TRD_Geometry_v13p_1n.C
PATCH=p3etop1n.patch
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

