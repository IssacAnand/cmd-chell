# CC = gcc
# SRC_DIR = ./source/system_programs
# BIN_DIR = ./bin
# SOURCES = $(wildcard $(SRC_DIR)/*.c)
# OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BIN_DIR)/%)
# MAIN_SRC = ./source/shell.c
# MAIN_EXEC = cseshell

# # Special rule for main executable
# all: $(OBJECTS) $(MAIN_EXEC) sys dspawn dcheck backup find ld ldr

# $(BIN_DIR)/%: $(SRC_DIR)/%.c
# 	@mkdir -p $(BIN_DIR)
# 	$(CC) $< -o $@

# $(MAIN_EXEC): $(MAIN_SRC)
# 	$(CC) $< -o $@

# sys: $(SRC_DIR)/sys.c
# 	$(CC) $< -o $(BIN_DIR)/sys

# dspawn: $(SRC_DIR)/dspawn.c
# 	$(CC) $< -o $(BIN_DIR)/dspawn

# dcheck: $(SRC_DIR)/dcheck.c
# 	$(CC) $< -o $(BIN_DIR)/dcheck

# backup: $(SRC_DIR)/backup.c
# 	$(CC) $< -o $(BIN_DIR)/backup

# find: $(SRC_DIR)/find.c
# 	$(CC) $< -o $(BIN_DIR)/find

# ld: $(SRC_DIR)/ld.c
# 	$(CC) $< -o $(BIN_DIR)/ld

# ldr: $(SRC_DIR)/ldr.c
# 	$(CC) $< -o $(BIN_DIR)/ldr

# clean:
# 	rm -f $(OBJECTS) $(MAIN_EXEC) $(BIN_DIR)/*

	
CC = gcc
SRC_DIR = ./source/system_programs
BIN_DIR = ./bin
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BIN_DIR)/%)
MAIN_SRC = ./source/shell.c
MAIN_EXEC = cseshell

# Special rule for main executable
all: $(OBJECTS) $(MAIN_EXEC) sys dspawn dcheck backup ld

$(BIN_DIR)/%: $(SRC_DIR)/%.c
	@mkdir -p $(BIN_DIR)
	$(CC) $< -o $@

$(MAIN_EXEC): $(MAIN_SRC)
	$(CC) $< -o $@

sys: $(SRC_DIR)/sys.c
	$(CC) $< -o $(BIN_DIR)/sys

dspawn: $(SRC_DIR)/dspawn.c
	$(CC) $< -o $(BIN_DIR)/dspawn

dcheck: $(SRC_DIR)/dcheck.c
	$(CC) $< -o $(BIN_DIR)/dcheck

backup: $(SRC_DIR)/backup.c
	$(CC) $< -o $(BIN_DIR)/backup

ld: $(SRC_DIR)/ld.c
	$(CC) $< -o $(BIN_DIR)/ld

clean:
	rm -f $(OBJECTS) $(MAIN_EXEC) $(BIN_DIR)/sys $(BIN_DIR)/dspawn $(BIN_DIR)/dcheck $(BIN_DIR)/backup $(BIN_DIR)/ld

