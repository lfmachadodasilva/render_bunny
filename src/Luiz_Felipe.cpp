#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <locale.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "vector.h"
#include "mesh.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

VMesh::VMesh(const char* filename)
{

    printf("Carregando o arquivo %s\n", filename);
    
    FlagSmooth = false;//Ligar flag//
    flagNormal = false;

    printf("\tAbre arquivo\n");
    FILE * pFile = fopen(filename, "rt");//Abre arquivo//
    int num;

    printf("\tVerifica se abriu o arquivo\n");
    if (pFile == NULL)//verifica se abriu o arquivo//
    {
        printf("\t\tNao foi possivel abrir o arquivo %s.\n", filename);
        delete listaVertices;
        delete listaTriangulos;
        delete listaNormalFace;
        delete listaNormalvertice;
        return;
    }

    fscanf(pFile, "%d %d", &qtdVertices, &qtdTriangulos);

    printf("\tQuantidade de vertices: %d:\tQuantidade de triangulos: %d\n", qtdVertices, qtdTriangulos);

    printf("\tCriar listas\n");
    //Cria listas//

    listaVertices = new VVector[qtdVertices];

    listaTriangulos = new unsigned int[qtdTriangulos*3];

    listaNormalFace = new VVector[qtdTriangulos];

    listaNormalvertice = new VVector[qtdVertices];

    printf("\tBuscando coordenadas de cada vertice.\n");
    for(int i = 0; i < qtdVertices; i++)//Busca do arquivo as coordenadas de cada vertice//
    {
        fscanf(pFile, "%d %f %f %f", &num, &listaVertices[i].x, &listaVertices[i].y, &listaVertices[i].z);
        printf("%d - %f %f %f\n", num, listaVertices[i].x, listaVertices[i].y, listaVertices[i].z);
    }

    printf("\tBuscando indices de vertices de cada triangulo\n");
    for(int i = 0; i < qtdTriangulos*3; i+=3)//Busca do arquivo os vertices que compoe cada triangulo//
    {
        fscanf(pFile, "%d %d %d %d", &num, &listaTriangulos[i], &listaTriangulos[i+1], &listaTriangulos[i+2]);
        printf("%d - %d %d %d\n", num, listaTriangulos[i], listaTriangulos[i+1], listaTriangulos[i+2]);
    }

    printf("\tCalculando normais\n");
    CalculaNormal();

    printf("Carregado o arquivo %s\n", filename);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

VMesh::~VMesh(void)
{
    delete listaVertices;
    delete listaTriangulos;
    delete listaNormalFace;
    delete listaNormalvertice;
}

void VMesh::SetSmooth(bool smooth)
{
    this->FlagSmooth = smooth;
}

void VMesh::SetWeightedNormals(bool weighted)
{
    this->flagNormal = weighted;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void VMesh::GetBBox(float* xmin, float* xmax, float* ymin, float* ymax, float* zmin, float* zmax)
{
    if(listaVertices == NULL)
    {
        *xmin = -1; *xmax = 1;
        *ymin = -1; *ymax = 1;
        *zmin = -1; *zmax = 1;
        return;
    }
    *xmin = listaVertices[0].x; *xmax = listaVertices[0].x;
    *ymin = listaVertices[0].y; *ymax = listaVertices[0].y;
    *zmin = listaVertices[0].z; *zmax = listaVertices[0].z;
    for(int i = 1; i < qtdVertices; i++)
    {
        if(listaVertices[i].x < *xmin)
            *xmin = listaVertices[i].x;
        else if(listaVertices[i].x > *xmax)
            *xmax = listaVertices[i].x;

        if(listaVertices[i].y < *ymin)
            *ymin = listaVertices[i].y;
        else if(listaVertices[i].y > *ymax)
            *ymax = listaVertices[i].y;

        if(listaVertices[i].z < *zmin)
            *zmin = listaVertices[i].z;
        else if(listaVertices[i].z > *zmax)
            *zmax = listaVertices[i].z;
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void VMesh::Draw(void)
{
    if(!FlagSmooth)
    {
        glBegin(GL_TRIANGLES);
        for(int i = 0, j = 0; i < qtdTriangulos*3; i++, j++)
        {
            glNormal3f(listaNormalFace[j].x, listaNormalFace[j].y, listaNormalFace[j].z);
            glVertex3f(listaVertices[listaTriangulos[i]].x, listaVertices[listaTriangulos[i]].y, listaVertices[listaTriangulos[i]].z); i++;
            glVertex3f(listaVertices[listaTriangulos[i]].x, listaVertices[listaTriangulos[i]].y, listaVertices[listaTriangulos[i]].z); i++;
            glVertex3f(listaVertices[listaTriangulos[i]].x, listaVertices[listaTriangulos[i]].y, listaVertices[listaTriangulos[i]].z);
        }
        glEnd();
    }
    else
    {
        glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
        
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        {
            glNormalPointer(GL_FLOAT,    sizeof (VVector), listaNormalvertice);
            glVertexPointer(3, GL_FLOAT, sizeof (VVector), listaVertices);

            glDrawElements(GL_TRIANGLES, qtdTriangulos*3, GL_UNSIGNED_INT, listaTriangulos);
        }
        glPopClientAttrib();
    }
    glFlush();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void VMesh::CalculaNormal(void)
{
    VVector v0, v1;

    VVector p1, p2, p3;

    VVector normal;

    int qtd_cont = 0;

     for(int i = 0; i < qtdVertices; i++)
    {
        listaNormalvertice[i].x = 0; listaNormalvertice[i].z = 0; listaNormalvertice[i].z = 0;
    }

    printf("Calculando normal para cada face");
    for(int i = 0, j = 0; i < qtdTriangulos*3; i += 3, j++)//Calcula normal para cada triagunlo//
    {
        p1 = listaVertices[listaTriangulos[i]]; p2 = listaVertices[listaTriangulos[i+1]]; p3 = listaVertices[listaTriangulos[i+2]];

        //Calcula dois vetores a partir dos tres pontos//
        v0.Set(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z); v1.Set(p2.x - p3.x, p2.y - p3.y, p2.z - p3.z);//Coloca na estrutura do vector.h//

        normal = Cross(v0, v1);//Calcula normal//

        normal.Normalize();//Normaliza//

        listaNormalFace[j] = normal;
    }
    printf("Calculando normal para cada vertice");
    for(int i = 0; i < qtdVertices; i++)
    {
        for(int j = 0; j < qtdTriangulos*3; j++)
        {
            if(listaTriangulos[j] == i)
            {
                listaNormalvertice[i] += listaNormalFace[j/3];
                //printf("%d -\t", qtd_cont++);
            }
        }
        listaNormalvertice[i].Normalize();
    }
}