#pragma once

typedef enum STATUS { FAIL, SUCCESS } STATUS;

void	print_row(Row* row);
STATUS	Delete(Table* table);
STATUS	Update(Table* table);
STATUS	Insert(Table* table);
STATUS	Select(Table* table);
int		PrintUI(void);
void	event_loop(Table* table);
