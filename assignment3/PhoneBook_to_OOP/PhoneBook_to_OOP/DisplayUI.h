#pragma once

class DisplayUI
{
public:
	DisplayUI(void);
	~DisplayUI(void);

	virtual void PrintStatus(int status); // SUCCESS, FAIL
	virtual void Print_Numbers_Of_Found(int found); // numbers of found rows
	virtual void Print_Numbers_Of_Change(int changes); // numbers of found rows

private:
	DisplayUI(const DisplayUI& rhs);
};

