#create the blender-libmv patch
svn diff ../../../../source/blender/src/buttons_editing.c \
         ../../../../source/blender/include/butspace.h \
         ../../../../source/blender/makesdna/DNA_libmv_types.h \
         ../../../../source/blender/makesdna/DNA_userdef_types.h \
         ../../../../source/blender/makesdna/intern/makesdna.c \
         > blender_libmv.diff
