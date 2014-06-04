
BUILD=build

JAVA=java
JAVAC=javac
JAR=jar

ifneq (, $(wildcard Makefile.local))
include Makefile.local
endif

CXXFLAGS+=-fPIC -W -g -Wall -Wextra -O3 -Wno-unused-value

#
# Rules to make $(LIBJNIF)
#
LIBJNIF=$(BUILD)/libjnif.a
LIBJNIF_BUILD=$(BUILD)/libjnif
LIBJNIF_SRC=src
LIBJNIF_HPPS=$(wildcard $(LIBJNIF_SRC)/*.hpp)
LIBJNIF_SRCS=$(wildcard $(LIBJNIF_SRC)/*.cpp)
LIBJNIF_OBJS=$(LIBJNIF_SRCS:$(LIBJNIF_SRC)/%=$(LIBJNIF_BUILD)/%.o)

$(LIBJNIF): $(LIBJNIF_OBJS)
	$(AR) cr $@ $^

$(LIBJNIF_BUILD)/%.cpp.o: $(LIBJNIF_SRC)/%.cpp $(LIBJNIF_HPPS) | $(LIBJNIF_BUILD)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(LIBJNIF_BUILD):
	mkdir -p $@

#
# Rules to make $(TESTUNIT)
#
TESTUNIT=$(BUILD)/testunit.bin
TESTUNIT_BUILD=$(BUILD)/testunit
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
# Rules to make $(TESTCOVERAGE)
#
TESTCOVERAGE=$(BUILD)/testcoverage.bin
TESTCOVERAGE_BUILD=$(BUILD)/testcoverage
TESTCOVERAGE_SRC=src-testcoverage
TESTCOVERAGE_HPPS=$(wildcard $(TESTCOVERAGE_SRC)/*.hpp)
TESTCOVERAGE_SRCS=$(wildcard $(TESTCOVERAGE_SRC)/*.cpp)
TESTCOVERAGE_OBJS=$(TESTCOVERAGE_SRCS:$(TESTCOVERAGE_SRC)/%=$(TESTCOVERAGE_BUILD)/%.o)

$(TESTCOVERAGE): $(TESTCOVERAGE_OBJS) $(LIBJNIF)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TESTCOVERAGE_BUILD)/%.cpp.o: $(TESTCOVERAGE_SRC)/%.cpp $(TESTCOVERAGE_HPPS) | $(TESTCOVERAGE_BUILD)
	$(CXX) $(CXXFLAGS) -I$(LIBJNIF_SRC) -c -o $@ $<

$(TESTCOVERAGE_BUILD):
	mkdir -p $@

#
# Rules to make $(TESTAGENT)
#
TESTAGENT=$(BUILD)/libtestagent.dylib
TESTAGENT_BUILD=$(BUILD)/libtestagent
TESTAGENT_SRC=src-testagent
TESTAGENT_HPPS=$(wildcard $(TESTAGENT_SRC)/*.hpp)
TESTAGENT_SRCS=$(wildcard $(TESTAGENT_SRC)/*.cpp) $(wildcard $(TESTAGENT_SRC)/frproxy/*.java)
TESTAGENT_OBJS=$(TESTAGENT_SRCS:$(TESTAGENT_SRC)/%=$(TESTAGENT_BUILD)/%.o)

$(TESTAGENT): $(TESTAGENT_OBJS) $(LIBJNIF)
	$(CXX) -fPIC -g -lpthread -shared -lstdc++ -o $@ $^ $(CXXFLAGS)

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
TESTAPP_BUILD=$(BUILD)/testapp
TESTAPP_SRC=src-testapp
TESTAPP_SRCS=$(wildcard $(TESTAPP_SRC)/*/*.java)
TESTAPP_SRCS+=$(wildcard $(TESTAPP_SRC)/*/*/*.java)
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
INSTRSERVER_BUILD=$(BUILD)/instrserver
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

all: $(LIBJNIF) $(TESTUNIT) $(TESTCOVERAGE) $(TESTAGENT) $(TESTAPP) $(INSTRSERVER)

#
# rununit
#
rununit: $(TESTUNIT)
	$(TESTUNIT)

#
# runcoverage
#
JARS=$(wildcard jars/*.jar)
DIRS=$(JARS:%.jar=$(BUILD)/%)
runcoverage: cp=$(BUILD)
runcoverage: test=
runcoverage: $(TESTCOVERAGE) $(DIRS)
	$(TESTCOVERAGE) $(cp) $(test)

$(BUILD)/jars/%: jars/%.jar | $(BUILD)/jars
	unzip $< -d $@

$(BUILD)/jars:
	mkdir -p $@

#
# Rules to run $(INSTRSERVER)
#
start: CLASSPREFIX=ch.usi.inf.sape.frheap.FrHeapInstrumenter
start:
	$(JAVA) -jar $(INSTRSERVER) $(CLASSPREFIX)$(INSTRSERVERCLASS) instrserver,$(APP),$(RUN),$(INSTR) &
	sleep 2

stop:
	kill `jps -mlv | grep $(INSTRSERVER) | cut -f 1 -d' '`
	sleep 1

runjar:
	time $(JAVA) $(JVMARGS) -jar $(JARAPP)

CMD=runjar

runagent: LOGDIR=$(BUILD)/run/$(APP)/log/$(INSTR).$(APP)
runagent: PROF=$(BUILD)/eval-$(BACKEND)-$(APP)-$(RUN)-$(INSTR)
runagent: JVMARGS+=-agentpath:$(TESTAGENT)=$(FUNC):$(PROF):$(LOGDIR)/:$(BACKEND),$(APP),$(RUN),$(INSTR)
runagent: logdir $(TESTAGENT) $(CMD)

logdir:
	mkdir -p $(LOGDIR)

runserver: INSTRSERVERCLASS=$(INSTR)
runserver: FUNC=ClientServer
runserver: $(INSTRSERVER) start runagent stop

RUN=4
INSTR=Compute
FUNC=$(INSTR)
BACKEND=runagent

testapp: JARAPP:=$(TESTAPP)
testapp: APP=jnif-testapp
testapp: $(TESTAPP)
testapp: $(BACKEND)

BENCH=avrora

DACAPO_SCRATCH=$(BUILD)/run/dacapo/scratch/$(INSTR).$(BENCH)

dacapo: JARAPP=jars/dacapo-9.12-bach.jar --scratch-directory $(DACAPO_SCRATCH) $(BENCH)
dacapo: APP=dacapo-$(BENCH)
dacapo: | $(DACAPO_SCRATCH)
dacapo: $(BACKEND)

#dacapo-avrora:
#dacapo-avrora: dacapo

$(DACAPO_SCRATCH):
	mkdir -p $@

scala: JARAPP=jars/scala-benchmark-suite-0.1.0-20120216.103539-3.jar --scratch-directory $(DACAPO_SCRATCH) $(BENCH)
scala: APP=scala-$(BENCH)
scala: | $(DACAPO_SCRATCH)
scala: $(BACKEND) 

runeval:
	$(MAKE) cleaneval $(foreach r,$(shell seq 1 $(times)),\
		$(foreach be,$(backends),\
			$(foreach i,$(instrs),\
				$(foreach b,$(benchs),\
					&& $(MAKE) $(SUITE) BACKEND=$(be) RUN=$(r) INSTR=$(i) BENCH=$(b) \
				)\
			)\
		)\
	)
	cat $(BUILD)/eval-runagent-*.prof > $(BUILD)/eval.$(UNAME).prof
	cat $(BUILD)/eval-runserver-*.prof >> $(BUILD)/eval.$(UNAME).prof
	cat $(BUILD)/eval-instrserver-*.prof >> $(BUILD)/eval.$(UNAME).prof


eval-scala: times=1
eval-scala: backends=runagent runserver
eval-scala: instrs=Empty Identity Compute
eval-scala: benchs=actors apparat dummy factorie kiama scalac scaladoc scalap scalariform scalatest scalaxb specs tmt
eval-scala: SUITE=scala
eval-scala: runeval
#	$(MAKE) cleaneval $(foreach r,$(shell seq 1 $(times)),\
#		$(foreach be,$(backends),\
#			$(foreach i,$(instrs),\
#				$(foreach b,$(benchs),\
#					&& $(MAKE) scala BACKEND=$(be) RUN=$(r) INSTR=$(i) BENCH=$(b) \
#				)\
#			)\
#		)\
#	)
#	cat $(BUILD)/eval-runagent-*.prof $(BUILD)/eval-server-*.prof > $(BUILD)/eval.prof

#
# eval
#
#Print ObjectInit NewArray ANewArray Main

eval: times=1
eval: backends=runagent runserver
eval: instrs=Empty Identity Compute Stats
eval: benchs=avrora batik eclipse fop h2 jython luindex lusearch pmd sunflow tomcat xalan  #tradebeans tradesoap
eval: SUITE=dacapo
eval: runeval
#	$(MAKE) cleaneval $(foreach r,$(shell seq 1 $(times)),\
#		$(foreach be,$(backends),\
#			$(foreach i,$(instrs),\
#				$(foreach b,$(benchs),\
#					&& $(MAKE) dacapo BACKEND=$(be) RUN=$(r) INSTR=$(i) BENCH=$(b) \
#				)\
#			)\
#		)\
#	)
#	cat $(BUILD)/eval-*.prof > $(BUILD)/eval.prof

tiny-eval: times=1
tiny-eval: backends=runagent runserver
tiny-eval: instrs=Empty Identity
tiny-eval: benchs=avrora batik
tiny-eval: SUITE=dacapo
tiny-eval: runeval

small-eval: times=1
small-eval: backends=runagent runserver
small-eval: instrs=Empty Identity Compute Stats
small-eval: benchs=avrora batik eclipse
small-eval: SUITE=dacapo
small-eval: runeval

large-eval: times=1
large-eval: backends=runagent runserver
large-eval: instrs=Empty Identity Compute Stats All
large-eval: benchs=avrora batik eclipse fop h2 
large-eval: SUITE=dacapo
large-eval: runeval

huge-eval: times=1
huge-eval: backends=runagent runserver
huge-eval: instrs=Empty Identity Compute Stats All
huge-eval: benchs=avrora batik eclipse fop h2 jython luindex lusearch pmd sunflow xalan
huge-eval: SUITE=dacapo
huge-eval: runeval

full-eval: times=5
full-eval: backends=runagent runserver
full-eval: instrs=Empty Identity Compute Stats All
full-eval: benchs=avrora batik eclipse fop h2 jython luindex lusearch pmd sunflow xalan
full-eval: SUITE=dacapo
full-eval: runeval

test: times=1
test: backends=runagent
test: benchs=avrora batik eclipse fop h2 jython luindex lusearch pmd sunflow tomcat xalan  #tradebeans tradesoap
test: eval

test-compute: instrs=Compute
test-compute: test

test-stats: instrs=Stats
test-stats: test

plots:
	$(foreach p,$(wildcard $(BUILD)/eval.*.prof),\
		$(R) --slave --vanilla --file=charts/charts.r --args $(p) ; \
	)
#plotsl:
#	$(R) --slave --vanilla --file=charts/charts.r --args $(BUILD)/eval.Linux.prof

docs:
	doxygen

dots: DOTS=$(shell find build -name *.dot)
dots: PNGS=$(DOTS:%.dot=%.png)
dots: $(PNGS)

scp:
	scp steklov:work/jnif/build/eval.Linux.prof build

$(BUILD)/%.png: $(BUILD)/%.dot
	dot -Tpng $< > $@

run: $(BACKEND)

eclipse:
	$(ECLIPSE_HOME)/eclipse -vmargs $(JVMARGS)

JRUBY_HOME=/Users/luigi/Downloads/jruby-1.7.12
jruby:
	java -Djruby.home=$(JRUBY_HOME) -Djruby.lib=$(JRUBY_HOME)/lib -jar jruby.jar xslt.rb

clean:
	rm -rf $(BUILD)

cleaneval:
	rm -rf $(BUILD)/eval-*.prof $(BUILD)/run
