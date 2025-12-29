#include "../include/util.h"

void abAppend(struct abuf *ab, const char *s, int len) {
	char *new = realloc(ab->b, ab->len + len);

	if (new == NULL) {
		return;
	}

	memcpy(&new[ab->len], s, len);
	ab->b = new;
	ab->len += len;
}

void abFree(struct abuf *ab) { free(ab->b); }

void editorScroll(void) {
	E.renderx = 0;
	if (E.cursory < E.numrows)
		E.renderx = editorRowCursorxToRenderx(&E.row[E.cursory], E.cursorx);

	if (E.cursory < E.rowoff)
		E.rowoff = E.cursory;

	if (E.cursory >= E.rowoff + E.screenrows)
		E.rowoff = E.cursory - E.screenrows + 1;

	if (E.renderx < E.coloff)
		E.coloff = E.renderx;

	if (E.renderx >= E.coloff + E.screencols)
		E.coloff = E.renderx - E.screencols + 1;
}

void editorDrawRows(struct abuf *ab) {
	for (int i = 0; i < E.screenrows; i++) {
		int filerow = i + E.rowoff;
		if (filerow >= E.numrows) {
			if (i == E.screenrows / 3 && E.numrows == 0) {
				char welcome[80];
				int welcomelen = snprintf(welcome, sizeof(welcome), "Gram editor -- version %s", GRAM_VERSION);
				if (welcomelen > E.screencols) {
					welcomelen = E.screencols;
				}

				int padding = (E.screencols - welcomelen) / 2;
				if (padding) {
					abAppend(ab, "~", 1);
					padding--;
				}

				while (padding--) {
					abAppend(ab, " ", 1);
				}

				abAppend(ab, welcome, welcomelen);
			} else {
				abAppend(ab, "~", 1);
			}
		} else {
			int len = E.row[filerow].rsize - E.coloff;
			if (len < 0)
				len = 0;
			if (len > E.screencols)
				len = E.screencols;

			char *c = &E.row[filerow].render[E.coloff];
			unsigned char *hl = &E.row[filerow].hl[E.coloff];
			int current_color = -1;

			// Row numbers
			char rowidx[12];
			int rowidxlen = snprintf(rowidx, sizeof(rowidx), "%4d ", E.row[filerow].idx + 1);
			abAppend(ab, rowidx, rowidxlen);

			for (int i = 0; i < len; i++) {
				if (iscntrl(c[i])) { // Control characters
					char sym = (c[i] <= 26) ? '@' + c[i] : '?';
					abAppend(ab, "\x1b[7m", 4);
					abAppend(ab, &sym, 1);
					abAppend(ab, "\x1b[m", 3);

					if (current_color != -1) {
						char buf[16];
						int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", current_color);
						abAppend(ab, buf, clen);
					}
				} else if (hl[i] == HL_NORMAL) { // Normal text
					if (current_color != -1) {
						abAppend(ab, "\x1b[39m", 5);
						current_color = -1;
					}

					abAppend(ab, &c[i], 1);
				} else { // Anything else
					int color = editorSyntaxToColor(hl[i]);
					if (color != current_color) {
						current_color = color;
						char buf[16];
						int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", color);
						abAppend(ab, buf, clen);
					}

					abAppend(ab, &c[i], 1);
				}
			}

			abAppend(ab, "\x1b[39m", 5);
		}

		abAppend(ab, "\x1b[K", 3);
		abAppend(ab, "\r\n", 2);
	}
}

void editorDrawStatusBar(struct abuf *ab) {
	abAppend(ab, "\x1b[7m", 4);

	char status[80], mstatus[10], rstatus[80];
	char *mode;

	if (E.vimmode == NORMAL) {
		mode = "NORMAL";
	}
	if (E.vimmode == INSERT) {
		mode = "INSERT";
	}

	int len = snprintf(status, sizeof(status), "%.20s %s", E.filename ? E.filename : "[No Name]",
					   E.dirty ? "(modified)" : "");
	int mlen = snprintf(mstatus, sizeof(mstatus), "%s", mode);
	int rlen = snprintf(rstatus, sizeof(rstatus), "%s | %d/%d", E.syntax ? E.syntax->filetype : "No filetype",
						E.cursory + 1, E.numrows);

	if (len > E.screencols)
		len = E.screencols;

	abAppend(ab, status, len);

	while (len < E.screencols) {
		if (len == E.screencols / 2 - mlen / 2) {
			abAppend(ab, mstatus, mlen);
			len += mlen;
		} else if (E.screencols - len == rlen) {
			abAppend(ab, rstatus, rlen);
			break;
		} else {
			abAppend(ab, " ", 1);
			len++;
		}
	}

	abAppend(ab, "\x1b[m", 3);
	abAppend(ab, "\r\n", 2);
}

void editorDrawMessageBar(struct abuf *ab) {
	abAppend(ab, "\x1b[K", 3);

	int msglen = strlen(E.statusmsg);
	if (msglen > E.screencols)
		msglen = E.screencols;
	if (msglen && time(NULL) - E.statusmsg_time < 5)
		abAppend(ab, E.statusmsg, msglen);
}

void editorRefreshScreen(void) {
	editorScroll();

	struct abuf ab = ABUF_INIT;

	abAppend(&ab, "\x1b[?25l", 6);
	abAppend(&ab, "\x1b[H", 3);

	editorDrawRows(&ab);
	editorDrawStatusBar(&ab);
	editorDrawMessageBar(&ab);

	char buf[32];
	snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (E.cursory - E.rowoff) + 1, (E.renderx - E.coloff) + 1);
	abAppend(&ab, buf, strlen(buf));

	abAppend(&ab, "\x1b[?25h", 6);

	write(STDOUT_FILENO, ab.b, ab.len);
	abFree(&ab);
}

void editorSetStatusMessage(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(E.statusmsg, sizeof(E.statusmsg), fmt, ap);
	va_end(ap);
	E.statusmsg_time = time(NULL);
}
