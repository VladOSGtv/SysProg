TARGET = lab6
SRC = lab6.c
RES = dest.txt
CC = gcc 

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(RES)
