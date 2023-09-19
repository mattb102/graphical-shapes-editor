#ifndef COMMAND_H
#define COMMAND_H

#include "ECObserver.h"
#include "ECGraphicViewImp.h"
#include "Model.h"
#include <iostream>
using namespace std;

class ClickObserver;
class RenderObserver;

class Command
{
public:
	virtual void Execute() = 0;
	virtual void Unexecute() = 0;
};

class CommandHistory
{
public:
	void Undo();
	void Redo();
	void ExecuteCmd(Command * cmd);
	CommandHistory();
	vector<Command*> history;
	int hist_index;

};

class CreateShapeCommand :public Command
{
public:
	CreateShapeCommand(Model &model, Shape &shape);
	void Execute();
	void Unexecute();
private:
	bool filled;
	Shape &shape;
	Model &model;
};


class RemoveShapeCommand :public Command
{
public:
	RemoveShapeCommand(Shape &shape, Model &model);
	void Execute();
	void Unexecute();
private:
	Shape &shape;
	int position;
	Model &model;
};


class MoveShapeCommand :public Command
{
public:
	MoveShapeCommand(Shape &shape, int old_x, int old_y, int new_x, int new_y, bool first_time);
	void Execute();
	void Unexecute();
private:
	bool first_time;
	int old_x;
	int old_y;
	int new_x;
	int new_y;
	Shape &shape;
};

class CreateGroupCommand :public Command
{
public:
	 CreateGroupCommand(Shape &shape, Model &model);
	 void Execute();
	 void Unexecute();
private:
	Shape &shape;
	Model &model;
};

class UngroupCommand :public Command
{
public:
	UngroupCommand(Shape &comp, Model &model);
	void Execute();
	void Unexecute();
private:
	Shape &comp;
	Model &model;
	vector<Shape*> removed;
};

#endif
