Java Native Instrumentation Framework
=================================================

# Introduction

JNIF is the first native Java bytecode rewriting library.
JNIF is a C++ library for decoding, analyzing, editing, 
and encoding Java bytecode.
The main benefit of JNIF is that it can be plugged into a JVMTI agent for 
instrumenting all classes in a JVM transparently, i.e., 
without connecting to another JVM and without perturbing the observed JVM.

JNIF includes a data-flow analysis for stack map generation, 
a complication necessary for any library that provides editing and encoding 
support for modern JVMs with split-time verification.
It is written in C++11, 
in an object-oriented style similar to Java-based class rewriting APIs.

# Documentation

The complete API documentation is available online at

http://acuarica.bitbucket.org/jnif/docs/

# Usage

JNIF can be used both in stand-alone tools or embedded inside a JVMTI agent.

The following listing shows how to read and write a class file.


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


JNIF's **ClassFile** class provides fields and methods for analyzing and 
editing a Java class.

The following listing shows how to traverse all methods in a class
to dump their names and descriptors.

	for (jnif::Method* m : cf.methods) {
	  cout << "Method: ";
	  cout << cf.getUtf8(m->nameIndex);
	  cout << cf.getUtf8(m->descIndex);
	  cout << endl;
	}


The following listing shows how to find all constructors in a class
and how to inject instrumentation, in the form of a call to a static method
**static void alloc(Object o)** of an analysis class,
at the beginning of each constructor.

	ConstIndex mid = cf.addMethodRef(classIndex, "alloc", "(Ljava/lang/Object;)V");

	for (Method* method : cf.methods) {
	  if (method->isInit()) {
	    InstList& instList = method->instList();
	
	    Inst* p = *instList.begin();
	    instList.addZero(OPCODE_aload_0, p);
	    instList.addInvoke(OPCODE_invokestatic, mid, p);
	  }
	}

Besides providing access to all members of a class,
**ClassFile** also provides access to the constant pool
via methods like **getUtf8()** and **addMethodRef()**.

