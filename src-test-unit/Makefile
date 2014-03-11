
INCLUDE=../src
BUILD=../build

EXE=test-unit
HPPS=$(wildcard $(INCLUDE)/*.hpp)
SRCS=$(wildcard *.cpp) $(wildcard jnif/*.java)
OBJS=$(SRCS:%=$(BUILD)/%.o)

CFLAGS += -fPIC -W -O3 -Wall -Wextra -I$(INCLUDE)

.PHONY: all run

all: $(BUILD)/$(EXE)

run: $(BUILD)/$(EXE)
	$(BUILD)/$(EXE) > $(BUILD)/$(EXE).out

$(BUILD)/$(EXE): $(OBJS) $(BUILD)/libjnif.a
	clang++ $(CFLAGS) -stdlib=libc++ -o $@ $^

$(BUILD)/%.java.o: %.java
	javac -d $(BUILD)/ $<
	cd $(BUILD) ; xxd -i $*.class $*.c
	$(LINK.c) -c -o $@ $(BUILD)/$*.c

$(BUILD)/%.cpp.o: %.cpp $(HPPS) | $(BUILD)
	$(LINK.c) -std=c++11 -stdlib=libc++ -c -o $@ $<

$(BUILD)/libjnif.a:
	$(MAKE) -C ../src
