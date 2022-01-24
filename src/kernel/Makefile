# @author : eltertrias
# @brief : makes the main parts of the kernel



### Variables ###
KERNEL_DIR = kernel
OUTPUT_DIR_KERNEL = $(BUILDDIR)/$(KERNEL_DIR)
OBJECTS += $(addprefix $(OUTPUT_DIR_KERNEL)/, kernel.o)



### Make rules ###
all_KERNEL:
	mkdir -p $(OUTPUT_DIR_KERNEL)
	mkdir -p $(BUILDDIR)/kernel/mm

$(OUTPUT_DIR_KERNEL)/%.o : $(KERNEL_DIR)/%.c
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: all_KERNEL




### Includes ###
include kernel/mm/Makefile