
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

INSTRSERVER=$(BUILD)/instrserver.jar
INSTRSERVER_SRC=src-instrserver
INSTRSERVER_SRCS=$(shell find $(INSTRSERVER_SRC) -name *.java)
INSTRSERVER_OBJS=$(BUILD)/instrserver/log4j.properties $(INSTRSERVER_SRCS:$(INSTRSERVER_SRC)/src/%.java=$(BUILD)/instrserver/%.class)
INSTRSERVER_CP=$(subst $(_EMPTY) $(_EMPTY),:,$(wildcard $(INSTRSERVER_SRC)/lib/*.jar))

JARS=$(wildcard jars/*.jar)
DIRS=$(JARS:jars/%.jar=$(BUILD)/%)

BENCHS=avrora batik eclipse fop h2 jython luindex lusearch pmd sunflow tomcat tradebeans tradesoap xalan
INSTRS=Compute
#INSTRS=Print Identity ObjectInit NewArray ANewArray Main ClientServer

CFLAGS += -fPIC -W -O0 -g -Wall -Wextra 

.PHONY: all docs clean show testunit testapp testdacapo

all: $(LIBJNIF) $(TESTUNIT) $(TESTAGENT) $(TESTAPP) $(INSTRSERVER)

docs:
	doxygen

clean:
	rm -rf $(BUILD)

show:
	@echo [Configuration]
	@echo BUILD: $(BUILD)
	@echo
	@echo LIBJNIF: $(LIBJNIF)
	@echo LIBJNIF_SRC: $(LIBJNIF_SRC)
	@echo LIBJNIF_HPPS: $(LIBJNIF_HPPS)
	@echo LIBJNIF_SRCS: $(LIBJNIF_SRCS)
	@echo LIBJNIF_OBJS: $(LIBJNIF_OBJS)
	@echo
	@echo TESTUNIT: $(TESTUNIT)
	@echo TESTUNIT_SRC: $(TESTUNIT_SRC)
	@echo TESTUNIT_HPPS: $(TESTUNIT_HPPS)
	@echo TESTUNIT_SRCS: $(TESTUNIT_SRCS)
	@echo TESTUNIT_OBJS: $(TESTUNIT_OBJS)
	@echo
	@echo TESTAGENT: $(TESTAGENT)
	@echo TESTAGENT_SRC: $(TESTAGENT_SRC)
	@echo TESTAGENT_HPPS: $(TESTAGENT_HPPS)
	@echo TESTAGENT_SRCS: $(TESTAGENT_SRCS)
	@echo TESTAGENT_OBJS: $(TESTAGENT_OBJS)
	@echo
	@echo TESTAPP: $(TESTAPP)
	@echo TESTAPP_SRC: $(TESTAPP_SRC)
	@echo TESTAPP_SRCS: $(TESTAPP_SRCS)
	@echo TESTAPP_OBJS: $(TESTAPP_OBJS)
	@echo
	@echo INSTRSERVER: $(INSTRSERVER)
	@echo INSTRSERVER_SRC: $(INSTRSERVER_SRC)
	@echo INSTRSERVER_SRCS: $(INSTRSERVER_SRCS)
	@echo INSTRSERVER_OBJS: $(INSTRSERVER_OBJS)
	@echo INSTRSERVER_CP: $(INSTRSERVER_CP)
	@echo
	@echo JARS: $(JARS)
	@echo DIRS: $(DIRS)
	@echo
	@echo BENCHS: $(BENCHS)
	@echo INSTRS: $(INSTRS)
	@echo
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
testapp: $(TESTAGENT) $(TESTAPP) startserver | $(BUILD)/testapplog
	time $(JAVA) $(JVMARGS) -agentpath:$(TESTAGENT)=$(INSTRS):$(BUILD)/testapplog/ -jar $(TESTAPP)

$(BUILD)/testapplog:
	mkdir -p $@

#
# Rules to run $(TESTDACAPO)
#
testdacapo: $(TESTAGENT) | $(BUILD)/testdacapolog
	time $(JAVA) $(JVMARGS) -agentpath:$(TESTAGENT)=$(INSTRS):$(BUILD)/testdacapolog/ -jar jars/dacapo-9.12-bach.jar --scratch-directory $(BUILD)/scratch $(BENCHS)

$(BUILD)/testdacapolog:
	mkdir -p $@

#
# Rules to run $(INSTRSERVER)
#
startserver: $(INSTRSERVER).pid

$(INSTRSERVER).pid: $(INSTRSERVER) $(TESTAPP)
	-$(MAKE) stopserver
	$(JAVA) -classpath $(CP):$(TESTAPP) -jar $(INSTRSERVER) & echo "$$!" > $(INSTRSERVER).pid
	sleep 1

stopserver:
	test -s $(INSTRSERVER).pid || { echo "Server stopped."; exit 1; }
	-kill `cat $(INSTRSERVER).pid`
	rm -f $(INSTRSERVER).pid

#
# Rules for $(LIBJNIF)
#
$(LIBJNIF): $(LIBJNIF_OBJS)
	ar crv $@ $^

$(BUILD)/%.cpp.o: $(LIBJNIF_SRC)/%.cpp $(LIBJNIF_HPPS) | $(BUILD)
	$(LINK.c) -O4 -std=c++11 -stdlib=libc++ -c -o $@ $<

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
$(TESTAPP): $(TESTAPP_SRC)/MANIFEST.MF $(TESTAPP_OBJS)
	jar cfm $@ $< -C $(BUILD)/testapp .

$(BUILD)/testapp/%.class: $(TESTAPP_SRC)/%.java | $(BUILD)/testapp
	$(JAVAC) -sourcepath $(TESTAPP_SRC) -d $(BUILD)/testapp $<

$(BUILD)/testapp:
	mkdir -p $(BUILD)/testapp

#
# Rules for $(INSTRSERVER)
#
$(INSTRSERVER): $(INSTRSERVER_SRC)/MANIFEST.MF $(INSTRSERVER_OBJS)
	jar cfm $@ $< -C $(BUILD)/instrserver .

$(BUILD)/instrserver/%.class: $(INSTRSERVER_SRC)/src/%.java | $(BUILD)/instrserver
	$(JAVAC) -classpath $(INSTRSERVER_CP) -sourcepath $(INSTRSERVER_SRC)/src -d $(BUILD)/instrserver $<

$(BUILD)/instrserver/%: $(INSTRSERVER_SRC)/src/% | $(BUILD)/instrserver
	cp $< $@

$(BUILD)/instrserver:
	mkdir -p $(BUILD)/instrserver

#
# Common rules used by all targets.
#
$(BUILD):
	mkdir -p $(BUILD)
