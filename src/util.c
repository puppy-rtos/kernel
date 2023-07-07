/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <puppy.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

static int _p_vsnprintf(char *buf, size_t size, const char *fmt, va_list args);
static char _printk_buf[P_PRINTK_BUF_SIZE];

p_weak int p_hw_cons_getc(void)
{
    return -1;
}

p_weak int p_hw_cons_output(const char *str, int len)
{
    return 0;
}

int printk(const char *fmt, ...)
{
    va_list args;
    int n;

    va_start(args, fmt);
    n = _p_vsnprintf(_printk_buf, P_PRINTK_BUF_SIZE, fmt, args);
    va_end(args);

    p_hw_cons_output(_printk_buf, n);

    return 0;
}

#define ZEROPAD     (1 << 0)    /* pad with zero */
#define SIGN        (1 << 1)    /* unsigned/signed long */
#define PLUS        (1 << 2)    /* show plus */
#define SPACE       (1 << 3)    /* space if plus */
#define LEFT        (1 << 4)    /* left justified */
#define SPECIAL     (1 << 5)    /* 0x */
#define LARGE       (1 << 6)    /* use 'ABCDEF' instead of 'abcdef' */

#define _ISDIGIT(c)  ((unsigned)((c) - '0') < 10)

static inline int divide(long *n, int base)
{
    int res;

    res = (int)(((unsigned long)*n) % base);
    *n = (long)(((unsigned long)*n) / base);

    return res;
}

static inline int skip_atoi(const char **s)
{
    int i = 0;
    while (_ISDIGIT(**s))
        i = i * 10 + *((*s)++) - '0';

    return i;
}
static char *print_number(char *buf,
                          char *end,
                          long  num,
                          int   base,
                          int   s,
                          int   precision,
                          int   type)
{
    char c, sign;
    char tmp[32];
    int precision_bak = precision;
    const char *digits;
    static const char small_digits[] = "0123456789abcdef";
    static const char large_digits[] = "0123456789ABCDEF";
    int i, size;

    size = s;

    digits = (type & LARGE) ? large_digits : small_digits;
    if (type & LEFT)
        type &= ~ZEROPAD;

    c = (type & ZEROPAD) ? '0' : ' ';

    /* get sign */
    sign = 0;
    if (type & SIGN)
    {
        if (num < 0)
        {
            sign = '-';
            num = -num;
        }
        else if (type & PLUS)
            sign = '+';
        else if (type & SPACE)
            sign = ' ';
    }

    i = 0;
    if (num == 0)
        tmp[i++] = '0';
    else
    {
        while (num != 0)
            tmp[i++] = digits[divide(&num, base)];
    }

    if (i > precision)
        precision = i;
    size -= precision;

    if (!(type & (ZEROPAD | LEFT)))
    {
        if ((sign) && (size > 0))
            size--;

        while (size-- > 0)
        {
            if (buf < end)
                *buf = ' ';
            ++ buf;
        }
    }

    if (sign)
    {
        if (buf < end)
        {
            *buf = sign;
        }
        -- size;
        ++ buf;
    }

    /* no align to the left */
    if (!(type & LEFT))
    {
        while (size-- > 0)
        {
            if (buf < end)
                *buf = c;
            ++ buf;
        }
    }

    while (i < precision--)
    {
        if (buf < end)
            *buf = '0';
        ++ buf;
    }

    /* put number in the temporary buffer */
    while (i-- > 0 && (precision_bak != 0))
    {
        if (buf < end)
            *buf = tmp[i];
        ++ buf;
    }

    while (size-- > 0)
    {
        if (buf < end)
            *buf = ' ';
        ++ buf;
    }

    return buf;
}

static int _p_vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
    uint32_t num;
    int i, len;
    char *str, *end, c;
    const char *s;

    uint8_t base;            /* the base of number */
    uint8_t flags;           /* flags to print number */
    uint8_t qualifier;       /* 'h', 'l', or 'L' for integer fields */
    int32_t field_width;     /* width of output field */
    int precision;      /* min. # of digits for integers and max for a string */

    str = buf;
    end = buf + size;

    /* Make sure end is always >= buf */
    if (end < buf)
    {
        end  = ((char *) - 1);
        size = end - buf;
    }

    for (; *fmt ; ++fmt)
    {
        if (*fmt != '%')
        {
            if (str < end)
                *str = *fmt;
            ++ str;
            continue;
        }

        /* process flags */
        flags = 0;

        while (1)
        {
            /* skips the first '%' also */
            ++ fmt;
            if (*fmt == '-') flags |= LEFT;
            else if (*fmt == '+') flags |= PLUS;
            else if (*fmt == ' ') flags |= SPACE;
            else if (*fmt == '#') flags |= SPECIAL;
            else if (*fmt == '0') flags |= ZEROPAD;
            else break;
        }

        /* get field width */
        field_width = -1;
        if (_ISDIGIT(*fmt)) field_width = skip_atoi(&fmt);
        else if (*fmt == '*')
        {
            ++ fmt;
            /* it's the next argument */
            field_width = va_arg(args, int);
            if (field_width < 0)
            {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        /* get the precision */
        precision = -1;
        if (*fmt == '.')
        {
            ++ fmt;
            if (_ISDIGIT(*fmt)) precision = skip_atoi(&fmt);
            else if (*fmt == '*')
            {
                ++ fmt;
                /* it's the next argument */
                precision = va_arg(args, int);
            }
            if (precision < 0) precision = 0;
        }
        /* get the conversion qualifier */
        qualifier = 0;
        if (*fmt == 'h' || *fmt == 'l')
        {
            qualifier = *fmt;
            ++ fmt;
        }

        /* the default base */
        base = 10;

        switch (*fmt)
        {
        case 'c':
            if (!(flags & LEFT))
            {
                while (--field_width > 0)
                {
                    if (str < end) *str = ' ';
                    ++ str;
                }
            }

            /* get character */
            c = (uint8_t)va_arg(args, int);
            if (str < end) *str = c;
            ++ str;

            /* put width */
            while (--field_width > 0)
            {
                if (str < end) *str = ' ';
                ++ str;
            }
            continue;

        case 's':
            s = va_arg(args, char *);
            if (!s) s = "(NULL)";

            for (len = 0; (len != field_width) && (s[len] != '\0'); len++);
            if (precision > 0 && len > precision) len = precision;

            if (!(flags & LEFT))
            {
                while (len < field_width--)
                {
                    if (str < end) *str = ' ';
                    ++ str;
                }
            }

            for (i = 0; i < len; ++i)
            {
                if (str < end) *str = *s;
                ++ str;
                ++ s;
            }

            while (len < field_width--)
            {
                if (str < end) *str = ' ';
                ++ str;
            }
            continue;

        case 'p':
            if (field_width == -1)
            {
                field_width = sizeof(void *) << 1;
                flags |= ZEROPAD;
            }
            str = print_number(str, end,
                               (long)va_arg(args, void *),
                               16, field_width, precision, flags);
            continue;

        case '%':
            if (str < end) *str = '%';
            ++ str;
            continue;

        /* integer number formats - set up the flags and "break" */
        case 'b':
            base = 2;
            break;
        case 'o':
            base = 8;
            break;

        case 'X':
            flags |= LARGE;
        case 'x':
            base = 16;
            break;

        case 'd':
        case 'i':
            flags |= SIGN;
        case 'u':
            break;

        default:
            if (str < end) *str = '%';
            ++ str;

            if (*fmt)
            {
                if (str < end) *str = *fmt;
                ++ str;
            }
            else
            {
                -- fmt;
            }
            continue;
        }

        if (qualifier == 'l')
        {
            num = va_arg(args, uint32_t);
            if (flags & SIGN) num = (int32_t)num;
        }
        else if (qualifier == 'h')
        {
            num = (uint16_t)va_arg(args, int);
            if (flags & SIGN) num = (int16_t)num;
        }
        else
        {
            num = va_arg(args, uint32_t);
            if (flags & SIGN) num = (int32_t)num;
        }
        str = print_number(str, end, num, base, field_width, precision, flags);
    }

    if (size > 0)
    {
        if (str < end) *str = '\0';
        else
        {
            end[-1] = '\0';
        }
    }

    /* the trailing null byte doesn't count towards the total
    * ++str;
    */
    return str - buf;
}

const P_SECTION_START_DEFINE(P_TC_SECTION, _tc_list_start);
const P_SECTION_END_DEFINE(P_TC_SECTION, _tc_list_end);
void tc_list(void)
{
    unsigned int *ptr_begin, *ptr_end;
    ptr_begin = (unsigned int *)P_SECTION_START_ADDR(_tc_list_start);
    ptr_end = (unsigned int *)P_SECTION_END_ADDR(_tc_list_end);
    for (unsigned int *ptr = ptr_begin; ptr < ptr_end;)
    {
        struct p_tc_fn *_obj = ptr;
        printk("Get a testcase: %s\n", _obj->name);
        ptr += (sizeof(struct p_tc_fn) / sizeof(unsigned int));
    }
}

void tc_runall(bool verbose)
{
    unsigned int *ptr_begin, *ptr_end;
    ptr_begin = (unsigned int *)P_SECTION_START_ADDR(_tc_list_start);
    ptr_end = (unsigned int *)P_SECTION_END_ADDR(_tc_list_end);
    for (unsigned int *ptr = ptr_begin; ptr < ptr_end;)
    {
        struct p_tc_fn *_obj = ptr;
        printk("Start test: %s\n", _obj->name);
        _obj->tc();
        printk("Test end\n");
        ptr += (sizeof(struct p_tc_fn) / sizeof(unsigned int));
    }
}
static void print_help(void)
{
    printk("useage: tc [options]\r\n");
    printk("options: \r\n");
    printk("\t -h \t: show help\r\n");
    printk("\t -v \t: verbose mode\r\n");
    printk("\t list \t: show all testcase\r\n");
    printk("\t runall \t: run all testcase\r\n");
}
void shell_tc_cmd(char argc, char *argv)
{
    unsigned int i = 0;
    if (argc > 1)
    {
        if (!strcmp("list", &argv[argv[1]]))
        {
            tc_list();
        }
        else if (!strcmp("runall", &argv[argv[1]]))
        {
            tc_runall(true);
        }
        else if (!strcmp("-v", &argv[argv[1]]))
        {
            printk("verbose mode not support\r\n");
        }
        else if (!strcmp("-h", &argv[argv[1]]))
        {
            print_help();
        }
    }
    else
    {
        print_help();
    }
}
#ifdef ENABLE_NR_SHELL
#include <nr_micro_shell.h>
NR_SHELL_CMD_EXPORT(tc, shell_tc_cmd);
#endif