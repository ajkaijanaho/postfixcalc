/*
  Copyright © 2016 Antti-Juhani Kaijanaho

  Licensed under the Apache License, Version 2.0 (the "License"); you
  may not use this file except in compliance with the License.  You
  may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
  implied.  See the License for the specific language governing
  permissions and limitations under the License.
*/

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/*************************************************************************
READING INPUT

The interface is defined both with and without readline compiled
in, with as similar as possible behavior (modulo line editing, of
course).  The interface comprises of end_of_input and get_char.
**************************************************************************/

static bool end_of_input = false;

#ifdef WITH_READLINE
#  include <readline/readline.h>
#  include <readline/history.h>

static char *cur_line = NULL;
static size_t linep = 0;

static int get_char(void)
{
        if (cur_line == NULL) {
                cur_line = readline("> ");
                if (cur_line == NULL) {
                        end_of_input = true;
                        return EOF;
                }
                if (cur_line[0] != '\0') add_history(cur_line);
                linep = 0;
        }
        char c = cur_line[linep++];
        if (c == '\0') {
                free(cur_line);
                cur_line = NULL;
                return '\n';
        }
        return (unsigned char)c;
}

#else /* WITH_READLINE */

static bool beginning_of_line = true;

static int get_char(void)
{
        if (beginning_of_line) {
                fputs("> ", stdout);
                fflush(stdout);
                beginning_of_line = false;
        }
        int c = getc(stdin);
        switch (c) {
        case EOF:
                end_of_input = true;
                break;
        case '\n': case '\r':
                beginning_of_line = true;
                break;
        }
        return c;
}

#endif /* WITH_READLINE */

/*************************************************************************
STACK HANDLING
**************************************************************************/

static double *stack = NULL;
static size_t sp = 0;
static size_t spmax = 0;

static void push(double val)
{
        if (sp >= spmax) {
                size_t nspmax = spmax*2;
                if (nspmax == 0) nspmax = 4;
                double *nstack = realloc(stack, nspmax * sizeof *nstack);
                if (nstack == NULL) {
                        fputs("Out of memory.\n", stderr);
                        exit(EXIT_FAILURE);
                }
                stack = nstack;
                spmax = nspmax;
        }
        assert(stack != NULL);
        assert(sp < spmax);
        stack[sp++] = val;
}

static double pop(void)
{
        if (sp == 0) {
                fputs("Stack underflow.\n", stderr);
                return NAN;
        }
        assert(stack != NULL);
        assert(sp > 0);
        return stack[--sp];
}

/* The following allows building constants a character a time.  The
   idea is, cur_const is the current constant, and new digits are
   added to it as they are encountered.  When a non-digit is
   encountered, push_const is called to push the built constant to the
   stack; in_const indicates whether there is a (not yet pushed)
   constant being built.
*/

static long cur_const = 0;
static bool in_const = false;

static void push_const(void)
{
        if (in_const) {
                push(cur_const);
                cur_const = 0;
                in_const = false;
        }
}

/*************************************************************************
INTERPRETER AND MAIN PROGRAM
**************************************************************************/

static void interp(void)
{
        while (true) {
                int c = get_char();
                switch (c) {
                case EOF: case '\r': case '\n':
                        return;
                case '0': case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                        in_const = true;
                        cur_const = 10*cur_const + (c - '0');
                        break;
                case ' ': case '\t':
                        push_const();
                        break;
                case '+':
                        push_const();
                        push(pop() + pop());
                        break;
                case '-':
                        push_const();
                        push(pop() - pop());
                        break;
                case '*':
                        push_const();
                        push(pop() * pop());
                        break;
                case '/':
                        push_const();
                        push(pop() / pop());
                        break;
                }
        }
}

int main(void)
{
        while (!end_of_input) {
                sp = 0;
                interp();
                fputc('=', stdout);
                while (sp-- > 0) {
                        printf(" %f", stack[sp]);
                }
                fputc('\n', stdout);
        }
}
