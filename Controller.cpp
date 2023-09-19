#include "Controller.h"
#include "Model.h"
#include "Command.h"
#include <cmath>
using namespace std;

Controller :: Controller(Model &model, ECGraphicViewImp &view, string file): model(model), view(view), keystroke_observer(view, *this), click_observer(view,*this), render_observer(view,*this), insertion_mode(false), rectangle_mode(true), fill_mode(false), group_mode(false), file(file), selected(false)
{
	view.Attach(&click_observer);
	view.Attach(&render_observer);
	view.Attach(&keystroke_observer);
	LoadFile();
}

void Controller :: LoadFile()
{
	if (file != "")
		model.LoadFile(file);
}

void Controller :: SaveToFile()
{
	if (file != "")
		model.SaveToFile();
}

void Controller :: CreateShape(Shape &shape)
{
	Command* cmd = new CreateShapeCommand(this->model, shape);
	cmdHist.ExecuteCmd(cmd);
}

void Controller :: EditShape(int x2, int y2)
{
	model.EditShape(x2, y2);
	view.SetRedraw(true);
}

bool Controller :: ShapeIsSelected()
{
	return selected;
}

void Controller :: SetSelected(bool new_selected)
{
	selected = new_selected;
}

void Controller :: EditShape(int thickness)
{
	model.EditShape(thickness);
	view.SetRedraw(true);
}


void Controller :: MoveShapeOnScreen(int new_x, int new_y, int old_x, int old_y)
{
	int diff_x = new_x - old_x;
	int diff_y = new_y - old_y;
	ReturnSelectedShape().TranslateShape(diff_x, diff_y); 
	view.SetRedraw(true);
}

void Controller :: CreateMoveShapeCommand(int old_x, int old_y, int new_x, int new_y, bool first_time = true)
{	
	if(selected)
	{	
		Command *cmd = new MoveShapeCommand(ReturnSelectedShape(), old_x, old_y, new_x, new_y, first_time );
		cmdHist.ExecuteCmd(cmd);
		view.SetRedraw(true);
	}
}

void Controller :: Recalibrate()
{
	for(auto shape : model.listShapes())
	{
		shape->Recalibrate();
	}
}

Shape & Controller :: ReturnSelectedShape()
{
	CompositeUnit* comp = new CompositeUnit(false);
	int num_shapes = 0;
	for(auto shape : model.listShapes())
	{
		if(shape->Selected() && !shape->IsGrouped())
		{		
				comp->AddChild(shape);
				num_shapes++;
		}
	}
	if (num_shapes == 1)
		return *comp->Children()[0];
	else
		return *comp;
}
void Controller :: CreateGroup()
{
	if (selected)
	{
		Command *cmd = new CreateGroupCommand(ReturnSelectedShape(), this->model);
		cmdHist.ExecuteCmd(cmd);
		selected = false;
	}
}

bool Controller :: SelectShape(int x, int y)
{
	bool selected = false;
	for(auto shape : model.listShapes())
	{
		if (shape->IsPointInside(x,y))
		{
			if (!group_mode)
				model.UnselectAll();
			shape->Select();
			view.SetRedraw(true);
			selected = true;
		}
	}
		return selected;
}

void Controller :: RemoveShape()
{
	if (selected)
	{
		Command *cmd = new RemoveShapeCommand(ReturnSelectedShape(), this->model);
		cmdHist.ExecuteCmd(cmd);
		view.SetRedraw(true);
	}
	selected = false;
}

void Controller :: Undo()
{
	cmdHist.Undo();
	view.SetRedraw(true);
}

void Controller :: Redo()
{
	cmdHist.Redo();
	view.SetRedraw(true);
}

void Controller :: ChangeMode()
{
	insertion_mode = !insertion_mode;
	if(insertion_mode)
		model.UnselectAll();
}

bool Controller :: InsertionMode()
{
	return insertion_mode;
}

bool Controller :: RectangleMode()
{
	return rectangle_mode;
}

bool Controller :: FillMode()
{
	return fill_mode;
}

void Controller :: SwitchShape()
{
	rectangle_mode = !rectangle_mode;
}

void Controller :: SwitchFillMode()
{
	fill_mode = !fill_mode;
}

void Controller :: SwitchGroupingMode()
{
	group_mode = !group_mode;
}

void Controller :: UnselectAll()
{
	model.UnselectAll();
}

void Controller :: Ungroup()
{
	if (selected)
	{
		Command *cmd = new UngroupCommand(ReturnSelectedShape(), this->model);
		cmdHist.ExecuteCmd(cmd);
		view.SetRedraw(true);
		selected = false;
	}	
}

ClickObserver :: ClickObserver(ECGraphicViewImp &view, Controller &controller): view(view), controller(controller), clicked(false), insertion_mode(true), moved(false), selected(false) {}

void ClickObserver :: Update()
{
	if(controller.InsertionMode())
	{
		if (view.GetCurrEvent() == ECGV_EV_MOUSE_BUTTON_DOWN)
		{
			view.GetCursorPosition(x1,y1);
			if (controller.RectangleMode())
			{
				Shape *rec = new Rectangle(x1,y1,x1,y1,view, controller.FillMode(), ECGV_YELLOW);	
				controller.CreateShape(*rec);
			}
			else
			{	
				Shape *rec = new Ellipse(x1,y1,0,0,view, controller.FillMode(), ECGV_YELLOW);
				controller.CreateShape(*rec);
			}
			clicked = true;
		}
		if (view.GetCurrEvent() == ECGV_EV_MOUSE_MOVING && clicked)
		{
			view.GetCursorPosition(x2,y2);
			if(controller.RectangleMode())
				controller.EditShape(x2, y2);
			else
				controller.EditShape(fabs(x2- x1),fabs(y2 - y1));
		}
		if (view.GetCurrEvent() == ECGV_EV_MOUSE_BUTTON_UP)
		{
			controller.EditShape(3);
			clicked = false;
		}	
	}
	else // edit mode
	{
		if (view.GetCurrEvent() == ECGV_EV_MOUSE_BUTTON_DOWN)
		{
			view.GetCursorPosition(x1,y1);
			selected = controller.SelectShape(x1,y1);
			controller.SetSelected(selected);
			if (selected)
			{
				orig_x = x1;
				orig_y = y1;		
			}		
		}
		if (view.GetCurrEvent() == ECGV_EV_MOUSE_MOVING && selected)
		{
			moved = true;
			view.GetCursorPosition(x2,y2);
			controller.MoveShapeOnScreen(x2,y2,x1,y1);
			x1 = x2;
			y1 = y2;
		}	
		if (view.GetCurrEvent() == ECGV_EV_MOUSE_BUTTON_UP && selected)
		{
			if (moved)
				controller.CreateMoveShapeCommand(orig_x,orig_y,x2,y2);
			selected = false;
			moved = false;
		}
	}
}

KeyStrokeObserver :: KeyStrokeObserver(ECGraphicViewImp &view, Controller &controller): view(view), controller(controller) {}

void KeyStrokeObserver :: Update()
{
	switch(view.GetCurrEvent())
	{
		case ECGV_EV_KEY_DOWN_SPACE:
			controller.ChangeMode();
			controller.Recalibrate();
			break;	
		case ECGV_EV_KEY_DOWN_Z:
			controller.Undo();
			break;
		case ECGV_EV_KEY_DOWN_Y:
			controller.Redo();
			break;
		case ECGV_EV_KEY_DOWN_G:
			if(controller.InsertionMode())
				controller.SwitchShape();
			else
				if(controller.ReturnSelectedShape().AttachedToModel())
					controller.Ungroup();
				else
					controller.CreateGroup();
			break;
		case ECGV_EV_KEY_DOWN_D:
			controller.RemoveShape();
			break;
		case ECGV_EV_KEY_DOWN_F:
			controller.SwitchFillMode();
			break;
		case ECGV_EV_KEY_DOWN_UP:
			if(!controller.InsertionMode())
				controller.CreateMoveShapeCommand(0,10,0,0,false);
			break;
		case ECGV_EV_KEY_DOWN_DOWN:
			if(!controller.InsertionMode())
				controller.CreateMoveShapeCommand(0,0,0,10,false);
			break;
		case ECGV_EV_KEY_DOWN_LEFT:
			if(!controller.InsertionMode())
				controller.CreateMoveShapeCommand(10,0,0,0,false);
			break;
		case ECGV_EV_KEY_DOWN_RIGHT:
			if(!controller.InsertionMode())
				controller.CreateMoveShapeCommand(0,0,10,0,false);
			break;
		case ECGV_EV_KEY_DOWN_CTRL:
			controller.SwitchGroupingMode();
			break;
		case ECGV_EV_KEY_UP_CTRL:
			controller.SwitchGroupingMode();
			break;
		case ECGV_EV_KEY_UP_G:
			controller.UnselectAll();
			break;
	}
}

RenderObserver :: RenderObserver(ECGraphicViewImp &view, Controller &controller): view(view), controller(controller) {}

void RenderObserver :: Update()
{
	if (view.GetCurrEvent() == ECGV_EV_TIMER)
	{
		for(auto x : controller.model.listShapes())
			x->Render();	
	}
	if (view.GetCurrEvent() == ECGV_EV_KEY_UP_ESCAPE)
	{
		controller.SaveToFile();
	}	
}
