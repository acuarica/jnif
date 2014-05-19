
INCLUDE=../src
BUILD=../build

EXE=test-unit
HPPS=$(wildcard $(INCLUDE)/*.hpp)
SRCS=$(wildcard jnif/*.java) $(wildcard *.cpp)
OBJS=$(SRCS:%=$(BUILD)/%.o)
CFLAGS += -fPIC -W -O0 -Wall -Wextra -I$(INCLUDE) 
#-fopenmp

#CLSS=$(shell find . -name "*.class")

.PHONY: compile
compile: $(BUILD)/$(EXE)

.PHONY: run
run: compile
	cd $(BUILD) && ./$(EXE)

$(BUILD)/$(EXE): $(OBJS) $(BUILD)/libjnif.a
	clang++ $(CFLAGS) -fsanitize=leak -stdlib=libc++ -o $@ $^

$(BUILD)/%.java.o: %.java | $(BUILD)
	javac -d $(BUILD)/ $<
	cd $(BUILD) ; xxd -include $*.class $*.c
	$(LINK.c) -c -o $@ $(BUILD)/$*.c

$(BUILD)/%.cpp.o: %.cpp $(HPPS) | $(BUILD)
	$(LINK.c) -std=c++11 -stdlib=libc++ -c -o $@ $<

$(BUILD)/libjnif.a: FORCE
	$(MAKE) -C ../src

$(BUILD):
	mkdir -p $(BUILD)
	mkdir -p $(BUILD)/instr

.PHONY: FORCE
FORCE:
