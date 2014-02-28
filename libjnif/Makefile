
ifneq (, $(wildcard Makefile.local))
include Makefile.local
endif

INCLUDE=src
BUILD=build

INCS=$(wildcard $(INCLUDE)/*.h)
HPPS=$(wildcard $(INCLUDE)/**/*.hpp) $(wildcard $(INCLUDE)/**/parser/*.hpp)

.PHONY: agent unit clean docs

agent: | $(BUILD)
	$(MAKE) -C src-test-agent

unit: | $(BUILD)
	$(MAKE) -C src-test-unit

docs:
	doxygen

clean:
	rm -rf $(BUILD)

$(BUILD):
	mkdir -p $(BUILD)
