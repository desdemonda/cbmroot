#
# create TRD geometry from patch file
#
SOURCE=Create_TRD_Geometry_v14b300.C
TARGET=Create_TRD_Geometry_v14b100.C
PATCH=b3tob1.patch
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


