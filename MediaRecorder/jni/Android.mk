
LOCAL_PATH := $(call my-dir)

   include $(CLEAR_VARS)

   LOCAL_MODULE    := millet-ndk

   LOCAL_CPPFLAGS += -fexceptions

   LOCAL_C_INCLUDES := $(LOCAL_PATH)/mcdull/

   LOCAL_SRC_FILES := millet-ndk.c millet_api.c \
       mcdull/millet.c \
       mcdull/dull_analyse.c mcdull/dull_analyse_p.c mcdull/dull_analyse_p_best.c \
       mcdull/dull_analyse_p_fast.c mcdull/dull_analyse_p_good.c mcdull/dull_encoder.c \
       mcdull/dull_macroblock_p.c mcdull/dull_me.c mcdull/dull_me_0.c mcdull/dull_me_1.c \
       \
       mcdull/mcdull.c mcdull/mcdull_core.c mcdull/mcdull_swataw.c mcdull/mcdull_swataw_p_best.c \
       mcdull/mcdull_swataw_p_fast.c mcdull/mcdull_swataw_p_good.c mcdull/mcdull_swataw_p_skip.c \
       \
       swataw/swataw.cpp swataw/swataw_core.cpp \
       \
       mcdull/common/bitstream.c mcdull/common/cabac.c mcdull/common/common.c mcdull/common/cpu.c \
       mcdull/common/dct.c mcdull/common/deblock.c mcdull/common/frame.c mcdull/common/macroblock.c \
       mcdull/common/mc.c  mcdull/common/mvpred.c mcdull/common/osdep.c mcdull/common/pixel.c \
       mcdull/common/predict.c mcdull/common/quant.c mcdull/common/set.c mcdull/common/vlc.c \
       mcdull/common/rectangle.c \
       mcdull/encoder/analyse.c mcdull/encoder/cabac.c mcdull/encoder/cavlc.c mcdull/encoder/encoder.c \
       mcdull/encoder/lookahead.c mcdull/encoder/macroblock.c mcdull/encoder/me.c \
       mcdull/encoder/ratecontrol.c mcdull/encoder/set.c 
       
# define a static library containing our NEON code
##   ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)

##      LOCAL_ARM_NEON  := true

##   endif # TARGET_ARCH_ABI == armeabi-v7a


       
   include $(BUILD_SHARED_LIBRARY)
