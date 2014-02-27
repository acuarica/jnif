
ifneq (, $(wildcard Makefile.local))
include Makefile.local
endif

INCLUDE=src
BUILD=build

INCS=$(wildcard $(INCLUDE)/*.h)
HPPS=$(wildcard $(INCLUDE)/**/*.hpp) $(wildcard $(INCLUDE)/**/parser/*.hpp)

agent:	OUTPUTNAME=libfrheapagent
agent:	SRC=src-test-agent
agent:	SRCS:=$(wildcard $(SRC)/*.cpp)
agent:	OBJS:=$(SRCS:$(SRC)/%=$(BUILD)/%.o) $(BUILD)/__FrInstrProxy.c.o
agent:	OUTPUT=$(BUILD)/$(OUTPUTNAME)
agent:	CFLAGS += -fPIC -W -Wall -Wextra -O3 -Wno-unused-parameter -lpthread -shared -I$(JAVA_HOME)/include -Isrc -Iinclude

unit:	OUTPUTNAME=test-unit
unit:	SRC=src-test-unit
unit:	SRCS:=$(wildcard $(SRC)/*.cpp)
unit:	OBJS:=$(SRCS:$(SRC)/%=$(BUILD)/%.o)
unit:	OUTPUT=$(BUILD)/$(OUTPUTNAME)
unit:	CFLAGS += -fPIC -W -Wall -Wextra -O3 -Wno-unused-parameter -I$(JAVA_HOME)/include -Isrc -Iinclude

.PHONY: all agent unit clean show

all: agent unit
#$(LIB) $(BUILD)/frproxy/FrInstrProxy.class

agent: $(BUILD)/java-test/frheapagent/HeapTest.class $(OUTPUT)
	@echo $(SRC) $(OBJS)
	java $(JDEBUG) -agentpath:$(OUTPUT) -Xbootclasspath/a:$(BUILD) -cp $(BUILD)/java-test frheapagent.HeapTest

.SECONDEXPANSION:

unit: | $(BUILD)
	$(MAKE) -C src-test-unit

$(BUILD)/frproxy/FrInstrProxy.class: resources/frproxy/FrInstrProxy.java
	javac -d build/ resources/frproxy/FrInstrProxy.java

$(BUILD)/__FrInstrProxy.c.o: $(BUILD)/frproxy/FrInstrProxy.class
	cd $(BUILD) ; \
	xxd -i frproxy/FrInstrProxy.class __FrInstrProxy.c
	$(LINK.c) -c -o $@ $(BUILD)/__FrInstrProxy.c

$(BUILD)/java-test/frheapagent/HeapTest.class: java-test/frheapagent/HeapTest.java
	mkdir -p $(BUILD)/java-test
	javac java-test/frheapagent/HeapTest.java -d $(BUILD)/java-test

$(BUILD)/libfrheapagent.jnilib: $$(OBJS)
	clang++ $(CFLAGS) -stdlib=libc++ -o $@ $^

$(BUILD)/test-unit: $$(OBJS)
	@echo $(OBJS)
	@echo $^
#clang++ $(CFLAGS) -stdlib=libc++ -o $@ $^

$(BUILD)/%.c.o: $$(SRC)/%.c $(INCS) | $(BUILD)
	$(LINK.c) -c -o $@ $<
	
$(BUILD)/%.cpp.o: $$(SRC)/%.cpp $(INCS) $(HPPS) | $(BUILD)
	echo $(SRC) hoaaaaa
	$(LINK.c) -std=c++11 -stdlib=libc++ -c -o $@ $<

$(BUILD):
	mkdir -p $(BUILD)

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
