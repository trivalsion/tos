# @author : eltertrias
# @brief : compiles the libraries used for standardization of functions in the kernel



### Variables ###
LIB_DIR = lib
OUTPUT_DIR_LIB = $(BUILDDIR)/$(LIB_DIR)
OBJECTS += $(addprefix $(OUTPUT_DIR_LIB)/, string.o)



### Make rules ###
# Rule to build files in the lib directory
# Rule to do any extra stuff with local files
all_LIBS:
	mkdir -p $(OUTPUT_DIR_LIB)

$(OUTPUT_DIR_LIB)/%.o : $(LIB_DIR)/%.c
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: all_LIBS