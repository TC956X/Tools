
DEV_I2C ?= 1
MDIO_PHYID ?= 0

CC = gcc

CFLAGS = -Wall -DDEV_I2C=$(DEV_I2C)

TC9560_RAMLOADER_OBJS = tc9560_ramloader.o tc9560_i2c.o tc9560_spi.o tc9560_mem.o
TC956X_RAMLOADER_OBJS = tc956x_ramloader.o tc956x_i2c.o tc956x_spi.o tc956x_mem.o

TARGET = tc9560_ramloader tc956x_ramloader
#TARGET = tc9560_ramloader

.PHONY: all clean

all: $(TARGET)

tc9560_ramloader: $(TC9560_RAMLOADER_OBJS)
	$(CC) -o $@ $^

tc9560%.o: tc956x%.c
	$(CC) $(CFLAGS) -DTC9560 $(INCLUDES) -o $@ -c $<


tc956x_ramloader: $(TC956X_RAMLOADER_OBJS)
	$(CC) -o $@ $^

tc956x%.o: tc956x%.c
	$(CC) $(CFLAGS) -DTC956X $(INCLUDES) -o $@ -c $<

clean:
	rm -f *.o $(TARGET)

