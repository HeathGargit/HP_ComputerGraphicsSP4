#include "Application.h"
#include "MyApplication.h"

int main()
{
	MyApplication* myApp = new MyApplication("Week 7");
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