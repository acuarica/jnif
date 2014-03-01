(Low Level) Java Native Instrumentation Framework
=================================================

# Introduction


Why instrument?
*  Better control
*  Aspect programming
*  and?

# Related work

Ability to **instrument** the JVM by only adding a small agent.


Usually to instrument the *JVM* another JVM is needed.
To instrument regular applications this is just fine,
but when you need to instrument the java library itself you encounter 
bootstrap problems.
That is why a native approach is more accurate.

Instrumentation and client code are more coupled, this is how it should be.

It allows to simplify the development because you do not need a TCP 
connection to instrument code on other JVM process.
Lowoverhead agent.
Modular parser, you pay what you need. Full power of templates for better 
performance.


Since the instrumentation should be really really fast, and therefore having a 
negligent overhead,
* it could be added in production environments?
* and also it will be more benefitial in debuggers and profilers where you may 
instrument on demand. 

# Development

Modular parser using templates.

# Evaluation

## TODO: try to come up with an performance evaluation 
(maybe against asm, disl, bcel) also with gui apps 
like eclipse, netbeans.

## And also with java agent, i.e., agent with a premain method.
 
## try diffents kind of instrumentations, e.g., instrument heap graph, 
i.e., all allocations (by bytecode, reflection and native).

## 
# Conclusions


# References

1. ASM

TODO:
Implement the agent in several JVMs, at least Oracle, Jikes and IBM.

> ## To be published in PPPJ'14.
> Why PPPJ is the best fit for this article?
> They allow tool papers, and that is exacty what it is.
