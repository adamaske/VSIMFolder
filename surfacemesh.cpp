#include "surfacemesh.h"
#include "vertex.h"
SurfaceMesh::SurfaceMesh(Shader* s) : VisualObject(s)
{
    //Vertex'er
    Vertex v1(-2.4f, 0.45f, 2.f,    1.f, 0.3f,0.5f);
    Vertex v2(0.f, 0.f, 2.f,        0.3f, 1.f,0.2f);
    Vertex v3(-2.5f, 0.f, -2.f,     0.5f, 0.6f,1.f);
    Vertex v4(0.f, .5f, -2.f,       1.f, 0.4f,0.2f);
    Vertex v5(3.0f, 0.1f, 1.925f,   0.3f, 1.f,0.3f);
    Vertex v6(3.0f, 0.f, -1.9f,     0.1f, 0.2f,2.f);

    //1
    mVertices.push_back(v1);
    mVertices.push_back(v2);
    mVertices.push_back(v3);
    mVertices.push_back(v4);
    mVertices.push_back(v5);
    mVertices.push_back(v6);

    mIndices.push_back(0);
    mIndices.push_back(1);
    mIndices.push_back(2);

    mIndices.push_back(1);
    mIndices.push_back(3);
    mIndices.push_back(2);

    mIndices.push_back(1);
    mIndices.push_back(5);
    mIndices.push_back(3);

    mIndices.push_back(1);
    mIndices.push_back(4);
    mIndices.push_back(5);


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
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, reinterpret_cast<const void*>(0));
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




