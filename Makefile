
ifneq (, $(wildcard Makefile.local))
include Makefile.local
endif

BUILD=build

JARS=$(wildcard jars/*.jar)
DIRS=$(JARS:jars/%.jar=$(BUILD)/%)

#INSTRS=Print Identity ObjectInit NewArray ANewArray Main ClientServer
#BENCHS=avrora batik eclipse fop h2 jython luindex lusearch pmd sunflow \
#       tomcat tradebeans tradesoap xalan

INSTRS=Empty Identity Compute ClientServer
BENCHS=avrora batik eclipse fop h2 jython luindex lusearch pmd sunflow 

INSTR=Compute
BENCH=avrora

CFLAGS += -fPIC -W -O4 -g -Wall -Wextra 

#
# Rules to make $(LIBJNIF)
#
LIBJNIF=$(BUILD)/libjnif.a
LIBJNIF_BUILD=$(BUILD)/libjnif.objs
LIBJNIF_SRC=src
LIBJNIF_HPPS=$(wildcard $(LIBJNIF_SRC)/*.hpp)
LIBJNIF_SRCS=$(wildcard $(LIBJNIF_SRC)/*.cpp)
LIBJNIF_OBJS=$(LIBJNIF_SRCS:$(LIBJNIF_SRC)/%=$(LIBJNIF_BUILD)/%.o)

$(LIBJNIF): $(LIBJNIF_OBJS)
	ar crv $@ $^

$(LIBJNIF_BUILD)/%.cpp.o: $(LIBJNIF_SRC)/%.cpp $(LIBJNIF_HPPS) | $(LIBJNIF_BUILD)
	$(LINK.c) -O4 -std=c++11 -stdlib=libc++ -c -o $@ $<

$(LIBJNIF_BUILD):
	mkdir -p $@

#
# Rules to make $(TESTUNIT)
#
TESTUNIT=$(BUILD)/testunit.bin
TESTUNIT_BUILD=$(BUILD)/testunit.objs
TESTUNIT_SRC=src-testunit
TESTUNIT_HPPS=$(wildcard $(TESTUNIT_SRC)/*.hpp)
TESTUNIT_SRCS=$(wildcard $(TESTUNIT_SRC)/*.cpp)
TESTUNIT_OBJS=$(TESTUNIT_SRCS:$(TESTUNIT_SRC)/%=$(TESTUNIT_BUILD)/%.o)

$(TESTUNIT): $(TESTUNIT_OBJS) $(LIBJNIF)
	clang++ $(CFLAGS) -fsanitize=leak -stdlib=libc++ -o $@ $^

$(TESTUNIT_BUILD)/%.cpp.o: $(TESTUNIT_SRC)/%.cpp $(TESTUNIT_HPPS) | $(TESTUNIT_BUILD)
	$(LINK.c) -std=c++11 -stdlib=libc++ -I$(LIBJNIF_SRC) -c -o $@ $<

$(TESTUNIT_BUILD):
	mkdir -p $@

#
# Rules to make $(TESTAGENT)
#
TESTAGENT=$(BUILD)/libtestagent.dylib
TESTAGENT_BUILD=$(BUILD)/libtestagent.objs
TESTAGENT_SRC=src-testagent
TESTAGENT_HPPS=$(wildcard $(TESTAGENT_SRC)/*.hpp)
TESTAGENT_SRCS=$(wildcard $(TESTAGENT_SRC)/*.cpp) $(wildcard $(TESTAGENT_SRC)/frproxy/*.java)
TESTAGENT_OBJS=$(TESTAGENT_SRCS:$(TESTAGENT_SRC)/%=$(TESTAGENT_BUILD)/%.o)

$(TESTAGENT): $(TESTAGENT_OBJS) $(LIBJNIF)
	clang++ $(CFLAGS) -fPIC -O0 -g -lpthread -shared -lstdc++ -stdlib=libc++ -o $@ $^

$(TESTAGENT_BUILD)/%.cpp.o: $(TESTAGENT_SRC)/%.cpp $(TESTAGENT_HPPS) | $(TESTAGENT_BUILD)
	$(LINK.c) -std=c++11 -stdlib=libc++ -I$(LIBJNIF_SRC) -Wno-unused-parameter -I$(JAVA_HOME)/include -c -o $@ $<

$(TESTAGENT_BUILD)/%.java.o: $(TESTAGENT_SRC)/%.java
	$(JAVAC) -d $(TESTAGENT_BUILD)/ $<
	cd $(TESTAGENT_BUILD) && xxd -i $*.class $*.c
	$(LINK.c) -c -o $@ $(TESTAGENT_BUILD)/$*.c

$(TESTAGENT_BUILD):
	mkdir -p $@

#
# Rules to make $(TESTAPP)
#
TESTAPP=$(BUILD)/testapp.jar
TESTAPP_BUILD=$(BUILD)/testapp.objs
TESTAPP_SRC=src-testapp
TESTAPP_SRCS=$(wildcard $(TESTAPP_SRC)/*/*.java)
TESTAPP_OBJS=$(TESTAPP_SRCS:$(TESTAPP_SRC)/%.java=$(TESTAPP_BUILD)/%.class)

$(TESTAPP): $(TESTAPP_SRC)/MANIFEST.MF $(TESTAPP_OBJS)
	jar cfm $@ $< -C $(TESTAPP_BUILD) .

$(TESTAPP_BUILD)/%.class: $(TESTAPP_SRC)/%.java | $(TESTAPP_BUILD)
	$(JAVAC) -sourcepath $(TESTAPP_SRC) -d $(TESTAPP_BUILD) $<

$(TESTAPP_BUILD):
	mkdir -p $@

#
# Rules to make $(INSTRSERVER)
#
INSTRSERVER=$(BUILD)/instrserver.jar
INSTRSERVER_BUILD=$(BUILD)/instrserver.objs
INSTRSERVER_SRC=src-instrserver
INSTRSERVER_SRCS=$(shell find $(INSTRSERVER_SRC) -name *.java)
INSTRSERVER_OBJS=$(INSTRSERVER_BUILD)/log4j.properties $(INSTRSERVER_SRCS:$(INSTRSERVER_SRC)/src/%.java=$(INSTRSERVER_BUILD)/%.class)
INSTRSERVER_CP=$(subst $(_EMPTY) $(_EMPTY),:,$(wildcard $(INSTRSERVER_SRC)/lib/*.jar))

$(INSTRSERVER): $(INSTRSERVER_SRC)/MANIFEST.MF $(INSTRSERVER_OBJS)
	jar cfm $@ $< -C $(INSTRSERVER_BUILD) .

$(INSTRSERVER_BUILD)/%.class: $(INSTRSERVER_SRC)/src/%.java | $(INSTRSERVER_BUILD)
	$(JAVAC) -classpath $(INSTRSERVER_CP) -sourcepath $(INSTRSERVER_SRC)/src -d $(INSTRSERVER_BUILD) $<

$(INSTRSERVER_BUILD)/%: $(INSTRSERVER_SRC)/src/% | $(INSTRSERVER_BUILD)
	cp $< $@

$(INSTRSERVER_BUILD):
	mkdir -p $@

#
# PHONY rules
#

.PHONY: all testunit start stop testapp dacapo eval docs clean cleaneval

all: $(LIBJNIF) $(TESTUNIT) $(TESTAGENT) $(TESTAPP) $(INSTRSERVER)

#
# Rules to run $(TESTUNIT)
#
testunit: $(TESTUNIT) $(DIRS)
	$(TESTUNIT) $(BUILD) > $(TESTUNIT).log

$(BUILD)/%: jars/%.jar
	unzip $< -d $@

#
# Rules to run $(INSTRSERVER)
#
#$(INSTRSERVER).pid: $(INSTRSERVER) $(TESTAPP)
# & echo "$$!" > $(INSTRSERVER).pid
#$(INSTRSERVER).pid

start:
	$(JAVA) -jar $(INSTRSERVER) &
	sleep 2

#$(MAKE) stop

stop:
	kill `jps -mlv | grep build/instrserver.jar | cut -f 1 -d' '`
	sleep 1

#-IPID=`cat $(INSTRSERVER).pid 2> /dev/null` && kill $$IPID
#rm -f $(INSTRSERVER).pid

#
# Rules to run $(TESTAPP)
#
TESTAPP_LOG=$(BUILD)/run/testapp.$(INSTR).log
TESTAPP_PROF=$(BUILD)/eval-testapp.$(INSTR)

testapp: $(TESTAGENT) $(TESTAPP) $(INSTRSERVER) | $(TESTAPP_LOG)
	$(MAKE) start
	time $(JAVA) $(JVMARGS) -agentpath:$(TESTAGENT)=$(INSTR):testapp:$(TESTAPP_PROF):$(TESTAPP_LOG)/ -jar $(TESTAPP)
	$(MAKE) stop

$(TESTAPP_LOG):
	mkdir -p $@

#
# Rules to run dacapo benchmarks
#
DACAPO_LOG=$(BUILD)/run/dacapo/log/$(INSTR).$(BENCH)
DACAPO_PROF=$(BUILD)/eval-dacapo-$(BENCH).$(INSTR)
DACAPO_SCRATCH=$(BUILD)/run/dacapo/scratch/$(INSTR).$(BENCH)

dacapo: $(TESTAGENT) $(INSTRSERVER) | $(DACAPO_LOG) $(DACAPO_SCRATCH)
	$(MAKE) start
	time $(JAVA) $(JVMARGS) -agentpath:$(TESTAGENT)=$(INSTR):$(BENCH):$(DACAPO_PROF):$(DACAPO_LOG)/ -jar jars/dacapo-9.12-bach.jar --scratch-directory $(DACAPO_SCRATCH) $(BENCH)
	$(MAKE) stop

$(DACAPO_LOG):
	mkdir -p $@

$(DACAPO_SCRATCH):
	mkdir -p $@

#
# eval
#
eval:
	$(MAKE) cleaneval
	$(foreach i,$(INSTRS),$(foreach b,$(BENCHS),$(MAKE) dacapo INSTR=$(i) BENCH=$(b); ))

pdfs:
	cat $(BUILD)/eval-*.prof > $(BUILD)/eval.prof
	r --slave --vanilla --file=charts/charts.r --args $(BUILD)/eval.prof

docs:
	doxygen

clean:
	rm -rf $(BUILD)

cleaneval:
	rm -rf $(BUILD)/eval-*.prof $(BUILD)/run
