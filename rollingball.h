#ifndef ROLLINGBALL_H
#define ROLLINGBALL_H

#include "objmesh.h"

class Vertex;
class RollingBall : public ObjMesh
{
public:
    RollingBall(std::string fileName, Shader* shader);

    void SetSurface(VisualObject* surface);
    VisualObject* GetSurface(){return m_Surface;};
    void draw() override;
    void DoPhysics() override;
    void ResetPhysics();
    void AddLife();
    bool Alive(){
        return mLived <= mLifetime ? true : false;
    }
    void EnableSpline();
    void DisableSpline();
    int GetLived(){return mLived;};
    int GetLifetime(){return mLifetime;};
    class VisualPoint* GetSpline(){return mSpline;};
private:
    VisualObject* m_Surface;

    //Variabler for levetiden
    int mLived = 0;
    int mLifetime = 600;

    //Bspline variabler
    int mSplinePointCounter = 60;
    void CreateSplinePoint();
    std::vector<Vertex> mControlPoints;

    void CreateSpline();
    QVector3D EvaluateBezier(float x);

    class VisualPoint* mSpline;
    class VisualPoint* mControlPointVisual;
    bool bDrawSpline = false;

    std::vector<float> t;
    std::vector<QVector3D> c;

    int FindKnotInterval(float x);
    int n;
    int d;

    class SurfaceMesh* mSurfaceMesh;
    //Variabler for barysentriske osv
    QVector3D p1;
    QVector3D p2;
    QVector3D p3;
    QVector3D a;
    QVector3D b;
    QVector3D normal;
    Vertex* oldv1;
    Vertex* oldv2;
    Vertex* oldv3;
    QVector3D oldNormal;
    QVector3D oldVel = QVector3D(0,0,0);
    QVector3D oldPos;
    Vertex* v1;
    Vertex* v2;
    Vertex* v3;
    QVector3D m_Velocity;
    QVector3D mAcceleration;
    float m_Weight = 0.2f;

    float m_GravityConstant = 9.81f;

    float radius = 0.575f;
};

#endif // ROLLINGBALL_H
