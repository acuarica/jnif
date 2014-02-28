
INCLUDE=../src
BUILD=../build

INCS=$(wildcard $(INCLUDE)/*.h)
HPPS=$(wildcard $(INCLUDE)/**/*.hpp) $(wildcard $(INCLUDE)/**/parser/*.hpp)

SRC=.
OUTPUT=$(BUILD)/test-unit
SRCS=$(wildcard $(SRC)/*.cpp) $(wildcard $(SRC)/**/*.java)
OBJS=$(SRCS:$(SRC)/%=$(BUILD)/%.o)

CFLAGS += -fPIC -W -O3 -Wall -Wextra -Wno-unused-parameter -I../src -I../include

.PHONY: all

all: $(OUTPUT)
	$(OUTPUT)

$(OUTPUT): $(OBJS)	
	clang++ $(CFLAGS) -stdlib=libc++ -o $@ $^

$(BUILD)/%.java.o: $(SRC)/%.java
	javac -d $(BUILD)/ $<
	cd $(BUILD) ; xxd -i $*.class $*.c
	$(LINK.c) -c -o $@ $(BUILD)/$*.c

$(BUILD)/%.cpp.o: $(SRC)/%.cpp $(INCS) $(HPPS) | $(BUILD)
	$(LINK.c) -std=c++11 -stdlib=libc++ -c -o $@ $<
