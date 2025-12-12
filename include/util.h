#ifndef UTIL_H
#define UTIL_H

#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

// includes

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

// defines

#define GRAM_VERSION "0.0.1"
#define GRAM_TAB_STOP 8
#define GRAM_QUIT_TIMES 3

#define CTRL_KEY(k) ((k) & 0x1f)

#define ABUF_INIT                                                                                                      \
	{ NULL, 0 }

#define HL_HIGHLIGHT_NUMBERS (1 << 0)
#define HL_HIGHLIGHT_STRINGS (1 << 1)
#define HLDB_ENTRIES (sizeof(HLDB) / sizeof(HLDB[0]))

enum editorKey {
	BACKSPACE = 127,
	ARROW_LEFT = 1000,
	ARROW_RIGHT,
	ARROW_UP,
	ARROW_DOWN,
	DEL_KEY,
	HOME_KEY,
	END_KEY,
	PAGE_UP,
	PAGE_DOWN
};

enum editorHighlight {
	HL_NORMAL = 0,
	HL_COMMENT,
	HL_MLCOMMENT,
	HL_KEYWORD1,
	HL_KEYWORD2,
	HL_STRING,
	HL_NUMBER,
	HL_MATCH
};

// data

struct editorSyntax {
	char *filetype;
	char **filematch;
	char **keywords;
	char *singleline_comment_start;
	char *multiline_comment_start;
	char *multiline_comment_end;
	int flags;
};

typedef struct erow {
	int idx;
	int size;
	int rsize;
	char *chars;
	char *render;
	unsigned char *hl;
	int hl_open_comment;
} erow;

struct editorConfig {
	int cursorx, cursory;
	int renderx;
	int rowoff;
	int coloff;
	int screenrows;
	int screencols;
	int numrows;
	erow *row;
	int dirty;
	char *filename;
	char statusmsg[80];
	time_t statusmsg_time;
	struct editorSyntax *syntax;
	struct termios orig_termios;
};

extern struct editorConfig E;

struct abuf {
	char *b;
	int len;
};

// filetypes

extern char *C_HL_extensions[];
extern char *C_HL_keywords[];
extern struct editorSyntax HLDB[];

// prototypes

void editorUpdateSyntax(erow *);
int editorSyntaxToColor(int);
int is_seperator(int);
void editorSelectSyntaxHighlight(void);

void editorFindCallBack(char *, int);
void editorFind(void);

char *editorPrompt(char *, void (*)(char *, int));
void editorMoveCursor(int);
void editorProcessKeyPress(void);

void abAppend(struct abuf *, const char *, int);
void abFree(struct abuf *);
void editorScroll(void);
void editorDrawRows(struct abuf *);
void editorDrawStatusBar(struct abuf *);
void editorDrawMessageBar(struct abuf *);
void editorRefreshScreen(void);
void editorSetStatusMessage(const char *, ...);

int editorRowCursorxToRenderx(erow *, int);
int editorRowRenderxToCursorx(erow *, int);
void editorUpdateRow(erow *);
void editorInsertRow(int, char *, size_t);
void editorFreeRow(erow *);
void editorDelRow(int);
void editorRowInsertChar(erow *, int, int);
void editorRowAppendString(erow *, char *, size_t);
void editorRowDelChar(erow *, int);

char *editorRowsToString(int *);
void editorOpen(char *);
void editorSave(void);

void die(const char *);
void disableRawMode(void);
void enableRawMode(void);
int editorReadKey(void);
int getCursorPosition(int *, int *);
int getWindowSize(int *, int *);

void editorInsertChar(int);
void editorInsertNewline(void);
void editorDelChar(void);

#endif // !UTIL_H
