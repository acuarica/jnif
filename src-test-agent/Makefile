
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
JVMARGS=-Xverify:none

all: agent unit
#$(LIB) $(BUILD)/frproxy/FrInstrProxy.class

agent: $(BUILD)/java-test/frheapagent/HeapTest.class $(OUTPUT)
	java $(JVMARGS) -agentpath:$(OUTPUT)=$(BUILD) -Xbootclasspath/a:$(BUILD) -cp $(BUILD)/java-test frheapagent.HeapTest

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
