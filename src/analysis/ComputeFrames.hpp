/*
 * ComputeFrames.hpp
 *
 *  Created on: Jun 25, 2014
 *      Author: luigi
 */

#ifndef JNIF_ANALYSIS_COMPUTEFRAMES_HPP
#define JNIF_ANALYSIS_COMPUTEFRAMES_HPP

namespace jnif {

class ComputeFrames {
public:

	bool isAssignable(const Type& subt, const Type& supt) {
		if (subt == supt) {
			return true;
		}

		if (supt.isTop()) {
			return true;
		}

		if (subt.isNull() && supt.isObject()) {
			return true;
		}

		if (subt.isIntegral() && supt.isInt()) {
			Error::assert(!subt.isInt(), "Invalid subt");
			return true;
		}

		return false;
	}

	String getCommonSuperClass(const String& classLeft,
			const String& classRight, IClassPath* classPath) {
		if (classLeft == "java/lang/Object"
				|| classRight == "java/lang/Object") {
			return "java/lang/Object";
		}

		return classPath->getCommonSuperClass(classLeft, classRight);
	}

	bool assign(Type& t, Type o, IClassPath* classPath,
			TypeFactory& typeFactory) {
		if (!isAssignable(t, o) && !isAssignable(o, t)) {
			if (t.isClass() && o.isClass()) {
				String clazz1 = t.getClassName();
				String clazz2 = o.getClassName();

				String res = getCommonSuperClass(clazz1, clazz2, classPath);

				Type superClass = typeFactory.objectType(res);
				Error::assert((superClass == t) == (res == clazz1),
						"Invalid super class: ", superClass, t, o);

				if (superClass == t) {
					return false;
				}

				t = superClass;
				return true;
			}

			if (t.isArray() && o.isArray()) {
				if (t.getDims() != o.getDims()) {
					t = typeFactory.objectType("java/lang/Object");
					return true;
				}

				Type st = t.stripArrayType(typeFactory);
				Type so = o.stripArrayType(typeFactory);

				//bool change =
				assign(st, so, classPath, typeFactory);
//				Error::assert(change, "Assigning types between ", t,
//						" (with stripped array type ", st, ") and ", o,
//						" (with stripped array type ", so,
//						") should have change the assign result.");

//				Error::assert(!st.isTop(), "Assigning types between ", t,
//						" and ", o,
//						" should have not change assign result to Top.");
				if (st.isTop()) {
					t = typeFactory.objectType("java/lang/Object");
					return true;
				}

				t = typeFactory.arrayType(st, t.getDims());
				return true;
			}

			if ((t.isClass() && o.isArray()) || (t.isArray() && o.isClass())) {
				t = typeFactory.objectType("java/lang/Object");
				return true;
			}

//			Error::raise("We arrived here, and we returning top: ", t, " and ",
//					o);

			t = typeFactory.topType();
			return true;
		}

		if (isAssignable(t, o)) {
			if (t == o) {
				return false;
			}

			t = o;
			return true;
		}

		Error::assert(isAssignable(o, t), "Invalid assign type: ", t, " <> ",
				o);

		return false;
	}

	bool join(TypeFactory& typeFactory, Frame& frame, Frame& how,
			IClassPath* classPath, Method* method = NULL) {
		Error::check(frame.stack.size() == how.stack.size(),
				"Different stack sizes: ", frame.stack.size(), " != ",
				how.stack.size(), ": #", frame, " != #", how, "Method: ",
				method);

		if (frame.lva.size() < how.lva.size()) {
			frame.lva.resize(how.lva.size(), typeFactory.topType());
		} else if (how.lva.size() < frame.lva.size()) {
			how.lva.resize(frame.lva.size(), typeFactory.topType());
		}

		Error::assert(frame.lva.size() == how.lva.size(), "%ld != %ld",
				frame.lva.size(), how.lva.size());

		bool change = false;

		for (u4 i = 0; i < frame.lva.size(); i++) {
			bool assignChanged = assign(frame.lva[i], how.lva[i], classPath,
					typeFactory);
			change = change || assignChanged;
		}

		std::list<Type>::iterator i = frame.stack.begin();
		std::list<Type>::iterator j = how.stack.begin();

		for (; i != frame.stack.end(); i++, j++) {
			bool assignChanged = assign(*i, *j, classPath, typeFactory);
			change = change || assignChanged;
		}

		return change;
	}

	Type getExceptionType(const ConstPool& cp, ConstIndex catchIndex,
			TypeFactory& typeFactory) {
		if (catchIndex != ConstPool::NULLENTRY) {
			const String& className = cp.getClassName(catchIndex);
			return typeFactory.fromConstClass(className);
		} else {
			return typeFactory.objectType("java/lang/Throwable");
		}
	}

	void visitCatch(const CodeExceptionEntry& ex, InstList& instList,
			const ClassFile& cf, const CodeAttr* code, IClassPath* classPath,
			const ControlFlowGraph* cfg, Frame frame, Method* method,
			TypeFactory& typeFactory) {
		int handlerPcId = ex.handlerpc->label()->id;
		BasicBlock* handlerBb = cfg->findBasicBlockOfLabel(handlerPcId);

//		Type exType = [&]() {
//			if (ex.catchtype != ConstPool::NULLENTRY) {
//				const String& className = cf.getClassName(ex.catchtype);
//				return Type::fromConstClass(className);
//			} else {
//				return Type::objectType("java/lang/Throwable");
//			}
//		}();
		const Type& exType = getExceptionType(cf, ex.catchtype, typeFactory);

		frame.clearStack();
		frame.push(exType);

		computeState(*handlerBb, frame, instList, cf, code, classPath, method,
				typeFactory);
	}

	bool contains(const CodeExceptionEntry& ex, const Inst* inst) {
		int start = ex.startpc->label()->_offset;
		int end = ex.endpc->label()->_offset;
		int off = inst->_offset;

		return start <= off && off < end;
	}

	void computeState(BasicBlock& bb, Frame& how, InstList& instList,
			const ClassFile& cf, const CodeAttr* code, IClassPath* classPath,
			Method* method, TypeFactory& typeFactory) {
		if (bb.start == instList.end()) {
			Error::assert(bb.name == "Exit" && bb.exit == instList.end(),
					"exit bb");
			return;
		}

		Error::assert(how.valid, "how valid");
		//Error::assert(bb.in.valid == bb.out.valid, "in.valid != out.valid");

		bool change;
		if (!bb.in.valid) {
			bb.in = how;
			change = true;
		} else {
			change = join(typeFactory, bb.in, how, classPath, method);
		}

		if (change) {

			//auto prepareCatchHandlerFrame = [&](Inst* inst, const Frame& out) {
			//};

			//bb.out = bb.in;
			Frame out = bb.in;
			//Frame outp = out;

			SmtBuilder builder(out, cf, method, typeFactory);
			for (InstList::Iterator it = bb.start; it != bb.exit; ++it) {
				Inst* inst = *it;
				builder.processInst(*inst);
				//prepareCatchHandlerFrame(inst, out);

				//join(typeFactory, outp, out, classPath, method);

				for (const CodeExceptionEntry& ex : code->exceptions) {
					if (contains(ex, inst)) {
						visitCatch(ex, instList, cf, code, classPath, bb.cfg,
								out, method, typeFactory);
					}
				}
			}

			bb.out = out;
			Frame h = bb.out;

			for (BasicBlock* nid : bb) {
				computeState(*nid, h, instList, cf, code, classPath, method,
						typeFactory);
			}
		}
	}
};

}

#endif
