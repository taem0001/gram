#include "../include/util.h"

char *editorPrompt(char *prompt, void (*callback)(char *, int)) {
	size_t bufsize = 128;
	char *buf = malloc(bufsize);

	size_t buflen = 0;
	buf[0] = '\0';

	while (1) {
		editorSetStatusMessage(prompt, buf);
		editorRefreshScreen();

		int c = editorReadKey();
		if (c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE) {
			if (buflen != 0)
				buf[--buflen] = '\0';
		} else if (c == '\x1b') {
			editorSetStatusMessage("");
			if (callback)
				callback(buf, c);
			free(buf);
			return NULL;
		} else if (c == '\r') {
			if (buflen != 0) {
				editorSetStatusMessage("");
				if (callback)
					callback(buf, c);
				return buf;
			}
		} else if (!iscntrl(c) && c < 128) {
			if (buflen == bufsize - 1) {
				bufsize *= 2;
				buf = realloc(buf, bufsize);
			}
			buf[buflen++] = c;
			buf[buflen] = '\0';
		}

		if (callback)
			callback(buf, c);
	}
}

void editorMoveCursor(int key) {
	erow *row = (E.cursory >= E.numrows) ? NULL : &E.row[E.cursory];

	switch (key) {
	case ARROW_LEFT:
		if (E.cursorx != 0)
			E.cursorx--;
		break;
	case ARROW_RIGHT:
		if (row && E.cursorx < row->size)
			E.cursorx++;
		break;
	case ARROW_UP:
		if (E.cursory != 0)
			E.cursory--;
		break;
	case ARROW_DOWN:
		if (E.cursory < E.numrows)
			E.cursory++;
		break;
	}

	row = (E.cursory >= E.numrows) ? NULL : &E.row[E.cursory];
	int rowlen = row ? row->size : 0;
	if (E.cursorx > rowlen) {
		E.cursorx = rowlen;
	}
}

void editorProcessKeyPress(void) {
	static int quit_times = GRAM_QUIT_TIMES;

	int c = editorReadKey();

	if (E.vimmode == INSERT) {
		switch (c) {
		case '\r':
			editorInsertNewline();
			break;

		case HOME_KEY:
			E.cursorx = 0;
			break;
		case END_KEY:
			if (E.cursory < E.numrows)
				E.cursorx = E.row[E.cursory].size;
			break;

		case BACKSPACE:
		case CTRL_KEY('h'):
		case DEL_KEY:
			if (c == DEL_KEY)
				editorMoveCursor(ARROW_RIGHT);
			editorDelChar();
			break;

		case PAGE_UP: {
			E.cursory = E.rowoff;
			int times = E.screenrows;
			while (times--)
				editorMoveCursor(ARROW_UP);
		} break;
		case PAGE_DOWN: {
			E.cursory = E.rowoff + E.screenrows - 1;
			if (E.cursory > E.numrows)
				E.cursory = E.numrows;
			int times = E.screenrows;
			while (times--)
				editorMoveCursor(ARROW_DOWN);
		} break;

		case ARROW_UP:
		case ARROW_DOWN:
		case ARROW_RIGHT:
		case ARROW_LEFT:
			editorMoveCursor(c);
			break;

		case CTRL_KEY('l'):
		case '\x1b':
			E.vimmode = NORMAL;
			break;

		default:
			editorInsertChar(c);
			break;
		}
	} else if (E.vimmode == NORMAL) {
		switch (c) {
		case '\r':
			editorInsertNewline();
			break;

		case CTRL_KEY('q'):
			if (E.dirty && quit_times > 0) {
				editorSetStatusMessage("WARNING!!! File has unsaved changes. "
									   "Press Ctrl-Q %d more times to quit.",
									   quit_times);
				quit_times--;
				return;
			}
			write(STDOUT_FILENO, "\x1b[2J", 4);
			write(STDOUT_FILENO, "\x1b[H", 3);
			exit(0);
			break;

		case CTRL_KEY('s'):
			editorSave();
			break;

		case HOME_KEY:
			E.cursorx = 0;
			break;
		case END_KEY:
			if (E.cursory < E.numrows)
				E.cursorx = E.row[E.cursory].size;
			break;

		case CTRL_KEY('f'):
			editorFind();
			break;

		case PAGE_UP: {
			E.cursory = E.rowoff;
			int times = E.screenrows;
			while (times--)
				editorMoveCursor(ARROW_UP);
		} break;
		case PAGE_DOWN: {
			E.cursory = E.rowoff + E.screenrows - 1;
			if (E.cursory > E.numrows)
				E.cursory = E.numrows;
			int times = E.screenrows;
			while (times--)
				editorMoveCursor(ARROW_DOWN);
		} break;

		case ARROW_UP:
		case ARROW_DOWN:
		case ARROW_RIGHT:
		case ARROW_LEFT:
			editorMoveCursor(c);
			break;
		case 'k':
			editorMoveCursor(ARROW_UP);
			break;
		case 'j':
			editorMoveCursor(ARROW_DOWN);
			break;
		case 'l':
			editorMoveCursor(ARROW_RIGHT);
			break;
		case 'h':
			editorMoveCursor(ARROW_LEFT);
			break;

		case 'i':
			E.vimmode = INSERT;
			break;

		default:
			break;
		}
	}

	quit_times = GRAM_QUIT_TIMES;
}
