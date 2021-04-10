# CC=gcc
# CFLAGS=-c   -std=c11
# LDFLAGS=
# SOURCES=test/test_main.c src/conv_133_171_encoder.c fs_io.c fs_cmd.c
# OBJECTS=$(SOURCES:.c=.o)
# EXECUTABLE=filesystem


# all: $(SOURCES) $(EXECUTABLE)

# $(EXECUTABLE): $(OBJECTS)
# 	$(CC) $(LDFLAGS) $(OBJECTS) -o $@


# .c.o:
# 	$(CC) $(CFLAGS) $< -o $@

# clear:
# 	rm -rf *.o filesystem



.PHONY: all clean

all: conv_codes Makefile

conv_codes: build/conv_codes build

build:
	@echo "[\033[01;32mMKDIR\033[00m]\t" $(notdir $@)
	@mkdir -p build/

build/conv_codes: test/test_main.c src/conv_133_171_encoder.c src src/conv_decoder.c | build
	@echo "[\033[01;32mCC\033[00m]\t" $(addprefix "\033[35m"build"\033[0m"/,$(notdir $@))
	@gcc -DSTANDALONE -Iinc/ $< -o $@

clean:
	@echo "[\033[01;32mRM\033[00m]\t" build
	@rm -rf build/
