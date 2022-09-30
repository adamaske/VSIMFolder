#include "surfacemesh.h"
#include "vertex.h"
#include <iostream>
#include <string>
SurfaceMesh::SurfaceMesh(Shader* s) : VisualObject(s)
{
    //K0de for å merge alle tekstfilene tatt fra las2txt
    //std::string line;
    //std::ofstream file("../VSIMFolder/HeightData/fullData.txt", std::ios::trunc);
    //
    //std::ifstream file1("../VSIMFolder/HeightData/32-1-508-114-75.txt");
    //std::ifstream file2("../VSIMFolder/HeightData/32-1-508-114-76.txt");
    //std::ifstream file3("../VSIMFolder/HeightData/32-1-509-114-05.txt");
    //std::ifstream file4("../VSIMFolder/HeightData/32-1-509-114-06.txt");
    //std::ifstream file5("../VSIMFolder/HeightData/32-1-509-114-07.txt");
    //
    ////The file to be written in is open
    //if (file.is_open()) { 
    //    while (std::getline(file1, line)) {
    //        file << line << '\n';
    //    };
    //    while (std::getline(file2, line)) {
    //        file << line << '\n';
    //    };
    //    while (std::getline(file3, line)) {
    //        file << line << '\n';
    //    };
    //    while (std::getline(file4, line)) {
    //        file << line << '\n';
    //    };
    //    while (std::getline(file5, line)) {
    //        file << line << '\n';
    //    };
    //    file1.close();
    //    file2.close();
    //    file3.close();
    //    file4.close();
    //    file5.close();
    //}
    //file.close();
    //Can now read file to go trough
    std::ifstream file("../VSIMFolder/HeightData/fullData.txt");
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
                    //number.erase(number.end()-2);
                    points.push_back(std::stod(number));
                    lastSpace = i;
                }//Gå fra forrige space til siste, for det siste tallet
                else if(i == line.size() -1){
                    number = line.substr(lastSpace, i);
                    //number.erase(number.end()-2);
                    points.push_back((std::stof(number)));
                }
            }           
        }
    }
    file.close();

    //Numerne er ganske ville så gå gjennom, så jeg går gjennom for å endre de
    for (int i = 0; i < points.size(); i++) {
        if (points[i] >557000 && points[i] < 558000) {
            points[i] -= 557250;
        }
        else if (points[i] > 6550000 && points[i] < 6560000) {
            points[i] -= 6550700;
        }
    }

    //Swapped y and z axis
    for (int i = 0; i < points.size(); i+=3) {
        //qDebug() << points[i] << " " << points[i +1] << " " << points[i+2];
        mVertices.push_back(Vertex(points[i], points[i + 2], points[i + 1], ((rand() % 10) / 10), ((rand() % 10) / 10), ((rand() % 10) / 10)));
    }

    for (int i = 0; i < 50; i+=3) {
        qDebug() << points[i] << " " << points[i +2] << " " << points[i+1];
    }

    //for(unsigned int i = 0; i < 10000; i++)       // for each row a.k.a. each strip
    //{
    //    for(unsigned int j = 0; j < 10000; j++)      // for each column
    //    {
    //        for(unsigned int k = 0; k < 2; k++)      // for each side of the strip
    //        {
    //            mIndices.push_back(j + 10000 * (i + k));
    //        }
    //    }
    //}
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
void SurfaceMesh::draw()
{
    initializeOpenGLFunctions();
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEAB);

    glUniformMatrix4fv(mMatrixUniform, 1, GL_FALSE, mMatrix.constData());
    glPointSize(2.0f);
    glDrawArrays(GL_POINTS, 0, mVertices.size());
    //glDrawArrays(GL_TRIANGLES, 0, mVertices.size());//mVertices.size());
    //glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, reinterpret_cast<const void*>(0));
}

Result SurfaceMesh::GetHeight(QVector3D pos)
{
    Result r;
    float a = 0, b = 0, c = 0, height = 0;
    //Sjekker for trekant 1
    QVector3D bary =  GetBarycentric(pos, mVertices[0], mVertices[1], mVertices[2]);
    if(((0 <= bary.x()) && (bary.x() <= 1)) && ((0 <= bary.y()) && (bary.y() <= 1)) && ((0 <= bary.z()) && (bary.z() <= 1))){
        //Er denne trekanten
        a = mVertices[0].y * bary.x();
        b = mVertices[1].y * bary.y();
        c = mVertices[2].y * bary.z();
        height = a + b + c;
        r.height = height;
        r.v1 = mVertices[0];
        r.v2 = mVertices[1];
        r.v3 = mVertices[2];
        qDebug() << "Returned triangle 1";
        r.friction = 0.2;
        return r;
    }
    bary = GetBarycentric(pos, mVertices[1], mVertices[3], mVertices[2]);
    if(((0 <= bary.x()) && (bary.x() <= 1)) && ((0 <= bary.y()) && (bary.y() <= 1)) && ((0 <= bary.z()) && (bary.z() <= 1))){
        //Er denne trekanten
        a = mVertices[1].y * bary.x();
        b = mVertices[3].y * bary.y();
        c = mVertices[2].y * bary.z();
        height = a + b + c;
        r.height = height;
        r.v1 = mVertices[1];
        r.v2 = mVertices[3];
        r.v3 = mVertices[2];
        qDebug() << "Returned triangle 2";
        r.friction = 0.2;
        return r;
    }
    bary = GetBarycentric(pos, mVertices[1], mVertices[5], mVertices[3]);
    if(((0 <= bary.x()) && (bary.x() <= 1)) && ((0 <= bary.y()) && (bary.y() <= 1)) && ((0 <= bary.z()) && (bary.z() <= 1))){
        //Er denne trekanten
        a = mVertices[1].y * bary.x();
        b = mVertices[5].y * bary.y();
        c = mVertices[3].y * bary.z();
        height = a + b + c;
        r.height = height;
        r.v1 = mVertices[1];
        r.v2 = mVertices[5];
        r.v3 = mVertices[3];
        qDebug() << "Returned triangle 3";
        r.friction = 0.4;
        return r;
    }
    bary = GetBarycentric(pos, mVertices[1], mVertices[4], mVertices[5]);
    if(((0 <= bary.x()) && (bary.x() <= 1)) && ((0 <= bary.y()) && (bary.y() <= 1)) && ((0 <= bary.z()) && (bary.z() <= 1))){
        //Er denne trekanten
        a = mVertices[1].y * bary.x();
        b = mVertices[4].y * bary.y();
        c = mVertices[5].y * bary.z();
        height = a + b + c;
        r.height = height;
        r.v1 = mVertices[1];
        r.v2 = mVertices[4];
        r.v3 = mVertices[5];
        qDebug() << "Returned triangle 4";
        r.friction = 0.2;
        return r;
    }
    return r;
}

QVector3D SurfaceMesh::GetBarycentric(QVector3D point, Vertex v1,  Vertex v2, Vertex v3){
    double u = 0, v = 0, w = 0;
    QVector3D a(v1.x,v1.y,v1.z);
    QVector3D b(v2.x,v2.y,v2.z);
    QVector3D c(v3.x,v3.y,v3.z);
    //Finner x
    QVector3D x1 = b - a;
    QVector3D x2 = c - a;
    x1.setY(0);
    x2.setY(0);
    QVector3D normal = QVector3D::crossProduct(x1, x2);
    float x = normal.length();
    //Regner kryssprodukt av p og q, og deler på x
    //Bruker a - point, b - point og c - point og kryssproduktet av kombinasjonen av de
    QVector3D p = b - point;
    QVector3D q = c - point;
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




