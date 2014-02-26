#ifndef __BCI_LVTATTRPARSER_H__
#define __BCI_LVTATTRPARSER_H__

#include "../base.hpp"

namespace bci {

/**
 *
 * Local variable table
 */
class LvtAttrParser {
public:

	static constexpr const char* AttrName = "LocalVariableTable";

	template<typename TCodeVisitor>
	inline void parse(BufferReader& br, TCodeVisitor& av, ConstPool& cp,
			u2 nameIndex) {
		u2 lvtlen = br.readu2();

		for (u2 i = 0; i < lvtlen; i++) {
			u2 startPc = br.readu2();
			u2 len = br.readu2();
			u2 varNameIndex = br.readu2();
			u2 varDescIndex = br.readu2();
			u2 index = br.readu2();

			av.visitLvt(startPc, len, varNameIndex, varDescIndex, index);
		}
	}
};

}

#endif
