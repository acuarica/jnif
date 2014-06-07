
suppressPackageStartupMessages(library("ggplot2"))
suppressPackageStartupMessages(library("reshape2"))
suppressPackageStartupMessages(library("tools"))

printf <- function(format, ...) print(sprintf(format, ...))

#rename <- function(v) {
#  result <- vector(length=length(v))
#  for ( ii in 1:length(v)) {
#    name <- v[ii]
#    if (name == "Empty") result[ii] <- "Empty"
 #   else if (name == "Identity") result[ii] <- "JNIF Identity"
  #  else if (name == "Compute") result[ii] <- "JNIF Frames"
   # else if (name == "ClientServer") result[ii] <- "ASM Frames"
  #  else result[ii] <- name
  #}
  #return (result)
#}

argv <- commandArgs(trailingOnly = TRUE)

if (interactive()) {
  csvfilename <- '../build/eval-scala.prof'
} else {
  csvfilename <- argv[1]
}

path <- file_path_sans_ext(csvfilename)
save <- function(p, d, s, w=12, h=8) {
  path <- sprintf('%s-chart-%s.pdf', d, s)
  printf("Saving plot %s to %s", s, path)
  pdf(file=path, paper='special', width=w, height=h, pointsize=12)
  print(p)
  null <- dev.off()
}

printf('Loading table from %s...', csvfilename);
csv <- read.csv(csvfilename, strip.white=TRUE, sep=',', header=FALSE);
colnames(csv) <- c('backend', 'bench', 'run', 'instr', 'stage', 'time');

csv <- subset(csv, bench != 'dacapo-tomcat')
csv <- subset(csv, !is.na(time))

csv$backend <- factor(csv$backend, levels=c('runagent', 'instrserver', 'runserver'))
levels(csv$backend) <- c('JNIF', 'ASM Server', 'ASM Server on Client')
csv$instr <- factor(csv$instr, levels=c('Empty', 'Identity', 'Compute', 'Stats', 'All'))
levels(csv$instr) <- c('Empty', 'Identity', 'Compute Frames', 'Allocations', 'Nop Padding')

csv.classes <- subset(csv, !(stage %in% '@total'))
csv.classes <- dcast(csv.classes, backend+bench+run+instr~'count', value.var='time', fun.aggregate=length)

# Instrumentation
csv.instrumentation <- subset(csv, !(stage %in% '@total'))
csv.instrumentation <- dcast(csv.instrumentation, backend+bench+run+instr~'time', value.var='time', fun.aggregate=sum)
colnames(csv.instrumentation) <- c('backend', 'bench', 'run', 'instr', 'instrumentation');

csv.total <- subset(csv, stage %in% '@total')
csv.total$stage <- NULL
colnames(csv.total) <- c('backend', 'bench', 'run', 'instr', 'total');

csv.all <- merge(csv.instrumentation, csv.total, by=c('backend', 'bench', 'run', 'instr'))
csv.all <- melt(csv.all, id.vars=c('backend', 'bench', 'run', 'instr'), variable.name='stage', value.name='time')

theme.config.top <- theme(axis.text.x=element_text(angle=25, hjust=1), legend.box="horizontal", legend.position="top")
theme.config.right <- theme(axis.text.x=element_text(angle=25, hjust=1), legend.box="horizontal", legend.position="right")

# Classes per benchmark
p <-
  ggplot(csv.classes)+
  geom_bar(aes(bench, count, fill=bench), stat='identity', position='dodge')+
  labs(title='Classes loaded per benchmark', x="Benchmark", y = "Loaded classes")+
  theme.config.right
save(p, path, "classes")

# Instrumentation
p <-
  ggplot(csv.instrumentation)+facet_wrap(~bench, scales="free")+
  geom_boxplot(aes(instr, instrumentation, color=backend))+
  labs(x="", y = "Instrumentation time (in seconds)", title='Instrumentation time')+
  theme.config.top
save(p, path, "instr")

# Instrumentation with total
p <-
  ggplot(csv.all)+facet_wrap(~bench, scales="free")+
  geom_boxplot(aes(instr, time, color=stage, fill=backend))+
  labs(x="", y = "Instrumentation and total time (in seconds)", title='Instrumentation and total time')+
  theme.config.top
save(p, path, "all")

# Average time for instrumentation
csv.mean <- dcast(csv, backend+bench+instr~'time', value.var='time', fun.aggregate=mean)

p <-
  ggplot(csv.mean)+facet_grid(instr~backend)+geom_boxplot(aes(bench, time, color=bench))+
  labs(x="Instrumentation kind", y="Average Instrumentation Time (in seconds)", title='Average Instrumentation Time')+
  theme.config.right
save(p, path, "mean")

#csv.stats$diff <- csv.stats[["ASM Frames"]] - csv.stats[["JNIF Frames"]]
#csv.stats <- csv.stats[order(csv.stats$diff),]

#csv.hist <- subset(csv, !(stage %in% '@total'))
#csv.hist <- subset(csv, stage!='@total' & run==1 & bench=="dacapo-eclipse" & instr=='Frame')
#ggplot(csv.hist)+facet_wrap(~backend)+geom_density(aes(x=time, ymax=400))
