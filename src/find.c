#include "../include/util.h"

void editorFind(void) {
	char *query = editorPrompt("Search: %s (ESC to cancel)");
	if (query == NULL)
		return;

	for (int i = 0; i < E.numrows; i++) {
		erow *row = &E.row[i];
		char *match = strstr(row->render, query);
		if (match) {
			E.cursory = i;
			E.cursorx = editorRowRenderxToCursorx(row, match - row->render);
			E.rowoff = E.numrows;
			break;
		}
	}

	free(query);
}
