#include "../include/util.h"

void editorFindCallBack(char *query, int key) {
	static int last_match = -1;
	static int direction = 1;

	if (key == '\r' || key == '\x1b') {
		last_match = -1;
		direction = 1;
		return;
	} else if (key == ARROW_RIGHT || key == ARROW_DOWN) {
		direction = 1;
	} else if (key == ARROW_LEFT || key == ARROW_UP) {
		direction = -1;
	} else {
		last_match = -1;
		direction = 1;
	}

	if (last_match == -1)
		direction = 1;
	int current = last_match;
	for (int i = 0; i < E.numrows; i++) {
		current += direction;
		if (current == -1) {
			current = E.numrows - 1;
		} else if (current == E.numrows) {
			current = 0;
		}

		erow *row = &E.row[current];
		char *match = strstr(row->render, query);
		if (match) {
			last_match = current;
			E.cursory = current;
			E.cursorx = editorRowRenderxToCursorx(row, match - row->render);
			E.rowoff = E.numrows;
			break;
		}
	}
}

void editorFind(void) {
	int saved_cursorx = E.cursorx;
	int saved_cursory = E.cursory;
	int saved_rowoff = E.rowoff;
	int saved_coloff = E.coloff;

	char *query = editorPrompt("Search: %s (ESC/Arrows/Enter)", editorFindCallBack);

	if (query) {
		free(query);
	} else {
		E.cursorx = saved_cursorx;
		E.cursory = saved_cursory;
		E.rowoff = saved_rowoff;
		E.coloff = saved_coloff;
	}
}
