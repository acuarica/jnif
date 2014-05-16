
INCLUDE=../src
BUILD=../build

EXE=test-unit
HPPS=$(wildcard $(INCLUDE)/*.hpp)
SRCS=$(wildcard jnif/*.java) $(wildcard *.cpp)
OBJS=$(SRCS:%=$(BUILD)/%.o)

CFLAGS += -fPIC -W -O0 -Wall -Wextra -I$(INCLUDE)

.PHONY: all run

all: $(BUILD)/$(EXE)

run: $(BUILD)/$(EXE)
	$(BUILD)/$(EXE) > $(BUILD)/$(EXE).out

$(BUILD)/$(EXE): $(OBJS) $(BUILD)/libjnif.a
	clang++ $(CFLAGS) -fsanitize=leak -stdlib=libc++ -o $@ $^

$(BUILD)/%.java.o: %.java
	javac -d $(BUILD)/ $<
	cd $(BUILD) ; xxd -include $*.class $*.c
	$(LINK.c) -c -o $@ $(BUILD)/$*.c
#	echo "// AUTOMATICALLY GENERATED; DO NOT MODIFY" > $(BUILD)/$*.h
#	echo "extern u1 $(subst /,_,$*)_class[];" >> $(BUILD)/$*.h
#	echo "extern int $(subst /,_,$*)_class_len;" >> $(BUILD)/$*.h

#$(BUILD)/filelist.cpp: jnif
#	echo "// AUTOMATICALLY GENERATED; DO NOT MODIFY" > $(BUILD)/filelist.cpp
#	$(foreach dir,$(dirs),$(wildcard $(dir)/*))

$(BUILD)/filelist.cpp.o: $(BUILD)/filelist.cpp | $(BUILD)
	$(LINK.c) -std=c++11 -stdlib=libc++ -c -o $@ $<

$(BUILD)/%.cpp.o: %.cpp $(HPPS) | $(BUILD)
	$(LINK.c) -std=c++11 -stdlib=libc++ -c -o $@ $<

$(BUILD)/libjnif.a: FORCE
	$(MAKE) -C ../src

.PHONY: FORCE
FORCE:
