#include "MyApplication.h"

int main()
{
	MyApplication* myApp = new MyApplication("Solar System");
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