#include "rollingball.h"
#include "surfacemesh.h"
#include "visualpoint.h"
#include "shader.h"
RollingBall::RollingBall(std::string fileName, Shader* shader) : ObjMesh(fileName, shader)
{
    //oldPos = GetPosition();
}

void RollingBall::SetSurface(VisualObject* surface)
{
    m_Surface = surface;
    mSurfaceMesh = dynamic_cast<SurfaceMesh*>(m_Surface);
}

void RollingBall::draw()
{
    //use my shader
    glUseProgram(mShader->getProgram());
    //Send my model matrix
    mShader->SetUniformMatrix4fv(mMatrix, "mMatrix");
    //Draw object
    glBindVertexArray( mVAO);
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
    if(bDrawSpline && mSpline){
        mSpline->draw();
        if(mControlPointVisual){
           mControlPointVisual->draw();
        }
    }

}

void RollingBall::DoPhysics()
{
    QVector3D gravity(0, -9.81f, 0);
    radius = GetScale().x()/2;
    if(mSurfaceMesh){
        //Får resultatet fra surfacemesh
        Result r = mSurfaceMesh->GetHeight(GetPosition());
        //Er ballen i luften? Bare rull på overflaten så lenge ballen er minst x nære overflaten
        if(GetPosition().y() > r.height + 5){
            //Bare tyngdekraften påvirker ballen
            SetPosition(GetPosition() + oldVel/60 + 1/2*gravity/60);
            oldVel = oldVel + gravity/60;
            oldPos = GetPosition();
            return;
        }
        //Lagerer vertexene på triangelt
        v1 = &r.v1;
        v2 = &r.v2;
        v3 = &r.v3;
        //Nåværende posistjon
        QVector3D currPos = GetPosition();
        //Sjekk at vertex finnes
        if(v1 && v2 && v3){
            //Lager punkter fra vertexene
            p1 = QVector3D(v1->x, v1->y, v1->z);
            p2 = QVector3D(v2->x, v2->y, v2->z);
            p3 = QVector3D(v3->x, v3->y, v3->z);
            //Finner normalen
            a = p2 - p1;
            b = p3 - p1;
            normal = QVector3D::crossProduct(a, b).normalized();
            //qDebug() << "Normal : " << normal;
            //Akselerasjon, Lignign 7
            QVector3D acc = -gravity.y() * QVector3D(normal.x() * normal.y(), normal.y() * normal.z(), pow(normal.y(), 2)-1);
            //R = r0 + v0t + 1/2at^2, delt på 60 for dt = 1/60, 60 fps
            QVector3D nyPos = currPos + oldVel/60 + 1/2*acc/60;
            //Setter høyde + radius så den sitter på flaten
            nyPos.setY(r.height + radius);
            //Setter nye posisjonen
            SetPosition(nyPos);
            //Oppdater hastigheten
            oldVel = oldVel + acc /60;

            //SJekk om ballen er på ny trekant
            if(oldv1 && oldv2 && oldv3 && oldv1 != v1 &&oldv2 != v2 &&oldv3 != v3){
                if(normal != oldNormal){
                     //Ballen har rullet over på nytt triangel
                     //Beregner normalen til kolisjonsplanet, ligning 9
                     //Lage punkter fra de gamle vertexene
                     //Regne normalen mellom planene    
                     //Finner en vector mellom de to veggene
                     QVector3D x = (normal + oldNormal)/((normal+oldNormal).lengthSquared());
                     //Formel 8.8 skal finne den nye hastighet vektoren
                     QVector3D nyHastighet = oldVel - 2*(oldVel*x)*x;     
                     oldVel = nyHastighet;
                }
            }
            oldv1 = &r.v1;
            oldv2 = &r.v2;
            oldv3 = &r.v3;
            oldNormal = normal;
            oldPos = GetPosition();
        }

    }else{
        //Bare tyngdekraften påvirker ballen
        SetPosition(GetPosition() + oldVel/60 + 1/2*gravity/60 + QVector3D(sin(mLived), 0 , sin(mLived)));
        oldVel = oldVel + gravity /60;
        oldPos = GetPosition();
        return;
    }
}

void RollingBall::ResetPhysics()
{
    oldVel = QVector3D(0,0,0);
}

void RollingBall::AddLife()
{
    mLived += 1;

    if(mLived % mSplinePointCounter == 0){
        CreateSplinePoint();
    }
}

void RollingBall::EnableSpline()
{
    if(!bDrawSpline){
        CreateSpline();
         bDrawSpline = true;
    }
}

void RollingBall::DisableSpline()
{
    bDrawSpline = false;
}

void RollingBall::CreateSplinePoint()
{
    QVector3D pos = GetPosition();
    Vertex* v = new Vertex(pos.x(), pos.y(), pos.z());
    qDebug() << "Creating control point at " << pos;
    qDebug() << "Vertexen er  på : " <<  QVector3D(v->x, v->y, v->z);
    mControlPoints.push_back(*v);
}

void RollingBall::CreateSpline()
{
    t.clear();
    float step = 0.01;
    n = mControlPoints.size();
    d = 2; //for kvadratisk
    //Skjøter i hver ende
    float s = d + 1; //d + 1 like skjøter i hver ende
    float total = n + d + 1;
    //Lager skjøtevektoren
    for(int i = 0; i < total; i++){
        if(i == 0){
            for(int k = 0 ; k < s ; k++){
                t.push_back(i);

            }
        }else
        if(i == total-s-1){
            for(int k = 0; k < s; k++){
                t.push_back(i);
            }
            break;
        }else{
            t.push_back(i);
        }
    }
    for(int i = 0; i < t.size(); i++){
        qDebug() << t[i];
    }
    //Lager C, y kordinatene, z i mitt prosjekt
    for(int i = 0; i < mControlPoints.size(); i++){
        c.push_back(QVector3D(mControlPoints[i].x, mControlPoints[i].y,mControlPoints[i].z));
    }
    //Trenger basisfunksjoner??
    std::vector<Vertex> mVisualPoints;
    for(float time = 0; time <= 1; time += step){
        QVector3D point = EvaluateBezier(time);
        qDebug() << "Bezier function returned : " << point << " for time " << time;

        mVisualPoints.push_back(Vertex(point.x(), point.y(), point.z(), sin(time), tan(time),cos(time)));
    }
    mSpline = new VisualPoint(mVisualPoints);
    mControlPointVisual = new VisualPoint(mControlPoints);
    mControlPointVisual->init();
    mSpline->init();
    bDrawSpline = true;
}

QVector3D RollingBall::EvaluateBezier(float x)
{
    //B_i,_d(x) = w_i,_d(x)*B_i,_d-1(x)+(1-w_i+1,d(x))*B_i+1,_d-1(x)
    //hvor w_i,_d = (x-t)/(t_i+d - t_i) dersom t_i < t_i+d, 0 ellers
    //Og B_i,0 = 1 dersom t_i <= t_i+1
    //C_i = y kordinat til points[i]
    //f(x) = for(i = 0; i < n-1; i++){ c_i * B_i,_d(x)}

    int my = FindKnotInterval(x) ;
    std::vector<QVector3D> a ;
    a.reserve(d+1);
    for( int j =0; j<=d ; j++) {
        a[d-j]= c[my-j];
    }
    for(int k=d ; k>0; k--) {
        int j = my-k;
        for ( int i =0; i<k ; i++) {
            j ++;
            float w = (x-t[j] ) / ( t[j+k]-t[j] ) ;
            a[i] = a[i] * (1-w) + a [i +1] * w;
        }
    }
    return a[0];
}

//Dette er en clamped curve så denne funksjonen holder
int RollingBall::FindKnotInterval(float x){
    int my = n;
    while(x < t[my]){
        my--;
    }
    return my;
}


