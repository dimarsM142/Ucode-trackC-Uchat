SRC_DIR := src
OBJ_DIR := obj

BIN := uchat_server
LIB := libmx.a
LIBDIR := ../libmx/
SQL := sqlite3.a
SQLDIR := ../sqlite3/
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CC=clang
CPPFLAGS=-Iinc -I$(LIBDIR)/inc -I$(SQLDIR) -MMD -MP
DDG=-g -O0 #-fsanitize=address
LDLIBS := -lssl -lcrypto -lpthread -lm -ldl
CFLAGS=-std=c11 -Wall -Wextra -Werror -Wpedantic

all: $(BIN)

$(BIN): $(OBJ) 
	$(CC) $(DDG) $(LDFLAGS) $^ $(LIBDIR)$(LIB) $(SQLDIR)$(SQL) $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(DDG) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

uninstall: clean
	@$(RM) -f $(BIN)

clean:
	@$(RM) -rf $(OBJ_DIR)

reinstall: uninstall all
