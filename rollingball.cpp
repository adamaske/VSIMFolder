#include "rollingball.h"
#include "surfacemesh.h"
RollingBall::RollingBall(std::string fileName, Shader* shader) : ObjMesh(fileName, shader)
{

}

void RollingBall::SetSurface(VisualObject* surface)
{
    m_Surface = surface;
}

void RollingBall::DoPhysics()
{
    QVector3D gravity(0, -9.81f, 0);

    float radius = 0.125f;
    if(m_Surface){
        //Får resultatet fra surfacemesh
        Result r = dynamic_cast<SurfaceMesh*>(m_Surface)->GetHeight(GetPosition());
        //Er ballen i luften?
        if(GetPosition().y() > r.height + 0.2){
            //Bare tyngdekraften påvirker ballen
            SetPosition(GetPosition() + oldVel/60 + 1/2*gravity/60);
            oldVel = oldVel + gravity /60;
            oldPos = GetPosition();
            return;
        }
        //Bare tyngdekraften påvirker ballen
        SetPosition(GetPosition() + oldVel/60 + 1/2*gravity/60);
        oldVel = oldVel + gravity /60;
        oldPos = GetPosition();
        return;
        //Lagerer vertexene på triangelt
        v1 = &r.v1;
        v2 = &r.v2;
        v3 = &r.v3;
        //Nåværende posistjon
        QVector3D currPos = GetPosition();
        //Finner friksjonen på denne trekanten
        r.height;
        //Sjekk at vertex finnes
        if(v1 && v2 && v3){
            //Lager punkter fra vertexene
            QVector3D p1(v1->x, v1->y, v1->z);
            QVector3D p2(v2->x, v2->y, v2->z);
            QVector3D p3(v3->x, v3->y, v3->z);
            //Finner normalen
            QVector3D a = p2 - p1;
            QVector3D b = p3 - p1;
            QVector3D normal = QVector3D::crossProduct(a, b).normalized();

            long double t = 1/60;
            //Akselerasjon, Lignign 7
            QVector3D acc = -gravity.y() * QVector3D(normal.x() * normal.y(), normal.y() * normal.z(), pow(normal.y(), 2)-1);
            //R = r0 + v0t + 1/2at^2
            QVector3D nyPos = currPos + oldVel/60 + 1/2*acc/60;
            //Setter høyde + radius så den sitter på flaten
            nyPos.setY(r.height + radius);
            //Setter nye posisjonen
            SetPosition(nyPos);
            //Oppdater hastigheten
            oldVel = oldVel + acc /60;

            //SJekk om ballen er på ny trekant
            if(oldv1 && oldv2 && oldv3){
                if(normal != oldNormal){
                     //Ballen har rullet over på nytt triangel
                     //Beregner normalen til kolisjonsplanet, ligning 9
                     //Lage punkter fra de gamle vertexene
                     //Regne normalen mellom planene
                     float alpha = QVector3D::crossProduct(normal, oldNormal).length();
                     qDebug() << "alpha: " << alpha;
                     if(alpha <= 3.14f){
                         //Oppover møte
                     }else{
                         //Nedover møte
                     }
                     //Finner en vector mellom de to veggene
                     QVector3D x = (normal + oldNormal)/(normal+oldNormal).lengthSquared();
                     qDebug() << "x: " << x;
                     //Formel 8.8 skal finne den nye hastighet vektoren
                     QVector3D nyHastighet = oldVel - 2*(oldVel*x)*x;

                     qDebug() << "Ny hast:" << nyHastighet;
                     qDebug() << "Gammel hast: " << oldVel;
                     oldVel = nyHastighet;
                    //Finne om det er kollisjon
                     QVector3D r = GetPosition() - oldPos;
                     if(r.lengthSquared() <= radius){
                        //Har kollisjon
                        qDebug() << "kollisjojn";
                        //Sett ny posisjon over punktet

                     }else{
                        //Har ikke kollisjon
                        qDebug() << "ikke kollisjon";
                     }
                }
            }

            oldv1 = &r.v1;
            oldv2 = &r.v2;
            oldv3 = &r.v3;
            oldNormal = normal;
            oldPos = GetPosition();
        }

    }
}

void RollingBall::ResetPhysics()
{
    oldVel = QVector3D(0,0,0);
}

void RollingBall::AddLife()
{
    mLived += 1;
}

void RollingBall::EnableSpline()
{
    bDrawSpline = true;
}

void RollingBall::DisableSpline()
{
    bDrawSpline = false;
}

void RollingBall::CreateSplinePoint()
{
    QVector3D pos = GetPosition();
    Vertex* v = new Vertex(pos.x(), pos.y(), pos.z());

    mControlPoints.push_back(v);
}

void RollingBall::CreateSpline()
{
    float step = 0.1;
    float n = mControlPoints.size()-1;
    float d = 2;
    //Skjøter i hver ende
    float s = d + 1;
    float total = n + d + 1;
    //Lager skjøtevektoren
    std::vector<float> t;
    for(int i = 0; i < total-s; i++){
        if(i == 0){
            for(int k = 0 ; k < s ; k++){
                t.push_back(i);
            }
        }else
        if(i == total-s-1){
            for(int k = 0; k < s; k++){
                t.push_back(i);
            }
        }else{
            t.push_back(i);
        }
    }

    //Trenger basisfunksjoner??

    for(float time = 0; time < 1; time += step){
        QVector3D point = EvaluateBezier(time);
        mVisualPoints.push_back(new Vertex(point.x(), point.y(), point.z(), 1, 1,1));
    }

}

QVector3D RollingBall::EvaluateBezier(float t)
{

    return QVector3D{};
}

//int RollingBall::FindKnotInterval(float x){
//    int my = n-1;
//    while(x < t[my]){
//        my--;
//    }
//    return my;
//}


