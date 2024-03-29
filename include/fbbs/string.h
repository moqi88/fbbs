#ifndef FB_STRING_H
#define FB_STRING_H

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "fbbs/pool.h"
#include "fbbs/util.h"

#define streq(a, b)          (!strcmp(a, b))
#define strneq(a, b, n)      (!strncmp(a, b, n))
#define strcaseeq(a, b)      (!strcasecmp(a, b))
#define strncaseeq(a, b, n)  (!strncasecmp(a, b, n))

#define strneq2(a, s)        (!strncmp(a, s, sizeof(s) - 1))
#define strncaseeq2(a, s)    (!strncasecmp(a, s, sizeof(s) - 1))

enum {
	PSTRING_DEFAULT_LEN = 7,
};

extern char *strtolower(char *dst, const char *src);
extern char *strtoupper(char *dst, const char *src);
extern char *strcasestr_gbk(const char *haystack, const char *needle);
extern char *string_remove_ansi_control_code(char *dst, const char *src);
extern int ellipsis(char *str, int len);
extern char *rtrim(char *str);
extern char *trim(char *str);
extern size_t strlcpy(char *dst, const char *src, size_t siz);
extern void strtourl(char *url, const char *str);
extern void strappend(char **dst, size_t *size, const char *src);
extern void string_remove_non_printable_gbk(char *str);
extern void string_remove_non_printable(char *str);
extern size_t string_check_tail(char *begin, char *end);
extern size_t string_cp(char *dst, const char *src, size_t siz);
extern size_t string_copy_allow_null(char *dst, const char *src, size_t size);
extern int valid_gbk(unsigned char *str, int len, int replace);
extern const char *check_gbk(const char *title);

extern wchar_t next_wchar(const char **str, size_t *leftp);
extern int fb_wcwidth(wchar_t ch);
extern int string_validate_utf8(const char *str, size_t max_chinese_chars, bool allow_zero_width_or_esc);
extern const char *get_line_end(const char *begin, const char *end);

static inline bool isprint2(int ch)
{
	unsigned char c = ch;
	return (((c & 0x80) && c != 0xFF) || isprint(c));
}

typedef struct pstring_t pstring_t;

extern pstring_t *pstring_new(pool_t *p);
extern pstring_t *pstring_sized_new(pool_t *p, uint_t size);
extern pstring_t *pstring_append_c(pool_t *p, pstring_t *s, int c);
extern pstring_t *pstring_append_string(pool_t *p, pstring_t *s, const char *str);
extern pstring_t *pstring_append_printf(pool_t *p, pstring_t *s, const char *format, ...);
extern pstring_t *pstring_append_space(pool_t *p, pstring_t *s);
extern const char *pstring(const pstring_t *s);

#endif // FB_STRING_H
