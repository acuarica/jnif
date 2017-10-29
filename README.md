![Build Status](https://gitlab.com/pages/doxygen/badges/master/build.svg)


# Java Native Instrumentation Framework

## Introduction

JNIF is the first native Java bytecode rewriting library.
JNIF is a C++ library for decoding, analyzing, editing, 
and encoding Java bytecode.
The main benefit of JNIF is that it can be plugged into a JVMTI agent for 
instrumenting all classes in a JVM transparently, i.e., 
without connecting to another JVM and without perturbing the observed JVM.
Besides, JNIF can be used in stand-alone tools as well.

JNIF includes a data-flow analysis for stack map generation, 
a complication necessary for any library that provides editing and encoding 
support for modern JVMs with split-time verification.
It is written in C++11, 
in an object-oriented style similar to Java-based class rewriting APIs.

## Documentation

The complete API documentation is available online at

http://acuarica.bitbucket.io/jnif/

## Installation

The JNIF library can be used as a static library which can be then linked 
with your project.

To compile JNIF it is enough to "make" the library:

```sh
make
```

All sources of JNIF are contained in the `src` folder.

To use JNIF classes, the header file `jnif.hpp` must be included.

```c++
#include <jnif.hpp>
```

Make sure to add the `src` folder to the include search directories so 
your compiler can found JNIF's headers, *i.e.*, with the `-I` compiler flag 
and then link your project against the `libjnif.a`.

## Getting started

This section shows common use cases of the JNIF library, 
such as writing instrumentation code and analyzing class files, 
thus giving an overview of the library. 
We present the examples in an incremental fashion, 
adding complexity in each example.

In order to be able to work with class files, first they must me parsed. 
Given a memoery buffer with a class file and its length, 
the following snippet shows how to parse it.

```c++
const char* data = ...;
int len = ...;

jnif::ClassFile cf(data, len);
```

JNIF's `ClassFile` class provides fields and methods for analyzing and 
editing a Java class. 
It contains the definition of each method and field declared in the Java class. 

Once a class file is correctly parsed and loaded it can be manipulated using 
the methods and fields in `ClassFile`. 
For instance, in order to write back the parsed class file in a new buffer, 
the write method is used in conjunction with the `computeSize` method as 
shown below.

```c++
const char* data = ...;
int len = ...;
jnif::ClassFile cf(data, len);
int newlen = cf.computeSize();
u1* newdata = new u1[newlen];
cf.write(newdata, newlen);

// Use newdata and newlen

delete [] newdata;
```

Putting all together, 
the following listing shows how to read and write a class file.

```c++
// Decode the binary data into a ClassFile object
const char* data = ...;
int len = ...;
jnif::ClassFile cf(data, len);

// Analyze or edit the ClassFile
...

// Encode the ClassFile into binary
int newlen = cf.computeSize();
u1* newdata = new u1[newlen];
cf.write(newdata, newlen);

// Use newdata and newlen
...

// Free the new binary
delete [] newdata;
```

The `ClassFile` class has a collection of fields and methods which can 
be used to discover the members of the class file. 
The snippet below shows how to traverse all methods in a class
to dump their names and descriptors.
Note that every `jnif` class overloads the `operator<<` in order 
send it to an `std::ostream`.

```c++
const char* data = ...;
int len = ...;
jnif::ClassFile cf(data, len);

for (jnif::Method* m : cf.methods) {
  cout << "Method: ";
  cout << cf.getUtf8(m->nameIndex);
  cout << cf.getUtf8(m->descIndex);
  cout << endl;
}
```

The following listing shows how to find all constructors 
(named `<init>` at Java bytecode level) in a class
and how to inject instrumentation, in the form of a call to a static method
`static void alloc(Object o)` of an analysis class,
at the beginning of each constructor.

```c++
ConstIndex mid = cf.addMethodRef(classIndex, "alloc", "(Ljava/lang/Object;)V");

for (Method* method : cf.methods) {
  if (method->isInit()) {
    InstList& instList = method->instList();

    Inst* p = *instList.begin();
    instList.addZero(OPCODE_aload_0, p);
    instList.addInvoke(OPCODE_invokestatic, mid, p);
  }
}
```

Another common use case is to instrument every method entry and exit. 
In order to do so, it is possible to add the instrumentation code at the 
beginning of the instruction list to detect the method entry. 
To detect method exit, 
it is necessary to look for instructions that terminate the current method 
execution, i.e., 
xRETURN family and ATHROW as showed in the following snippet.

```c++
ConstIndex sid = cf.addMethodRef(proxyClass, "enterMethod",
                "(Ljava/lang/String;Ljava/lang/String;)V");
ConstIndex eid = cf.addMethodRef(proxyClass, "exitMethod",
                "(Ljava/lang/String;Ljava/lang/String;)V");
ConstIndex classNameIdx = cf.addStringFromClass(cf.thisClassIndex);

...

InstList& instList = method->instList();

ConstIndex methodIndex = cf.addString(m->nameIndex);

Inst* p = *instList.begin();

instList.addLdc(OPCODE_ldc_w, classNameIdx, p);
instList.addLdc(OPCODE_ldc_w, methodIndex, p);
instList.addInvoke(OPCODE_invokestatic, sid, p);

for (Inst* inst : instList) {
    if (inst->isExit()) {
        instList.addLdc(OPCODE_ldc_w, classNameIdx, inst);
        instList.addLdc(OPCODE_ldc_w, methodIndex, inst);
        instList.addInvoke(OPCODE_invokestatic, eid, inst);
    }
}
```

Besides providing access to all members of a class,
`ClassFile` also provides access to the constant pool
via methods like `getUtf8()` and `addMethodRef()`.

## Test and evaluation

To run these evaluations, a Makefile script is provided in the git repository.
These tasks take care of the compilation of the JNIF library and also 
all java files needed. 
The repository is self-contained, 
no need to download dacapo benchmarks separately.

```sh
> make testapp
```

To run a particular dacapo benchmark with default settings:

```sh
> make dacapo BENCH=avrora
```

To run a full evaluation with all dacapo benchmarks in all configuration a 
task -eval- is provided. 
You can set how many times run each configuration with the variable times, like

```sh
> make eval times=5
```

Finally, there is a task to create plots for the evaluation. 
This task needs R with the package ggplot2.

```sh
> make plots
```
