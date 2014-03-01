
ifneq (, $(wildcard ../Makefile.local))
include ../Makefile.local
endif

INCLUDE=../src
BUILD=../build

INCS=$(wildcard $(INCLUDE)/*.h)
HPPS=$(wildcard $(INCLUDE)/**/*.hpp) $(wildcard $(INCLUDE)/**/parser/*.hpp)

SRC=.
OUTPUTNAME=libfrheapagent.jnilib
SRCS=$(wildcard $(SRC)/*.c) $(wildcard $(SRC)/*.cpp) $(wildcard $(SRC)/*/*.java)
OBJS=$(SRCS:$(SRC)/%=$(BUILD)/%.o) 
OUTPUT=$(BUILD)/$(OUTPUTNAME)
CFLAGS += -fPIC -W -Wall -Wextra -O3 -Wno-unused-parameter -lpthread -shared -I$(JAVA_HOME)/include -I../src -I../include

.PHONY: all

#$(LIB) $(BUILD)/frproxy/FrInstrProxy.class

all: $(BUILD)/java-test/frheapagent/HeapTest.class $(OUTPUT)
	java $(JVMARGS) -agentpath:$(OUTPUT)=$(BUILD) -Xbootclasspath/a:$(BUILD) -cp $(BUILD)/java-test frheapagent.HeapTest

#$(BUILD)/frproxy/FrInstrProxy.class: resources/frproxy/FrInstrProxy.java
#	javac -d build/ resources/frproxy/FrInstrProxy.java

$(BUILD)/java-test/frheapagent/HeapTest.class: ../java-test/frheapagent/HeapTest.java
	mkdir -p $(BUILD)/java-test
	javac ../java-test/frheapagent/HeapTest.java -d $(BUILD)/java-test

$(OUTPUT): $(OBJS)
	clang++ $(CFLAGS) -stdlib=libc++ -o $@ $^

$(BUILD)/%.java.o: $(SRC)/%.java
	$(JAVAC) -d $(BUILD)/ $<
	cd $(BUILD) ; xxd -i $*.class $*.c
	$(LINK.c) -c -o $@ $(BUILD)/$*.c

$(BUILD)/%.c.o: %.c $(INCS)
	$(LINK.c) -c -o $@ $<

$(BUILD)/%.cpp.o: %.cpp $(INCS) $(HPPS)
	$(LINK.c) -std=c++11 -stdlib=libc++ -c -o $@ $<
