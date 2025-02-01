#pragma once

char**	ft_split(char const* s, char c);
void	free_split_arr(const char** str_arr);
char*	ft_strsep(char** s, const char* ct);
void	error_msg(const char* str);
