SRC_DIR := src
OBJ_DIR := obj

BIN := uchat
LIB := libmx.a
LIBDIR := ../libmx/
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CC=clang
CPPFLAGS=-Iinc -I../libmx/inc -MMD -MP
DDG=-g -O0 #-fsanitize=address
LDLIBS := -lssl -lcrypto -lpthread  `pkg-config --libs gtk+-3.0`
CFLAGS=-std=c11 -Wall -Wextra -Werror -Wpedantic `pkg-config --cflags gtk+-3.0`

all: $(BIN)

$(BIN): $(OBJ) 
	$(CC) $(DDG) $(LDFLAGS)  $^ $(LIBDIR)$(LIB)  $(LDLIBS) -o $@ 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(DDG) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

uninstall: clean
	@$(RM) -f $(BIN)

clean:
	@$(RM) -rf $(OBJ_DIR)

reinstall: uninstall all
