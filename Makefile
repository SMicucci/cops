PROJ = cops
TARGETs_d = src
DIRS = $(patsubst %/., %,$(wildcard $(TARGETs_d)/*/.))

INCL_d = /usr/local/include/
LIB_d = /usr/local/lib/

.PHONY: build debug init clean

build: init
	@for dir in $(DIRS); do \
		echo "enter $$dir"; \
		make -s -C $$dir $@; \
		echo "exit $$dir"; \
	done;

debug: init
	@for dir in $(DIRS); do \
		echo "enter $$dir"; \
		make -s -C $$dir $@; \
		echo "exit $$dir"; \
	done;

init:
	@echo "link headers"
	@for dir in $(DIRS); do \
		target=$$(echo "$$dir" | sed 's;[^/]*/;;'); \
		sudo mkdir -p /usr/local/include/cops/$$target; \
		echo "enter $$dir"; \
		make -s -C $$dir init; \
		echo "exit $$dir"; \
		done;

clean:
	@sudo rm -rf $(INCL_d)/$(PROJ)
	@for dir in $(DIRS); do \
		echo "enter $$dir"; \
		make -s -C $$dir $@; \
		echo "exit $$dir"; \
	done;
