CC = gcc
CFLAGS = -Wall -Wextra -O2 -g

BIN_DIR = out
TARGET = $(BIN_DIR)/server

CLIENT_TARGET = $(BIN_DIR)/client

all: server client

server: server.c util.c
	$(CC) $(CFLAGS) server.c util.c network.c logger.c -o ./out/server

client: client.c util.c
	$(CC) $(CFLAGS) client.c util.c logger.c -o ./out/client

run: $(TARGET)
	./$(TARGET)

r_client: $(CLIENT_TARGET)
	./$(CLIENT_TARGET)

r_server: $(TARGET)
	./$(TARGET)

clean:
	rm -f server client

# CC = gcc
# CFLAGS = -Wall -Wextra -g
#
# BIN_DIR = bin
# OBJ_DIR = out
#
# TARGET = $(BIN_DIR)/app
#
# SRCS = main.c
# OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))
#
# all: $(TARGET)
#
# $(TARGET): $(OBJS) | $(BIN_DIR)
# 	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
#
# $(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
# 	$(CC) $(CFLAGS) -c $< -o $@
#
# $(BIN_DIR) $(OBJ_DIR):
# 	mkdir -p $@
#
# clean:
# 	rm -rf $(BIN_DIR) $(OBJ_DIR)
#
# run: $(TARGET)
# 	./$(TARGET)
#
# .PHONY: all clean run
