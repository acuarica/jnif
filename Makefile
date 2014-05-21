
BENCHS=avrora batik eclipse fop h2 jython luindex lusearch pmd sunflow tomcat tradebeans tradesoap xalan
INSTRS=Print Identity ObjectInit NewArray ANewArray Main ClientServer

ifneq (, $(wildcard Makefile.local))
include Makefile.local
endif

BUILD=build

LIBJNIF=$(BUILD)/libjnif.a
LIBJNIF_SRC=src
LIBJNIF_HPPS=$(wildcard $(LIBJNIF_SRC)/*.hpp)
LIBJNIF_SRCS=$(wildcard $(LIBJNIF_SRC)/*.cpp)
LIBJNIF_OBJS=$(LIBJNIF_SRCS:$(LIBJNIF_SRC)/%=$(BUILD)/%.o)

TESTUNIT=$(BUILD)/testunit
TESTUNIT_SRC=src-testunit
TESTUNIT_HPPS=$(wildcard $(TESTUNIT_SRC)/*.hpp)
TESTUNIT_SRCS=$(wildcard $(TESTUNIT_SRC)/*.cpp)
TESTUNIT_OBJS=$(TESTUNIT_SRCS:$(TESTUNIT_SRC)/%=$(BUILD)/%.o)

TESTAGENT=$(BUILD)/libtestagent.dylib
TESTAGENT_SRC=src-testagent
TESTAGENT_HPPS=$(wildcard $(TESTAGENT_SRC)/*.hpp)
TESTAGENT_SRCS=$(wildcard $(TESTAGENT_SRC)/*.cpp) $(wildcard $(TESTAGENT_SRC)/frproxy/*.java)
TESTAGENT_OBJS=$(TESTAGENT_SRCS:$(TESTAGENT_SRC)/%=$(BUILD)/%.o)

TESTAPP=$(BUILD)/testapp.jar
TESTAPP_SRC=src-testapp
TESTAPP_SRCS=$(wildcard $(TESTAPP_SRC)/*/*.java)
TESTAPP_OBJS=$(TESTAPP_SRCS:$(TESTAPP_SRC)/%.java=$(BUILD)/testapp/%.class)

JARS=$(wildcard jars/*.jar)
DIRS=$(JARS:jars/%.jar=$(BUILD)/%)

CFLAGS += -fPIC -W -O0 -g -Wall -Wextra 

.PHONY: all docs clean show testunit testapp testdacapo

all: $(LIBJNIF) $(TESTUNIT) $(TESTAGENT) $(TESTAPP)

docs:
	doxygen

clean:
	rm -rf $(BUILD)

show:
	@echo [Configuration]
	@echo BUILD: $(BUILD)
	@echo LIBJNIF: $(LIBJNIF)
	@echo LIBJNIF_SRC: $(LIBJNIF_SRC)
	@echo LIBJNIF_HPPS: $(LIBJNIF_HPPS)
	@echo LIBJNIF_SRCS: $(LIBJNIF_SRCS)
	@echo LIBJNIF_OBJS: $(LIBJNIF_OBJS)
	@echo TESTUNIT: $(TESTUNIT)
	@echo TESTUNIT_SRC: $(TESTUNIT_SRC)
	@echo TESTUNIT_HPPS: $(TESTUNIT_HPPS)
	@echo TESTUNIT_SRCS: $(TESTUNIT_SRCS)
	@echo TESTUNIT_OBJS: $(TESTUNIT_OBJS)
	@echo TESTAGENT: $(TESTAGENT)
	@echo TESTAGENT_SRC: $(TESTAGENT_SRC)
	@echo TESTAGENT_HPPS: $(TESTAGENT_HPPS)
	@echo TESTAGENT_SRCS: $(TESTAGENT_SRCS)
	@echo TESTAGENT_OBJS: $(TESTAGENT_OBJS)
	@echo TESTAPP: $(TESTAPP)
	@echo TESTAPP_SRC: $(TESTAPP_SRC)
	@echo TESTAPP_SRCS: $(TESTAPP_SRCS)
	@echo TESTAPP_OBJS: $(TESTAPP_OBJS)
	@echo JARS: $(JARS)
	@echo DIRS: $(DIRS)
	@echo CFLAGS: $(CFLAGS)

#
# Rules to run $(TESTUNIT)
#
testunit: $(TESTUNIT) $(DIRS)
	$(TESTUNIT) $(BUILD) > $(TESTUNIT).log

$(BUILD)/%: jars/%.jar
	unzip $< -d $@

#
# Rules to run $(TESTAPP)
#
testapp: $(TESTAGENT) $(TESTAPP)
	$(JAVA) $(JVMARGS) -agentpath:$(TESTAGENT)=Compute:build/ -jar $(TESTAPP)

#
# Rules to run $(TESTDACAPO)
#
testdacapo: $(TESTAGENT)
	time $(JAVA) -agentpath:$(TESTAGENT)=Compute:build/ -jar jars/dacapo-9.12-bach.jar --scratch-directory $(BUILD)/scratch $(BENCHS)

#
# Rules for $(LIBJNIF)
#
$(LIBJNIF): $(LIBJNIF_OBJS)
	ar crv $@ $^

$(BUILD)/%.cpp.o: $(LIBJNIF_SRC)/%.cpp $(LIBJNIF_HPPS) | $(BUILD)
	$(LINK.c) -std=c++11 -stdlib=libc++ -c -o $@ $<

#
# Rules for $(TESTUNIT)
#
$(TESTUNIT): $(TESTUNIT_OBJS) $(LIBJNIF)
	clang++ $(CFLAGS) -fsanitize=leak -stdlib=libc++ -o $@ $^

$(BUILD)/%.cpp.o: $(TESTUNIT_SRC)/%.cpp $(TESTUNIT_HPPS) | $(BUILD)
	$(LINK.c) -std=c++11 -stdlib=libc++ -I$(LIBJNIF_SRC) -c -o $@ $<

#
# Rules for $(TESTAGENT)
#
$(TESTAGENT): $(TESTAGENT_OBJS) $(LIBJNIF)
	clang++ $(CFLAGS) -fPIC -O0 -g -lpthread -shared -lstdc++ -stdlib=libc++ -o $@ $^

$(BUILD)/%.cpp.o: $(TESTAGENT_SRC)/%.cpp $(TESTAGENT_HPPS) | $(BUILD)
	$(LINK.c) -std=c++11 -stdlib=libc++ -I$(LIBJNIF_SRC) -Wno-unused-parameter -I$(JAVA_HOME)/include -c -o $@ $<

$(BUILD)/%.java.o: $(TESTAGENT_SRC)/%.java
	$(JAVAC) -d $(BUILD)/ $<
	cd $(BUILD) ; xxd -i $*.class $*.c
	$(LINK.c) -c -o $@ $(BUILD)/$*.c

#
# Rules for $(TESTAPP)
#
$(TESTAPP): $(TESTAPP_SRC)/META-INF/MANIFEST.MF $(TESTAPP_OBJS)
	jar cvfm $@ $< -C $(BUILD)/testapp .

$(BUILD)/testapp/%.class: $(TESTAPP_SRC)/%.java | $(BUILD)/testapp
	$(JAVAC) -sourcepath $(TESTAPP_SRC) -d $(BUILD)/testapp $<

$(BUILD)/testapp:
	mkdir -p $(BUILD)/testapp

#
# Basic rules
#
$(BUILD):
	mkdir -p $(BUILD)
