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