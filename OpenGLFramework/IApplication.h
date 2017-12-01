/*---------------------------------------------------------
File Name: iApplication.h
Purpose: Abstract class for creating applications in the framework
Author: Heath Parkes (gargit@gargit.net)
Modified: 1/12/2017
-----------------------------------------------------------
Copyright 2017 AIE/HP
---------------------------------------------------------*/

#ifndef APPLICATION_HEADER
#define APPLICATION_HEADER

class IApplication
{
public:
	virtual bool startup() = 0;
	virtual bool update() = 0;
	virtual void draw() = 0;
	virtual void shutdown() = 0;
};

#endif