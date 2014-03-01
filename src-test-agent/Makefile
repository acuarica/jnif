
ifneq (, $(wildcard ../Makefile.local))
include ../Makefile.local
endif

INCLUDE=../src
BUILD=../build

INCS=$(wildcard $(INCLUDE)/*.h)
HPPS=$(wildcard $(INCLUDE)/**/*.hpp) $(wildcard $(INCLUDE)/**/parser/*.hpp)

SRC=.
OUTPUTNAME=libfrheapagent.jnilib
SRCS=$(wildcard $(SRC)/*.c) $(wildcard $(SRC)/*.cpp) $(wildcard $(SRC)/**/*.java)
OBJS=$(SRCS:$(SRC)/%=$(BUILD)/%.o)
OUTPUT=$(BUILD)/$(OUTPUTNAME)
CFLAGS += -fPIC -W -Wall -Wextra -O3 -Wno-unused-parameter -lpthread -shared -I$(JAVA_HOME)/include -I../src -I../include

.PHONY: all agent unit clean show

all: agent unit
#$(LIB) $(BUILD)/frproxy/FrInstrProxy.class

agent: $(BUILD)/java-test/frheapagent/HeapTest.class $(OUTPUT)
	@echo $(SRC) $(OBJS)
	java $(JDEBUG) -agentpath:$(OUTPUT)=$(BUILD) -Xbootclasspath/a:$(BUILD) -cp $(BUILD)/java-test frheapagent.HeapTest

$(BUILD)/frproxy/FrInstrProxy.class: resources/frproxy/FrInstrProxy.java
	javac -d build/ resources/frproxy/FrInstrProxy.java

$(BUILD)/__FrInstrProxy.c.o: $(BUILD)/frproxy/FrInstrProxy.class
	cd $(BUILD) ; \
	xxd -i frproxy/FrInstrProxy.class __FrInstrProxy.c
	$(LINK.c) -c -o $@ $(BUILD)/__FrInstrProxy.c

$(BUILD)/java-test/frheapagent/HeapTest.class: ../java-test/frheapagent/HeapTest.java
	mkdir -p $(BUILD)/java-test
	javac ../java-test/frheapagent/HeapTest.java -d $(BUILD)/java-test

$(OUTPUT): $(OBJS)
	clang++ $(CFLAGS) -stdlib=libc++ -o $@ $^

$(BUILD)/%.c.o: $(SRC)/%.c $(INCS) | $(BUILD)
	$(LINK.c) -c -o $@ $<
	
$(BUILD)/%.cpp.o: $(SRC)/%.cpp $(INCS) $(HPPS) | $(BUILD)
	$(LINK.c) -std=c++11 -stdlib=libc++ -c -o $@ $<
