LIB_n = core

CC = gcc
CPPFLAG = -std=c99 -Wall -Wextra -pedantic -Werror -O3
DBFLAG = -g -O0
LBFLAG = 

SRC_d = src
OBJ_d = obj
BIN_d = bin
BIN = libcops_$(LIB_n).so

SRCs = $(wildcard $(SRC_d)/*.c)
OBJs = $(patsubst $(SRC_d)/%, $(OBJ_d)/%, $(SRCs:.c=.o))

.PHONY: build debug clean dirs init

debug: CPPFLAG += $(DBFLAG)
debug: build

build: dirs $(BIN_d)/$(BIN)
	sudo cp -f $(BIN_d)/$(BIN) /usr/local/lib/$(BIN_n)

dirs:
	mkdir -p $(BIN_d) $(OBJ_d)

$(OBJ_d)/%.o: $(SRC_d)/%.c
	$(CC) -c -fPIC $(CPPFLAG) -o $@ $<

$(BIN_d)/$(BIN): $(OBJs)
	$(CC) -shared -o $@ $^ $(LBFLAG)

clean:
	@rm -rf $(BIN_d) $(OBJ_d)
	@sudo rm -f /usr/local/lib/$(BIN)

init:
	@echo "> link header to default directories"
	for h in $$PWD/include/*.h; do \
		filename=$$(echo $$h | sed 's;^\(.*\)/\([^/]*\.h\);\2;');\
		sudo ln -sf $$h /usr/local/include/cops/$(LIB_n)/$$filename; \
		done
