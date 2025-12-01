#include "../include/util.h"

int editorRowCursorxToRenderx(erow *row, int cursorx) {
	int renderx = 0;
	for (int j = 0; j < cursorx; j++) {
		if (row->chars[j] == '\t') {
			renderx += (KILO_TAB_STOP - 1) - (renderx % KILO_TAB_STOP);
		}
		renderx++;
	}
	return renderx;
}

int editorRowRenderxToCursorx(erow *row, int renderx) {
	int cur_renderx = 0;
	int i;

	for (i = 0; i < row->size; i++) {
		if (row->chars[i] == '\t')
			cur_renderx += (KILO_TAB_STOP - 1) - (cur_renderx % KILO_TAB_STOP);
		cur_renderx++;

		if (cur_renderx > renderx)
			return i;
	}
	return i;
}

void editorUpdateRow(erow *row) {
	int tabs = 0;
	for (int j = 0; j < row->size; j++)
		if (row->chars[j] == '\t')
			tabs++;

	free(row->render);
	row->render = malloc(row->size + tabs * (KILO_TAB_STOP - 1) + 1);

	int i = 0;
	for (int j = 0; j < row->size; j++) {
		if (row->chars[j] == '\t') {
			row->render[i++] = ' ';
			while (i % KILO_TAB_STOP != 0)
				row->render[i++] = ' ';
		} else {
			row->render[i++] = row->chars[j];
		}
	}
	row->render[i] = '\0';
	row->rsize = i;
}

void editorInsertRow(int at, char *s, size_t len) {
	if (at < 0 || at > E.numrows)
		return;

	E.row = realloc(E.row, sizeof(erow) * (E.numrows + 1));
	memmove(&E.row[at + 1], &E.row[at], sizeof(erow) * (E.numrows - at));

	E.row[at].size = len;
	E.row[at].chars = malloc(len + 1);
	memcpy(E.row[at].chars, s, len);
	E.row[at].chars[len] = '\0';

	E.row[at].rsize = 0;
	E.row[at].render = NULL;
	editorUpdateRow(&E.row[at]);

	E.numrows++;
	E.dirty++;
}

void editorFreeRow(erow *row) {
	free(row->render);
	free(row->chars);
}

void editorDelRow(int at) {
	if (at < 0 || at >= E.numrows)
		return;
	editorFreeRow(&E.row[at]);
	memmove(&E.row[at], &E.row[at + 1], sizeof(erow) * (E.numrows - at - 1));
	E.numrows--;
	E.dirty++;
}

void editorRowInsertChar(erow *row, int at, int c) {
	if (at < 0 || at > row->size)
		at = row->size;
	row->chars = realloc(row->chars, row->size + 2);
	memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1);
	row->size++;
	row->chars[at] = c;
	editorUpdateRow(row);
	E.dirty++;
}

void editorRowAppendString(erow *row, char *s, size_t len) {
	row->chars = realloc(row->chars, row->size + len + 1);
	memcpy(&row->chars[row->size], s, len);
	row->size += len;
	row->chars[row->size] = '\0';
	editorUpdateRow(row);
	E.dirty++;
}

void editorRowDelChar(erow *row, int at) {
	if (at < 0 || at > row->size)
		return;
	memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
	row->size--;
	editorUpdateRow(row);
	E.dirty++;
}
