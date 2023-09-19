#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "ECObserver.h"
#include "ECGraphicViewImp.h"
#include "Model.h"
#include "Command.h"
#include <iostream>
using namespace std;

class Controller;

class ClickObserver :public ECObserver
{
public:
	ClickObserver(ECGraphicViewImp &view, Controller &controller);
	void Update();	
private:
	vector<int> coords;
	bool insertion_mode;
	bool clicked;
	bool moved;
	bool selected;
	int orig_x;
	int orig_y;
	int x1;
	int y1;
	int x2;
	int y2;
	ECGraphicViewImp &view;
	Controller &controller;
};

class KeyStrokeObserver :public ECObserver
{
public:
	KeyStrokeObserver(ECGraphicViewImp &view, Controller &controller);
	void Update();
private:
	ECGraphicViewImp &view;
	Controller &controller;

};

class RenderObserver :public ECObserver
{
public:
	RenderObserver(ECGraphicViewImp &view, Controller &controller);
	void Update();
private:
	ECGraphicViewImp &view;
	Controller &controller;
};

class Controller
{
public:
	Controller(Model &model, ECGraphicViewImp &view, string file = "");
	void Undo();
	void Redo();
	void LoadFile();
	void SaveToFile();
	void CreateShape(Shape &shape);
	void EditShape(int x2, int y2);
	void EditShape(int thickness);
	bool SelectShape(int x, int y);
	void SetSelected(bool new_selected);
	bool ShapeIsSelected();
	void UnselectAll();
	void ChangeMode();
	bool InsertionMode();
	bool RectangleMode();
	void SwitchShape();
	void SwitchFillMode();
	void SwitchGroupingMode();
	bool FillMode();
	void RemoveShape();
	void CreateGroup();
	void Ungroup();
	void MoveShapeOnScreen(int new_x, int new_y, int old_x, int old_y);
	void CreateMoveShapeCommand(int old_x1, int old_y1, int old_x2, int old_y2, bool first_time);
	Shape& ReturnSelectedShape();
	void Recalibrate();
	
	Model &model;
private:
	bool insertion_mode;
	bool rectangle_mode;
	bool fill_mode;
	bool group_mode;
	bool selected;
	CommandHistory cmdHist;
	string file;
	ClickObserver click_observer;
	RenderObserver render_observer;
	KeyStrokeObserver keystroke_observer;
	ECGraphicViewImp &view;
};


#endif
