TARGET = process
SRC = process.c
CC = gcc 
FILE_LOWLEVEL = lowlevel.bin
FILE_STANDARD = standard.bin
FILE_MMAP = mmap.bin
FILE_COPY_LOWLEVEL = lowlevel_copy.bin
FILE_COPY_STANDARD = standard_copy.bin
FILE_COPY_MMAP = mmap_copy.bin
FILE_SIZE = 000000000

.PHONY: all clean create_lowlevel create_standard create_mmap copy_lowlevel copy_standard copy_mmap

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

create_lowlevel: $(TARGET)
	./$(TARGET) create_lowlevel $(FILE_LOWLEVEL) $(FILE_SIZE)

create_standard: $(TARGET)
	./$(TARGET) create_standard $(FILE_STANDARD) $(FILE_SIZE)

create_mmap: $(TARGET)
	./$(TARGET) create_mmap $(FILE_MMAP) $(FILE_SIZE)

copy_lowlevel: $(TARGET)
	./$(TARGET) copy_lowlevel $(FILE_LOWLEVEL) $(FILE_COPY_LOWLEVEL)

copy_standard: $(TARGET)
	./$(TARGET) copy_standard $(FILE_STANDARD) $(FILE_COPY_STANDARD)

copy_mmap: $(TARGET)
	./$(TARGET) copy_mmap $(FILE_MMAP) $(FILE_COPY_MMAP)

clean:
	rm -f $(TARGET) $(FILE_LOWLEVEL) $(FILE_STANDARD) $(FILE_MMAP) $(FILE_COPY_LOWLEVEL) $(FILE_COPY_STANDARD) $(FILE_COPY_MMAP)