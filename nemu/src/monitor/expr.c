#include "nemu.h"
#include "cpu/reg.h"
#include "cpu/cpu.h"
#include "memory/memory.h"

#include <stdlib.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
uint32_t eval(int s, int e, bool *success);
static bool check_parentheses(int s, int e);
enum
{
	NOTYPE = 256,
	EQ,
	NUM,
	REG,
	SYMB,
	HEX,
	NEG,
	DEREF,
	LS,
	RS,
	LE,
	GE,
	NE

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
	{"0[xX][0-9a-fA-F]+", HEX},
	{"[0-9]{1,10}", NUM},     
	{"\\$e[a-d]x", REG},
	{"\\$e[sb]p", REG},
	{"\\$e[ds]i", REG},
	{"!=", NE},
	{"==", EQ},
	{"\\*",'*'},
	{"/",'/'},
	{"\\(",'('},
	{"\\)",')'},
	{"<<",LS},
	{">>",RS},
	{">=",GE},
	{"<=",LE},
	{"<",'<'},
	{">",'>'},
	{"\\+", '+'},
	{"\\-", '-'},
	{"%",'%'},
};

static struct Priority{
    int operand;
    int num;
} value_pri[] ={
  {DEREF, 11},
  {'~', 11},
  {'-', 11},
  {'*', 10},
  {'/', 10},
  {'%', 10},
  {'+', 9},
  {'-', 9},
  {LS, 8},
  {RS, 8},
  {GE, 7},
  {LE, 7},
  {'>', 7},
  {'<', 7},
  {EQ, 6},
  {NE, 6},
  
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]))
#define NR_VALUE (sizeof(value_pri) / sizeof(value_pri[0]))

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

				int k;
				switch (rules[i].token_type)
				{
				case NOTYPE:break;
				case REG:
				case NUM:
				case HEX:
				case SYMB:
				    for (k = 0; k < substr_len; k++) {
				        tokens[nr_token].str[k] = substr_start[k];
				    }
				    tokens[nr_token].str[k] = '\0';
				default:
					tokens[nr_token].type = rules[i].token_type;
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

static bool IsCertainType(int get_int) 
{
    return (get_int != NOTYPE && get_int != NUM && get_int != REG && get_int !=SYMB && get_int != '(' && get_int != ')' && get_int != HEX);
}

static bool check_parentheses(int s, int e){
    int num = 0;
    for (int i = s; i <= e; i++) {
        if (tokens[i].type == '(') num++;
        else if (tokens[i].type == ')') num--;
        if (num == 0 && i != e) return 0;
    }
    if (num == 0 && tokens[s].type == '(' && tokens[e].type == ')')
        return true;
    else
        return false;
}

uint32_t eval(int s, int e, bool *success)
{
    printf("%d !!!%d %d\n",check_parentheses(s,e),s,e);
    if (s > e){
        *success = false;
        return 0;
    }
    else if (s == e){
        if (tokens[s].type == NUM){
            uint32_t ans = 0;
            sscanf(tokens[s].str, "%d", &ans);
            return ans;
        }
        else if (tokens[s].type == REG){
            if (!strcmp(tokens[s].str, "$eax")) return cpu.eax;
            else if (!strcmp(tokens[s].str, "$ecx")) return cpu.ecx;
            else if (!strcmp(tokens[s].str, "$edx")) return cpu.edx;
            else if (!strcmp(tokens[s].str, "$ebx")) return cpu.ebx;
            else if (!strcmp(tokens[s].str, "$esp")) return cpu.esp;
            else if (!strcmp(tokens[s].str, "$ebp")) return cpu.ebp;
            else if (!strcmp(tokens[s].str, "$esi")) return cpu.esi;
            else if (!strcmp(tokens[s].str, "$edi")) return cpu.edi;
        }
        else if (tokens[s].type == HEX){
            uint32_t ans = 0;
            sscanf(tokens[s].str, "%x", &ans);
            return ans;
        }
    }
    else if (check_parentheses(s, e)){
        return eval(s + 1, e - 1, success);
    }
    else {
        int op = 0;
        int min_of_pri = 100;
        for (int i = s; i <= e; i++){
            if (tokens[i].type == '('){
                i++;
                int num = 1, j = i + 1;
                while (num && j < e) {
                    if (tokens[j].type == '(')
                        num++;
                    else if (tokens[j].type == ')')
                        num--;
                    j = j + 1;
                }
            }
            if (IsCertainType(i)){
                int j = 0;
                int flag = 0;
                for (j = 0; j < NR_VALUE; j++){
                    if (value_pri[j].operand == tokens[i].type) {
                        flag = 1; break;
                    }
                }
                if (flag && value_pri[j].num <= min_of_pri){
                    min_of_pri = value_pri[j].num;
                    op = i;
                }
            }
        }
        if (op == 0) return 0;
        printf("%d, %d, %d\n", s, op, e);
        uint32_t val1, val2;
        val1 = eval(s, op - 1, success);
        val2 = eval(op + 1, e, success);
        int op_type = tokens[op].type;
        switch(op_type) {
            case '+': return val1 + val2; break;
            case '-': return val1 - val2; break;
            case '*': return val1 * val2; break;
            case '/': return val1 / val2; break;
            case LS: return val1 << val2; break;
            case RS: return val1 >> val2; break;
            case NE: return val1 != val2; break;
            case EQ: return val1 == val2; break;
            case '<': return val1 < val2; break;
            case '>': return val1 > val2; break;
            case LE: return val1 <= val2; break;
            case GE: return val1 >= val2; break;
            case DEREF: return *(hw_mem+val2);break;
            default: assert(0);
        }
    }
    return 0;
}

uint32_t expr(char *e, bool *success)
{
	if (!make_token(e))
	{
		*success = false;
		return 0;
	}

	/*printf("\nPlease implement expr at expr.c\n");
	fflush(stdout);
	assert(0);
	return 0;*/
    for(int i = 0; i < nr_token; i ++) {
        if(tokens[i].type == '*' && (i == 0 || IsCertainType(tokens[i - 1].type))) {
            tokens[i].type = DEREF;
        }
        else if (tokens[i].type == '-' && (i == 0 || IsCertainType(tokens[i - 1].type))) {
            tokens[i].type = NEG;
        }
    }
    for (int i=0;i<nr_token;i++){
        printf("%d,%c,%d, %s\n",i,tokens[i].type, tokens[i].type,tokens[i].str);
    }
    return eval(0, nr_token-1, success);
}
