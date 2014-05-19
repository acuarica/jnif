
INCLUDE=../src
BUILD=../build

EXE=test-unit
HPPS=$(wildcard $(INCLUDE)/*.hpp)
SRCS=$(wildcard jnif/*.java) $(wildcard *.cpp)

CLSS=$(shell find . -name "*.class")

OBJS=$(SRCS:%=$(BUILD)/%.o) $(CLSS:./classes/%=$(BUILD)/%.o) $(BUILD)/filelist.cpp.o

LINK=$(OBJS:%='%')
JVS:=$(CLSS:./classes/%.class=%)
JVS:=$(subst /,_,$(JVS))
JVS:=$(subst $$,_,$(JVS))
CFLAGS += -fPIC -W -O0 -Wall -Wextra -I$(INCLUDE)


.PHONY: all run

all: $(BUILD)/$(EXE)

run: $(BUILD)/$(EXE)
	cd $(BUILD) && ./$(EXE) "java/lang/Object.class"
#java/lang/Object.class
#> $(BUILD)/$(EXE).out

$(BUILD)/$(EXE): $(OBJS) $(BUILD)/libjnif.a
	clang++ $(CFLAGS) -fsanitize=leak -stdlib=libc++ -o $@ $(LINK) $(BUILD)/libjnif.a
#$^

$(BUILD)/%.java.o: %.java | $(BUILD)
	javac -d $(BUILD)/ $<
	cd $(BUILD) ; xxd -include $*.class $*.c
	$(LINK.c) -c -o $@ $(BUILD)/$*.c
#	echo "// AUTOMATICALLY GENERATED; DO NOT MODIFY" > $(BUILD)/$*.h
#	echo "extern u1 $(subst /,_,$*)_class[];" >> $(BUILD)/$*.h
#	echo "extern int $(subst /,_,$*)_class_len;" >> $(BUILD)/$*.h

$(BUILD)/%.class.o: classes/%.class | $(BUILD)
	mkdir -p $(dir $@)
	cp -v '$<' $(dir $@)
	cd $(BUILD) ; xxd -include '$*.class' '$*.c'
	$(LINK.c) -c -o '$@' '$(BUILD)/$*.c'

$(BUILD)/filelist.cpp:
filelist: 
	echo "// AUTOMATICALLY GENERATED; DO NOT MODIFY" > $(BUILD)/filelist.cpp
	for c in $(JVS); do echo "extern unsigned char "$$c"_class[];\nextern int "$$c"_class_len;\n" >> $(BUILD)/filelist.cpp; done
	echo "struct JavaFile {unsigned char* data;int len;const char* name;};" >> $(BUILD)/filelist.cpp
	echo "" >> $(BUILD)/filelist.cpp
	echo "JavaFile tests2[] = { " >> $(BUILD)/filelist.cpp
	for c in $(JVS); do echo "{ "$$c"_class, "$$c"_class_len, \""$$c"\" } ," >> $(BUILD)/filelist.cpp; done
	echo "};\n" >> $(BUILD)/filelist.cpp
	echo "int tests2_size = sizeof(tests2)/sizeof(tests2[0]);\n" >> $(BUILD)/filelist.cpp


$(BUILD)/filelist.cpp.o: $(BUILD)/filelist.cpp | $(BUILD)
	$(LINK.c) -std=c++11 -stdlib=libc++ -c -o $@ $<

$(BUILD)/%.cpp.o: %.cpp $(HPPS) | $(BUILD)
	$(LINK.c) -std=c++11 -stdlib=libc++ -c -o $@ $<

$(BUILD)/libjnif.a: FORCE
	$(MAKE) -C ../src

$(BUILD):
	mkdir -p $(BUILD)
	mkdir -p $(BUILD)/instr

show:
	echo $(CLSS)
	
.PHONY: FORCE
FORCE:
