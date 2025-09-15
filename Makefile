CFLAGS = -Wall -Wextra -Wpedantic -Werror -std=c99 -O2 -g

SRC_d = test
OBJ_d = obj
BIN_d = bin

SRCs = $(wildcard $(SRC_d)/*.c)
OBJs = $(patsubst $(SRC_d)/%.c,$(OBJ_d)/%.o,$(SRCs))
BIN = $(BIN_d)/test

.PHONY: all clean

all: $(BIN)
	@echo -e "[\e[36mcops\e[0m]: \e[32mtest created!\e[0m"

install:
	@if [ "$$(id -u)" -ne 0 ]; then \
		echo "[\e36mcops\e[0m]: \e[31mrun make install with root permission\e[0m"; \
		exit 1; \
	fi
	@echo -e "[\e[36mcops\e[0m]: \e[35minstalling headers...\e[0m"
	@mkdir -p /usr/include/cops
	@cp -v $(wildcard src/*.h) /usr/include/cops
	@echo -e "[\e[36mcops\e[0m]: \e[32minstalled headers succesfully\e[0m"

uninstall:
	@if [ "$$(id -u)" -ne 0 ]; then \
		echo "[\e36mcops\e[0m]: \e[31mrun make install with root permission\e[0m"; \
		exit 1; \
	fi
	@echo -e "[\e[36mcops\e[0m]: \e[35muninstalling headers...\e[0m"
	@rm -rv /usr/include/cops
	@echo -e "[\e[36mcops\e[0m]: \e[32mremoved headers succesfully\e[0m"

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
