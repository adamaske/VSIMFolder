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
    void DoPhysics() override;
    void ResetPhysics();
    void AddLife();
    bool Alive(){
        return mLived <= mLifetime ? true : false;
    }
    void EnableSpline();
    void DisableSpline();
private:
    VisualObject* m_Surface;
    float m_Weight = 0.2f;
    QVector3D m_Velocity;
    QVector3D mAcceleration;
    float m_GravityConstant = 9.81f;
    Vertex* oldv1;
    Vertex* oldv2;
    Vertex* oldv3;
    QVector3D oldNormal;
    QVector3D oldVel = QVector3D(0,0,0);
    QVector3D oldPos;
    Vertex* v1;
    Vertex* v2;
    Vertex* v3;
    int mLived = 0;
    int mLifetime = 600;
    int mSplinePointCounter = 60;
    void CreateSplinePoint();
    std::vector<Vertex*> mControlPoints;
    std::vector<Vertex*> mVisualPoints;
    void CreateSpline();
    QVector3D EvaluateBezier(float t);
    bool bDrawSpline = false;
    class SurfaceMesh* mSurfaceMesh;

    QVector3D p1;
    QVector3D p2;
    QVector3D p3;
    QVector3D a;
    QVector3D b;
    QVector3D normal;
    float radius = 0.575f;
};

#endif // ROLLINGBALL_H
