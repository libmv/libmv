#create the blender-libmv patch
svn diff ../../../../source/blender/src/buttons_editing.c \
         ../../../../source/blender/include/butspace.h \
         ../../../../source/blender/makesdna/DNA_libmv_types.h \
         > blender_libmv.diff
