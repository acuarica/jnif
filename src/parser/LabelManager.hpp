/*
 * LabelManager.hpp
 *
 *  Created on: Jun 23, 2014
 *      Author: luigi
 */

#ifndef JNIF_PARSER_LABELMANAGER_HPP
#define JNIF_PARSER_LABELMANAGER_HPP

namespace jnif {

class LabelManager {
public:

	LabelManager(u4 codeLen, InstList& instList) :
			codeLen(codeLen), instList(instList), labels(
					new LabelInst*[codeLen + 1]) {
		for (u4 i = 0; i < codeLen + 1; i++) {
			labels[i] = NULL;
		}
	}

	~LabelManager() {
		delete[] labels;
	}

	LabelInst* createLabel(int labelPos) {
		Error::check(0 <= labelPos, "Invalid position for label: ", labelPos);
		Error::check((u4) labelPos < codeLen + 1,
				"Invalid position for label: ", labelPos, ", : ", codeLen);

		LabelInst*& lab = labels[labelPos];
		if (lab == NULL) {
			lab = instList.createLabel();
		}

		return lab;
	}

	LabelInst* createExceptionLabel(u2 labelPos, bool isTryStart, bool isTryEnd,
			bool isCatchHandler) {
		Error::check(labelPos != codeLen || isTryEnd,
				"Only tryEnd can have a labelPos equal to codeLen.");

		LabelInst* label = createLabel(labelPos);
		label->isTryStart = label->isTryStart || isTryStart;
		label->isTryEnd = label->isTryEnd || isTryEnd;
		label->isCatchHandler = label->isCatchHandler || isCatchHandler;

		return label;
	}

	bool hasLabel(u2 labelPos) const {
		Error::assert(labelPos < codeLen + 1, "Invalid position for label: ",
				labelPos);

		return labels[labelPos] != NULL;
	}

	void putLabelIfExists(u2 labelPos) const {
		if (hasLabel(labelPos)) {
			LabelInst* label = (*this)[labelPos];
			label->_offset = labelPos;
			instList.addLabel(label);
		}
	}

	LabelInst* operator[](u2 labelPos) const {
		Error::assert(hasLabel(labelPos), "No label in position: ", labelPos);

		return labels[labelPos];
	}

	u4 codeLen;

	InstList& instList;
private:

	LabelInst** labels;
};

}

#endif
