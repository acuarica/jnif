
BUILD=build

.PHONY: all unit docs clean

all:
	$(MAKE) -C src
	$(MAKE) -C src-test-unit
	$(MAKE) -C src-test-agent
	$(MAKE) -C src-test-unit run
	$(MAKE) -C src-test-agent run

instr-empty: | $(BUILD)
	$(MAKE) -C src && $(MAKE) -C src-test-agent instr-empty

instr-dump: | $(BUILD)
	$(MAKE) -C src && $(MAKE) -C src-test-agent instr-dump

instr-print: | $(BUILD)
	$(MAKE) -C src && $(MAKE) -C src-test-agent instr-print

instr-identity: | $(BUILD)
	$(MAKE) -C src && $(MAKE) -C src-test-agent instr-identity

instr-compute: | $(BUILD)
	$(MAKE) -C src && $(MAKE) -C src-test-agent instr-compute

instr-computeapp: | $(BUILD)
	$(MAKE) -C src && $(MAKE) -C src-test-agent instr-computeapp

unit: | $(BUILD)
	$(MAKE) -C src; $(MAKE) -C src-test-unit run

docs:
	doxygen

clean:
	rm -rf $(BUILD)

$(BUILD):
	mkdir -p $(BUILD)
