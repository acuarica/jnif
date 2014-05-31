
JAVA=java
JAVAC=javac
JAR=jar

ifneq (, $(wildcard Makefile.local))
include Makefile.local
endif

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
  CXXFLAGS+=
endif
ifeq ($(UNAME), Darwin)
  CXXFLAGS+=-stdlib=libc++
endif

BUILD=build

CXXFLAGS+=-fPIC -W -g -Wall -Wextra -o3 -std=c++11 -Wno-unused-parameter

JARS=$(wildcard jars/*.jar)
DIRS=$(JARS:%.jar=$(BUILD)/%)

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
	$(AR) crv $@ $^

$(LIBJNIF_BUILD)/%.cpp.o: $(LIBJNIF_SRC)/%.cpp $(LIBJNIF_HPPS) | $(LIBJNIF_BUILD)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

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
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TESTUNIT_BUILD)/%.cpp.o: $(TESTUNIT_SRC)/%.cpp $(TESTUNIT_HPPS) | $(TESTUNIT_BUILD)
	$(CXX) $(CXXFLAGS) -I$(LIBJNIF_SRC) -c -o $@ $<

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
	$(CXX) $(CXXFLAGS) -fPIC -g -lpthread -shared -lstdc++ -o $@ $^

$(TESTAGENT_BUILD)/%.cpp.o: $(TESTAGENT_SRC)/%.cpp $(TESTAGENT_HPPS) | $(TESTAGENT_BUILD)
	$(CXX) $(CXXFLAGS) -I$(LIBJNIF_SRC) -Wno-unused-parameter -I$(JAVA_HOME)/include -c -o $@ $<

$(TESTAGENT_BUILD)/%.java.o: $(TESTAGENT_SRC)/%.java
	$(JAVAC) -d $(TESTAGENT_BUILD)/ $<
	cd $(TESTAGENT_BUILD) && xxd -i $*.class $*.c
	$(LINK.c) -c -o $@ $(TESTAGENT_BUILD)/$*.c

$(TESTAGENT_BUILD):
	mkdir -p $@
	mkdir -p $(BUILD)/instr

#
# Rules to make $(TESTAPP)
#
TESTAPP=$(BUILD)/testapp.jar
TESTAPP_BUILD=$(BUILD)/testapp.objs
TESTAPP_SRC=src-testapp
TESTAPP_SRCS=$(wildcard $(TESTAPP_SRC)/*/*.java)
TESTAPP_OBJS=$(TESTAPP_SRCS:$(TESTAPP_SRC)/%.java=$(TESTAPP_BUILD)/%.class)

$(TESTAPP): $(TESTAPP_SRC)/MANIFEST.MF $(TESTAPP_OBJS)
	$(JAR) cfm $@ $< -C $(TESTAPP_BUILD) .

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
	$(JAR) cfm $@ $< -C $(INSTRSERVER_BUILD) .

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

$(BUILD)/jars/%: jars/%.jar | $(BUILD)/jars
	unzip $< -d $@

$(BUILD)/jars:
	mkdir -p $@

#
# Rules to run $(INSTRSERVER)
#
start:
	$(JAVA) -jar $(INSTRSERVER) ch.usi.inf.sape.frheap.FrHeapInstrumenter$(INSTRSERVERCLASS) &
	sleep 2

stop:
	kill `jps -mlv | grep $(INSTRSERVER) | cut -f 1 -d' '`
	sleep 1

#
# Rules to run a jar
#
runjar:
	time $(JAVA) $(JVMARGS) -jar $(JARAPP)

runagent: LOGDIR=$(BUILD)/run/$(APP)/log/$(INSTR).$(APP)
runagent: PROF=$(BUILD)/eval-$(RUN)-$(APP).$(FUNC)
runagent: JVMARGS+=-agentpath:$(TESTAGENT)=$(FUNC):$(APP):$(PROF):$(LOGDIR)/:$(RUN)
runagent: logdir $(TESTAGENT) runjar

logdir:
	mkdir -p $(LOGDIR)

runserver: INSTRSERVERCLASS=$(INSTR)
runserver: FUNC=ClientServer
runserver: $(INSTRSERVER) start runagent stop

#ifeq ($(backend), asm)
#	$(MAKE) start
#	$(MAKE) runjar
#endif
#ifeq ($(backend), asm)
#	$(MAKE) stop
#endif

#
# Rules to run $(TESTAPP)
#
TESTAPP_LOG=$(BUILD)/run/testapp.$(INSTR).log
TESTAPP_PROF=$(BUILD)/eval-testapp-$(RUN).$(INSTR)

RUN=4
INSTR=Compute
FUNC=$(INSTR)
BACKEND=runagent

testapp: JARAPP=$(TESTAPP)
testapp: APP=jnif-testapp
testapp: $(TESTAPP)
# | $(TESTAPP_LOG)
testapp: $(BACKEND)

BENCH=avrora

DACAPO_SCRATCH=$(BUILD)/run/dacapo/scratch/$(INSTR).$(BENCH)

dacapo: JARAPP=jars/dacapo-9.12-bach.jar --scratch-directory $(DACAPO_SCRATCH) $(BENCH)
dacapo: | $(DACAPO_SCRATCH)
dacapo: $(BACKEND)

$(DACAPO_SCRATCH):
	mkdir -p $@

#	$(MAKE) runjar AGENT=-agentpath:$(TESTAGENT)=$(INSTR):testapp:$(TESTAPP_PROF):$(TESTAPP_LOG)/:$(RUN) JARAPP=$(TESTAPP)
#time $(JAVA) $(JVMARGS) -agentpath:$(TESTAGENT)=$(INSTR):testapp:$(TESTAPP_PROF):$(TESTAPP_LOG)/:$(RUN) -jar $(TESTAPP)

#$(TESTAPP_LOG):
#	mkdir -p $@

#
# Rules to run dacapo benchmarks
#

#DACAPO_LOG=$(BUILD)/run/dacapo/log/$(INSTR).$(BENCH)
#DACAPO_PROF=$(BUILD)/eval-dacapo-$(RUN)-$(BENCH).$(INSTR)
#DACAPO_SCRATCH=$(BUILD)/run/dacapo/scratch/$(INSTR).$(BENCH)

#dacapo: $(TESTAGENT) $(INSTRSERVER) | $(DACAPO_LOG) $(DACAPO_SCRATCH)
#	$(MAKE) runjar AGENT=-agentpath:$(TESTAGENT)=$(INSTR):$(BENCH):$(DACAPO_PROF):$(DACAPO_LOG)/:$(RUN) JARAPP="jars/dacapo-9.12-bach.jar --scratch-directory $(DACAPO_SCRATCH) $(BENCH)"

#$(DACAPO_LOG):
#	mkdir -p $@

#$(DACAPO_SCRATCH):
#	mkdir -p $@

SCALA_LOG=$(BUILD)/run/scala/log/$(INSTR).$(BENCH)
SCALA_PROF=$(BUILD)/eval-scala-$(RUN)-$(BENCH).$(INSTR)

scala: $(TESTAGENT) $(INSTRSERVER) | $(SCALA_LOG)
	time $(JAVA) $(JVMARGS) -agentpath:$(TESTAGENT)=$(INSTR):$(BENCH):$(SCALA_PROF):$(SCALA_LOG)/:$(RUN) -jar jars/scala-benchmark-suite-0.1.0-20120216.103539-3.jar $(BENCH)

$(SCALA_LOG):
	mkdir -p $@

#
# eval
#
#runs=10

#convs=Identity Compute   #Print ObjectInit NewArray ANewArray Main

eval: times=1
eval: backends=runagent runserver
eval: instrs=Identity Compute
eval: benchs=avrora batik eclipse fop h2 jython luindex lusearch pmd sunflow tomcat xalan  #tradebeans tradesoap
eval:
	$(MAKE) cleaneval $(foreach r,$(shell seq 1 $(times)),\
		$(foreach be,$(backends),\
			$(foreach instr,$(instrs),\
				$(foreach bench,$(benchs),\
					&& $(MAKE) dacapo BACKEND=$(be) RUN=$(r) INSTR=$(instr) BENCH=$(bench) \
				)\
			)\
		)\
	)
	cat $(BUILD)/eval-*.prof > $(BUILD)/eval.prof

eval-compute: 
	$(MAKE) eval TIMES=1 INSTRS=Compute

plots:
	r --slave --vanilla --file=charts/charts.r --args $(BUILD)/eval.prof

docs:
	doxygen

DOTS=$(shell find build -name *.dot)
PNGS=$(DOTS:%.dot=%.png)
dots: $(PNGS)

$(BUILD)/%.png: $(BUILD)/%.dot
	dot -Tpng $< > $@

clean:
	rm -rf $(BUILD)

cleaneval:
	rm -rf $(BUILD)/eval-*.prof $(BUILD)/run
