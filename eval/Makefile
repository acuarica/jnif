
BUILD=../build
BENCHS=avrora batik eclipse fop h2 jython luindex lusearch pmd sunflow tomcat tradebeans tradesoap xalan

INSTRS=Empty Dump Print Identity ObjectInit NewArray ANewArray Main ClientServer

LIB=libfrheapagent.jnilib

INSTR=ComputeApp
#INSTR=Heap
#INSTR=ANewArray
#INSTR=Print
#INSTR=Identity
#INSTR=Empty
RUNDACAPO=cd $(BUILD)/ && java -agentpath:$(LIB)=$(INSTR) -jar ../eval/dacapo-9.12-bach.jar

RUNSCALA=java -Xverify:all -agentpath:$(LIB)=$(INSTR) -jar ../eval/scala-benchmark-suite-0.1.0-20120216.103539-3.jar

#RUNSCALA=java -Xverify:all -jar scala-benchmark-suite-0.1.0-20120216.103539-3.jar

dargs=avrora

.PHONY: run all scala show clean

run: $(LIB)
	$(RUNDACAPO) $(dargs)

all: $(LIB)
	$(foreach b,$(shell $(RUNDACAPO) -l),$(RUNDACAPO) $(b) &&) :

scala: $(LIB)
	$(RUNSCALA) actors

$(LIB): ../jnif/build/$(LIB)
	cp ../jnif/build/$(LIB) .

show:
	$(RUNDACAPO) -l

clean:
	rm -rf tlog.*.log hs_err_pid*.log *.disasm scratch/
