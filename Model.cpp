#include "Controller.h"
#include "Model.h"
#include <cmath>
#include <fstream>
#include <iostream>

using namespace std;

void Shape :: UpdateThickness(int new_thickness)
{
	thickness = new_thickness;
}

void Shape :: Unselect()
{
	color = original_color;
	selected = false;
}

void Shape :: ChangeColor(ECGVColor new_color)
{
	color = new_color;
	original_color = new_color;
}

void Shape :: SetGrouped(bool new_grouped)
{
	grouped = new_grouped;
}


Rectangle :: Rectangle(int x1,int y1,int x2,int y2, ECGraphicViewImp &view, bool filled, ECGVColor color) : x1(x1), y1(y1), x2(x2), y2(y2), view(view), Shape(true,filled,1,false,color) {}

void Rectangle :: Render()
{
	if (render)
	{
		if (filled)
			view.DrawFilledRectangle(x1,y1,x2,y2,color);
		else
			view.DrawRectangle(x1,y1,x2,y2,thickness,color);
		view.SetRedraw(true);
	}
}

void Rectangle :: SetChildrenGrouped(bool new_grouped)
{
	return;
}

string Rectangle :: GetShapeFileInfo()
{
	string info;
	if (filled)
		info += "2 4 ";
	else
		info += "0 4 ";

	info += to_string(x1) + " ";
	info += to_string(y1) + " ";
	info += to_string(x1) + " ";
	info += to_string(y2) + " ";
	info += to_string(x2) + " ";
	info += to_string(y1) + " ";
	info += to_string(x2) + " ";
	info += to_string(y2) + " ";
	info += to_string(color) + "\n";
	
	return info;	
}

vector<Shape*> Rectangle :: RemoveChildren()
{
	vector<Shape*> null;
	return null;
}

void Rectangle :: Select()
{
	color = ECGV_BLUE;
	selected = true;
}



void Rectangle :: SetRender(bool new_render)
{
	render = new_render;
}

void Rectangle :: Resize(int new_x2,int new_y2)
{
	x2 = new_x2;
	y2 = new_y2;

}


void Rectangle :: TranslateShape(int diff_x, int diff_y)
{
	x1 += diff_x;
	y1 += diff_y;
	x2 += diff_x;
	y2 += diff_y;
}



void Rectangle :: Recalibrate()
{
	int old_x1 = x1;
	int old_x2 = x2;
	int old_y1 = y1;
	int old_y2 = y2;

	x1 = max(old_x1, old_x2);
	x2 = min(old_x1, old_x2);
	y1 = max(old_y1, old_y2);
	y2 = min(old_y1, old_y2);
}

bool Rectangle :: IsPointInside(int cursor_x, int cursor_y)
{

	if (cursor_x < x1 && cursor_x > x2 && cursor_y < y1 && cursor_y > y2)
	{
		return true;
	}
	return false;

}

Ellipse :: Ellipse(int x, int y, int radiusx, int radiusy, ECGraphicViewImp &view, bool filled, ECGVColor color): x(x), y(y), radiusx(radiusx), radiusy(radiusy), view(view), Shape(true, filled, 1, false, color) {}


void Ellipse :: Render()
{
	if (render)
	{
		if (filled)
			view.DrawFilledEllipse(x,y,radiusx,radiusy, color);
		else
			view.DrawEllipse(x,y,radiusx,radiusy,thickness,color);
		view.SetRedraw(true);
	}
}

void Ellipse :: SetChildrenGrouped(bool new_grouped)
{
	return;
}

string Ellipse :: GetShapeFileInfo()
{
	string info;
	if (filled)
		info += "3 ";
	else
		info += "1 ";

	info += to_string(x) + " ";
	info += to_string(y) + " ";
	info += to_string(radiusx) + " ";
	info += to_string(radiusy) + " ";
	info += to_string(color) + "\n";
	
	return info;
	
}


vector<Shape*> Ellipse :: RemoveChildren()
{
	vector<Shape*> null;
	return null;
}

void Ellipse :: Select()
{
	color = ECGV_BLUE;
	selected = true;
}



void Ellipse :: SetRender(bool new_render)
{
	render = new_render;
}

void Ellipse :: TranslateShape(int diff_x, int diff_y)
{
	x += diff_x;
	y += diff_y;
}

void Ellipse :: Resize(int new_x2, int new_y2)
{
	radiusx = new_x2;
	radiusy = new_y2;
} 

bool Ellipse :: IsPointInside(int cursor_x, int cursor_y)
{
	bool in_x = cursor_x >= x - radiusx && cursor_x <= x + radiusx;
	bool in_y = cursor_y >= y - radiusy && cursor_y <= y + radiusy;
	return (in_x && in_y);
}

void Ellipse :: Recalibrate()
{
	return;
}

CompositeUnit :: CompositeUnit(bool attached): Shape(attached) {}

void CompositeUnit :: AddChild(Shape *shape)
{
//	attached = true;
	listChildren.push_back(shape);
}

void CompositeUnit :: Select()
{
	selected = true;
	for(auto shape : listChildren)
		shape->Select();
}

string CompositeUnit :: GetShapeFileInfo()
{
	if (listChildren.size() == 0)
		return "";
	
	string info = "4 " + to_string(listChildren.size()) + "\n";
	for(auto child : listChildren)
	{
		info += child->GetShapeFileInfo();
	}

	return info;
}

void CompositeUnit :: TranslateShape(int diff_x, int diff_y)
{
	for(auto shape : listChildren)
		shape->TranslateShape(diff_x, diff_y);
}

void CompositeUnit :: Render()
{
	for(auto shape : listChildren)
		shape->Render();
}

vector<Shape*> CompositeUnit :: RemoveChildren()
{
	vector<Shape*> removed;
	for(auto shape : listChildren)
	{
		removed.push_back(shape);
		shape->SetGrouped(false);
	}
	listChildren.clear();
	attached = false;	
	return removed;
}

void CompositeUnit :: SetRender(bool new_render)
{
	for(auto shape : listChildren)
		shape->SetRender(new_render);
	render = false;
}

void CompositeUnit :: Recalibrate()
{
	for(auto shape : listChildren)
		shape->Recalibrate();
}

void CompositeUnit :: Resize(int new_x2, int new_y2)
{
	for(auto shape : listChildren)
		shape->Resize(new_x2, new_y2);
}

void CompositeUnit :: SetChildrenGrouped(bool new_grouped)
{
	for (auto child : listChildren)
		child->SetGrouped(new_grouped);
}


bool CompositeUnit :: IsPointInside(int cursor_x, int cursor_y)
{
	for(auto shape : listChildren)
	{
		if(shape->IsPointInside(cursor_x, cursor_y))
			return true;
	}
		
	return false;
}


Model :: Model(ECGraphicViewImp &view): view(view), index(-1) {}

void Model :: AddShape(Shape &shape)
{
	shape.Attach(); 
	shapes.push_back(&shape);
	index++;
}

void Model :: ReAddShape(Shape &shape)
{
	shape.SetRender(true);
}

void Model :: RemoveShape(Shape &shape)
{
	shape.SetRender(false);
}

void Model :: RemoveShape()
{
	if (shapes.size() > 0)
	{
		shapes.pop_back();
		index--;
	}
}

void Model :: UnselectAll()
{
	for(auto shape : shapes)
		shape->Unselect();
}

void Model :: EditShape(int x2, int y2)
{
	shapes[index]->Resize(x2,y2);
}

void Model :: EditShape(int thickness)
{
	shapes[index]->UpdateThickness(thickness);
	shapes[index]->ChangeColor(ECGV_BLACK);
}


vector<Shape*> Model :: listShapes()
{
	return shapes;
}

ECGVColor Model :: GetColor(int color)
{
	switch(color)
	{
		case 0:
			return ECGV_BLACK;
			break;
		case 1:
			return ECGV_WHITE;
			break;
		case 2:
			return ECGV_RED;
			break;
		case 3:
			return ECGV_GREEN;
			break;
		case 4:
			return ECGV_BLUE;
			break;
		case 5:
			return ECGV_YELLOW;
			break;
		case 6:
			return ECGV_PURPLE;
			break;
		case 7:
			return ECGV_CYAN;
			break;
	}

	return ECGV_BLACK;	
}

Shape& Model :: CreateShapeFromFile(int shape_type, fstream &file, bool grouped)
{
	if(shape_type == 0 || shape_type ==  2){
		string x1;
		string y1;
		string x2;
		string y2;
		string num_corners;
		string dummy;
		string color;
		file >> num_corners >> x1 >> y1;
		file >> dummy >> y2 >> x2 >> dummy;
		file >> dummy >> dummy  >> color;
		Shape* rec = new Rectangle(stoi(x1), stoi(y1), stoi(x2), stoi(y2), view, shape_type == 2, GetColor(stoi(color)));
		rec->UpdateThickness(3);
		rec->SetGrouped(grouped);
		rec->Recalibrate();
		AddShape(*rec);
		return *rec;
	}

	else if(shape_type == 1 || shape_type == 3){
		string x;
		string y;
		string radius_x;
		string radius_y;
		string color;
		file >> x >> y >> radius_x >> radius_y >> color;
		Shape* ell = new Ellipse(stoi(x), stoi(y), stoi(radius_x), stoi(radius_y), view, shape_type == 3, GetColor(stoi(color)));
		ell->UpdateThickness(3);
		ell->SetGrouped(grouped);
		AddShape(*ell);
		return *ell;
	}

	else if(shape_type == 4)
	{
		CompositeUnit *group = new CompositeUnit(true);
		group->SetGrouped(grouped);
		string num_comp_shapes;
		file >> num_comp_shapes;
		string shape_type2;
		for(int i = 0; i < stoi(num_comp_shapes); i++)
		{	
			file >> shape_type2;
			Shape &shape = CreateShapeFromFile(stoi(shape_type2), file, true);
			group->AddChild(&shape);
		}

		AddShape(*group);
		return *group;
	}
}

void Model :: LoadFile(string filename)
{
	this->filename = filename;
	fstream file;
	file.open(filename);
	string num_shapes;
	string shape_type;
	
	file >> num_shapes;
	if (num_shapes == "")
		return;	
	for(int i = 0; i < stoi(num_shapes); ++i)
	{
		file >> shape_type;
		CreateShapeFromFile(stoi(shape_type), file, false);
	}	
	file.close();
}

void Model :: SaveToFile()
{
	UnselectAll();
	fstream file;
	file.open(filename, ios::out | ios::trunc);
	int num_shapes = 0;
	for(auto shape : shapes)
	{
		if (!shape->IsGrouped() && shape->AttachedToModel() && shape->IsRendered())
			num_shapes++;
	}
	file << to_string(num_shapes) + "\n";
	for(auto shape : shapes)
	{
		if (!shape->IsGrouped() && shape->IsRendered())
			file << shape->GetShapeFileInfo();	
			
	}
	file.close();	
}
