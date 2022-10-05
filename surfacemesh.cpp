#include "surfacemesh.h"
#include "vertex.h"
#include <iostream>
#include <string>
SurfaceMesh::SurfaceMesh(Shader* s) : VisualObject(s)
{
    std::ifstream file("../VSIMFolder/HeightData/fullData.txt");
    //Lagre alle puntkene som blir lest
    std::vector<float> points;
    if (file.is_open()) {
        //Lagre linjen den er på
        std::string line;
        //For å bruke substr så lagrer jeg sist space plass
        int lastSpace = 0;
        //lagrer det i egen string for å gjøre det om til float etterpå
        std::string number;
        while (std::getline(file, line)) {
            lastSpace = 0;
            for (int i = 0; i < line.size(); i++) {
                if (line[i] == ' ') {                               
                    number = line.substr(lastSpace, i-1);
                    points.push_back(std::stod(number));
                    lastSpace = i;
                }//Gå fra forrige space til siste, for det siste tallet
                else if(i == line.size()-1){
                    number = line.substr(lastSpace, i);
                    points.push_back((std::stof(number)));
                }
            }           
        }
    }
    file.close();



    //Numerne er ganske ville så gå gjennom, så jeg går gjennom for å endre de
    //for (int i = 0; i < points.size(); i++) {
    //    if (points[i] >557000 && points[i] < 558000) {
    //        points[i] -= 557060;
    //    }
    //    else if (points[i] > 6550000 && points[i] < 6560000) {
    //        points[i] -= 6550292;
    //    }
    //}
    for (int i = 0; i < points.size(); i++) {
        if (points[i] >557000 && points[i] < 558000) {
            points[i] /= 100000;
        }
        else if (points[i] > 6550000 && points[i] < 6560000) {
            points[i] /= 1000000;
        }
        points[i] = trunc(points[i]);
    }

    if(true){
        for(int i= 0; i < points.size(); i+=3){
            mVertices.push_back(Vertex(points[i], points[i+2], points[+1], 1,1,1));
        }
        return;
    }
    qDebug() << "Amount of points : " << points.size();
    if(true){
        float highestX;
        float  lowestX;
        float highestY;
        float  lowestY;
        float highestZ;
        float  lowestZ;
        for(int i = 0; i < points.size(); i+=3){
            if(points[i] > highestX){
                highestX = points[i];
            }
            if(points[i] < lowestX){
                lowestX = points[i];
            }
            if(points[i+1] > highestY){
                highestY = points[i+1];
            }
            if(points[i+1] < lowestY){
                lowestY = points[i+1];
            }
            if(points[i+2] > highestZ){
                highestZ = points[i+2];
            }
            if(points[i+2] < lowestZ){
                lowestZ = points[i+2];
            }
        }
        qDebug() << "Highest X: " << highestX;
        qDebug() << "Lowest  X: " <<  lowestX;
        qDebug() << "Highest Y: " << highestY;
        qDebug() << "Lowest  Y: " <<  lowestY;
        qDebug() << "Highest Z: " << highestZ;
        qDebug() << "Lowest  Z: " <<  lowestZ;
    }

    //Lager convex hull
    std::vector<Quad> mQuads;
    for (double i = 0; i < height; i+=res) {
        for (double j = 0; j < width; j+=res) {
            //Top right, top left, bottom right, bottom left
            mQuads.push_back(Quad{ Point{j, i}, Point{j + res, i}, Point{j, i + res}, Point{j + res, i + res} });
        }
    }

    //Finn hvem quad hvert punkt er i, høyden til quaden blir dratt 0.1 i retningen av punktet
    float index = 0;
    QVector3D pos;
    for(int i = 0; i < points.size(); i+=3){
        pos = {points[i], points[i+1], points[i+2]};
        //Incase the points are outside the width of convex hull
        if(pos.x() > width || pos.y() > height || pos.x() < 0 || pos.y() < 0 || pos.z() <0){

        }else{
            //pos /= res;
            index = (pos.x()/res) + (((width/res)-1) * pos.y()/res);
            index = trunc(index);
            //qDebug() << " Found index " << index << " for point " << pos;
            if(index < mQuads.size()){
                mQuads[index].AddHeight(pos.z());
            }
        }

    }

    //Lager vertexer
    for (int i = 0; i < mQuads.size(); i++) {
        //Lager vertexer
        mVertices.push_back(Vertex(mQuads[i].GetCenter().x, mQuads[i].GetHeight(), mQuads[i].GetCenter().y, sin(i), cos(i), tan(i)));

    }

    //Indeksering
    for(int j = 0; j < (height/res)-1; j++){
        for(int i = 0; i < (width/res)-1; i++){
            for(int k = 0; k < 2; k++){
                if(k == 0){
                    mIndices.push_back(i+((width/res)*j));
                    mIndices.push_back(i+((width/res)*j)+1);
                    mIndices.push_back(i+((width/res)*j)+width/res);
                }else{
                    mIndices.push_back(i+((width/res)*j)+1);
                    mIndices.push_back(i+((width/res)*j)+width/res);
                    mIndices.push_back(i+((width/res)*j)+width/res+1);
                }
            }
        }
    }


    mQuads.clear();
}
void SurfaceMesh::init()
{
    initializeOpenGLFunctions();

    //Vertex Array Object - VAO
    glGenVertexArrays( 1, &mVAO );
    glBindVertexArray( mVAO );

    //Vertex Buffer Object to hold vertices - VBO
    glGenBuffers( 1, &mVBO );
    glBindBuffer( GL_ARRAY_BUFFER, mVBO );
    glBufferData( GL_ARRAY_BUFFER, mVertices.size()*sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW );

    // 1rst attribute buffer : vertices
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE,sizeof(Vertex), reinterpret_cast<const void*>(0));
    glEnableVertexAttribArray(0);

    // 2nd attribute buffer : normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex),  reinterpret_cast<const void*>(3 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(1);

    // 3rd attribute buffer : UV.
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,  sizeof(Vertex),  reinterpret_cast<const void*>(6 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(2);

    //enable the matrixUniform
    // mMatrixUniform = glGetUniformLocation( matrixUniform, "matrix" );
    glGenBuffers(1, &mEAB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEAB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size()*sizeof(GLuint), mIndices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

}

void SurfaceMesh::SetDrawMode(DrawMode m)
{
    mDrawMode = m;
}
void SurfaceMesh::draw()
{
    initializeOpenGLFunctions();
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEAB);

    glUniformMatrix4fv(mMatrixUniform, 1, GL_FALSE, mMatrix.constData());

    switch(mDrawMode){
        case points:
        glPointSize(2.0f);
        glDrawArrays(GL_POINTS, 0, mVertices.size());
        break;
    case drawElements:
        glDrawArrays(GL_TRIANGLES, 0, mVertices.size());//mVertices.size());
        break;
    case arrays:
        glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, reinterpret_cast<const void*>(0));
        break;
    }
}

Result SurfaceMesh::GetHeight(QVector3D pos)
{
    Result r;
    r.height =0;
    float a = 0, b = 0, c = 0, height = 0;
    float x = (pos.x())/res;
    float z = (pos.z())/res;
    //qDebug() << "Getting barycetric for point " << pos << ", x and z is " << x << " , " << z;
    //qDebug() << "Index is therefore " << x+(((width)/res)*z) << " for point " << pos;
    //First vertex of the triangle

    v1 = mVertices[trunc(x+(((width/res)-1)*z)                  )];
    //Above
    v2 = mVertices[trunc(x+(((width/res)-1)*z)-(width/res)-1    )];
    //To the right
    v3 = mVertices[trunc(x+(((width/res)-1)*z)+1                )];
    //Under the rigtrunc(ht
    v4 = mVertices[trunc(x+(((width/res)-1)*z)+(width/res)-1+1  )];
    //Under v1
    v5 = mVertices[trunc(x+(((width/res)-1)*z)+(width/res)-1    )];
    //Left of v1
    v6 = mVertices[trunc(x+(((width/res)-1)*z)-1                )];

    //Sjekker for hver trekant som er inni quadden
     QVector3D bary;
    for(int i = 0; i < 5; i++){
        if(i == 0){
            //Første
            inUseVertex1 = v1;
            inUseVertex2 = v2;
            inUseVertex3 = v3;
        }else if(i == 1){
            inUseVertex1 = v1;
            inUseVertex2 = v3;
            inUseVertex3 = v4;
        }else if(i == 2){
            inUseVertex1 = v1;
            inUseVertex2 = v3;
            inUseVertex3 = v4;
        }else if(i == 3){
            inUseVertex1 = v1;
            inUseVertex2 = v5;
            inUseVertex3 = v6;
        }else if(i == 4){
            inUseVertex1 = v1;
            inUseVertex2 = v2;
            inUseVertex3 = v6;
        }
        bary =  GetBarycentric(pos, inUseVertex1, inUseVertex2, inUseVertex3);
        if(((0 <= bary.x()) && (bary.x() <= 1)) && ((0 <= bary.y()) && (bary.y() <= 1)) && ((0 <= bary.z()) && (bary.z() <= 1))){
            //Er denne trekanten
            a = inUseVertex1.y * bary.x();
            b = inUseVertex2.y * bary.y();
            c = inUseVertex3.y * bary.z();
            height = a + b + c;
            r.height = height;
            r.v1 = inUseVertex1;
            r.v2 = inUseVertex2;
            r.v3 = inUseVertex3;
            //qDebug() << "Returned triangle 1 " << r.height;
            //qDebug() << bary;
            r.friction = 0.2;
            return r;
        }
        qDebug() << bary;
    }

    return r;
}

QVector3D SurfaceMesh::GetBarycentric(QVector3D point, Vertex v1,  Vertex v2, Vertex v3){
    double u = 0, v = 0, w = 0;
    a = QVector3D(v1.x,v1.y,v1.z);
    b = QVector3D(v2.x,v2.y,v2.z);
    c = QVector3D(v3.x,v3.y,v3.z);
    //Finner x
    x1 = b - a;
    x2 = c - a;
    x1.setY(0);
    x2.setY(0);
    normal = QVector3D::crossProduct(x1, x2);
    float x = normal.length();
    //Regner kryssprodukt av p og q, og deler på x
    //Bruker a - point, b - point og c - point og kryssproduktet av kombinasjonen av de
    p = b - point;
    q = c - point;
    p.setY(0);
    q.setY(0);
    //Kryss produkt for u, delt på x
    normal = QVector3D::crossProduct(p,q);
    u = normal.y() / x;

    p = c - point;
    q = a - point;
    p.setY(0);
    q.setY(0);
    //Kryss produkt for v, delt på x
    normal = QVector3D::crossProduct(p, q);
    v = normal.y() / x;

    p = a - point;
    q = b - point;
    p.setY(0);
    q.setY(0);
    //Kryssprodukt for w, delt på x
    normal = QVector3D::crossProduct(p, q);
    w = normal.y() / x;
    //qDebug() << u << v << w;
    //Finn om punkten er innenfor eller utenfor triangelt
    return QVector3D(u, v, w);

}





