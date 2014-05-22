
## Prerequisites ----
suppressPackageStartupMessages(library("ggplot2"))
#suppressPackageStartupMessages(library("grid"))
#suppressPackageStartupMessages(library("gridExtra"))
suppressPackageStartupMessages(library("reshape2"))
suppressPackageStartupMessages(library("tools"))
#suppressPackageStartupMessages(library("parallel"))
#suppressPackageStartupMessages(library("foreach"))
#suppressPackageStartupMessages(library("iterators"))
#suppressPackageStartupMessages(library("doParallel"))

## Preparations ----
printf <- function(format, ...) print(sprintf(format, ...))

## Arguments and output file ----
argv <- commandArgs(trailingOnly = TRUE)
csvfilename <- '../build/eval.prof'
csvfilename <- argv[1]

path <- file_path_sans_ext(csvfilename)
save <- function( p, d, s, w = 12, h = 8 ) {
  path <- sprintf('%s.chart_%s.pdf', d, s)
  pdf(file = path, paper = 'special', width = w, height = h, pointsize = 12)
  print( p )
  null <- dev.off()
}

printf('Loading table from %s...', csvfilename);
csv <- read.csv(csvfilename, strip.white=TRUE, sep=':', header=FALSE);
colnames(csv) <- c('bench', 'instr', 'stage', 'time');

# Instrumentation
csv.instrumentation <- subset(csv, !(stage %in% '@total'))
csv.instrumentation <- dcast(csv.instrumentation, bench+instr~'time', value.var='time', fun.aggregate=sum)
colnames(csv.instrumentation) <- c('bench', 'instr', 'instrumentation');

csv.total <- subset(csv, stage %in% '@total')
csv.total$stage <- NULL
colnames(csv.total) <- c('bench', 'instr', 'total');

csv.all <- merge(csv.instrumentation, csv.total, by=c("bench", "instr"))
csv.all <- melt(csv.all, id.vars=c('bench', 'instr'), variable.name='stage', value.name='time' )

# Instrumentation
p <-
  ggplot(csv.instrumentation)+facet_wrap(~bench, scales="free")+geom_bar(aes(instr, instrumentation, fill=instr), stat="identity")+
  labs(x="Instrumentation", y = "Instrumentation time (in seconds)" )+theme(legend.position="top")
save(p, path, "instr")

# Instrumentation with total
p <-
  ggplot(csv.all)+facet_wrap(~bench, scales="free")+geom_bar(aes(instr, time, fill=stage), stat="identity")+
  labs(x="Instrumentation", y = "Instrumentation and total time (in seconds)" )+theme(legend.position="top")
save( p, path, "all" )
