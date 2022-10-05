#ifndef SURFACEMESH_H
#define SURFACEMESH_H

#include "vertex.h"
#include "visualobject.h"

class Vertex;

struct Result{
    float height;
    Vertex v1;
    Vertex v2;
    Vertex v3;
    float friction = 0;
};

struct Point {
public:
    double x;
    double y;

    void SetXY(double a, double b) {
        x = a;
        y = b;
    }
};

struct Quad {
    float topRight;
    float topLeft;
    float bottomRight;
    float bottomLeft;

    float height = 1;
    int amount = 1;

    void AddHeight(float h){
        height += h;
        amount++;
    }

    void SetHeight(float h){
        height = h;
    }

    float GetHeight() {
        return height / amount;
    }

    Point GetCenter() {
        return Point{ (topRight + topLeft) / 2, (bottomRight + bottomLeft) / 2};
    }
};

enum DrawMode{ points, drawElements, arrays};
class SurfaceMesh : public VisualObject
{
public:
    SurfaceMesh(Shader* s);
    void draw() override;
    void init() override;
    //
    DrawMode mDrawMode = DrawMode::points;
    void SetDrawMode(DrawMode m);
    QVector3D GetBarycentric(QVector3D point, Vertex v1,  Vertex v2, Vertex v3);
    //
    Result GetHeight(QVector3D pos);

    float width =   1000;
    float height=   1000;
    float res =     20 ;

    //Barysentriske ting
    //First v
    Vertex v1;
    //Get the;
    Vertex v2;
    //Get the;
    Vertex v3;
    //Vertex ;
    Vertex v4;
    //Vertex ;
    Vertex v5;
    //Vertex ;
    Vertex v6;

    Vertex inUseVertex1;
    Vertex inUseVertex2;
    Vertex inUseVertex3;

    QVector3D a;
    QVector3D b;
    QVector3D c;

    QVector3D x1;
    QVector3D x2;

    QVector3D normal;

    QVector3D p;
    QVector3D q;
};

#endif // SURFACEMESH_H
