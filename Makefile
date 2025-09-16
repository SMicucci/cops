CFLAGS = -Wall -Wextra -Wpedantic -Werror -std=c99 -fPIC
TFLAG = -fsanitize=address,undefined,leak -fno-omit-frame-pointer -g -O0
RFLAG = -O3
LDFLAGS = -shared -Wl,-soname,libcops.so.1

SRC_d = src
TEST_d = test
OBJ_d = obj
BIN_d = bin

SRCs = $(wildcard $(SRC_d)/*.c)
TESTs = $(wildcard $(TEST_d)/*.c)
T_OBJs = $(patsubst $(TEST_d)/%.c,$(OBJ_d)/%.o,$(TESTs))
S_OBJs = $(patsubst $(SRC_d)/%.c,$(OBJ_d)/%.o,$(SRCs))

TEST = $(BIN_d)/test
LIB = $(BIN_d)/libcops.so

.PHONY: all test install uninstall clean

all: $(LIB)
	@echo -e "[\e[36mcops\e[0m]: \e[32mdynamic library created!\e[0m"

test: $(TEST)
	@echo -e "[\e[36mcops\e[0m]: \e[32mtest created!\e[0m"

install:
	@if [ "$$(id -u)" -ne 0 ]; then \
		echo "[\e[36mcops\e[0m]: \e[31mrun make install with root permission\e[0m"; \
		exit 1; \
	fi
	@echo -e "[\e[36mcops\e[0m]: \e[35minstalling headers...\e[0m"
	@mkdir -p /usr/include/cops
	@cp -v $(wildcard src/*.h) /usr/include/cops
	@echo -e "[\e[36mcops\e[0m]: \e[32minstalled headers succesfully\e[0m"

uninstall:
	@if [ "$$(id -u)" -ne 0 ]; then \
		echo "[\e[36mcops\e[0m]: \e[31mrun make install with root permission\e[0m"; \
		exit 1; \
	fi
	@echo -e "[\e[36mcops\e[0m]: \e[35muninstalling headers...\e[0m"
	@rm -rv /usr/include/cops
	@echo -e "[\e[36mcops\e[0m]: \e[32mremoved headers succesfully\e[0m"

$(OBJ_d)/%.o: $(TEST_d)/%.c | $(OBJ_d)
	@echo -e "[\e[36mcops\e[0m]: \e[34mbuilding "$@"\e[0m..."
	@$(CC) $(CFLAGS) $(TFLAG) -c $< -o $@

$(OBJ_d)/%.o: $(SRC_d)/%.c | $(OBJ_d)
	@echo -e "[\e[36mcops\e[0m]: \e[34mbuilding "$@"\e[0m..."
	@$(CC) $(CFLAGS) $(RFLAG) -c $< -o $@

$(OBJ_d) $(BIN_d):
	@mkdir -p $@

$(TEST): $(T_OBJs) | $(BIN_d)
	@echo -e "[\e[36mcops\e[0m]: \e[33mcreating "$@"\e[0m..."
	@$(CC) $(CFLAGS) $(TFLAG) $^ -o $@


$(LIB): $(S_OBJs) | $(BIN_d)
	@echo -e "[\e[36mcops\e[0m]: \e[33mcreating "$@"\e[0m..."
	@$(CC) -shared $(CFLAGS) $(LDFLAGS) $^ -o $@

clean:
	@echo -e "[\e[36mcops\e[0m]: \e[31mcleaning reference\e[0m..."
	@rm -rf $(BIN_d) $(OBJ_d)
