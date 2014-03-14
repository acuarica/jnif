
BUILD=build

.PHONY: all agent unit docs clean

all:
	$(MAKE) -C src
	$(MAKE) -C src-test-unit
	$(MAKE) -C src-test-agent
	$(MAKE) -C src-test-unit run
	$(MAKE) -C src-test-agent run

agent: | $(BUILD)
	$(MAKE) -C src
	$(MAKE) -C src-test-agent run

unit: | $(BUILD)
	$(MAKE) -C src
	$(MAKE) -C src-test-unit run

docs:
	doxygen

clean:
	rm -rf $(BUILD)

$(BUILD):
	mkdir -p $(BUILD)
