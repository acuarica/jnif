
BUILD=build
RUNPATTERN=run.sh.o*

RUNFILES=$(wildcard $(BUILD)/*.out)

ifeq ($(RUNFILES),)
#ZSH_RESULT:=$(shell zsh myscript.zsh)
ifneq ($(MAKECMDGOALS),scp)
$(error Need to run <<make scp>> to download outfiles from boole)
endif
endif 

#BENCHS=b+tree backprop bfs cfd gaussian heartwall kmeans lavaMD lud myocyte nn nw particlefilter pathfinder srad streamcluster

ifneq ($(BENCHS),)
DASH:= -
EMPTY:=
SPACE:= $(EMPTY) $(EMPTY)
SUFFIX:=$(DASH)$(subst $(SPACE),$(DASH),$(BENCHS))
else
SUFFIX:=
endif

CSVS=$(RUNFILES:$(BUILD)/%.out=$(BUILD)/%$(SUFFIX).csv)
PDFS=$(CSVS:$(BUILD)/%.csv=$(BUILD)/%.pdf)
UPLS=$(PDFS:$(BUILD)/%.pdf=$(BUILD)/%.upload~)

.PHONY: all upload pdf csv scp clean cleanall
.SECONDARY: $(CSVS)

all: pdf

upload: $(UPLS)
pdf: $(PDFS)
csv: $(CSVS)
scp: $(BUILD)/.scpdone

$(BUILD)/%.upload~: $(BUILD)/%.pdf upload.py
	./upload.py --verbose $<
	touch $@

$(BUILD)/%.pdf: $(BUILD)/%.csv charts.r
	r --slave --vanilla --file=charts.r --args $<

$(BUILD)/%$(SUFFIX).csv: $(BUILD)/%.out charts.py
	./charts.py $(CHARTSFLAG) $< $@ $(BENCHS)

$(BUILD)/.scpdone:
	mkdir -p $(BUILD)
	scp boole:$(RUNPATTERN) ./$(BUILD)
	for f in $(BUILD)/$(RUNPATTERN); do mv $$f $${f/run.sh.o/run-}.out; done
	touch $(BUILD)/.scpdone

clean:
	rm -rf $(BUILD)/*.csv $(BUILD)/*.pdf $(BUILD)/*.upload~

cleanall:
	rm -rf $(BUILD)
