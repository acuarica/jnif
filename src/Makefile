
BUILD=../build
OUTPUT=$(BUILD)/libjnif.a
INCS=$(wildcard *.hpp)
SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:%=$(BUILD)/%.o)
CFLAGS += -fPIC -W -O0 -g -Wall -Wextra

.PHONY: all
all: $(OUTPUT)

.PHONY: clean
clean:
	rm -rf $(BUILD)

$(OUTPUT): $(OBJS)
	ar crv $@ $^

$(BUILD)/%.cpp.o: %.cpp $(INCS) | $(BUILD)
	$(LINK.c) -std=c++11 -stdlib=libc++ -c -o $@ $<

$(BUILD):
	mkdir -p $(BUILD)
