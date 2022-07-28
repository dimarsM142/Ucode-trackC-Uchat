SRC_DIR := 
OBJ_DIR := obj

EXE := sqlite3.a
SRC := $(wildcard *.c)
OBJ := $(SRC:%.c=$(OBJ_DIR)/%.o)

CC=clang
CPPFLAGS=-Iinc
DBG=-g -O0
CFLAGS=-std=c11 -Wall

all: $(EXE)

$(EXE): $(OBJ)
	ar rcs $@ $^

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) $(DBG) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

uninstall: clean
	@$(RM) -f $(EXE)
clean:
	@$(RM) -rf $(OBJ_DIR)

#-include $(OBJ:.o=.d)

reinstall: uninstall all


