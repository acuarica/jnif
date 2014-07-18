
suppressPackageStartupMessages(library("ggplot2"))
suppressPackageStartupMessages(library("reshape2"))
suppressPackageStartupMessages(library("tools"))

printf <- function(format, ...) print(sprintf(format, ...))

argv <- commandArgs(trailingOnly = TRUE)

if (interactive()) {
  csvfilename <- '../build/eval.Linux.steklov.all.prof'
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
csv.instrumentation <- subset(csv, !grepl('@', stage) & !grepl('#', stage))
csv.instrumentation <- dcast(csv.instrumentation, backend+bench+run+instr~'time', value.var='time', fun.aggregate=sum)
colnames(csv.instrumentation) <- c('backend', 'bench', 'run', 'instr', 'instrumentation');

# Total times
#csv.totals <- subset(csv, grepl('@', stage) & !(stage %in% '@total'))
#csv.totals <- dcast(csv.totals, backend+bench+run+instr~stage, value.var='time', fun.aggregate=sum)
#csv.totals <- merge(csv.totals, csv.instrumentation, by=c('backend', 'bench', 'run', 'instr'), all=TRUE)
#csv.totals <- melt(csv.totals, id.vars=c('backend', 'bench', 'run', 'instr'), variable.name='stage', value.name='time')


# Plots

theme.config.top <- theme(axis.text.x=element_text(angle=25, hjust=1), legend.box="horizontal", legend.position="top", 
                          panel.background = element_rect(fill = "white", colour = NA), panel.border = element_rect(fill = NA, colour = "grey50"), 
                          panel.grid.major = element_line(colour = "grey90", size = 0.2), 
                          panel.grid.minor = element_line(colour = "grey98", size = 0.5) )
theme.config.right <- theme(axis.text.x=element_text(angle=25, hjust=1), legend.box="horizontal", legend.position="right")

# Classes per benchmark
#p <-
#  ggplot(csv.classes)+
#  geom_bar(aes(bench, count, fill=bench), stat='identity', position='dodge')+
#  labs(title='Classes loaded per benchmark', x="Benchmark", y = "Loaded classes")+
#  theme.config.right
#save(p, path, "classes")

# Instrumentation
p <-
  ggplot(csv.instrumentation)+facet_wrap(~bench, ncol=4, scales="free")+
  geom_boxplot(aes(instr, instrumentation, colour=backend))+scale_colour_grey(start = 0.2, end = 0.8)+
  labs(x="", y = "Instrumentation time (in seconds)")+theme.config.top
save(p, path, "instr", w=12, h=16)

# Total times
#p <-
#  ggplot(csv.totals)+facet_wrap(~bench, scales="free")+
#  geom_bar(aes(interaction(backend,instr), time, fill=stage), stat='identity', position_dodge='dodge')+
#  labs(x="", y = "Instrumentation and total time (in seconds)", title='Instrumentation and total time')+
#  theme.config.right
#save(p, path, "totals")

# Average time for instrumentation
#csv.mean <- dcast(csv, backend+bench+instr~'time', value.var='time', fun.aggregate=mean)

#p <-
#  ggplot(csv.mean)+facet_grid(instr~backend)+geom_boxplot(aes(bench, time, color=bench))+
#  labs(x="Instrumentation kind", y="Average Instrumentation Time (in seconds)", title='Average Instrumentation Time')+
#  theme.config.right
#save(p, path, "mean")

#csv.stats$diff <- csv.stats[["ASM Frames"]] - csv.stats[["JNIF Frames"]]
#csv.stats <- csv.stats[order(csv.stats$diff),]

#csv.hist <- subset(csv, !(stage %in% '@total'))
#csv.hist <- subset(csv, stage!='@total' & run==1 & bench=="dacapo-eclipse" & instr=='Frame')
#ggplot(csv.hist)+facet_wrap(~backend)+geom_density(aes(x=time, ymax=400))

#csvfilename <- 'stats.csv'
#printf('Loading table from %s...', csvfilename);
#stats <- read.csv(csvfilename, strip.white=TRUE, sep=',', header=FALSE);
#colnames(stats) <- c('backend', 'bench', 'run', 'instr', 'stage', 'count');
#stats <- subset(stats, bench != 'dacapo-tomcat')
#stats$stage <- factor(stats$stage, levels=c('#loadedClasses', '#exceptionEntries'))
#levels(stats$stage) <- c('# Loaded Classes', '# Exception Entries')

#p <-
#  ggplot(stats)+
#  geom_bar(aes(bench, count, fill=stage), stat='identity', position='dodge')+
#  labs(title='Classes loaded and exception entries per benchmark', x="Benchmark", y = "Loaded classes and exception entries")+
#  theme(axis.text.x=element_text(angle=25, hjust=1), legend.box="horizontal", legend.position="right",legend.title=element_blank())
#save(p, path, "exceptions")
