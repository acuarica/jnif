
CLASSPATH=classes/
TESTNAME=testParser

INCLUDE=../src
BUILD=../build

EXE=test-unit
HPPS=$(wildcard $(INCLUDE)/*.hpp)
SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:%=$(BUILD)/%.o)
CFLAGS += -fPIC -W -O0 -Wall -Wextra -I$(INCLUDE) 

JAVAS=$(wildcard jnif/*.java)
CLSS=$(JAVAS:%.java=$(BUILD)/classes/test/%.class)

LIST=$(shell cd $(BUILD) && ./$(EXE) --list)

.PHONY: compile
compile: $(BUILD)/$(EXE) $(CLSS)

.PHONY: list
list: compile
	cd $(BUILD) && ./$(EXE) --list

.PHONY: all
all: compile
	cd $(BUILD) && $(foreach t,$(LIST),./$(EXE) $(CLASSPATH) $(t) > $(EXE).log;)

.PHONY: run
run: compile $(BUILD)/classes/rt $(BUILD)/classes/dacapo $(BUILD)/classes/dacapo-jars
	cd $(BUILD) && ./$(EXE) $(CLASSPATH) $(TESTNAME) > $(EXE).log

$(BUILD)/classes/rt:
	unzip rt.jar -d $@

$(BUILD)/classes/dacapo:
	unzip ../eval/dacapo-9.12-bach.jar -d $@

$(BUILD)/classes/dacapo-jars: $(BUILD)/classes/dacapo
	unzip -o '$(BUILD)/classes/dacapo/jar/*.jar' -d $@

$(BUILD)/classes/test/%.class: %.java | $(BUILD)
	javac -d $(BUILD)/classes/test/ $<

$(BUILD)/$(EXE): $(OBJS) $(BUILD)/libjnif.a
	clang++ $(CFLAGS) -fsanitize=leak -stdlib=libc++ -o $@ $^

$(BUILD)/%.cpp.o: %.cpp $(HPPS) | $(BUILD)
	$(LINK.c) -std=c++11 -stdlib=libc++ -c -o $@ $<

$(BUILD)/libjnif.a: FORCE
	$(MAKE) -C ../src

$(BUILD):
	mkdir -p $(BUILD)
	mkdir -p $(BUILD)/classes/test

.PHONY: clean
clean:
	rm -rf $(BUILD)

.PHONY: FORCE
FORCE:
