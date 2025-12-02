#include "../include/util.h"

char *C_HL_extensions[] = {".c", ".h", ".cpp", NULL};
char *C_HL_keywords[] = {"switch", "if",	  "while",	 "for",	   "break",		"continue", "return", "else",
						 "struct", "union",	  "typedef", "static", "enum",		"class",	"case",	  "int|",
						 "long|",  "double|", "float|",	 "char|",  "unsigned|", "signed|",	"void|",  NULL};

struct editorSyntax HLDB[] = {
	{"c", C_HL_extensions, C_HL_keywords, "//", HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS},
};

void editorUpdateSyntax(erow *row) {
	row->hl = realloc(row->hl, row->rsize);
	memset(row->hl, HL_NORMAL, row->rsize);

	if (E.syntax == NULL)
		return;

	char **keywords = E.syntax->keywords;

	char *scs = E.syntax->singleline_comment_start;
	int scs_len = scs ? strlen(scs) : 0;

	int prev_sep = 1;
	int in_string = 0;

	int i = 0;
	while (i < row->rsize) {
		char c = row->render[i];
		unsigned char prev_hl = (i > 0) ? row->hl[i - 1] : HL_NORMAL;

		// higlight comments
		if (scs_len && !in_string) {
			if (!strncmp(&row->render[i], scs, scs_len)) {
				memset(&row->hl[i], HL_COMMENT, row->rsize - i);
				break;
			}
		}

		// highlight strings
		if (E.syntax->flags & HL_HIGHLIGHT_STRINGS) {
			if (in_string) {
				row->hl[i] = HL_STRING;
				if (c == '\\' && i + 1 < row->rsize) {
					row->hl[i + 1] = HL_STRING;
					i += 2;
					continue;
				}
				if (c == in_string)
					in_string = 0;
				i++;
				prev_sep = 1;
				continue;
			} else {
				if (c == '"' || c == '\'') {
					in_string = c;
					row->hl[i] = HL_STRING;
					i++;
					continue;
				}
			}
		}

		// higlight numbers
		if (E.syntax->flags & HL_HIGHLIGHT_NUMBERS) {
			if ((isdigit(c) && (prev_sep || prev_hl == HL_NUMBER)) || (c == '.' && prev_hl == HL_NUMBER)) {
				row->hl[i] = HL_NUMBER;
				i++;
				prev_sep = 0;
				continue;
			}
		}

		// higlight keywords
		if (prev_sep) {
			int j;
			for (j = 0; keywords[j]; j++) {
				int klen = strlen(keywords[j]);
				int kw2 = keywords[j][klen - 1] == '|';
				if (kw2)
					klen--;

				if (!strncmp(&row->render[i], keywords[j], klen) && is_seperator(row->render[i + klen])) {
					memset(&row->hl[i], kw2 ? HL_KEYWORD2 : HL_KEYWORD1, klen);
					i += klen;
					break;
				}
			}
			if (keywords[j] != NULL) {
				prev_sep = 0;
				continue;
			}
		}

		prev_sep = is_seperator(c);
		i++;
	}
}

int editorSyntaxToColor(int hl) {
	switch (hl) {
	case HL_NUMBER:
		return 31;
	case HL_KEYWORD1:
		return 33;
	case HL_KEYWORD2:
		return 32;
	case HL_STRING:
		return 35;
	case HL_MATCH:
		return 34;
	case HL_COMMENT:
		return 36;
	default:
		return 37;
	}
}

int is_seperator(int c) { return isspace(c) || c == '\0' || strchr(",.()+-/*=~%<>[];", c) != NULL; }

void editorSelectSyntaxHighlight(void) {
	E.syntax = NULL;
	if (E.filename == NULL)
		return;

	char *ext = strrchr(E.filename, '.');

	for (unsigned int i = 0; i < HLDB_ENTRIES; i++) {
		struct editorSyntax *s = &HLDB[i];
		unsigned int j = 0;

		while (s->filematch[j]) {
			int is_ext = (s->filematch[j][0] == '.');
			if ((is_ext && ext && !strcmp(ext, s->filematch[j])) || (!is_ext && strstr(E.filename, s->filematch[j]))) {
				E.syntax = s;

				for (int filerow = 0; filerow < E.numrows; filerow++) {
					editorUpdateSyntax(&E.row[filerow]);
				}

				return;
			}
			j++;
		}
	}
}
