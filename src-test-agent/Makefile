
ifneq (, $(wildcard ../Makefile.local))
include ../Makefile.local
endif

INCLUDE=../src
BUILD=../build

INCS=$(wildcard *.h)
HPPS=$(wildcard $(INCLUDE)/*.hpp)

LIB=libfrheapagent.jnilib
SRCS=$(wildcard *.c) $(wildcard *.cpp) $(wildcard frproxy/*.java)
OBJS=$(SRCS:%=$(BUILD)/%.o)
JAVAS=$(wildcard frheapagent/*.java)
CLASSES=$(JAVAS:%.java=$(BUILD)/%.class)

DOTS=$(wildcard $(BUILD)/instr/*.dot)
PNGS=$(DOTS:%.dot=$(BUILD)/%.png)

CFLAGS= -W -Wall -Wextra -O0 -g -Wno-unused-parameter -I$(JAVA_HOME)/include
MAINCLASS=frheapagent.HeapTest
MAINCLASSFILE=$(subst .,/,$(MAINCLASS)).class
MAINJAVAFILE=$(subst .,/,$(MAINCLASS)).java
RUNINSTR=cd $(BUILD) && time java $(JVMARGS) -agentpath:$(LIB)

.PHONY: all
all: $(BUILD)/$(MAINCLASSFILE) $(BUILD)/$(LIB)

#	cd $(BUILD) && time java $(JVMARGS) -agentpath:$(LIB)=ObjectInit $(MAINCLASS)
#	cd $(BUILD) && time java $(JVMARGS) -agentpath:$(LIB)=NewArray $(MAINCLASS)
#	cd $(BUILD) && time java $(JVMARGS) -agentpath:$(LIB)=ANewArray $(MAINCLASS)
#	cd $(BUILD) && time java $(JVMARGS) -agentpath:$(LIB)=Main $(MAINCLASS)

.PHONY: instr-print
instr-print: instr
	$(RUNINSTR)=Print $(MAINCLASS)

.PHONY: instr-identity
instr-identity: instr
	$(RUNINSTR)=Identity $(MAINCLASS)

.PHONY: instr-compute
instr-compute: instr
	$(RUNINSTR)=Compute $(MAINCLASS)

.PHONY: instr
instr: $(CLASSES) $(BUILD)/$(LIB)

$(BUILD)/%.class: %.java
	javac $^ -d $(BUILD)

$(BUILD)/$(LIB): $(OBJS) $(BUILD)/libjnif.a
	clang++ -fPIC -O0 -g -lpthread -shared -lstdc++ -stdlib=libc++ -o $@ $^

$(BUILD)/%.java.o: %.java
	$(JAVAC) -d $(BUILD)/ $<
	cd $(BUILD) ; xxd -i $*.class $*.c
	$(LINK.c) -c -o $@ $(BUILD)/$*.c

$(BUILD)/%.c.o: %.c $(INCS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.cpp.o: %.cpp $(INCS) $(HPPS)
	$(LINK.c) -std=c++11 -stdlib=libc++ -I../src -c -o $@ $<

$(BUILD)/libjnif.a:
	$(MAKE) -C ../src

.PHONY: pngs
pngs: $(PNGS)

$(BUILD)/%.png: $(BUILD)/%.dot
	dot -Tpng $^ > $@
