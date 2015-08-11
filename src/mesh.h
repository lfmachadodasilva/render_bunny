
#include "vector.h"

#ifndef _MESH_H
#define	_MESH_H

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

class VMesh
{
private:
    bool FlagSmooth;//Flag de suavizacao//

    bool flagNormal;//Flag da normal//

    unsigned int qtdVertices;//Quantidade de vertices//

    unsigned int qtdTriangulos;//Quantidade de triangulos//

    VVector * listaVertices;//Lista de vertices//

    unsigned int * listaTriangulos;//Lista de triangulos//

    VVector * listaNormalFace;//Lista de normais de cada triangulo//

    VVector * listaNormalvertice;//Lista de normais de cada vertice//

    void CalculaNormal(void);//Calcula normais//

public:

  // Carrega malha do arquivo especificado

  VMesh (const char* filename);

  // Destroi malha

  virtual ~VMesh ();



  void SetSmooth (bool smooth);

  void SetWeightedNormals(bool weighted);

  // Retorna caixa envolvente do modelo

  void GetBBox (float* xmin, float* xmax,

                float* ymin, float* ymax,

                float* zmin, float* zmax);

  // Desenha modelo, suavizado ou facetado, dependendo do flag de suavizacao

  virtual void Draw ();

};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#endif	/* _MESH_H */

