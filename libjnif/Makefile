
ifneq (, $(wildcard Makefile.local))
include Makefile.local
endif

SRC=src-test
INCLUDE=src
BUILD=build

INCS=$(wildcard $(INCLUDE)/*.h)
HPPS=$(wildcard $(INCLUDE)/**/*.hpp) $(wildcard $(INCLUDE)/**/parser/*.hpp)
SRCS=$(wildcard $(SRC)/*.c) $(wildcard $(SRC)/*.cpp)
OBJS=$(SRCS:$(SRC)/%=$(BUILD)/%.o) $(BUILD)/__FrInstrProxy.c.o

LIBNAME=frheapagent
LIB=$(BUILD)/lib$(LIBNAME).jnilib

CFLAGS += -fPIC -W -Wall -Wextra -O3 -Wno-unused-parameter -I$(JAVA_HOME)/include -Isrc

.PHONY: all test clean show

#EVENTSPERSAMPLE=500
#XMS=2048m
#XMX=2048m
#JOPT="-Xint -Xms$XMS -Xmx$XMX"
# -verbose:class|gc|jni
#JDEBUG=-Xcheck:jni -Xdiag -verbose:jni
#java $JOPT -Xverify:none -agentpath:$AGENT=$EVENTSPERSAMPLE -Xbootclasspath/a:$PROXYBCP -cp $CP "$MAINCLASS"
#java -Xverify:none -agentpath:$AGENT=5000 -Xbootclasspath/a:$PROXYBCP -jar lib/dacapo-9.12-bach.jar avrora
#java -Xverify:none $JDEBUG $JOPT -agentpath:$AGENT=5000 -Xbootclasspath/a:$PROXYBCP -jar scala-benchmark-suite-0.1.0-20120216.103539-3.jar -t 1 -s tiny actors
#java -Xverify:none $JDEBUG $JOPT -agentpath:$AGENT=5000 -Xbootclasspath/a:$PROXYBCP -jar scala-benchmark-suite-0.1.0-20120216.103539-3.jar avrora

all: $(LIB) $(BUILD)/frproxy/FrInstrProxy.class

#-Xbootclasspath/a:$(BUILD)
#-Xverify:none
test: $(BUILD)/java-test/frheapagent/HeapTest.class $(LIB)
	java $(JDEBUG) -agentpath:$(LIB) -Xbootclasspath/a:$(BUILD) -cp $(BUILD)/java-test frheapagent.HeapTest

clean:
	rm -rf $(BUILD)

show:
	@echo INCS: $(INCS)
	@echo HPPS: $(HPPS)
	@echo SRCS: $(SRCS)
	@echo OBJS: $(OBJS)
	@echo LIBNAME: $(LIBNAME)
	@echo LIB: $(LIB)
	@echo CC: $(CC)
	@echo LINK.c: $(LINK.c)


$(BUILD)/frproxy/FrInstrProxy.class: resources/frproxy/FrInstrProxy.java
	javac -d build/ resources/frproxy/FrInstrProxy.java

$(BUILD)/__FrInstrProxy.c.o: $(BUILD)/frproxy/FrInstrProxy.class
	cd $(BUILD) ; \
	xxd -i frproxy/FrInstrProxy.class __FrInstrProxy.c
	$(LINK.c) -c -o $@ $(BUILD)/__FrInstrProxy.c

$(BUILD)/java-test/frheapagent/HeapTest.class: java-test/frheapagent/HeapTest.java
	mkdir -p $(BUILD)/java-test
	javac java-test/frheapagent/HeapTest.java -d $(BUILD)/java-test

$(LIB): $(OBJS)
	clang++ $(CFLAGS) -stdlib=libc++ -lpthread -shared -o $@ $^

$(BUILD)/%.c.o: $(SRC)/%.c $(INCS) | $(BUILD)
	$(LINK.c) -c -o $@ $<

$(BUILD)/%.cpp.o: $(SRC)/%.cpp $(INCS) $(HPPS) | $(BUILD)
	$(LINK.c) -std=c++11 -stdlib=libc++ -c -o $@ $<

$(BUILD):
	mkdir -p $(BUILD)
