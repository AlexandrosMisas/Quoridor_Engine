CC			:= gcc
TARGET_EXEC	:= quoridor
BUILD_DIR	:= ./build
BIN_DIR		:= ./bin
SRC_DIRS	:= ./src
INCLUDE		:= ./include

SRCS	:= $(shell find $(SRC_DIRS) -name '*.c')
OBJS	:= $(SRCS:%=$(BIN_DIR)/%.o)
DEPS	:= $(OBJS:.o=.d)

INC_DIRS	:= $(shell find $(INCLUDE) -type d)
INC_FLAGS	:= $(addprefix -I,$(INC_DIRS))

CFLAGS	:= $(INC_FLAGS) -MMD -MP -Wall -Werror -std=c11 
VFLAGS	:= valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes


ifeq ($(DEBUG), ON)
	CFLAGS += -g3
endif

$(BIN_DIR)/$(TARGET_EXEC): $(OBJS)
	@$(CC) $(OBJS) -o $@
	@mkdir -p $(BUILD_DIR)
	@mv $@ $(BUILD_DIR)

$(BIN_DIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@


.PHONY: run clean time


run: $(BUILD_DIR)/$(TARGET_EXEC)
	@./$(BUILD_DIR)/$(TARGET_EXEC)


valgrind: $(BUILD_DIR)/$(TARGET_EXEC)
	@$(VFLAGS) ./$(BUILD_DIR)/$(TARGET_EXEC)


clean: $(BUILD_DIR)/$(TARGET_EXEC)
	@rm -rf $(BUILD_DIR) $(BIN_DIR) *.sh


time: $(BUILD_DIR)/$(TARGET_EXEC)
	@time ./$(BUILD_DIR)/$(TARGET_EXEC)


gdb: $(BUILD_DIR)/$(TARGET_EXEC)
	@gdb ./$(BUILD_DIR)/$(TARGET_EXEC)


-include $(DEPS)
