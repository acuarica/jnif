/*
 * analysis.cpp
 *
 *  Created on: Apr 10, 2014
 *      Author: luigi
 */
#include "jnif.hpp"

#include <map>
#include <set>
#include <sstream>
#include <tuple>

using namespace std;

namespace jnif {

struct BasicBlock {
	BasicBlock(InstList::iterator start, InstList::iterator end, string name) :
			start(start), end(end), name(name) {
	}

	InstList::iterator start;
	InstList::iterator end;
	string name;
	Frame in;
	Frame out;
};

template<typename TNode>
class Graph {
public:

	class NodeKey {
		friend class Graph;

	public:
		NodeKey next() const {
			return NodeKey(_index + 1);
		}

		NodeKey(int index) :
				_index(index) {
		}

		int _index;
	private:
	};

	class NodeIterator {
		friend Graph;
	public:
		bool operator!=(const NodeIterator& other) const {
			return current._index != other.current._index;
		}

		NodeKey operator*() {
			return current;
		}

		void operator++() {
			current._index++;
		}

	private:
		NodeIterator(NodeKey current) :
				current(current) {
		}

		NodeKey current;
	};

	class EdgeIterator {
		friend Graph;
	public:
		bool operator!=(const EdgeIterator& other) const {
			return it != other.it;
		}

		NodeKey operator*() {
			return *it;
		}

		void operator++() {
			it++;
		}

	private:
		EdgeIterator(set<int>::iterator it) :
				it(it) {
		}
		set<int>::iterator it;
	};

	class EdgeIterable {
		friend Graph;
	public:
		EdgeIterator begin() const {
			return EdgeIterator(value.begin());
		}

		EdgeIterator end() const {
			return EdgeIterator(value.end());
		}

	private:
		EdgeIterable(set<int> value) :
				value(value) {

		}
		set<int> value;
	};

	NodeKey addNode(const TNode& nodeValue) {
		NodeKey nodeKey(nodes.size());
		nodes.emplace_back(nodeValue);
		return nodeKey;
	}

	inline int nodeCount() const {
		return nodes.size();
	}

	TNode& getNode(NodeKey nodeId) {
		return nodes[nodeId._index].value;
	}

	void addEdge(NodeKey to, NodeKey from) {
		nodes[to._index].outEdges.addEdge(from._index);
		nodes[from._index].inEdges.addEdge(to._index);
	}

	inline NodeIterator begin() {
		return NodeIterator(0);
	}

	inline NodeIterator end() {
		return NodeIterator(nodeCount());
	}

	EdgeIterable outEdges(NodeKey nodeKey) const {
		return EdgeIterable(nodes[nodeKey._index].outEdges.edges);
	}

	EdgeIterable inEdges(NodeKey nodeId) const {
		return EdgeIterable(nodes[nodeId._index].inEdges.edges);
	}

private:

	struct EdgeSet {
		void addEdge(NodeKey nodeId) {
			edges.insert(nodeId._index);
		}

		set<int> edges;
	};

	struct Node {
		Node(const TNode& value) :
				value(value) {
		}
		TNode value;
		EdgeSet outEdges;
		EdgeSet inEdges;
	};

	vector<Node> nodes;
};

struct ControlFlowGraph: Graph<BasicBlock>, private ErrorManager {

	ControlFlowGraph(InstList& instList) :
			entry(getFixedBb(instList, "Entry")), exit(
					getFixedBb(instList, "Exit")) {
		buildCfg(instList);
	}

	NodeKey getFixedBb(InstList& instList, const char* name) {
		return addNode(BasicBlock(instList.end(), instList.end(), name));
	}

	const NodeKey entry;
	const NodeKey exit;

private:

	inline static bool isExit(Inst* inst) {
		Opcode opcode = inst->opcode;
		return (opcode >= OPCODE_ireturn && opcode <= OPCODE_return)
				|| opcode == OPCODE_athrow;
	}

	void buildBasicBlocks(InstList& instList) {
		int bbid = 0;
		auto beginBb = instList.begin();

		auto getBasicBlockName = [&](int bbid) {
			stringstream ss;
			ss << "BB" << bbid;
			return ss.str();
		};

		auto addBasicBlock = [&](InstList::iterator eit) {
			if (beginBb != eit) {
				string name = getBasicBlockName(bbid);
				BasicBlock bb(beginBb, eit, name);
				addNode(bb);

				beginBb = eit;
				bbid++;
			}
		};

		for (auto it = instList.begin(); it != instList.end(); it++) {
			Inst* inst = *it;

			if (inst->kind == KIND_LABEL) {
				addBasicBlock(it);
			}

			if (inst->kind == KIND_JUMP) {
				auto eit = it;
				eit++;
				addBasicBlock(eit);
			}

			if (isExit(inst)) {
				auto eit = it;
				eit++;
				addBasicBlock(eit);
			}
		}
	}

	NodeKey findNodeOfLabel(int labelId, InstList& instList) {
		for (auto nid : *this) {
			BasicBlock& bb = getNode(nid);

			if (bb.start == instList.end()) {
				assert(bb.name == "Entry" || bb.name == "Exit", "");
				assert(bb.end == instList.end(), "");
				continue;
			}

			Inst* inst = *bb.start;
			if (inst->kind == KIND_LABEL && inst->label.id == labelId) {
				return nid;
			}
		}

		raise("se pudrio el chorran");
	}

	void buildCfg(InstList& instList) {

		buildBasicBlocks(instList);

		for (auto nid : *this) {
			BasicBlock& bb = getNode(nid);

			if (bb.start == instList.end()) {
				assert(bb.name == "Entry" || bb.name == "Exit", "");
				assert(bb.end == instList.end(), "");
				continue;
			}

			cerr << bb.name << endl;

			auto e = bb.end;
			e--;
			assert(e != instList.end(), "");

			//Inst* first = *b;
			Inst* last = *e;

			if (bb.start == instList.begin()) {
				addEdge(entry, nid);
			}

			if (last->kind == KIND_JUMP) {
				int labelId = last->jump.label2->label.id;
				NodeKey tbbid = findNodeOfLabel(labelId, instList);
				addEdge(nid, tbbid);

				if (last->opcode != OPCODE_goto) {
					addEdge(nid, nid.next());
				}
			} else if (isExit(last)) {
				addEdge(nid, exit);
			} else {
				addEdge(nid, nid.next());
			}
		}
	}
};

struct DescParser: protected ErrorManager {

	static Type parseFieldDesc(const char*& fieldDesc) {
		const char* originalFieldDesc = fieldDesc;

		int dims = 0;
		while (*fieldDesc == '[') {
			check(*fieldDesc != '\0',
					"Reach end of string while searching for array. Field descriptor: ",
					originalFieldDesc);
			fieldDesc++;
			dims++;
		}

		check(*fieldDesc != '\0', "");

		auto parseBaseType = [&] () {
			switch (*fieldDesc) {
				case 'Z':
				case 'B':
				case 'C':
				case 'S':
				case 'I':
				return Type::intt();
				case 'D':
				return Type::doublet();
				case 'F':
				return Type::floatt();
				case 'J':
				return Type::longt();
				case 'L': {
					fieldDesc++;

					//const char* className = fieldDesc;
				int len = 0;
				while (*fieldDesc != ';') {
					check(*fieldDesc != '\0', "");
					fieldDesc++;
					len++;
				}
				return Type::objectt(-1);
			}
			default:
			raise("Invalid field desc ", originalFieldDesc);
		}};

		Type t = [&]() {
			Type baseType = parseBaseType();
			if (dims == 0) {
				return baseType;
			} else {
				return Type::objectt(-2);
			}
		}();

		fieldDesc++;

		return t;
	}

	static Type parseMethodDesc(const char* methodDesc,
			vector<Type>* argsType) {
		const char* originalMethodDesc = methodDesc;

		check(*methodDesc == '(', "Invalid beginning of method descriptor: ",
				originalMethodDesc);
		methodDesc++;

		while (*methodDesc != ')') {
			check(*methodDesc != '\0', "Reached end of string: ",
					originalMethodDesc);

			Type t = parseFieldDesc(methodDesc);
			argsType->push_back(t);
		}

		check(*methodDesc == ')', "Expected ')' in method descriptor: ",
				originalMethodDesc);
		methodDesc++;

		check(*methodDesc != '\0', "Reached end of string: ",
				originalMethodDesc);

		Type returnType = [&]() {
			if (*methodDesc == 'V') {
				methodDesc++;
				return Type::voidType();
			} else {
				return parseFieldDesc(methodDesc);
			}
		}();

		check(*methodDesc == '\0', "Expected end of string: %s",
				originalMethodDesc);

		return returnType;
	}
};

class SmtBuilder: private DescParser {
public:

	static void computeState(ControlFlowGraph::NodeKey to, Frame& how,
			ControlFlowGraph& cfg, InstList& instList, ClassFile& cf) {

		BasicBlock& bb = cfg.getNode(to);

		if (bb.start == instList.end()) {
			assert(bb.name == "Exit", "");
			assert(bb.end == instList.end(), "");
			return;
		}

		assert(how.valid, "how valid");

		//State& s = states[to._index];

		assert(bb.in.valid == bb.out.valid, "");

		bool change;
		if (!bb.in.valid) {
			bb.in = how;
			bb.out = bb.in;
			change = true;
		} else {
			change = bb.in.join(how);
		}

		if (change) {
			for (auto it = bb.start; it != bb.end; it++) {
				Inst* inst = *it;
				computeFrame(*inst, cf, bb.out);
			}

			Frame h = bb.out;

			for (auto nid : cfg.outEdges(to)) {
//				InstList::iterator b;
//				InstList::iterator e;
//				string name;
//				tie(b, e, name) = cfg.getNode(nid);
//
//				if (b == instList.end()) {
//					ASSERT(name == "Entry" || name == "Exit", "");
//					ASSERT(e == instList.end(), "");
//					continue;
//				}

//				H h = outState;
//				for (auto it = b; it != e; it++) {
//					Inst* inst = *it;
//					computeFrame(*inst, cf, h);
//				}

//				h.print(os);
//				os << "  going to " << name << endl;

				computeState(nid, h, cfg, instList, cf);
			}
		}
	}

	static void computeFrame(Inst& inst, const ConstPool& cp, Frame& h) {

		auto xaload = [&]() {
			h.pop();
			h.pop();
		};
		auto istore = [&](int lvindex) {
			h.pop();
			h.setIntVar(lvindex);
		};
		auto lstore = [&](int lvindex) {
			h.pop();
			h.setLongVar(lvindex);
		};
		auto fstore = [&](int lvindex) {
			h.pop();
			h.setFloatVar(lvindex);
		};
		auto dstore = [&](int lvindex) {
			h.pop();
			h.setDoubleVar(lvindex);
		};
		auto astore = [&](int lvindex) {
			h.pop();
			h.setRefVar(lvindex);
		};
		auto xastore = [&]() {
			h.pop();
			h.pop();
			h.pop();
		};

		auto invoke = [&](u2 methodRefIndex, bool popThis) {
			string className, name, desc;
			cp.getMethodRef(methodRefIndex, &className, &name, &desc);

			const char* d = desc.c_str();
			vector<Type> argsType;
			Type returnType = parseMethodDesc(d, &argsType);

			for (Type argType : argsType) {
				assert(!argType.isTwoWord(), "Two word in method");
				h.pop();
			}

			if (popThis) {
				h.pop();
			}

			if (!returnType.isVoid()) {
				assert(!returnType.isTwoWord(), "Two word in return type");
				h.push(returnType);
			}
		};

		auto fieldType = [&](Inst& inst) {
			string className, name, desc;
			cp.getFieldRef(inst.field.fieldRefIndex, &className, &name, &desc);

			const char* d = desc.c_str();
			auto t = parseFieldDesc(d);

			assert(!t.isTwoWord(), "Two word in field");
			return t;
		};

		switch (inst.opcode) {
			case OPCODE_nop:
				break;
			case OPCODE_aconst_null:
				h.pushNull();
				break;
			case OPCODE_iconst_m1:
			case OPCODE_iconst_0:
			case OPCODE_iconst_1:
			case OPCODE_iconst_2:
			case OPCODE_iconst_3:
			case OPCODE_iconst_4:
			case OPCODE_iconst_5:
			case OPCODE_bipush:
			case OPCODE_sipush:
				h.pushInt();
				break;
			case OPCODE_lconst_0:
			case OPCODE_lconst_1:
				h.pushLong();
				break;
			case OPCODE_fconst_0:
			case OPCODE_fconst_1:
			case OPCODE_fconst_2:
				h.pushFloat();
				break;
			case OPCODE_dconst_0:
			case OPCODE_dconst_1:
				h.pushDouble();
				break;
			case OPCODE_ldc:
			case OPCODE_ldc_w:
			case OPCODE_ldc2_w: {

				//			cp.accept(2, [&](ConstPool::Class i) {
				//				h.pushDouble();
				//			}, [&](ConstPool::FieldRef i) {
				//				h.pushLong();
				//			});

				ConstPool::Tag tag = cp.getTag(inst.ldc.valueIndex);
				switch (tag) {
					case ConstPool::INTEGER:
						h.pushInt();
						break;
					case ConstPool::FLOAT:
						h.pushFloat();
						break;
					case ConstPool::LONG:
						h.pushLong();
						break;
					case ConstPool::DOUBLE:
						h.pushLong();
						break;
					default:
						h.pushRef();
						break;
				}
				break;
			}
			case OPCODE_iload:
			case OPCODE_iload_0:
			case OPCODE_iload_1:
			case OPCODE_iload_2:
			case OPCODE_iload_3:
				h.pushInt();
				break;
			case OPCODE_lload:
			case OPCODE_lload_0:
			case OPCODE_lload_1:
			case OPCODE_lload_2:
			case OPCODE_lload_3:
				h.pushLong();
				break;
			case OPCODE_fload:
			case OPCODE_fload_0:
			case OPCODE_fload_1:
			case OPCODE_fload_2:
			case OPCODE_fload_3:
				h.pushFloat();
				break;
			case OPCODE_dload:
			case OPCODE_dload_0:
			case OPCODE_dload_1:
			case OPCODE_dload_2:
			case OPCODE_dload_3:
				h.pushDouble();
				break;
			case OPCODE_aload:
			case OPCODE_aload_0:
			case OPCODE_aload_1:
			case OPCODE_aload_2:
			case OPCODE_aload_3:
				h.pushRef();
				break;
			case OPCODE_iaload:
			case OPCODE_baload:
			case OPCODE_caload:
			case OPCODE_saload:
				xaload();
				h.pushInt();
				break;
			case OPCODE_laload:
				xaload();
				h.pushLong();
				break;
			case OPCODE_faload:
				xaload();
				h.pushFloat();
				break;
			case OPCODE_daload:
				xaload();
				h.pushDouble();
				break;
			case OPCODE_aaload:
				xaload();
				h.pushRef();
				break;
			case OPCODE_istore:
				istore(inst.var.lvindex);
				break;
			case OPCODE_lstore:
				lstore(inst.var.lvindex);
				break;
			case OPCODE_fstore:
				fstore(inst.var.lvindex);
				break;
			case OPCODE_dstore:
				dstore(inst.var.lvindex);
				break;
			case OPCODE_astore:
				astore(inst.var.lvindex);
				break;
			case OPCODE_istore_0:
				istore(0);
				break;
			case OPCODE_istore_1:
				istore(1);
				break;
			case OPCODE_istore_2:
				istore(2);
				break;
			case OPCODE_istore_3:
				istore(3);
				break;
			case OPCODE_lstore_0:
				lstore(0);
				break;
			case OPCODE_lstore_1:
				lstore(1);
				break;
			case OPCODE_lstore_2:
				lstore(2);
				break;
			case OPCODE_lstore_3:
				lstore(3);
				break;
			case OPCODE_fstore_0:
				fstore(0);
				break;
			case OPCODE_fstore_1:
				fstore(1);
				break;
			case OPCODE_fstore_2:
				fstore(2);
				break;
			case OPCODE_fstore_3:
				fstore(3);
				break;
			case OPCODE_dstore_0:
				dstore(0);
				break;
			case OPCODE_dstore_1:
				dstore(1);
				break;
			case OPCODE_dstore_2:
				dstore(2);
				break;
			case OPCODE_dstore_3:
				dstore(3);
				break;
			case OPCODE_astore_0:
				astore(0);
				break;
			case OPCODE_astore_1:
				astore(1);
				break;
			case OPCODE_astore_2:
				astore(2);
				break;
			case OPCODE_astore_3:
				astore(3);
				break;
			case OPCODE_iastore:
				xastore();
				break;
			case OPCODE_lastore:
				xastore();
				break;
			case OPCODE_fastore:
				xastore();
				break;
			case OPCODE_dastore:
				xastore();
				break;
			case OPCODE_aastore:
				xastore();
				break;
			case OPCODE_bastore:
				xastore();
				break;
			case OPCODE_castore:
				xastore();
				break;
			case OPCODE_sastore:
				xastore();
				break;
			case OPCODE_pop:
				h.pop();
				break;
			case OPCODE_pop2:
				h.pop();
				h.pop();
				break;
			case OPCODE_dup: {
				auto t1 = h.pop();
				h.push(t1);
				h.push(t1);
				break;
			}
			case OPCODE_dup_x1: {
				auto t1 = h.pop();
				auto t2 = h.pop();
				h.push(t1);
				h.push(t2);
				h.push(t1);
				break;
			}
			case OPCODE_dup_x2: {
				auto t1 = h.pop();
				auto t2 = h.pop();
				auto t3 = h.pop();
				h.push(t1);
				h.push(t3);
				h.push(t2);
				h.push(t1);
				break;
			}
			case OPCODE_dup2: {
				auto t1 = h.pop();
				auto t2 = h.pop();
				h.push(t2);
				h.push(t1);
				h.push(t2);
				h.push(t1);
				break;
			}
			case OPCODE_dup2_x1: {
				auto t1 = h.pop();
				auto t2 = h.pop();
				auto t3 = h.pop();
				h.push(t2);
				h.push(t1);
				h.push(t3);
				h.push(t2);
				h.push(t1);
				break;
			}
			case OPCODE_dup2_x2: {
				auto t1 = h.pop();
				auto t2 = h.pop();
				auto t3 = h.pop();
				auto t4 = h.pop();
				h.push(t2);
				h.push(t1);
				h.push(t4);
				h.push(t3);
				h.push(t2);
				h.push(t1);
				break;
			}
			case OPCODE_swap: {
				auto t1 = h.pop();
				auto t2 = h.pop();
				h.push(t1);
				h.push(t2);
				break;
			}
			case OPCODE_iadd:
			case OPCODE_fadd:
			case OPCODE_isub:
			case OPCODE_fsub:
			case OPCODE_imul:
			case OPCODE_fmul:
			case OPCODE_idiv:
			case OPCODE_fdiv:
			case OPCODE_irem:
			case OPCODE_frem:
			case OPCODE_ishl:
			case OPCODE_ishr:
			case OPCODE_iushr:
			case OPCODE_iand:
			case OPCODE_ior:
			case OPCODE_ixor: {
				auto t1 = h.pop();
				h.pop();
				h.push(t1);
				break;
			}
			case OPCODE_ladd:
			case OPCODE_lsub:
			case OPCODE_lmul:
			case OPCODE_ldiv:
			case OPCODE_lrem:
			case OPCODE_lshl:
			case OPCODE_lshr:
			case OPCODE_lushr:
			case OPCODE_land:
			case OPCODE_lor:
			case OPCODE_lxor: {
				h.pop();
				h.pop();
				h.pop();
				h.pop();
				h.pushLong();
				break;
			}
			case OPCODE_dadd:
			case OPCODE_dsub:
			case OPCODE_dmul:
			case OPCODE_ddiv:
			case OPCODE_drem: {
				h.pop();
				h.pop();
				h.pop();
				h.pop();
				h.pushDouble();
				break;
			}
			case OPCODE_ineg:
			case OPCODE_fneg: {
				auto t1 = h.pop();
				h.push(t1);
				break;
			}
			case OPCODE_lneg: {
				h.pop();
				h.pop();
				h.pushLong();
				break;
			}
			case OPCODE_dneg: {
				h.pop();
				h.pop();
				h.pushDouble();
				break;
			}
			case OPCODE_iinc:
				h.setIntVar(inst.iinc.index);
				break;
			case OPCODE_i2l:
				h.pop();
				h.pushLong();
				break;
			case OPCODE_i2f:
				h.pop();
				h.pushFloat();
				break;
			case OPCODE_i2d:
				h.pop();
				h.pushDouble();
				break;
			case OPCODE_l2i:
				h.pop();
				h.pop();
				h.pushInt();
				break;
			case OPCODE_l2f:
				h.pop();
				h.pop();
				h.pushFloat();
				break;
			case OPCODE_l2d:
				h.pop();
				h.pop();
				h.pushDouble();
				break;
			case OPCODE_f2i:
				h.pop();
				h.pushInt();
				break;
			case OPCODE_f2l:
				h.pop();
				h.pushLong();
				break;
			case OPCODE_f2d:
				h.pop();
				h.pushDouble();
				break;
			case OPCODE_d2i:
				h.pop();
				h.pop();
				h.pushInt();
				break;
			case OPCODE_d2l:
				h.pop();
				h.pop();
				h.pushLong();
				break;
			case OPCODE_d2f:
				h.pop();
				h.pop();
				h.pushFloat();
				break;
			case OPCODE_i2b:
			case OPCODE_i2c:
			case OPCODE_i2s:
				h.pop();
				h.pushInt();
				break;
			case OPCODE_lcmp:
				h.pop();
				h.pop();
				h.pop();
				h.pop();
				h.pushInt();
				break;
			case OPCODE_fcmpl:
			case OPCODE_fcmpg:
				h.pop();
				h.pop();
				h.pushInt();
				break;
			case OPCODE_dcmpl:
			case OPCODE_dcmpg:
				h.pop();
				h.pop();
				h.pop();
				h.pop();
				h.pushInt();
				break;
			case OPCODE_ifeq:
			case OPCODE_ifne:
			case OPCODE_iflt:
			case OPCODE_ifge:
			case OPCODE_ifgt:
			case OPCODE_ifle:
				h.pop();
				break;
			case OPCODE_if_icmpeq:
			case OPCODE_if_icmpne:
			case OPCODE_if_icmplt:
			case OPCODE_if_icmpge:
			case OPCODE_if_icmpgt:
			case OPCODE_if_icmple:
				h.pop();
				h.pop();
				break;
			case OPCODE_if_acmpeq:
			case OPCODE_if_acmpne:
				h.pop();
				h.pop();
				break;
			case OPCODE_goto:
				break;
			case OPCODE_jsr:
				assert(false, "jsr not implemented");
				break;
			case OPCODE_ret:
				assert(false, "jsr not implemented");
				break;
			case OPCODE_tableswitch:
			case OPCODE_lookupswitch:
				h.pop();
				break;
			case OPCODE_ireturn:
				h.pop();
				break;
			case OPCODE_lreturn:
				h.pop();
				h.pop();
				break;
			case OPCODE_freturn:
				h.pop();
				break;
			case OPCODE_dreturn:
				h.pop();
				h.pop();
				break;
			case OPCODE_areturn:
				h.pop();
				break;
			case OPCODE_return:
				break;
			case OPCODE_getstatic: {
				auto t = fieldType(inst);
				h.push(t);
				break;
			}
			case OPCODE_putstatic:
				h.pop(); // wrong: could be double or long.
				break;
			case OPCODE_getfield:
				h.pop(); // wrong: could be double or long.
				h.pushRef();
				break;
			case OPCODE_putfield:
				h.pop();
				h.pop();
				break;
			case OPCODE_invokevirtual:
			case OPCODE_invokespecial:
				invoke(inst.invoke.methodRefIndex, true);
				break;
			case OPCODE_invokestatic:
				invoke(inst.invoke.methodRefIndex, false);
				break;
			case OPCODE_invokeinterface:
				invoke(inst.invokeinterface.interMethodRefIndex, true);
				break;
			case OPCODE_invokedynamic:
				raise("invoke dynamic instances not implemented");
				break;
			case OPCODE_new:
				h.pushRef();
				break;
			case OPCODE_newarray:
				h.pop();
				h.pushRef();
				break;
			case OPCODE_anewarray:
				h.pop();
				h.pushRef();
				break;
			case OPCODE_arraylength:
				h.pop();
				h.pushInt();
				break;
			case OPCODE_athrow: {
				auto t = h.pop();
				h.clearStack();
				h.push(t);
				break;
			}
			case OPCODE_checkcast:
			case OPCODE_instanceof:
			case OPCODE_monitorenter:
			case OPCODE_monitorexit:
				raise("athrow checkcast instanceof me, me not implemented");
				break;
			case OPCODE_wide:
			case OPCODE_multianewarray:
				raise("wide, multianewarray not implemented");
				break;
			case OPCODE_ifnull:
			case OPCODE_ifnonnull:
				h.pop();
				break;
			case OPCODE_goto_w:
			case OPCODE_jsr_w:
			case OPCODE_breakpoint:
			case OPCODE_impdep1:
			case OPCODE_impdep2:
				raise("goto_w, jsr_w breakpoint not implemented");
				break;
			default:
				break;
		}
	}

};

static void printCfg(ControlFlowGraph& cfg, ostream& os) {
	for (auto nid : cfg) {
		BasicBlock& bb = cfg.getNode(nid);

		os << "* " << bb.name;

		auto printEdges =
				[&](ControlFlowGraph::EdgeIterable edges, const char* kind, const char* arrow) {
					os << " @" << kind << " { ";
					for (auto eid : edges) {
						auto bbt = cfg.getNode(eid);
						os << arrow << bbt.name << ", ";
					}
					os << "} ";
				};

		printEdges(cfg.outEdges(nid), "Out", "->");
		printEdges(cfg.inEdges(nid), "In", "<-");
		os << endl;

//		for (auto it = b; it != e; it++) {
//			Inst* inst = *it;
//			//printInst(*inst);
//			//os << endl;
//		}
	}

	os << endl;
}

//static void appendComputedFrames(CodeAttr& code) {
//
//}

static void computeFramesMethod(CodeAttr& code, Method& method, ClassFile& cf) {

	code.instList.setLabelIds();

	cerr << cf.getUtf8(method.nameIndex) << endl;
	Frame initFrame;

	int lvindex = [&]() {
		if (method.accessFlags & ACC_STATIC) {
			return 0;
		} else {
			initFrame.setRefVar(0); // this argument
			return 1;
		}
	}();

	const char* methodDesc = cf.getUtf8(method.descIndex);
	vector<Type> argsType;
	DescParser::parseMethodDesc(methodDesc, &argsType);

	for (Type t : argsType) {
		initFrame.setVar(lvindex, t);
		lvindex++;
	}

	ControlFlowGraph cfg(code.instList);
//	printCfg(cfg, cerr);

	//vector<State> states(cfg.nodeCount());
	initFrame.valid = true;
	BasicBlock& bbe = cfg.getNode(cfg.entry);
	bbe.in = initFrame;
	bbe.out = initFrame;

	//states[cfg.entry._index] = {initFrame, initFrame};

	auto to = *cfg.outEdges(cfg.entry).begin();
	SmtBuilder::computeState(to, initFrame, cfg, code.instList, cf);
}

void ClassFile::computeFrames() {

	for (Method& method : methods) {
		CodeAttr* code = method.codeAttr();

		if (code != nullptr) {
			computeFramesMethod(*code, method, *this);
		}
	}
}

}
