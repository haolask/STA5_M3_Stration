NAME = Multic-S_Straton
#if @ is before a following commande $(..), call options (comments) are masked

CC = "../../../compile/GNUARM-RZv13.01-EABI/arm-rz-eabi/arm-rz-eabi/bin/arm-rz-eabi-gcc.exe"
AS = "../../../compile/GNUARM-RZv13.01-EABI/arm-rz-eabi/arm-rz-eabi/bin/arm-rz-eabi-as.exe"
LD = "../../../compile/GNUARM-RZv13.01-EABI/arm-rz-eabi/arm-rz-eabi/bin/arm-rz-eabi-ld.exe"
OBJCOPY = "../../../compile/GNUARM-RZv13.01-EABI/arm-rz-eabi/arm-rz-eabi/bin/arm-rz-eabi-objcopy.exe"
MKD = "../../../compile/yagarto-tools-20121018/bin/mkdir.exe"
SRECORD = "../../../compile/srecord/srec_cat.exe"

C_DEFINES =

S_DEFINES =

LIBS = 
#-I"D:/KPIT/GNUARM-RZv13.01-EABI/arm-rz-eabi/arm-rz-eabi/arm-rz-eabi/lib"\
#-I"D:/KPIT/GNUARM-RZv13.01-EABI/arm-rz-eabi/arm-rz-eabi/lib/gcc/arm-rz-eabi/4.7-GNUARM-RZ_v13.01"

SOURCE_DIR = ..
BUILD_DIR = ../Release

C_OPTS =	-c \
			-I. \
			-I"../../../compile/GNUARM-RZv13.01-EABI/arm-rz-eabi/arm-rz-eabi/arm-rz-eabi/include" \
			-I"../../../compile/GNUARM-RZv13.01-EABI/arm-rz-eabi/arm-rz-eabi/bin/newlib/libc/sys/arm" \
			-I"$(SOURCE_DIR)/Include" \
			-I"$(SOURCE_DIR)/Include/FreeRTOS" \
			-I"$(SOURCE_DIR)/Sources/API" \
			-I"$(SOURCE_DIR)/Sources/APP/CORE/VM/custom" \
			-I"$(SOURCE_DIR)/Sources/APP/CORE/VM/standard" \
			-I"$(SOURCE_DIR)/Sources/APP/CORE/base" \
			-I"$(SOURCE_DIR)/Sources/APP/COM/J1939" \
			-I"$(SOURCE_DIR)/Sources/APP/COM/diag_app" \
			-I"$(SOURCE_DIR)/Sources/APP/BLOCKS/Dtc" \
			-I"$(SOURCE_DIR)/Sources/APP/BLOCKS/J1939" \
			-I"$(SOURCE_DIR)/Sources/APP/BLOCKS/Lin" \
			-I"$(SOURCE_DIR)/Sources/APP/BLOCKS/Param" \
			-I"$(SOURCE_DIR)/Sources/APP/BLOCKS/RTC" \
			-I"$(SOURCE_DIR)/Sources/APP/BLOCKS/Sleep" \
			-I"$(SOURCE_DIR)/Sources/APP/BLOCKS/SpecifCalib" \
			-I"$(SOURCE_DIR)/Sources/APP/BLOCKS/BEEP" \
			-std=c99 \
			-fmessage-length=0 \
			-mcpu=cortex-a9 \
			-mlittle-endian \
			-marm \
			-mno-unaligned-access \
			-mfpu=vfp \
			-mfloat-abi=soft \
			-g \
			-Wall \
			-Wconversion \
			-v \
			-Os \
			-fPIC

S_OPTS =	-EL \
			-mcpu=cortex-a9 \
			-mlittle-endian \
			-mfpu=vfp \
			-mfloat-abi=soft \
			-mfpu=vfpv3
				
C_FILES =	Sources/APP/CORE/VM/custom/T5ACTIA.c \
			Sources/APP/CORE/VM/custom/t5can.c \
			Sources/APP/CORE/VM/custom/t5ddkcip.c \
			Sources/APP/CORE/VM/custom/t5flash.c \
			Sources/APP/CORE/VM/custom/t5ios.c \
			Sources/APP/CORE/VM/custom/t5memory.c \
			Sources/APP/CORE/VM/custom/t5retain.c \
			Sources/APP/CORE/VM/custom/t5rtclk.c \
			Sources/APP/CORE/VM/custom/t5serial.c \
			Sources/APP/CORE/VM/custom/t5socket.c \
			Sources/APP/CORE/VM/standard/t5cs.c \
			Sources/APP/CORE/VM/standard/t5csser.c \
			Sources/APP/CORE/VM/standard/t5cssock.c \
			Sources/APP/CORE/VM/standard/t5def.c \
			Sources/APP/CORE/VM/standard/t5heap.c \
			Sources/APP/CORE/VM/standard/t5mbfb.c \
			Sources/APP/CORE/VM/standard/t5mm.c \
			Sources/APP/CORE/VM/standard/t5tools.c \
			Sources/APP/CORE/VM/standard/t5vfs.c \
			Sources/APP/CORE/VM/standard/t5vm.c \
			Sources/APP/CORE/VM/standard/t5vmbsmp.c \
			Sources/APP/CORE/VM/standard/t5vmcan.c \
			Sources/APP/CORE/VM/standard/t5vmcnv.c \
			Sources/APP/CORE/VM/standard/t5vmcode.c \
			Sources/APP/CORE/VM/standard/t5vmfbl.c \
			Sources/APP/CORE/VM/standard/t5vmiox.c \
			Sources/APP/CORE/VM/standard/t5vmlog.c \
			Sources/APP/CORE/VM/standard/t5vmmap.c \
			Sources/APP/CORE/VM/standard/t5vmprg.c \
			Sources/APP/CORE/VM/standard/t5vmrsc.c \
			Sources/APP/CORE/VM/standard/t5vmsrv.c \
			Sources/APP/CORE/VM/standard/t5vmstf.c \
			Sources/APP/CORE/VM/standard/t5vmstfb.c \
			Sources/APP/CORE/VM/standard/t5vmstr.c \
			Sources/APP/CORE/VM/standard/t5vmtic.c \
			Sources/APP/CORE/VM/standard/t5vmtmr.c \
			Sources/APP/CORE/VM/standard/t5vmwarm.c \
			Sources/APP/CORE/VM/standard/t5barray.c \
			Sources/APP/COM/DIAG/CustomDiag.c \
			Sources/APP/COM/diag_app/diag_app.c \
			Sources/APP/COM/J1939/T5_J1939.c \
			Sources/APP/BLOCKS/Dtc/Dtc_Clr.c \
			Sources/APP/BLOCKS/Dtc/Dtc_List.c \
			Sources/APP/BLOCKS/Dtc/Dtc_Rd.c \
			Sources/APP/BLOCKS/Dtc/Dtc_Wr.c \
			Sources/APP/BLOCKS/J1939/FB_J1939_Status.c \
			Sources/APP/BLOCKS/LIN/Lin_Init.c \
			Sources/APP/BLOCKS/LIN/Lin_RdData.c \
			Sources/APP/BLOCKS/LIN/Lin_RqFrame.c \
			Sources/APP/BLOCKS/LIN/Lin_Tx.c \
			Sources/APP/BLOCKS/LIN/Lin_GetStatus.c \
			Sources/APP/BLOCKS/Param/Param_Rd.c \
			Sources/APP/BLOCKS/Param/Param_Wr.c \
			Sources/APP/BLOCKS/Sleep/SleepCmd.c \
			Sources/APP/BLOCKS/RTC/Rtc_Rd.c \
			Sources/APP/BLOCKS/RTC/Rtc_Wr.c \
			Sources/APP/BLOCKS/SpecifCalib/SpecifCalib_Rd.c \
			Sources/APP/BLOCKS/SpecifCalib/SpecifCalib_Wr.c \
			Sources/APP/BLOCKS/RTC/Rtc_Timer_Rd.c \
			Sources/APP/BLOCKS/RTC/Rtc_Timer_Wr.c \
			Sources/APP/BLOCKS/RTC/Rtc_Timer_Clr.c \
			Sources/APP/BLOCKS/RTC/Rtc_Alarm_Rd.c \
			Sources/APP/BLOCKS/RTC/Rtc_Alarm_Wr.c \
			Sources/APP/BLOCKS/RTC/Rtc_Alarm_Clr.c \
			Sources/APP/BLOCKS/BEEP/BeepCmd.c \
			Sources/APP/BLOCKS/t5blocks.c \
			Sources/APP/API_WRAPPER/api_wrapper.c \
			Sources/APP/API_WRAPPER/version.c


S_FILES =	Sources/APP/CORE/base/compiler_specific/asm/initsct.S

C_OBJS = $(C_FILES:%.c=$(BUILD_DIR)/%.o)

S_OBJS = $(S_FILES:%.S=$(BUILD_DIR)/%.o)

ALL_CFLAGS = $(C_OPTS) $(C_DEFINES)
ALL_SFLAGS = $(S_OPTS) $(S_DEFINES)

#ALL_LDFLAGS = $(LD_FLAGS) \
#			-Wl,-T"Settings/GNU_DS-5-QSPI.ld" \
#			-mcpu=cortex-a9 \
#			-g3 \
#			-gdwarf-2
ALL_LDFLAGS = -estart \
			 -EL \
			 -M=$(NAME).map \
			 -q \
 			 -L"$(SOURCE_DIR)/Include" \
			 -T"$(SOURCE_DIR)/Settings/GNU_DS-5-QSPI.ld" \
 			 --verbose \
 			 --strip-debug \
 			 -lm \
 			 -lc \
 			 -lg \
 			 -lgcc \
 			 -L"../../../compile/GNUARM-RZv13.01-EABI/arm-rz-eabi/arm-rz-eabi/lib/gcc/arm-rz-eabi/4.7-GNUARM-RZ_v13.01" \
 			 -L"../../../compile/GNUARM-RZv13.01-EABI/arm-rz-eabi/arm-rz-eabi/arm-rz-eabi/lib" \
 			 -L"$(SOURCE_DIR)/Lib" \
 			 --library-path="../../../compile/GNUARM-RZv13.01-EABI/arm-rz-eabi/arm-rz-eabi/arm-rz-eabi/lib" \
 			 --library-path="../../../compile/GNUARM-RZv13.01-EABI/arm-rz-eabi/arm-rz-eabi/lib/gcc/arm-rz-eabi/4.7-GNUARM-RZ_v13.01" \
 			 --warn-common \
 			 --warn-unresolved-symbols \
 			 --error-unresolved-symbols \
 			 --start-group \
 			 -lm \
 			 -lc \
 			 -lgcc \
 			 --end-group

#AUTODEPENDENCY_CFLAGS = -MMD -MF$(@:.o=.d) -MT$@


all: $(NAME).mot $(NAME).bin

clean:
#	echo "path=$(PATH)"
	rm -rf $(BUILD_DIR)/Sources $(NAME).elf $(NAME).x $(NAME).map $(NAME).mot $(NAME).mhx $(NAME).bin

$(NAME).bin: $(NAME).x
#	$(SRECORD) -VERSion
	$(SRECORD) -Disable_Sequence_Warnings \
	$(NAME).mot -crop 0x18280000 0x1843FFF7 -fill 0xFF 0x18280000 0x1843FFF8 \
	-fill 0x00 0x1843FFF8 0x1843FFFB -fill 0x03 0x1843FFFB 0x1843FFFC \
	-Checksum_Positive_Little_Endian 0x1843FFFC 4 4 \
	-offset - 0x18280000 \
	-output $(NAME).bin -binary

$(NAME).mot: $(NAME).x
	$(OBJCOPY) -O srec $(NAME).x $(NAME).mot

$(NAME).mhx: $(NAME).x
#	$(SRECORD) -VERSion
	$(SRECORD) -Disable_Sequence_Warnings \
	$(NAME).bin -binary -offset 0x18280000 \
	-output $(NAME).mhx -Motorola -address-length=4 -line-length=270 -header='P1xxxxx-(Generated-by-ACTIA)' -disable=data-count -Execution_Start_Address 0x18280000 -crlf

$(NAME).x: $(C_OBJS) $(S_OBJS)
	$(LD) -o $@ $^ $(LIBS) $(ALL_LDFLAGS)

.SUFFIXES: .o .c

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(MKD) -p $(dir $@)
	$(CC) $(ALL_CFLAGS)  -c $< -o $@

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.S
	$(MKD) -p $(dir $@)
	$(AS) $(ALL_SFLAGS)  -c $< -o $@

-include $(C_OBJS:.o=.d)
