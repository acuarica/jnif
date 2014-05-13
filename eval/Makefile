
BUILD=../build
BENCHS=avrora batik eclipse fop h2 jython luindex lusearch pmd sunflow tomcat tradebeans tradesoap xalan

INSTRS=Print Identity ObjectInit NewArray ANewArray Main ClientServer

LIB=libfrheapagent.jnilib

INSTR=Empty
INSTR=Compute
#INSTR=Heap
#INSTR=ANewArray
#INSTR=Print
#INSTR=Identity
RUNDACAPO=cd $(BUILD)/ && time java  -agentpath:$(LIB)=$(INSTR) -jar ../eval/dacapo-9.12-bach.jar
RUNSCALA=java -Xverify:all -agentpath:$(LIB)=$(INSTR) -jar ../eval/scala-benchmark-suite-0.1.0-20120216.103539-3.jar

#RUNSCALA=java -Xverify:all -jar scala-benchmark-suite-0.1.0-20120216.103539-3.jar

#BENCH=avrora

.PHONY: dacapo-avrora
dacapo-avrora: BENCH=avrora
dacapo-avrora: dacapo

.PHONY: dacapo-batik
dacapo-batik: BENCH=batik
dacapo-batik: dacapo

.PHONY: dacapo-eclipse
dacapo-eclipse: BENCH=eclipse
dacapo-eclipse: dacapo

.PHONY: dacapo-fop
dacapo-fop: BENCH=fop
dacapo-fop: dacapo

.PHONY: dacapo-h2
dacapo-h2: BENCH=h2
dacapo-h2: dacapo

.PHONY: dacapo-jython
dacapo-jython: BENCH=jython
dacapo-jython: dacapo

.PHONY: dacapo-luindex
dacapo-luindex: BENCH=luindex
dacapo-luindex: dacapo

.PHONY: dacapo-lusearch
dacapo-lusearch: BENCH=lusearch
dacapo-lusearch: dacapo

.PHONY: dacapo-pmd
dacapo-pmd: BENCH=pmd
dacapo-pmd: dacapo

.PHONY: dacapo-sunflow
dacapo-sunflow: BENCH=sunflow
dacapo-sunflow: dacapo

.PHONY: dacapo-tomcat
dacapo-tomcat: BENCH=tomcat
dacapo-tomcat: dacapo

.PHONY: dacapo-tradebeans
dacapo-tradebeans: BENCH=tradebeans
dacapo-tradebeans: dacapo

.PHONY: dacapo-tradesoap
dacapo-tradesoap: BENCH=tradesoap
dacapo-tradesoap: dacapo

.PHONY: dacapo-xalan
dacapo-xalan: BENCH=xalan
dacapo-xalan: dacapo

.PHONY: dacapo
dacapo: $(BUILD)/$(LIB)
	$(RUNDACAPO) $(BENCH)

.PHONY: all
all: $(BUILD)/$(LIB)
	$(foreach b,$(BENCHS),$(RUNDACAPO) $(b) ;)
#$(foreach b,$(BENCHS),$(RUNDACAPO) $(b) &&) :

.PHONY: scala
scala: $(LIB)
	$(RUNSCALA) actors

$(BUILD)/$(LIB): force
	$(MAKE) -C ../src-test-agent

.PHONY: force

#cp ../jnif/build/$(LIB) .

.PHONY: show
show:
	$(RUNDACAPO) -l

.PHONY: clean
clean:
	rm -rf tlog.*.log hs_err_pid*.log *.disasm scratch/
