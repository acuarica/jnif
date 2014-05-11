
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
RUNDACAPO=cd $(BUILD)/ && time java -agentpath:$(LIB)=$(INSTR) -jar ../eval/dacapo-9.12-bach.jar
RUNSCALA=java -Xverify:all -agentpath:$(LIB)=$(INSTR) -jar ../eval/scala-benchmark-suite-0.1.0-20120216.103539-3.jar

#RUNSCALA=java -Xverify:all -jar scala-benchmark-suite-0.1.0-20120216.103539-3.jar

args=avrora

.PHONY: run
run: $(BUILD)/$(LIB)
	$(RUNDACAPO) $(args)

.PHONY: all
all: $(LIB)
	$(foreach b,$(shell $(RUNDACAPO) -l),$(RUNDACAPO) $(b) &&) :

.PHONY: scala
scala: $(LIB)
	$(RUNSCALA) actors

$(BUILD)/$(LIB):
	$(MAKE) -C ../src-test-agent

#cp ../jnif/build/$(LIB) .

.PHONY: show
show:
	$(RUNDACAPO) -l

.PHONY: clean
clean:
	rm -rf tlog.*.log hs_err_pid*.log *.disasm scratch/
