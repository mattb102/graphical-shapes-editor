#include "Controller.h"
#include "Model.h"
#include "Command.h"

using namespace std;

CommandHistory :: CommandHistory(): hist_index(-1) {}

void CommandHistory :: ExecuteCmd(Command * cmd)
{
	cmd->Execute();
	if(hist_index >= -1)
	{
		int szList = history.size();
		for(int i = hist_index + 1; i < szList; ++i)
		{
			delete history.back();
			history.pop_back();
		}
	}
	history.push_back(cmd);
	hist_index++;
}

void CommandHistory :: Undo()
{
	if(hist_index >= 0)
	{
		cout << "undoing" << endl;
		history[hist_index]->Unexecute();
		hist_index--;
	}
}

void CommandHistory :: Redo()
{
		if(hist_index < (int)history.size() - 1)
		{
			cout << "redoing" << endl;
			hist_index++;
			history[hist_index]->Execute();
		}
}

CreateShapeCommand :: CreateShapeCommand(Model &model, Shape &shape): shape(shape), model(model)  {}

void CreateShapeCommand :: Execute()
{
	model.AddShape(shape);
}

void CreateShapeCommand :: Unexecute()
{
	model.RemoveShape();
}

RemoveShapeCommand :: RemoveShapeCommand(Shape &shape, Model &model): model(model), shape(shape) {}

void RemoveShapeCommand :: Execute()
{
	model.RemoveShape(shape);
}

void RemoveShapeCommand :: Unexecute()
{
	model.ReAddShape(shape);	
}

UngroupCommand :: UngroupCommand(Shape &comp, Model &model): comp(comp), model(model) {}

void UngroupCommand :: Execute()
{
	removed = comp.RemoveChildren();
	for(auto shape : removed)
	{
		shape->Unselect();
		shape->SetGrouped(false);
	}	
}

void UngroupCommand :: Unexecute()
{
	
	for(auto shape : removed)
	{
		shape->SetGrouped(true);
		shape->Select();
		dynamic_cast<CompositeUnit*>(&comp)->AddChild(shape);
	}

	comp.Attach();
}

MoveShapeCommand :: MoveShapeCommand(Shape &shape, int old_x, int old_y, int new_x, int new_y, bool first_time = true): old_x(old_x), old_y(old_y), new_x(new_x), new_y(new_y), shape(shape), first_time(first_time)  {}

void MoveShapeCommand :: Execute()
{
	if (!first_time)
		shape.TranslateShape(new_x-old_x,new_y-old_y);
	else
		first_time = false;
	
}

void MoveShapeCommand :: Unexecute()
{
	shape.TranslateShape(old_x-new_x, old_y-new_y );
}

CreateGroupCommand :: CreateGroupCommand(Shape &shape, Model &model): shape(shape), model(model) {}

void CreateGroupCommand :: Execute()
{
	shape.SetChildrenGrouped(true);
	model.AddShape(shape);
	
}

void CreateGroupCommand :: Unexecute()
{
	shape.SetChildrenGrouped(false);
	model.RemoveShape();
}
