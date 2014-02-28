#ifndef JNIF_STACKMAPTABLEATTRPARSER_HPP
#define JNIF_STACKMAPTABLEATTRPARSER_HPP

#include "../base.hpp"
#include "../tree/ConstPool.hpp"

namespace jnif {

#define ITEM_Top 0
#define ITEM_Integer 1
#define ITEM_Float 2
#define ITEM_Long 4
#define ITEM_Double 3
#define ITEM_Null 5
#define ITEM_UninitializedThis 6
#define ITEM_Object 7
#define ITEM_Uninitialized 8

/**
 *
 * SourceFile attribute parser.
 */
class StackMapTableAttrParser {
public:

	static constexpr const char* AttrName = "StackMapTable";

	template<typename TMethodVisitor, typename TReader>
	inline void parse(TReader& br, TMethodVisitor& v, ConstPool& cp,
			u2 nameIndex) {
		auto parseTs = [&](int count) {
			for (u1 i = 0; i < count; i++) {
				u1 tag = br.readu1();
				switch (tag) {
					case ITEM_Top:
					break;
					case ITEM_Integer:
					break;
					case ITEM_Float :

					break;
					case ITEM_Long :

					break;
					case ITEM_Double:
					break;
					case ITEM_Null :
					break;
					case ITEM_UninitializedThis :break;
					case ITEM_Object: {
						u2 cpIndex = br.readu2();
						break;
					}
					case ITEM_Uninitialized: {
						u2 offset= br.readu2();
						break;
					}
				}
			}
		};

		u2 numberOfEntries = br.readu2();

		for (u2 i = 0; i < numberOfEntries; i++) {
			u1 frameType = br.readu1();

			if (0 <= frameType && frameType <= 63) {
				v.visitFrameSame(frameType);
			} else if (64 <= frameType && frameType <= 127) {
				parseTs(1);
				v.visitFrameSameLocals1StackItem(frameType);
			} else if (frameType == 247) {
				u2 offsetDelta = br.readu2();
				parseTs(1);
			} else if (248 <= frameType && frameType <= 250) {
				u2 offsetDelta = br.readu2();

			} else if (frameType == 251) {
				u2 offsetDelta = br.readu2();

			} else if (252 <= frameType && frameType <= 254) {
				u2 offsetDelta = br.readu2();
				parseTs(frameType - 251);

			} else if (frameType == 255) {
				u2 offsetDelta = br.readu2();
				u2 numberOfLocals = br.readu2();
				parseTs(numberOfLocals);
				u2 numberOfStackItems = br.readu2();
				parseTs(numberOfStackItems);
			}
		}
	}
};

}

#endif
