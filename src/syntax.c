#include "../include/util.h"

void editorUpdateSyntax(erow *row) {
	row->hl = realloc(row->hl, row->rsize);
	memset(row->hl, HL_NORMAL, row->rsize);

	for (int i = 0; i < row->rsize; i++) {
		if (isdigit(row->render[i])) {
			row->hl[i] = HL_NUMBER;
		}
	}
}

int editorSyntaxToColor(int hl) {
	switch (hl) {
	case HL_NUMBER:
		return 31;
	default:
		return 37;
	}
}
