
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

void	error_msg(const char *msg)
{
	puts(msg);
	exit(1);
}

int	valid_check(const char *pszBuffer)
{
	const char	*tmp = pszBuffer;
	int			sign_cnt = 0;
	int			op_cnt = 0;

	if(*tmp == '-' || *tmp == '+')
		tmp++;
	while(*tmp)
	{
		if (!isdigit(*tmp) && !isblank(*tmp) && *tmp != '+' && *tmp != '-' && *tmp != '*' && *tmp != '/')
			return 0;
		else if (*tmp == '+' || *tmp == '-')
		{
			sign_cnt++;
			if (sign_cnt > 2)
				return 0;
		}
		else if (*tmp == '*' || *tmp == '/')
		{
			op_cnt++;
			if (op_cnt > 1)
				return 0;
		}
		tmp++;
	}
	return 1;
}

void	parse_expression(const char *pszBuffer, char *operator, int *num1, int *num2)
{
	char		*op_idx;
	const char	*tmp = pszBuffer;
	long long	n1 = 0, n2 = 0;

	if (!valid_check(tmp))
		error_msg("Invalid value --> Usage: [n + m], [n * m]  ...");

	if(tmp[0] == '+' || tmp[0] == '-')
		tmp++;
	op_idx = strpbrk(tmp,"+-*/");
	if (!op_idx)
		error_msg(">Usage: [n + m], [n * m]  ...");
	*operator = *op_idx;
	*op_idx = '\0';
	n1 = atol(pszBuffer);
	n2 = atol(op_idx + 1);
	if (*operator == '-')
		n2 *= -1;
	if ((n1 > INT_MAX || n2 > INT_MAX) && (n1 < INT_MIN || n2 < INT_MIN))
		error_msg("out of range");
	*num1 = n1;
	*num2 = n2;
}

int	Add(int *num1, int *num2)
{
	unsigned int nSum = 0, nCarry = 0;
	unsigned int x = *num1;
	unsigned int y = *num2;

	nSum = x ^ y;
	nCarry = x & y;

	while (nCarry != 0)
	{
		nCarry = nCarry << 1;
		x = nSum;
		y = nCarry;
		nSum = x ^ y;
		nCarry = x & y;
	}
	if ((int)nSum > INT_MAX || (int)nSum < INT_MIN)
		error_msg("out of range");
	return nSum;
}

int	Sub(int *num1, int *num2)
{
	return Add(num1, num2);
}

int	Multi(int *num1, int *num2)
{
	unsigned int nMul = 0;
	unsigned int x = *num1;
	unsigned int y = *num2;

	nMul = 0;
	while (y != 0) // Iterate the loop till b == 0
	{
		if (y & 1) // Bitwise & of the value of b with 1
		{
			nMul = nMul + x; // Add a to result if b is odd .
		}
		x <<= 1; // Left shifting the value contained in 'a' by 1
				 // Multiplies a by 2 for each loop
		y >>= 1; // Right shifting the value contained in 'b' by 1.
	}

	if ((int)nMul > INT_MAX || (int)nMul < INT_MIN)
		error_msg("out of range");
	return nMul;
}

int	Divide(int *num1, int *num2)
{

	(void)num1;	
	(void)num2;
	return 0;
}

int	main(void)
{
	char	szBuffer[32];
	char	operator;
	int		nResult = 0, num1 = 0, num2 = 0;

	fgets(szBuffer, 32, stdin);	
	szBuffer[strlen(szBuffer) - 1] = '\0';

	parse_expression(szBuffer, &operator, &num1, &num2);

	switch(operator)
	{
		case '+':
			nResult = Add(&num1, &num2);
			break;
		case '-':
			nResult = Sub(&num1, &num2);
			break;
		case '*':
			nResult = Multi(&num1, &num2);
			break;
		case '/':
			nResult = Divide(&num1, &num2);
			break;
		default:
			nResult = 0;
			break;
	}
	printf("Result: %d\n", nResult);
	return 0;
}
