#include "../include/util.h"

void editorInsertChar(int c) {
	if (E.cursory == E.numrows) {
		editorInsertRow(E.numrows, "", 0);
	}
	editorRowInsertChar(&E.row[E.cursory], E.cursorx, c);
	E.cursorx++;
}

void editorInsertNewline(void) {
	if (E.cursorx == 0) {
		editorInsertRow(E.cursory, "", 0);
	} else {
		erow *row = &E.row[E.cursory];
		editorInsertRow(E.cursory + 1, &row->chars[E.cursorx], row->size - E.cursorx);
		row = &E.row[E.cursory];
		row->size = E.cursorx;
		row->chars[row->size] = '\0';
		editorUpdateRow(row);
	}
	E.cursory++;
	E.cursorx = 0;
}

void editorDelChar(void) {
	if (E.cursory == E.numrows)
		return;
	if (E.cursorx == 0 && E.cursory == 0)
		return;

	erow *row = &E.row[E.cursory];
	if (E.cursorx > 0) {
		editorRowDelChar(row, E.cursorx - 1);
		E.cursorx--;
	} else {
		E.cursorx = E.row[E.cursory - 1].size;
		editorRowAppendString(&E.row[E.cursory - 1], row->chars, row->size);
		editorDelRow(E.cursory);
		E.cursory--;
	}
}
