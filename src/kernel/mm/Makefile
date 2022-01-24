# @author : eltertrias
# @brief : makes the memory management part of the kernel



MM_DIR = kernel/mm
OUTPUT_DIR_MM = $(BUILDDIR)/$(MM_DIR)
OBJECTS += $(addprefix $(OUTPUT_DIR_MM)/, vmm-global.o pmm.o)



$(OUTPUT_DIR_MM)/%.o : $(MM_DIR)/%.c
	$(CC) $(CFLAGS) $^ -o $@