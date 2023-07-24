current_dir := $(shell pwd)

CC = g++
CFLAGS = -Wall -Wextra
TARGET = test
SRCS = test.cpp
LDFLAGS = -L${current_dir} -luplinkc
FLAGS = -DUPLINK_DISABLE_NAMESPACE_COMPAT

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) ${FLAGS} -o $@ $(SRCS) $(LDFLAGS)

clean:
	rm -f $(TARGET)