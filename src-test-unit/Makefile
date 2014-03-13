
INCLUDE=../src
BUILD=../build

EXE=test-unit
HPPS=$(wildcard $(INCLUDE)/*.hpp)
SRCS=$(wildcard *.cpp) $(wildcard jnif/*.java) $(wildcard classes/*.class)
OBJS=$(SRCS:%=$(BUILD)/%.o)

CFLAGS += -fPIC -W -O0 -Wall -Wextra -I$(INCLUDE)

.PHONY: all run

all: $(BUILD)/$(EXE)

run: $(BUILD)/$(EXE)
	$(BUILD)/$(EXE) > $(BUILD)/$(EXE).out

$(BUILD)/$(EXE): $(OBJS) $(BUILD)/libjnif.a
	clang++ $(CFLAGS) -fsanitize=leak -stdlib=libc++ -o $@ $^

$(BUILD)/%.class.o: %.class
	mkdir -p $(@D) ; cp $< $(@D)
	cd $(BUILD) ; xxd -i $*.class $*.c
	$(LINK.c) -c -o $@ $(BUILD)/$*.c

$(BUILD)/%.java.o: %.java
	javac -d $(BUILD)/ $<
	cd $(BUILD) ; xxd -i $*.class $*.c
	$(LINK.c) -c -o $@ $(BUILD)/$*.c

$(BUILD)/%.cpp.o: %.cpp $(HPPS) | $(BUILD)
	$(LINK.c) -std=c++11 -stdlib=libc++ -c -o $@ $<

$(BUILD)/libjnif.a:
	$(MAKE) -C ../src
