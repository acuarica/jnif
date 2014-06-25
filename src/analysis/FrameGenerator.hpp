/*
 * FrameGenerator.hpp
 *
 *  Created on: Jun 25, 2014
 *      Author: luigi
 */

#ifndef JNIF_ANALYSIS_FRAMEGENERATOR_HPP
#define JNIF_ANALYSIS_FRAMEGENERATOR_HPP

namespace jnif {

class FrameGenerator {
public:

	FrameGenerator(ClassFile& cf, IClassPath* classPath) :
			_attrIndex(ConstPool::NULLENTRY), _cf(cf), _classPath(classPath) {
	}

	void setCpIndex(Type& type, InstList& instList) {
		if (type.isObject()) {
			const String& className = type.getClassName();

			ConstIndex utf8index = _cf.putUtf8(className.c_str());
			ConstIndex index = _cf.addClass(utf8index);
			type.setCpIndex(index);

			if (!type.init) {
				Error::assert(type.uninit.newinst->isType(), "It is not type");
				LabelInst* l = instList.addLabel(type.uninit.newinst);
				//cerr << instList << endl;
				//type.uninit.label = l;
				//type.uninit.newinst->

				type = _typeFactory.uninitType(-1, l);
			}
		}
	}

	void setCpIndex(Frame& frame, InstList& instList) {
		for (Type& type : frame.lva) {
			setCpIndex(type, instList);
		}

		for (Type& type : frame.stack) {
			setCpIndex(type, instList);
		}
	}

	void computeFrames(CodeAttr* code, Method* method) {

		for (auto it = code->attrs.begin(); it != code->attrs.end(); it++) {
			Attr* attr = *it;
			if (attr->kind == ATTR_SMT) {
				code->attrs.attrs.erase(it);
				delete attr;
				break;
			}
		}

		if (!code->instList.hasBranches() && !code->hasTryCatch()) {
			return;
		}

		if (_attrIndex == ConstPool::NULLENTRY) {
			_attrIndex = _cf.putUtf8("StackMapTable");
		}

		Frame initFrame(&_typeFactory);

		u4 lvindex;
		if (method->isStatic()) {
			lvindex = 0;
		} else {
			//|| method->isClassInit()
			String className = _cf.getThisClassName();
			if (method->isInit()) {
				Type u = _typeFactory.uninitThisType();
				u.init = false;
				u.typeId = Type::nextTypeId;
				u.className = className;
				Type::nextTypeId++;
				initFrame.setVar2(0, u);
			} else {
				initFrame.setRefVar(0, className);
			}

			lvindex = 1;
		}

		const char* methodDesc = _cf.getUtf8(method->descIndex);
		std::vector<Type> argsType;
		_typeFactory.fromMethodDesc(methodDesc, &argsType);

		for (Type t : argsType) {
			initFrame.setVar(&lvindex, t);
		}

		ControlFlowGraph* cfgp = new ControlFlowGraph(code->instList,
				_typeFactory);
		code->cfg = cfgp;

		ControlFlowGraph& cfg = *cfgp;

		initFrame.valid = true;
		BasicBlock* bbe = cfg.entry;
		bbe->in = initFrame;
		bbe->out = initFrame;

		BasicBlock* to = *cfg.entry->begin();
		ComputeFrames comp;
		comp.computeState(*to, initFrame, code->instList, _cf, code, _classPath,
				method, _typeFactory);

		SmtAttr* smt = new SmtAttr(_attrIndex, &_cf);

		int totalOffset = -1;

		Frame* f = &cfg.entry->out;
		f->cleanTops();

		class Ser {
		public:

			bool isSame(Frame& current, Frame& prev) {
				return current.lva == prev.lva && current.stack.size() == 0;
			}

			bool isSameLocals1StackItem(Frame& current, Frame& prev) {
				return current.lva == prev.lva && current.stack.size() == 1;
			}

			int isChopAppend(Frame& current, Frame& prev) {
				int diff = current.lva.size() - prev.lva.size();

				for (u4 i = 0;
						i < std::min(current.lva.size(), prev.lva.size());
						++i) {
					if (current.lva.at(i) != prev.lva.at(i)) {
						return 0;
					}
				}

				bool emptyStack = current.stack.size() == 0;
				bool res = diff != 0 && diff >= -3 && diff <= 3 && emptyStack;
				return res ? diff : 0;
			}
		} s;

		for (BasicBlock* bb : cfg) {
			if (bb->start != code->instList.end()) {
				Inst* start = *bb->start;
				if (start->isLabel()
						&& (start->label()->isBranchTarget
								|| start->label()->isCatchHandler)) {
					Frame& current = bb->in;
					current.cleanTops();

					setCpIndex(current, code->instList);

					SmtAttr::Entry e;

					e.label = start;

					totalOffset += 1;
					int offsetDelta = start->label()->offset - totalOffset;

					int diff;

					if (s.isSame(current, *f)) {
						if (offsetDelta <= 63) {
							e.frameType = offsetDelta;
						} else {
							e.frameType = 251;
							e.same_frame_extended.offset_delta = offsetDelta;
						}
					} else if (s.isSameLocals1StackItem(current, *f)) {
						if (offsetDelta <= 63) {
							e.frameType = 64 + offsetDelta;
							const Type& t = current.stack.front();
							e.sameLocals_1_stack_item_frame.stack.push_back(t);
						} else {
							e.frameType = 247;
							const Type& t = current.stack.front();
							e.same_locals_1_stack_item_frame_extended.stack.push_back(
									t);
							e.same_locals_1_stack_item_frame_extended.offset_delta =
									offsetDelta;
						}
					} else if ((diff = s.isChopAppend(current, *f)) != 0) {
						Error::assert(diff != 0 && diff >= -3 && diff <= 3);

						e.frameType = 251 + diff;
						if (diff > 0) {
							e.append_frame.offset_delta = offsetDelta;
							int size = current.lva.size();
							//list<Type> ts;
							for (int i = 0; i < diff; i++) {
								Type t = current.lva[size - diff + i];
								//ts.push_front(t);
								e.append_frame.locals.push_back(t);
							}
							//for (const Type& t : ts) {
							//e.append_frame.locals.push_back(t);
							//}
							//e.append_frame.locals.re
						} else {
							e.chop_frame.offset_delta = offsetDelta;
						}
					} else {
						e.frameType = 255;
						e.full_frame.offset_delta = offsetDelta;
						e.full_frame.locals = current.lva;

						std::list<Type> rs = current.stack;
						rs.reverse();
						for (const Type& t : rs) {
							e.full_frame.stack.push_back(t);
							//e.full_frame.stack.push_front(t);
						}
					}

					totalOffset += offsetDelta;
					smt->entries.push_back(e);
					f = &bb->in;
				}
			}
		}

		//code->cfg = cfgp;

		code->attrs.add(smt);
	}

private:

	ConstIndex _attrIndex;
	ClassFile& _cf;
	IClassPath* _classPath;
	TypeFactory _typeFactory;

};

}

#endif
