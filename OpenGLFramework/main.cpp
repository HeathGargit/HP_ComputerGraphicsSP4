/*---------------------------------------------------------
File Name: main.cpp
Purpose: the project's "run" file
Author: Heath Parkes (gargit@gargit.net)
Modified: 1/12/2017
-----------------------------------------------------------
Copyright 2017 AIE/HP
---------------------------------------------------------*/

#include "MyApplication.h"

int main()
{
	//create a new application based on the application abstract class
	MyApplication* myApp = new MyApplication("Undead Scene");

	if (myApp->startup())
	{
		while (myApp->update())
		{
			myApp->draw();
		}

		myApp->shutdown();
		return 0;
	}
}