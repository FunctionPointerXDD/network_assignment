
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"

static size_t	ft_strlcpy(char* dst, const char* src, size_t dstsize)
{
	size_t	src_len;
	size_t	ret;

	src_len = 0;
	ret = strlen(src);
	if (dstsize != 0)
	{
		if (ret >= dstsize)
			src_len = dstsize - 1;
		else
			src_len = ret;
		memcpy(dst, src, src_len);
		dst[src_len] = '\0';
	}
	return (ret);
}

static int	count_word(char const* str, char c)
{
	int	count;

	count = 0;
	if (*str == c)
		count = -1;
	while (*str)
	{
		while (*str && *str != c)
			++str;
		while (*str && *str == c)
			++str;
		++count;
	}
	return (count);
}

static void	free_arr(char** str_arr, int cnt)
{
	int	i;

	i = 0;
	while (i < cnt)
	{
		free(str_arr[i]);
		i++;
	}
	free(str_arr);
}

static int	create_arr(char** str_arr, char const* str, char c, int count_words)
{
	int	len;
	int	i;

	i = 0;
	while (*str && i < count_words)
	{
		len = 0;
		while (*str && *str == c)
			str++;
		while (*str && *str != c)
		{
			str++;
			len++;
		}
		str_arr[i] = (char*)malloc(sizeof(char) * len + 1);
		if (str_arr[i] == 0)
		{
			free_arr(str_arr, i);
			return (0);
		}
		ft_strlcpy(str_arr[i], str - len, len + 1);
		++i;
	}
	str_arr[i] = 0;
	return (1);
}

char** ft_split(const char* s, char c)
{
	char** str_array;
	int		count_words;

	count_words = count_word(s, c);
	str_array = (char**)malloc(sizeof(char*) * (count_words + 1));
	if (str_array == 0)
		return (0);
	if (create_arr(str_array, s, c, count_words) == 0)
		return (0);
	return (str_array);
}

void	free_split_arr(char** str_arr)
{
	int i = 0;

	while (str_arr[i])
		free(str_arr[i++]);
	free(str_arr);
}

/**
 * strsep - Split a string into tokens
 * @s: The string to be searched
 * @ct: The characters to search for
 * strsep() updates @s to point after the token,
 * --> ready for the next call.
 */
char* ft_strsep(char** s, const char* ct)
{
	char* sbegin;
	char* end;

	sbegin = *s;
	if (sbegin == NULL)
		return (NULL);
	end = strpbrk(sbegin, ct);
	if (end)
		*end++ = '\0';
	*s = end;
	return (sbegin);
}

void	error_msg(const char* str)
{
	fprintf(stderr, "%s\n", str);
	exit(1);
}

