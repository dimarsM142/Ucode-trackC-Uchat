SRC_DIR := src
OBJ_DIR := obj

EXE := libmx.a
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CC=clang
CPPFLAGS=-Iinc
DBG=-g -O0
CFLAGS=-std=c11 -Wall -Wextra -Werror -Wpedantic

all: $(EXE)

$(EXE): $(OBJ)
	ar rcs $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(DBG) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

uninstall: clean
	@$(RM) -f $(EXE)
clean:
	@$(RM) -rf $(OBJ_DIR)

#-include $(OBJ:.o=.d)

reinstall: uninstall all


