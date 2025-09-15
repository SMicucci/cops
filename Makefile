CFLAGS = -Wall -Wextra -Wpedantic -Werror -std=c99 -O2 -g

SRC_d = test
OBJ_d = obj
BIN_d = bin

SRCs = $(wildcard $(SRC_d)/*.c)
OBJs = $(patsubst $(SRC_d)/%.c,$(OBJ_d)/%.o,$(SRCs))
BIN = $(BIN_d)/test

.PHONY: all clean

all: $(BIN)
	@echo -e "[\e[36mcops\e[0m]: \e[32mcops test created!\e[0m"

$(OBJ_d)/%.o: $(SRC_d)/%.c | $(OBJ_d)
	@echo -e "[\e[36mcops\e[0m]: \e[34mbuilding "$@"\e[0m..."
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_d) $(BIN_d):
	@mkdir -p $@

$(BIN): $(OBJs) | $(BIN_d)
	@echo -e "[\e[36mcops\e[0m]: \e[33mcreating "$@"\e[0m..."
	@$(CC) $(CFLAGS) $^ -o $@

clean:
	@echo -e "[\e[36mcops\e[0m]: \e[31mcleaning reference\e[0m..."
	@rm -rf $(BIN_d) $(OBJ_d)
