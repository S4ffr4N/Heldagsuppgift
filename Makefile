CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c89 \
	-Iclient/include \
	-Icore/include \
	-Ilibs/include

LDFLAGS = 

SRC = \
	client/src/main.c \
	core/src/tcp_client.c \
	libs/src/http_client.c

OBJ = $(SRC:.c=.o)

TARGET = client_app

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)
