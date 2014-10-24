#
# Copyright (C) BlockWorks Consulting Ltd - All Rights Reserved.
# Unauthorized copying of this file, via any medium is strictly prohibited.
# Proprietary and confidential.
# Written by Steve Tickle <Steve@BlockWorks.co>, September 2014.
#


#
#
#
BLOCKWORKS_PLATFORM=$(BLOCKWORKS_PLATFORM_BASE)/$(PLATFORM)

CFLAGS=-fpermissive -DSTM32F407xx -save-temps -DUSE_USB_FS
CMSIS=OWN

#
#
#
INCLUDES=-I./$(PLATFORM)
INCLUDES+=-I$(BLOCKWORKS_PLATFORM)/Common
INCLUDES+=-I$(BLOCKWORKS_PLATFORM)
INCLUDES+=-I./USBAudioClass
INCLUDES+=-I./USBCore
INCLUDES+=-I./HAL
INCLUDES+=-I./CMSIS/Device/ST/STM32F4xx/Include/
INCLUDES+=-I./CMSIS/Include


#
#
#
VPATH+=./HAL
VPATH+=./USBAudioClass
VPATH+=./USBCore
VPATH+=./CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc

#
#
#
LIBS=


#
#
#
OBJS=\
main.o \
usbd_audio.o \
usbd_core.o \
usbd_ioreq.o \
usbd_ctlreq.o \
usbd_conf.o \
stm32f4xx_it.o \
stm32f4xx_hal_rcc.o \
stm32f4xx_ll_usb.o \
stm32f4xx_hal_pcd.o \
stm32f4xx_hal_pcd_ex.o \
stm32f4xx_hal_hcd.o \
stm32f4xx_hal_rcc.o \
stm32f4xx_hal_rcc_ex.o \
stm32f4xx_hal.o \
stm32f4xx_hal_gpio.o \
stm32f4xx_hal_cortex.o \
system_stm32f4xx.o \
usbd_desc.o \
usbd_audio_if.o \
Stubs.o \
startup_stm32f407xx.o


include $(BLOCKWORKS_PLATFORM)/Makefile





