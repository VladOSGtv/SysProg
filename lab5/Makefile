TARGET = lab5
SRC = lab5.c
CC = gcc 
FILE_LOWLEVEL = lowlevel.bin
FILE_STANDARD = standard.bin
FILE_MMAP = mmap.bin
FILE_COPY_LOWLEVEL = lowlevel_copy.bin
FILE_COPY_STANDARD = standard_copy.bin
FILE_COPY_MMAP = mmap_copy.bin
FILE_SIZE = 400000000

.PHONY: all clean create_lowlevel create_standard create_mmap copy_lowlevel copy_standard copy_mmap

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

doAll: $(TARGET)
	$(CC) -o $(TARGET) $(SRC)
	./$(TARGET) create_lowlevel $(FILE_LOWLEVEL) $(FILE_SIZE)
	./$(TARGET) create_standard $(FILE_STANDARD) $(FILE_SIZE)
	./$(TARGET) create_mmap $(FILE_MMAP) $(FILE_SIZE)
	./$(TARGET) copy_lowlevel $(FILE_LOWLEVEL) $(FILE_COPY_LOWLEVEL)
	./$(TARGET) copy_standard $(FILE_STANDARD) $(FILE_COPY_STANDARD)
	./$(TARGET) copy_mmap $(FILE_MMAP) $(FILE_COPY_MMAP)
	
	rm -f $(TARGET) $(FILE_LOWLEVEL) $(FILE_STANDARD) $(FILE_MMAP) $(FILE_COPY_LOWLEVEL) $(FILE_COPY_STANDARD) $(FILE_COPY_MMAP)