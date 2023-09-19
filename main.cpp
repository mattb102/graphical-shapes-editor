#include "ECObserver.h"
#include "ECGraphicViewImp.h"
#include "Controller.h"

int main(int argc, char **argv)
{
	
	ECGraphicViewImp imp(500,500);
	Model model(imp);
	if (argc > 1)
	{
		Controller controller(model,imp, argv[1]);
		imp.Show();
	}
	else
	{
		Controller controller(model,imp);
		imp.Show();
	}
}
