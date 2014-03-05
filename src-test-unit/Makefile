
INCLUDE=../src
BUILD=../build

HPPS=$(wildcard $(INCLUDE)/*.hpp)

OUTPUT=$(BUILD)/test-unit
SRCS=$(wildcard *.cpp) $(wildcard jnif/*.java)
OBJS=$(SRCS:%=$(BUILD)/%.o)

CFLAGS += -fPIC -W -O3 -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter -I../src -I../include

.PHONY: all

all: $(OUTPUT)
	$(OUTPUT)

$(OUTPUT): $(OBJS) ../build/libjnif.a
	clang++ $(CFLAGS) -stdlib=libc++ -o $@ $^

$(BUILD)/%.java.o: %.java
	javac -d $(BUILD)/ $<
	cd $(BUILD) ; xxd -i $*.class $*.c
	$(LINK.c) -c -o $@ $(BUILD)/$*.c

$(BUILD)/%.cpp.o: %.cpp $(HPPS) | $(BUILD)
	$(LINK.c) -std=c++11 -stdlib=libc++ -c -o $@ $<
