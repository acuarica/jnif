Java Native Instrumentation Framework
=================================================

# Introduction

Why instrument?
*  Better control
*  Aspect programming
*  and?

# Documentation

The JNIF documentation is hosted on:

http://acuarica.bitbucket.org/jnif/docs/

# Related work

Jnif uses a clean object model to represent the java class files. Modyfing the
class files is just matter of modifying the object models and then serializer
and deserializer to convert to a memory buffer and send to the jvm ready to 
execution.

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

Goals:
* Modular parser/writer using templates. no more modular!
  Each attribute kind can be plugged into the main parser.
* Zero-memory footprint.
  The API is built so that it uses zero dynamic memory, i.e., no calls to 
  malloc/new! to gain more performance and memory consumption.


# Evaluation

## TODO: try to come up with an performance evaluation 
(maybe against asm, disl, bcel) also with gui apps 
like eclipse, netbeans.

## And also with java agent, i.e., agent with a premain method.
 
## try diffents kind of instrumentations, e.g., instrument heap graph, 
i.e., all allocations (by bytecode, reflection and native).

## Features TODO:
- Control flow graph statically.
- Control flow graph dynamically.
- Object tagging/identification.


- Compare also with an javagent(premain) made with ASM.

## 
# Conclusions


# References

1. ASM

TODO:
Implement the agent in several JVMs, at least Oracle, Jikes and IBM.
