PROJECT     = lab1_uart
BUILD_DIR   = build
DEVICE      = stm32f103c8t6
OPENCM3_DIR = libopencm3
SRC         = src/main.c

include $(OPENCM3_DIR)/mk/genlink-config.mk
include $(OPENCM3_DIR)/mk/gcc-config.mk

OBJS     = $(SRC:%.c=$(BUILD_DIR)/%.o)
CPPFLAGS += $(DEFS) -I$(OPENCM3_DIR)/include
CFLAGS   += -Os -g -std=c11 -Wall -Wextra $(ARCH_FLAGS) \
            -ffunction-sections -fdata-sections
LDFLAGS  += -static -nostartfiles -T$(LDSCRIPT) $(ARCH_FLAGS) \
            -Wl,--gc-sections -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map
LDLIBS   += -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group

all: $(BUILD_DIR)/$(PROJECT).bin

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/$(PROJECT).elf: $(OBJS) $(LDSCRIPT)
	$(CC) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $@
	$(SIZE) $@

$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT).elf
	$(OBJCOPY) -O binary $< $@

flash: $(BUILD_DIR)/$(PROJECT).bin
	st-flash write $< 0x8000000

clean:
	rm -rf $(BUILD_DIR)

include $(OPENCM3_DIR)/mk/genlink-rules.mk

.PHONY: all flash clean
