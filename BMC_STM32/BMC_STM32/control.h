#pragma once


class Control
{
public:
	Control();
	~Control();

	void StartMotor(void);
	/* TODO: implement emergency stop */
	void StopMotor(void);

	void COMPInt(void);

private:

};
