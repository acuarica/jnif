
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
CFLAGS= -W -Wall -Wextra -O3 -Wno-unused-parameter -I$(JAVA_HOME)/include
MAINCLASS=frheapagent.HeapTest
MAINCLASSFILE=$(subst .,/,$(MAINCLASS)).class
MAINJAVAFILE=$(subst .,/,$(MAINCLASS)).java

.PHONY: all run

all: $(BUILD)/$(MAINCLASSFILE) $(BUILD)/$(LIB)

runall:
	cd $(BUILD) && time java $(JVMARGS) -agentpath:$(LIB)=Empty $(MAINCLASS)
	cd $(BUILD) && time java $(JVMARGS) -agentpath:$(LIB)=Dump $(MAINCLASS)
	cd $(BUILD) && time java $(JVMARGS) -agentpath:$(LIB)=Print $(MAINCLASS)
	cd $(BUILD) && time java $(JVMARGS) -agentpath:$(LIB)=Identity $(MAINCLASS)
	cd $(BUILD) && time java $(JVMARGS) -agentpath:$(LIB)=ObjectInit $(MAINCLASS)
	cd $(BUILD) && time java $(JVMARGS) -agentpath:$(LIB)=NewArray $(MAINCLASS)
	cd $(BUILD) && time java $(JVMARGS) -agentpath:$(LIB)=Main $(MAINCLASS)

run: $(BUILD)/$(MAINCLASSFILE) $(BUILD)/$(LIB)
	cd $(BUILD) && time java $(JVMARGS) -agentpath:$(LIB)=ANewArray $(MAINCLASS)

$(BUILD)/$(MAINCLASSFILE): $(MAINJAVAFILE)
	javac $^ -d $(BUILD)

$(BUILD)/$(LIB): $(OBJS) $(BUILD)/libjnif.a
	clang++ -fPIC -lpthread -shared -lstdc++ -stdlib=libc++ -o $@ $^

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
