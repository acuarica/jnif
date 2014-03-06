
BUILD=build

.PHONY: all agent unit docs clean

all: agent unit

agent: | $(BUILD)
	$(MAKE) -C src
	$(MAKE) -C src-test-agent

unit: | $(BUILD)
	$(MAKE) -C src
	$(MAKE) -C src-test-unit

docs:
	doxygen

clean:
	rm -rf $(BUILD)

$(BUILD):
	mkdir -p $(BUILD)
