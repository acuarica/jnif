
BUILD=../build

OUTPUT=$(BUILD)/libjnif.a
INCS=$(wildcard *.hpp)
SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:%=$(BUILD)/%.o)

CFLAGS += -fPIC -W -O3 -Wall -Wextra

.PHONY: all clean

all: $(OUTPUT)

$(OUTPUT): $(OBJS)
	ar crv $@ $^

$(BUILD)/%.cpp.o: %.cpp $(INCS) | $(BUILD)
	$(LINK.c) -std=c++11 -stdlib=libc++ -c -o $@ $<

$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)
