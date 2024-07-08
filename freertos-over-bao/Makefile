# Prepare enviroment for baremetal-runtime
NAME:=freertos
PLATFORM:=zcu102
ARCH:=armv8
ROOT_DIR:=$(realpath .)
BUILD_DIR:=$(ROOT_DIR)/build/$(PLATFORM)

# Setup baremetal-runtime build
bmrt_dir:=$(ROOT_DIR)/src/bao-baremetal-guest/
include $(bmrt_dir)/setup.mk



# Inlcude all freertos sources

# Main freertos application sources
app_src_dir:=$(ROOT_DIR)/src
include $(app_src_dir)/sources.mk
C_SRC+=$(addprefix $(app_src_dir)/, $(src_c_srcs))

# Freertos kernel sources
freertos_dir:=$(ROOT_DIR)/src/freertos
SRC_DIRS+=$(freertos_dir) $(freertos_memmng_dir)
INC_DIRS+=$(freertos_dir)/include
C_SRC+=$(wildcard $(freertos_dir)/*.c)
freertos_memmng_dir:=$(freertos_dir)/portable/MemMang
C_SRC+=$(freertos_memmng_dir)/heap_4.c

# Freertos port arch-specific
arch_dir=$(ROOT_DIR)/src/arch/$(ARCH)
SRC_DIRS+=$(arch_dir)
INC_DIRS+=$(arch_dir)/inc
-include $(arch_dir)/arch.mk
-include $(arch_dir)/sources.mk
C_SRC+=$(addprefix $(arch_dir)/, $(arch_c_srcs))
ASM_SRC+=$(addprefix $(arch_dir)/, $(arch_s_srcs))

# Sources for FreeRTOS + FAT
ROOT_DIR:=$(realpath .)
freertos_fat_dir:=$(ROOT_DIR)/src/FreeRTOS-FAT
freertos_ramdisk_dir:=$(freertos_fat_dir)/portable/common
SRC_DIRS+=$(freertos_fat_dir) $(freertos_ramdisk_dir)
INC_DIRS+=$(freertos_fat_dir)/include
INC_DIRS+=$(freertos_fat_dir)/portable/common
C_SRC+=$(wildcard $(freertos_fat_dir)/*.c)
C_SRC+=$(freertos_fat_dir)/portable/common/ff_ramdisk.c

# Sources for SDVB benchmarks 
sdvb_bmark_dir:=$(ROOT_DIR)/src/vision/benchmarks/$(BMARK)/src/c
C_SRC+=$(wildcard $(sdvb_bmark_dir)/*.c)
INC_DIRS+=$(sdvb_bmark_dir)

########### ADDING FOR OTHER BENCHMARKS INDIVIDUALLY ##########

sdvb_bmark_dir:=$(ROOT_DIR)/src/vision/benchmarks/mser/src/c
C_SRC+=$(wildcard $(sdvb_bmark_dir)/*.c)
INC_DIRS+=$(sdvb_bmark_dir)

sdvb_bmark_dir:=$(ROOT_DIR)/src/vision/benchmarks/pca/src/c
C_SRC+=$(wildcard $(sdvb_bmark_dir)/*.c)
INC_DIRS+=$(sdvb_bmark_dir)

sdvb_bmark_dir:=$(ROOT_DIR)/src/vision/benchmarks/sift/src/c
C_SRC+=$(wildcard $(sdvb_bmark_dir)/*.c)
INC_DIRS+=$(sdvb_bmark_dir)

stitch_dir:=$(ROOT_DIR)/src/vision/benchmarks/stitch/src/c
C_SRC+=$(wildcard $(stitch_dir)/*.c)
INC_DIRS+=$(stitch_dir)

svm_dir:=$(ROOT_DIR)/src/vision/benchmarks/svm/src/c
C_SRC+=$(wildcard $(svm_dir)/*.c)
INC_DIRS+=$(svm_dir)

localization_dir:=$(ROOT_DIR)/src/vision/benchmarks/localization/src/c
C_SRC+=$(wildcard $(localization_dir)/*.c)
INC_DIRS+=$(localization_dir)

texture_synthesis_dir:=$(ROOT_DIR)/src/vision/benchmarks/texture_synthesis/src/c
C_SRC+=$(wildcard $(texture_synthesis_dir)/*.c)
INC_DIRS+=$(texture_synthesis_dir)

multi_ncut_dir:=$(ROOT_DIR)/src/vision/benchmarks/multi_ncut/src/c
C_SRC+=$(wildcard $(multi_ncut_dir)/*.c)
INC_DIRS+=$(multi_ncut_dir)

tracking_dir:=$(ROOT_DIR)/src/vision/benchmarks/tracking/src/c
C_SRC+=$(wildcard $(tracking_dir)/*.c)
INC_DIRS+=$(tracking_dir)



################################################################

sdvb_common_dir:=$(ROOT_DIR)/src/vision/common/c
C_SRC+=$(wildcard $(sdvb_common_dir)/*.c)
INC_DIRS+=$(sdvb_common_dir)

# Include the final baremetal-runtime build
include $(bmrt_dir)/build.mk
