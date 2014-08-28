# BoardConfig.mk for lenovo stuttgart 
# Product-specific compile-time definitions.
# Thanks to scue


#BOARD_VENDOR := samsung exynos4
TARGET_BOARD_PLATFORM := exynos4
TARGET_SOC := exynos4x12
TARGET_BOARD_INFO_FILE := device/lenovo/stuttgart/board-info.txt

-include vendor/lenovo/stuttgart/BoardConfigVendor.mk
TARGET_BOOTANIMATION_PRELOAD := true

# CPU options
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_SMP := true
TARGET_CPU_VARIANT := cortex-a9
TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_ARCH_VARIANT_CPU := cortex-a9
ARCH_ARM_HAVE_NEON := true
ARCH_ARM_HAVE_TLS_REGISTER := true

TARGET_GLOBAL_CFLAGS += -mtune=cortex-a9 -mfpu=neon -mfloat-abi=softfp #mark add
TARGET_GLOBAL_CPPFLAGS += -mtune=cortex-a9 -mfpu=neon -mfloat-abi=softfp #mark add
#COMMON_GLOBAL_CFLAGS += -D__ARM_USE_PLD -D__ARM_CACHE_LINE_SIZE=64

TARGET_NO_BOOTLOADER := true
TARGET_NO_KERNEL := false
TARGET_NO_RADIOIMAGE := true
TARGET_BOOTLOADER_BOARD_NAME := stuttgart
# TARGET_RECOVERY_PIXEL_FORMAT := "BGRA_8888"
TARGET_UBOOT_RAMDISK := true
TARGET_UBOOT_RAMDISK_LOADADDR := 0x40800000
BOARD_USES_UBOOT := true
BOARD_LEGACY_NL80211_STA_EVENTS := true

EXYNOS4X12_ENHANCEMENTS := true
EXYNOS4_ENHANCEMENTS := true

ifdef EXYNOS4X12_ENHANCEMENTS
	COMMON_GLOBAL_CFLAGS += -DEXYNOS4210_ENHANCEMENTS
	COMMON_GLOBAL_CFLAGS += -DEXYNOS4X12_ENHANCEMENTS
	COMMON_GLOBAL_CFLAGS += -DEXYNOS4_ENHANCEMENTS
	COMMON_GLOBAL_CFLAGS += -DDISABLE_HW_ID_MATCH_CHECK
endif

USE_CAMERA_STUB := true

# kernel and recovery 
TARGET_PROVIDES_INIT_TARGET_RC := true
BOARD_USES_DEPRECATED_TOOLCHAIN := true
BOARD_KERNEL_CMDLINE :=
BOARD_KERNEL_BASE := 0x10000000
BOARD_KERNEL_PAGESIZE := 2048
BOARD_CUSTOM_BOOTIMG_MK := device/lenovo/stuttgart/shbootimg.mk
BOARD_KERNEL_IMAGE_NAME := device/lenovo/stuttgart/kernel
TARGET_PREBUILT_KERNEL := device/lenovo/stuttgart/kernel
#TARGET_KERNEL_SOURCE        := kernel/lenovo/stuttgart
#TARGET_KERNEL_CONFIG	    := stuttgart_android_defconfig

# head file
TARGET_SPECIFIC_HEADER_PATH := device/lenovo/stuttgart/overlay/include

# Invensense Gyroscope
BOARD_USES_GYRO := false

# Yamaha Compass
BOARD_USES_COMPASS := true

# Audio
BOARD_USES_GENERIC_AUDIO := false
BOARD_USES_I2S_AUDIO := true
BOARD_USES_PCM_AUDIO := false
BOARD_USES_SPDIF_AUDIO := false
#BOARD_USES_LIBMEDIA_WITH_AUDIOPARAMETER := true
#BOARD_USE_YAMAHAPLAYER := true
#BOARD_USE_SAMSUNG_SEPARATEDSTREAM := true
#BOARD_HAS_SAMSUNG_VOLUME_BUG := true

# ULP Audio
USE_ULP_AUDIO := false

# ALP Audio
BOARD_USE_ALP_AUDIO := false

# ALSA
BOARD_USES_ALSA_AUDIO := true
BUILD_WITH_ALSA_UTILS := true

# Filesystem
BOARD_HAS_TWOSTORAGES := true
BOARD_BOOTIMAGE_PARTITION_SIZE := 20940800
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 20940800
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 419430400
BOARD_USERDATAIMAGE_PARTITION_SIZE := 2621440000
TARGET_USERIMAGES_USE_EXT4 := true
BOARD_FLASH_BLOCK_SIZE := 4096
# the following is from vibe rom
# BOARD_FLASH_BLOCK_SIZE := 2048   
# BOARD_SYSTEMIMAGE_PARTITION_SIZE := 734003200
# BOARD_USERDATAIMAGE_PARTITION_SIZE := 3145728000
# BOARD_PRELOADIMAGE_FILE_SYSTEM_TYPE := ext4
# BOARD_PRELOADIMAGE_PARTITION_SIZE := 418381824

# Graphics
BOARD_USES_HWCOMPOSER := true
BOARD_USES_HGL := true
BOARD_NO_32BPP := true
BOARD_EGL_CFG := device/lenovo/stuttgart/configs/egl.cfg
DEFAULT_FB_NUM := 0
USE_OPENGL_RENDERER := true
BOARD_USES_SKIAHWJPEG := true
COMMON_GLOBAL_CFLAGS += -DSEC_HWJPEG_G2D
#COMMON_GLOBAL_CFLAGS += -DSEC_HWJPEG_G2D -DGL_EXT_discard_framebuffer

# BOARD_USE_SAMSUNG_COLORFORMAT := true
# BOARD_FIX_NATIVE_COLOR_FORMAT := true
BOARD_NONBLOCK_MODE_PROCESS := true
BOARD_USE_STOREMETADATA := true
BOARD_USE_METADATABUFFERTYPE := true
BOARD_USES_MFC_FPS := true
BOARD_USE_S3D_SUPPORT := true
BOARD_USE_CSC_FIMC := false
BOARD_USE_DRM := true
BOARD_USES_HIGH_RESOLUTION_LCD := false

BOARD_USE_EXYNOS_OMX := false
BOARD_HAVE_CODEC_SUPPORT := SAMSUNG_CODEC_SUPPORT
COMMON_GLOBAL_CFLAGS += -DSAMSUNG_CODEC_SUPPORT
BOARD_USES_PROPRIETARY_OMX := SAMSUNG
COMMON_GLOBAL_CFLAGS += -DSAMSUNG_OMX

# Bluetooth
BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_BCM := true
BLUETOOTH_USE_BPLUS := true
TARGET_NEEDS_BLUETOOTH_INIT_DELAY := true
BT_ALT_STACK := true
BRCM_BT_USE_BTL_IF := true
BRCM_BTL_INCLUDE_A2DP := true
TARGET_CUSTOM_BLUEDROID := ../../../device/lenovo/stuttgart/bluetooth/bluetooth.c
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := device/lenovo/stuttgart/bluetooth/include

# HDMI related
BOARD_USES_HDMI_SUBTITLES := false
BOARD_USES_HDMI := false
BOARD_HDMI_STD := STD_1080P
BOARD_HDMI_DDC_CH := DDC_CH_I2C_7
# BOARD_HDMI_DDC_CH := DDC_CH_I2C_2
BOARD_USES_FIMGAPI := true
BOARD_USES_SKIAHWJPEG := false
#BOARD_USES_SKTEXTBOX := true
#BOARD_USE_SECTVOUT := true

BOARD_USE_WFD := false

# Enable WEBGL in WebKit
ENABLE_WEBGL := true
BOARD_USE_SKIA_LCDTEXT := true

# FIMG Acceleration
#BOARD_USES_SKIA_FIMGAPI := true 

# RIL /* Radio Interface Layer */
BOARD_MOBILEDATA_INTERFACE_NAME := "rmnet0"

# Wifi 
BOARD_WPA_SUPPLICANT_DRIVER 	:= NL80211
WPA_SUPPLICANT_VERSION      	:= VER_0_8_X
BOARD_WPA_SUPPLICANT_PRIVATE_LIB:= lib_driver_cmd_bcmdhd
BOARD_HOSTAPD_DRIVER        	:= NL80211
BOARD_HOSTAPD_PRIVATE_LIB   	:= lib_driver_cmd_bcmdhd
BOARD_WLAN_DEVICE           	:= bcmdhd
WIFI_DRIVER_MODULE_NAME     	:= "bcmdhd"
WIFI_DRIVER_MODULE_ARG      	:= "firmware_path=/system/etc/firmware/fw_bcmdhd.bin nvram_path=/system/etc/wifi/bcmdhd.cal iface_name=wlan0"
WIFI_DRIVER_FW_PATH_PARAM   	:= "/sys/module/bcmdhd/parameters/firmware_path"
WIFI_DRIVER_MODULE_PATH     	:= "/system/lib/modules/bcmdhd.ko"
WIFI_DRIVER_FW_PATH_STA     	:= "/system/etc/firmware/fw_bcmdhd.bin"
WIFI_DRIVER_FW_PATH_P2P     	:= "/system/etc/firmware/fw_bcmdhd_p2p.bin"
WIFI_DRIVER_FW_PATH_AP      	:= "/system/etc/firmware/fw_bcmdhd_apsta.bin"
# WIFI_BAND                       := 802_11_ABG
# BOARD_WLAN_DEVICE_REV 		:= bcm4329
# WIFI_DRIVER_MODULE_APARG    	:= "firmware_path=/system/etc/firmware/fw_bcmdhd_apsta.bin nvram_path=/system/etc/wifi/bcmdhd.cal iface_name=wlan0"

# Camera
BOARD_USES_PROPRIETARY_LIBCAMERA := true
COMMON_GLOBAL_CFLAGS += -DSAMSUNG_CAMERA_HARDWARE
COMMON_GLOBAL_CFLAGS += -DSTUTTGART_CAMERA
COMMON_GLOBAL_CFLAGS += -DSTUTTGART_FM
#BOARD_USES_PROPRIETARY_LIBCAMERA := true
BOARD_USES_PROPRIETARY_LIBFIMC := true

# Enable JIT
WITH_JIT := true

# fix maps
BOARD_USE_LEGACY_SENSORS_FUSION := false

# Vold
TARGET_USE_CUSTOM_LUN_FILE_PATH := "/sys/devices/platform/s3c-usbgadget/gadget/lun%d/file"

# Recovery
BOARD_CUSTOM_GRAPHICS := ../../../device/lenovo/stuttgart/recovery/graphics.c
BOARD_USE_CUSTOM_RECOVERY_FONT := \"roboto_15x24.h\"
BOARD_UMS_LUNFILE := "/sys/devices/platform/s3c-usbgadget/gadget/lun%d/file"
BOARD_USES_MMCUTILS := true
BOARD_HAS_LARGE_FILESYSTEM := true
BOARD_HAS_NO_SELECT_BUTTON := true
TARGET_RECOVERY_FSTAB := device/lenovo/stuttgart/rootdir/fstab.stuttgart
TARGET_RECOVERY_INITRC := device/lenovo/stuttgart/recovery/recovery.rc
RECOVERY_FSTAB_VERSION := 2

# TWRP 
DEVICE_RESOLUTION := 720x1280

#
# TODO: Charging mode
#

# assert
TARGET_OTA_ASSERT_DEVICE := stuttgart,K860,K860i
#BOARD_VOLD_EMMC_SHARES_DEV_MAJOR := true
BOARD_VOLD_DISC_HAS_MULTIPLE_MAJORS := true

-include hardware/broadcom/wlan/bcmdhd/firmware/bcm4329/device-bcm.mk
BOARD_VOLD_MAX_PARTITIONS := 29
-include vendor/lenovo/stuttgart/BoardConfigVendor.mk
WITH_DEXPREOPT := false

# Selinux
BOARD_SEPOLICY_DIRS += \
    device/lenovo/stuttgart/selinux

BOARD_SEPOLICY_UNION += \
    device.te \
    domain.te \
    file.te \
    file_contexts \
    init.te \
    mediaserver.te \
    rild.te \
    system.te \
    ueventd.te \
    wpa_supplicant.te

#2012-4-14 add by liuwei28
#Lenovo shlould set product variable[MODEL/OPERATOR/SIMMODE] for each model.
#define model[ModelName]
TARGET_MODEL := stuttgart
#define operator type[none/chinatelecom/chinaunicom/chinamobile] 
TARGET_OPERATOR := none
#define sim mode[onesim/dualsim]
TARGET_SIMMODE := onesim

