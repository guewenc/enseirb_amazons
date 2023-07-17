# Set default values for GSL_PATH and CC
GSL_PATH ?= /net/ens/renault/save/gsl-2.6/install
CC := gcc

# Compiler flags
ifeq ($(TURBO), true)
	CFLAGS := -std=c99 -Wall -Wextra -fPIC -O3 -I$(GSL_PATH)/include
else
	CFLAGS := -std=c99 -Wall -Wextra -fPIC -g -I$(GSL_PATH)/include
endif

# Linker flags
LDFLAGS := -lm -lgsl -lgslcblas -ldl \
        -L$(GSL_PATH)/lib \
        -Wl,-rpath,$(GSL_PATH)/lib

# Directories
INSTALL_DIR := install
SRC_DIR := src
COMMON_DIR := $(SRC_DIR)/common
SERVER_DIR := $(SRC_DIR)/server
CLIENT_DIR := $(SRC_DIR)/client
TEST_DIR := tst

# Targets
SERVER_BIN := server
TEST_BIN := alltests

# Include directories
SERVER_DIR_INC := $(SRC_DIR)/common

# Main sources files
SERVER_MAIN_SRC = server.c
TEST_MAIN_SRC = test_main.c

# Source files
COMMON_SRC := utils.c graph.c queens.c move.c
SERVER_SRC := client_api.c game.c shape.c export.c
CLIENT_COMMON_SRC := player_common.c
CLIENT_SRC := $(filter-out $(addprefix $(CLIENT_DIR)/, $(CLIENT_COMMON_SRC)), $(wildcard $(CLIENT_DIR)/*.c))
TEST_SRC := $(filter-out $(TEST_MAIN_SRC), $(wildcard $(TEST_DIR)/test_*.c))

# Object files
COMMON_OBJ := $(addprefix $(COMMON_DIR)/, $(COMMON_SRC:%.c=%.o))
SERVER_OBJ := $(addprefix $(SERVER_DIR)/, $(SERVER_SRC:%.c=%.o))
TEST_OBJ := $(TEST_SRC:%.c=%.o)
CLIENT_COMMON_OBJ := $(addprefix $(CLIENT_DIR)/, $(CLIENT_COMMON_SRC:%.c=%.o))

SERVER_MAIN_OBJ := $(SERVER_DIR)/$(SERVER_MAIN_SRC:%.c=%.o)
TEST_MAIN_OBJ := $(TEST_DIR)/$(TEST_MAIN_SRC:%.c=%.o)

# Dynamic libraries
CLIENT_LIB := $(CLIENT_SRC:%.c=%.so)

# Phony targets
.PHONY: all build client test install install_server install_test install_client clean clean_install clean_src clangformat

# Default target
all: build

# Build targets
build: $(SERVER_BIN) client $(TEST_BIN)

$(SERVER_BIN): $(SERVER_OBJ) $(COMMON_OBJ) $(SERVER_MAIN_OBJ)
	$(CC) $(CFLAGS) -I$(SERVER_DIR_INC) $^ -o $@ $(LDFLAGS)

%.so: %.o $(CLIENT_COMMON_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) --shared -I$(SERVER_DIR_INC) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -I$(SERVER_DIR_INC) -I$(SERVER_DIR) -c $^ -o $@

client: $(CLIENT_LIB)

# Test targets
test: $(TEST_BIN)

$(TEST_BIN): $(TEST_OBJ) $(COMMON_OBJ) $(SERVER_OBJ) $(TEST_MAIN_OBJ)
	$(CC) $(CFLAGS) -I$(SERVER_DIR_INC) $^ -o $@ $(LDFLAGS)

# Installation targets
install: install_server install_test install_client

install_server: $(SERVER_BIN)
	@mv $(SERVER_BIN) $(INSTALL_DIR)/$(SERVER_BIN)

install_test: test
	@mv $(TEST_BIN) $(INSTALL_DIR)/$(TEST_BIN)

install_client: client
	@mv $(CLIENT_DIR)/*.so $(INSTALL_DIR)

# Clean targets
clean_src:
	@rm -f $(SRC_DIR)/*/*.o $(SRC_DIR)/*/*.gcno $(SRC_DIR)/*/*.gcda

clean_test:
	@rm -f $(TEST_DIR)/*.o $(TEST_DIR)/*.gcno $(TEST_DIR)/*.gcda

clean_install:
	@rm -f $(INSTALL_DIR)/*.so $(INSTALL_DIR)/$(TEST_BIN) $(INSTALL_DIR)/$(SERVER_BIN)

clean: clean_install clean_src clean_test
	@rm -f *~ $(SRC_DIR)/*~ $(TEST_DIR)/*~ $(CLIENT_LIB) $(SERVER_BIN) $(TEST_BIN)

# Clang-format
clangformat:
	find . -iname *.h -o -iname *.c | xargs clang-format -i

gif:
	convert -delay 10 -loop 0 *.png animation.gif

doc:
	doxygen dgenerate