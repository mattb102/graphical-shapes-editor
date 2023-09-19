#ifndef MODEL_H
#define MODEL_H

#include "ECObserver.h"
#include "ECGraphicViewImp.h"
#include <iostream>
using namespace std;

class Shape
{
public:
	Shape(bool attached = true, bool filled = false, int thickness = 3, bool selected = false, ECGVColor color = ECGV_BLACK, bool render = true, bool grouped = false): thickness(thickness), selected(selected), color(color), filled(filled), render(render), grouped(grouped), original_color(color) {};
	virtual bool AttachedToModel() { return attached; }
	virtual bool IsRendered()      { return render; }
	virtual bool IsGrouped()       { return grouped; }
	virtual bool Selected()        { return selected; }
	virtual void Attach()          { attached = true; }
	virtual void UpdateThickness(int thickness);
	virtual void Unselect();
	virtual void SetGrouped(bool new_grouped);
	virtual void ChangeColor(ECGVColor new_color);
	virtual bool IsPointInside(int cursor_x, int cursor_y) = 0;
	virtual string GetShapeFileInfo() = 0;
	virtual void TranslateShape(int diff_x, int diff_y) = 0;
	virtual void SetChildrenGrouped(bool new_grouped) = 0;
	virtual void Select() = 0;
	virtual void Recalibrate() = 0;
	virtual void Render() = 0;
	virtual vector<Shape*> RemoveChildren() = 0;
	virtual void SetRender(bool new_render) = 0;	
	virtual void Resize(int new_x2, int new_y2) = 0;
protected:
	int thickness;
	bool attached;
	bool grouped;
	bool filled;
	bool render;
	bool selected;
	ECGVColor color;
	ECGVColor original_color;
};

class Rectangle :public Shape
{
public:
	Rectangle(int x1,int y1,int x2,int y2, ECGraphicViewImp &view, bool filled, ECGVColor color = ECGV_BLACK);
	int NumChildren();
	bool IsPointInside(int cursor_x, int cursor_y); 
	string GetShapeFileInfo();
	void TranslateShape(int diff_x, int diff_y);
	void SetChildrenGrouped(bool new_grouped);
	void Select();
	void Recalibrate();
	void Render();
	vector<Shape*> RemoveChildren();
	void SetRender(bool new_render);
	void Resize(int new_x2, int new_y2);
private:
	int x1;
	int y1;
	int x2;
	int y2;
	ECGraphicViewImp &view;

};

class Ellipse : public Shape 
{
public:
	Ellipse(int x, int y, int radiusx, int radiusy, ECGraphicViewImp &view, bool filled, ECGVColor color = ECGV_BLACK);
	int NumChildren();
	bool IsPointInside(int cursor_x, int cursor_y); 
	string GetShapeFileInfo();
	void TranslateShape(int diff_x, int diff_y);
	void SetChildrenGrouped(bool new_grouped);
	void Select();
	void Recalibrate();
	void Render();
	vector<Shape*> RemoveChildren();
	void SetRender(bool new_render);
	void Resize(int new_x2, int new_y2);
private:
	int x;
	int y;
	int radiusx;
	int radiusy;
	ECGraphicViewImp &view;
};

class CompositeUnit :public Shape
{
public:
	CompositeUnit(bool attached = true);
	void Select();
	string GetShapeFileInfo();
	virtual void SetChildrenGrouped(bool new_grouped);
	void AddChild(Shape *shape);
	int NumChildren();
	vector<Shape*> RemoveChildren();
	void TranslateShape(int diff_x,int diff_y);
	void Render();
	vector<Shape*> Children() { return listChildren; }
	void SetRender(bool new_render);
	void Recalibrate();
	void Resize(int new_x2, int new_y2);
	bool IsPointInside(int cursor_x, int cursor_y);
private:
	vector<Shape*> listChildren;
};

class Model
{
public:
	Model(ECGraphicViewImp &view);
	ECGVColor GetColor(int color);
	void LoadFile(string filename);
	void SaveToFile();
	Shape& CreateShapeFromFile(int shape_type, fstream &file, bool grouped);
	void AddShape(Shape &shape);
	void ReAddShape(Shape &shape);
	void RemoveShape();
	void RemoveShape(Shape &shape);
	void UnselectAll();
	void EditShape(int x2, int y2);
	void EditShape(int thickness);
	vector<Shape*> listShapes();
private:
	string filename;
	vector<Shape*> shapes;
	int index;
	ECGraphicViewImp &view;

};
#endif
