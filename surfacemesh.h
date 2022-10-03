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

public:
    void setV1(const Vertex &newV1);
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
    Point topRight;
    Point topLeft;
    Point bottomRight;
    Point bottomLeft;

    float height = 1;
    int amount = 1;
    void AddHeight(float h){
        height += h;
        amount++;
    }
    float GetHeight() {
        return height / amount;
    }
    Point GetCenter() {
        return Point{ (topRight.x + topLeft.x) / 2, (topRight.y + bottomLeft.y) / 2};
    }
    bool IsPointInMe(Point p){
        if(p.x > topLeft.x && p.x < topRight.x  && p.y < topLeft.y && p.y > bottomLeft.y){
            return true;
        }else{
            return false;
        }
    }
};
struct Triangle {
public:
    Point p1;
    Point p2;
    Point p3;
    void SetXYZ(Point a, Point b, Point c) {
        p1 = a;
        p2 = b;
        p3 = c;
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


};

#endif // SURFACEMESH_H
