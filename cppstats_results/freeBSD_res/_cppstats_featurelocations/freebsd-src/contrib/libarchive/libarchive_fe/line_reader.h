


























#if !defined(LAFE_LINE_READER_H)
#define LAFE_LINE_READER_H

struct lafe_line_reader;

struct lafe_line_reader *lafe_line_reader(const char *, int nullSeparator);
const char *lafe_line_reader_next(struct lafe_line_reader *);
void lafe_line_reader_free(struct lafe_line_reader *);

#endif
