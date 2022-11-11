
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define ASSERT(b)   assert(b)

#define PRIORITY_MAX    9999

#define STACK_DEFINE(name, type, cap)               \
typedef struct {                                    \
    type values[cap];                               \
    int top;                                        \
} name##Stack;                                      \
_Bool name##Empty(name##Stack *stack)               \
{                                                   \
    return stack->top < 0;                          \
}                                                   \
void name##Push(name##Stack *stack, type value)     \
{                                                   \
    ASSERT(stack->top < cap);                       \
    stack->top++;                                   \
    stack->values[stack->top] = value;              \
}                                                   \
type name##Pop(name##Stack *stack)                  \
{                                                   \
    ASSERT(!name##Empty(stack));                    \
    return stack->values[stack->top--];             \
}                                                   \
type name##Top(name##Stack *stack)                  \
{                                                   \
    ASSERT(!name##Empty(stack));                    \
    return stack->values[stack->top];               \
}


STACK_DEFINE(Number, double, 1000)

STACK_DEFINE(Operator, char, 1000)


typedef struct {
    char *value;
    int length;
    int index;
    char cursor;
} Expression;


int priorityOf(char c)
{
    switch (c) {
        case '+':
        case '-':
            return 2;
        case '*':
        case '/':
        case '%':
            return 4;
        case '^':
            return 8;
        case '(':
        case ')':
            return PRIORITY_MAX;
        default:
            return -1;
    }
}

_Bool isSign(char c)
{
    return c == '-' || c == '+';
}

_Bool isWhitespace(char c)
{
    return c == ' ' || c == '\t';
}

_Bool isDigit(char c)
{
    return '0' <= c && c <= '9';
}

char nextChar(Expression *expr)
{
    ASSERT(0 <= expr->index);
    if (expr->index >= expr->length) {
        expr->cursor = EOF;
    } else {
        expr->cursor = expr->value[expr->index++];
    }
    return expr->cursor;
}

void skipWhitespace(Expression *expr)
{
    while (isWhitespace(expr->cursor)) {
        nextChar(expr);
    }
}

short seekNext(Expression *expr)
{
    nextChar(expr);
    skipWhitespace(expr);
    return expr->cursor;
}

double parseNumber(Expression *expr)
{
    ASSERT(isDigit(expr->cursor));
    uint32_t integer = 0;
    while (isDigit(expr->cursor)) {
        integer = integer * 10 + (expr->cursor - '0');
        nextChar(expr);
    }
    double number = integer;
    if (expr->cursor == '.') {
        double b = 0.1;
        while (isDigit(nextChar(expr))) {
            number += (expr->cursor - '0') * b;
            b *= 0.1;
        }
    }
    skipWhitespace(expr);
    return number;
}

double doCompute(NumberStack *numbers, OperatorStack *operators, int minPriority, _Bool popParen)
{
    double right = NumberPop(numbers);
    while (!NumberEmpty(numbers) && !OperatorEmpty(operators)) {
        char op = OperatorTop(operators);
        if (op == '(') {
            if (popParen) {
                OperatorPop(operators);
            }
            break;
        }

        if (minPriority > priorityOf(op)) { break; }
        OperatorPop(operators);

        switch (op) {
            case '+':
                right = NumberPop(numbers) + right;
                break;
            case '-':
                right = NumberPop(numbers) - right;
                break;
            case '*':
                right = NumberPop(numbers) * right;
                break;
            case '/':
                right = NumberPop(numbers) / right;
                break;
            case '%':
                right = fmod(NumberPop(numbers), right);
                break;
            case '^':
                right = pow(NumberPop(numbers), right);
                break;
            default:
                break;
        }
    }
    return right;
}

double compute(Expression *expression, int *err)
{
    NumberStack numbers = {{0}, -1};
    OperatorStack operators = {{0}, -1};
    _Bool prevOperator = 1;
    seekNext(expression);
    for (; expression->cursor != EOF;) {
        if (expression->cursor == '(') {
            OperatorPush(&operators, expression->cursor);
            seekNext(expression);
            prevOperator = 1;
            continue;
        }
        if (expression->cursor == ')') {
            double value = doCompute(&numbers, &operators, 0, true);
            NumberPush(&numbers, value);
            seekNext(expression);
            prevOperator = 0;
            continue;
        }

        if (prevOperator) {
            if (isDigit(expression->cursor)) {
                NumberPush(&numbers, parseNumber(expression));
            } else if (isSign(expression->cursor)) {
                uint8_t sign = expression->cursor;
                seekNext(expression);
                double number = parseNumber(expression);
                if (sign == '-') { number = -number; }
                NumberPush(&numbers, number);
            } else {
                *err = -2;
                return 0;
            }
            prevOperator = 0;
            continue;
        }
        int priority = priorityOf(expression->cursor);
        if (priority > -1) {
            if (OperatorEmpty(&operators)) {
                OperatorPush(&operators, expression->cursor);
            } else {
                char preOp = OperatorTop(&operators);
                if (preOp != '(' && priorityOf(preOp) >= priority) {
                    double value = doCompute(&numbers, &operators, priority, false);
                    NumberPush(&numbers, value);
                }
                OperatorPush(&operators, expression->cursor);
            }
            seekNext(expression);
            prevOperator = 1;
            continue;
        }

        *err = -1;
        return 0;
    }
    return doCompute(&numbers, &operators, 0, false);
}

int calculate(char *s)
{
    int len = strlen(s);
    Expression expression = {s, len, 0, 0};
    int err = 0;
    return (int) compute(&expression, &err);
}


#include <stdlib.h>

int main(void)
{
    char expr[] = "- (3 + (4 + 5))";
    printf("%s = %d\n", expr, calculate(expr));
    return 0;
}
