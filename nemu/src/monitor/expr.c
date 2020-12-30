#include "nemu.h"
#include "cpu/reg.h"
#include "memory/memory.h"

#include <stdlib.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum
{
	NOTYPE = 256,
	EQ,
	NEQ,
	AND,
	OR,
	NOT,
	HEX,
	NUM,
	REG,
	DEREF,
	SYMB

	/* TODO: Add more token types */

};

static struct rule
{
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +", NOTYPE}, // white space
	{"\\+", '+'},
	{"\\-", '-'},
	{"\\*", '*'},
	{"/", '/'},
	{"\\(", '('},
	{"\\)", ')'},
	{"0x[0-9a-fA-F]+", HEX},
	{"[0-9]+", NUM},
	{"\\$[a-zA-Z0-9]+", REG},
	{"==", EQ},
	{"\\!=", NEQ},
	{"&&", AND},
	{"\\|\\|", OR},
	{"!", NOT},
	{"[a-zA-Z0-9\\_]+", SYMB}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]))

static regex_t re[NR_REGEX];

/* Rules are used for more times.
 * Therefore we compile them only once before any usage.
 */
void init_regex()
{
	int i;
	char error_msg[128];
	int ret;

	for (i = 0; i < NR_REGEX; i++)
	{
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if (ret != 0)
		{
			regerror(ret, &re[i], error_msg, 128);
			assert(ret != 0);
		}
	}
}

typedef struct token
{
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e)
{
	int position = 0;
	int i;
	regmatch_t pmatch;

	nr_token = 0;

	while (e[position] != '\0')
	{
		/* Try all rules one by one. */
		for (i = 0; i < NR_REGEX; i++)
		{
			if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
			{
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				printf("match regex[%d] at position %d with len %d: %.*s\n", i, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. 
				 * Add codes to perform some actions with this token.
				 */
                
				switch (rules[i].token_type)
				{
				case NOTYPE: break;
				default:
					tokens[nr_token].type = rules[i].token_type;
                    sprintf(tokens[nr_token].str, "%.*s", substr_len, substr_start);
					nr_token++;
				}

				break;
			}
		}

		if (i == NR_REGEX)
		{
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true;
}

bool is_operator(int type) {
    return type != NUM && type != HEX && type != REG && type != '(' && type != ')';
}

int get_priority(int type) {
    switch (type) {
        case '+': return 4;
        case '-': return 4;
        case '*': return 5;
        case '/': return 5;
        case EQ: return 3;
	    case NEQ: return 3;
	    case AND: return 2;
	    case OR: return 1;
	    case NOT: return 6;
	    case DEREF: return 7;
	    default: return 256;
    }
}

bool check_parentheses(int p, int q) {
    int top = 0;
    if (tokens[p].type != '(' || tokens[q].type != ')') return 0;
    for (int i = p; i <= q; ++i) {
        if (tokens[i].type == '(') top++;
        else if (tokens[i].type == ')') top--;
        if (top < 0) return 0;
    }
    return !top;
}

extern uint32_t look_up_symtab(char *sym, bool *success);

uint32_t eval(int p, int q, bool *success) {
    if (p > q) {
        *success = false;
        return 0;
    } else if (p == q) {
        uint32_t x = 0;
        switch (tokens[p].type) {
            case NUM: {
                sscanf(tokens[p].str, "%u", &x);
                break;
            }
            case HEX: {
                sscanf(tokens[p].str, "%x", &x);
                break;
            }
            case REG: {
                x = get_reg_val(tokens[p].str+1, success);
                break;
            }
            case SYMB: {
                x = look_up_symtab(tokens[p].str, success);
                break;
            }
            default: *success = false;
        }
        return x;
    } else if (check_parentheses(p, q)) {
        return eval(p+1, q-1, success);
    } else {
        int dominant = NOTYPE, k = 0, level = 0;
        for (int i = p; i <= q; ++i) {
            if (tokens[i].type == '(') level++;
            else if (tokens[i].type == ')') level--;
            else if (!level && is_operator(tokens[i].type)) {
                if (get_priority(tokens[i].type) <= get_priority(dominant)) {
                    dominant = tokens[i].type;
                    k = i;
                }
            }
        }
        if (dominant == NOTYPE) {
            *success = false;
            return 0;
        }
        uint32_t y = eval(k+1, q, success);
        if (k == p) {
            if (!(*success)) return 0;
            if (dominant == '+') return y;
            if (dominant == '-') return -y;
            if (dominant == NOT) return !y;
            if (dominant == DEREF) return vaddr_read(y, SREG_CS, 4);
        }
        uint32_t x = eval(p, k-1, success);
        if (!(*success)) return 0;
        switch (dominant) {
	        case '+': return x+y;
            case '-': return x-y;
            case '*': return x*y;
            case '/': return x/y;
            case EQ: return x == y;
    	    case NEQ: return x != y;
    	    case AND: return x && y;
    	    case OR: return x || y;
    	    default: return *success = false;
        }
    }
}

uint32_t expr(char *e, bool *success)
{
	if (!make_token(e))
	{
		*success = false;
		return 0;
	}

    /*
	printf("\nPlease implement expr at expr.c\n");
	fflush(stdout);
	assert(0);
	*/
	for(int i = 0; i < nr_token; i ++) {
        if(tokens[i].type == '*' && (i == 0 || is_operator(tokens[i-1].type)) ) {
            tokens[i].type = DEREF;
        }
    }
	return eval(0, nr_token-1, success);
}
