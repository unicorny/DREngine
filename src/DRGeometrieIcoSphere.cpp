#include "Engine2Main.h"

float DRGeometrieIcoSphere::mVektorLength = 0.0f;

DRGeometrieIcoSphere::DRGeometrieIcoSphere(GLuint maxFaceBuffer)
: DRGeometrieSphere(), mMaxEbene(0), mMaxFaceBuffer(maxFaceBuffer),mVertexCursor(0), mEbeneNeighborCount(0), mFacesSphereCount(0)
{
    
}

DRGeometrieIcoSphere::~DRGeometrieIcoSphere()
{
    reset();
    for(std::list<IcoSphereFace*>::iterator it = mFreeIcoFaceMemory.begin(); it != mFreeIcoFaceMemory.end(); it++)
    {
        DR_SAVE_DELETE(*it);
    }
    mFreeIcoFaceMemory.clear();
}

DRReturn DRGeometrieIcoSphere::initIcoSphere(u8 maxEbene, int seed /*= 0*/)
{
    reset();
    float tao = 1.61803399f;
    mMaxEbene = maxEbene;
    if(maxEbene > 8)
        LOG_ERROR("a value greater than 8 for maxEbene required more then 1 GB RAM (need 1,6 GB)", DR_ERROR);
    
    GLuint indices[] =     {1,4,0,   4,9,0,    4,5,9,    8,5,4,    1,8,4,
                            1,10,8,  10,3,8,   8,3,5,    3,2,5,    3,7,2,
                            3,10,7,  10,6,7,   6,11,7,   6,0,11,   6,1,0,
                            10,1,6,  11,0,9,   2,11,9,   5,2,9,    11,2,7};  
    
    GLuint neighbors[] =   {4,1,14,  2,16,0,   3,18,1,   7,2,4,    5,3,0,
                            15,6,4,  10,7,5,   6,8,3,    9,18,7,   10,19,8,
                            6,11,9,  15,12,10, 13,19,11, 14,16,12, 15,0,13,
                            5,14,11, 13,1,17,  19,16,18, 8,17,2,   17,9,12}; 
    
    const GLuint indexCount = 60;
    
    if(init(12*(uint)powf(4.0f, (float)maxEbene), indexCount*(uint)powf(4.0f, (float)maxEbene), 0, true))
        LOG_ERROR("Fehler bei init Geometrie", DR_ERROR);
    if(initVertexBuffer())
        LOG_ERROR("Fehler bei init VertexBuffer", DR_ERROR);
    //memcpy(mIndices, indices, sizeof(GLuint)*indexCount);
            
    mVertices[mVertexCursor++] = DRVector3(-1.0f, 0.0f, tao);
    mVertices[mVertexCursor++] = DRVector3(1.0f, 0.0f, tao);
    mVertices[mVertexCursor++] = DRVector3(-1.0f, 0.0f, -tao);
    mVertices[mVertexCursor++] = DRVector3(1.0f, 0.0f, -tao);
    
    mVertices[mVertexCursor++] = DRVector3(0.0f, tao, 1.0f);
    mVertices[mVertexCursor++] = DRVector3(0.0f, tao, -1.0f);
    mVertices[mVertexCursor++] = DRVector3(0.0f, -tao, 1.0f);
    mVertices[mVertexCursor++] = DRVector3(0.0f, -tao, -1.0f);
    
    mVertices[mVertexCursor++] = DRVector3(tao, 1.0f, 0.0f);
    mVertices[mVertexCursor++] = DRVector3(-tao, 1.0f, 0.0f);
    mVertices[mVertexCursor++] = DRVector3(tao, -1.0f, 0.0f);
    mVertices[mVertexCursor++] = DRVector3(-tao, -1.0f, 0.0f);
    
    mVektorLength = mVertices[mVertexCursor-1].length();
    DRLog.writeToLog("VektorLength: %f", mVektorLength);
    
    //einfärben der Start Vertices
    for(uint i = 0; i < mVertexCursor; i++)
    {
        float percent = (float)i*(1.0f/12.0f);
        printf("percent: %f\n", percent);
        mColors[i] = DRColor(percent, 0.0f, 0.0f);//1.0f-percent, (percent+0.0001f)/2.0f);
    }
    srand(seed);
    // Initial Faces erstellen
    for(int iFace = 0; iFace < 20; iFace++)
    {
        //mRootSphereFaces[i]
        for(int i = 0; i < 3; i++)
        {
            mRootSphereFaces[iFace].mIndices[i] = indices[iFace*3+i];
            mRootSphereFaces[iFace].mNeighbors[i] = &mRootSphereFaces[neighbors[iFace*3+i]];
        }
        mRootSphereFaces[iFace].mSeed = rand();
    }
	for(u8 i = 0; i < maxEbene; i++)
	{
		subdivide();    
		printf("sub %d ende\n", i);
	}

    
    mRenderMode = GL_TRIANGLES;
    copyDataToVertexBuffer();
    
    grabIndicesFromFaces();
    updateIndexDataIntoVertexBuffer();
    
    //IcoSphereFace Konstruktor Test (nicht notwendig)
    IcoSphereFace f;
    if(f.mParent || f.mNeighbors[2] || f.mChilds[3] || f.mIndices[2])
        LOG_ERROR("IcoSphereFace Konstruktor Error", DR_ERROR);
            
    return DR_OK;
}

void DRGeometrieIcoSphere::reset()
{
     for(int i = 0; i < 20; i++)
         mRootSphereFaces[i].reset();
}

void DRGeometrieIcoSphere::subdivide(DRGeometrieIcoSphere::IcoSphereFace* current/* = NULL*/)
{
    if(!current)   
    {
        mEbeneNeighborCount = 0;
        for(int i = 0; i < 20; i++)
        {
            subdivide(&mRootSphereFaces[i]);
        }
        DRLog.writeToLog("added %d Neighbors, %d faces used (summe) (%f MByte)", mEbeneNeighborCount, mFacesSphereCount, (float)(mFacesSphereCount*sizeof(IcoSphereFace))/1024.0f/1024.0f);
    }
    else
    {
        srand(current->mSeed);
        for(int i = 0; i < 4; i++)
        {
            int seed = rand();
            if(current->mChilds[i])
            {
                subdivide(current->mChilds[i]);
            }
            else
            {
                current->mChilds[i] = newChildFace(current, i, seed);            
                mFacesSphereCount++;
            }
        }
    }
}

DRGeometrieIcoSphere::IcoSphereFace* DRGeometrieIcoSphere::newChildFace(DRGeometrieIcoSphere::IcoSphereFace* parent, int childCount, int seed)
{
    //printf("new child: %d\n", childCount);
    IcoSphereFace* newFace = this->newFace();
    newFace->mSeed = seed;
    if(!newFace) LOG_ERROR("no memory for new Face available", NULL);
    if(!parent) LOG_ERROR("no parent given", NULL);
    newFace->mParent = parent;
    
    DRVector3* parentPoints[3];
    for(int i = 0; i < 3; i++)
        parentPoints[i] = &mVertices[parent->mIndices[i]];   
    
    //child 0 berechnet und erstellt die 3 neuen Punkte
    if(childCount == 0)
    {
        if(mVertexCursor + 3 > mVertexCount)
        {
            this->deleteFace(newFace);
            LOG_ERROR("no memory left for new vertices", NULL);
        }
        // i1 is index for newVertex
        for(int i1 = 0; i1 < 3; i1++)
        {
            IcoSphereFace* neighborChild = NULL;
            // i2 is the index for the second vertex which will used to calculate a new vertex
            // i1 = i2: 0 = 1, 1 = 2, 2 = 0          
            int i2 = i1+1;
            if(i1 == 2) i2 = 0;
          //  DRLog.writeToLog("i1: %d, i2: %d", i1, i2);
            DRVector3 temp = (*parentPoints[i2] - *parentPoints[i1])/2.0f + *parentPoints[i1];
            if(parent->mNeighbors[i1]) neighborChild = parent->mNeighbors[i1]->mChilds[0];
            
            if(neighborChild)
            {
                float diffs[4];
                diffs[3] = 10000.0f;
                // i is the index for the indices from parent
                for(int i = 0; i < 3; i++)
                {
                    diffs[i] = DRVector3(mVertices[neighborChild->mIndices[i]]-temp).lengthSq();
                    if(diffs[i] < diffs[3])
                    {
                        diffs[3] = diffs[i];
                        newFace->mIndices[i1] = neighborChild->mIndices[i];
                    }
                }       
                DRVector3 c = temp.normalize() * mVektorLength;
           //     DRLog.writeVector3ToLog(c, "temp0");
           //     DRLog.writeVector3ToLog(mVertices[newFace->mIndices[i1]], "old0");
            }
            else
            {
                mVertices[mVertexCursor] = temp.normalize() * mVektorLength;
                mColors[mVertexCursor] = DRColor(0.0f, 1.0f, 0.0f);
                newFace->mIndices[i1] = mVertexCursor;
              //  DRLog.writeVector3ToLog(mVertices[mVertexCursor], "0");
                mVertexCursor++;
            }
        }        
    }
    else //if(childCount == 1)
    {
        IcoSphereFace* centerChild = parent->mChilds[0];
        if(!centerChild) LOG_ERROR("[critical]", NULL);  
        
        GLuint border[2];
        mEbeneNeighborCount++;
        if(childCount == 1)
        {
            centerChild->mNeighbors[2] = newFace;
            newFace->mIndices[0] = parent->mIndices[0];
            newFace->mIndices[1] = centerChild->mIndices[0];
            newFace->mIndices[2] = centerChild->mIndices[2];                    
            
            // neighbor pointer setzen            
            newFace->mNeighbors[1] = centerChild;    
            if(parent->mNeighbors[0])                                          // indices 0 und 1
				newFace->mNeighbors[0] = parent->mNeighbors[0]->getChildAtBorder(&newFace->mIndices[0], newFace);
                                     // indices 0 und 2
            border[0] = newFace->mIndices[0]; border[1] = newFace->mIndices[2];
			if(parent->mNeighbors[2])										// indices 0 und 2
				newFace->mNeighbors[2] = parent->mNeighbors[2]->getChildAtBorder(border, newFace);
        }
        else if(childCount == 2)
        {
            centerChild->mNeighbors[0] = newFace;
            newFace->mIndices[0] = centerChild->mIndices[0];
            newFace->mIndices[1] = parent->mIndices[1];
            newFace->mIndices[2] = centerChild->mIndices[1]; 
            
            //neighbor pointer setzen
            newFace->mNeighbors[2] = centerChild;
            if(parent->mNeighbors[1])                                         // indices 1 und 2
				newFace->mNeighbors[1] = parent->mNeighbors[1]->getChildAtBorder(&newFace->mIndices[1], newFace);
            if(parent->mNeighbors[0])                                          // indices 0 und 1
				newFace->mNeighbors[0] = parent->mNeighbors[0]->getChildAtBorder(&newFace->mIndices[0], newFace);
            
        }
        else if(childCount == 3)
        {
            centerChild->mNeighbors[1] = newFace;
            newFace->mIndices[0] = centerChild->mIndices[2];
            newFace->mIndices[1] = centerChild->mIndices[1];        
            newFace->mIndices[2] = parent->mIndices[2];
            
            //neighbor pointer setzen
            newFace->mNeighbors[0] = centerChild;
            if(parent->mNeighbors[1])                                                // indices 1 und 2
				newFace->mNeighbors[1] = parent->mNeighbors[1]->getChildAtBorder(&newFace->mIndices[1], newFace);
                                          // indices 2 und 0
            border[0] = newFace->mIndices[2]; border[1] = newFace->mIndices[0]; 
			if(parent->mNeighbors[2])                                       // indices 2 und 0
				newFace->mNeighbors[2] = parent->mNeighbors[2]->getChildAtBorder(border, newFace);
        }
        else
        {
            LOG_ERROR("[critical] childCount isn't valid", NULL)
        }
        for(int i = 0; i < 3; i++)
            if(newFace->mNeighbors[i]) mEbeneNeighborCount++;
    }
  //  printf("return: %d\n", newFace);
    return newFace;
}

DRReturn DRGeometrieIcoSphere::grabIndicesFromFaces(DRGeometrieIcoSphere::IcoSphereFace* current/* = NULL*/)
{
    static GLuint indexCurser = 0;
    if(!current)   
    {
        indexCurser = 0;
        for(int i = 0; i < 20; i++)
        {
            grabIndicesFromFaces(&mRootSphereFaces[i]);
        }
		DRLog.writeToLog("%d indices used", indexCurser);
    }
    else
    {
        if(current->hasChilds())
        {
                for(int i = 0; i < 4; i++)
                {
                        grabIndicesFromFaces(current->mChilds[i]);
                }
        }
        else
        {
            for(int i = 0; i < 3; i++)
            {
                if(indexCurser >= mIndexCount) LOG_ERROR("zu wenig indices reserviert!", DR_ERROR);
               mIndices[indexCurser++] = current->mIndices[i];
              // DRLog.writeToLog("indexCurser: %d, i: %d, index: %d", indexCurser, i, current->mIndices[i]);
               //DRLog.writeVector3ToLog(mVertices[current->mIndices[i]]);
            }       
        }
    }
     
   return DR_OK;
}
 //*/
DRGeometrieIcoSphere::IcoSphereFace* DRGeometrieIcoSphere::newFace()
{
    if(mFreeIcoFaceMemory.size())
    {
        IcoSphereFace* f = mFreeIcoFaceMemory.back();
        mFreeIcoFaceMemory.pop_back();
        f->reset();
        return f;
    }
    else
    {
        return new IcoSphereFace;
    }
}

void DRGeometrieIcoSphere::deleteFace(DRGeometrieIcoSphere::IcoSphereFace* face)
{
    if(!face) LOG_ERROR_VOID("face is a Zero-Pointer");
    if(mFreeIcoFaceMemory.size() >= mMaxFaceBuffer)
    {
        DR_SAVE_DELETE(face);
    }
    else
    {
        mFreeIcoFaceMemory.push_back(face);
    }
}
// ------------------------- IcoSphereFace ---------------------------------------------
//

DRGeometrieIcoSphere::IcoSphereFace::IcoSphereFace(int seed /* = 0*/)
: mParent(NULL), mSeed(seed)
{
    for(int i = 0; i < 4; i++)
        mChilds[i] = NULL;
    reset();
}

void DRGeometrieIcoSphere::IcoSphereFace::reset(DRGeometrieIcoSphere* sphere)
{
    for(u8 i = 0; i < 4; i++)
    {
        if(mChilds[i]) mChilds[i]->reset(sphere);
        if(sphere) sphere->deleteFace(mChilds[i]);
        else DR_SAVE_DELETE(mChilds[i]);            
        if(mParent && mParent->mChilds[i] == this)
            mParent->mChilds[i] = NULL;
    }
    mParent = NULL;
    memset(mNeighbors, 0, sizeof(IcoSphereFace*)*3);
    memset(mChilds, 0, sizeof(IcoSphereFace*)*4);
    memset(mIndices, 0, sizeof(GLuint)*3);
}



bool DRGeometrieIcoSphere::IcoSphereFace::hasChilds()
{
    if(mChilds[0] && mChilds[1] && mChilds[2] && mChilds[3])
        return true;
    if(mChilds[0] || mChilds[1] || mChilds[2] || mChilds[3])
        LOG_WARNING("not all childs a true, but some!");
    return false;
}

DRGeometrieIcoSphere::IcoSphereFace* DRGeometrieIcoSphere::IcoSphereFace::getChildAtBorder(GLuint borderIndices[2], DRGeometrieIcoSphere::IcoSphereFace* caller /* = NULL*/)
{
    if(!hasChilds()) return NULL;
    IcoSphereFace* returnValue = NULL;
    for(int iChild = 1; iChild < 4; iChild++)
    {
        for(int iIndex = 0; iIndex < 3; iIndex++)
        {
                if(mChilds[iChild]->mIndices[iIndex] == borderIndices[0])
                {
                    s8 low = iIndex-1;
                    if(low < 0) low = 2;
                    u8 high = iIndex+1;
                    if(high > 2) high = 0;
                    if(mChilds[iChild]->mIndices[low] == borderIndices[1])
                    {
                        returnValue = mChilds[iChild];
                        if(caller)
                        {
                            if(iIndex == 0 && low == 2)
                                mChilds[iChild]->mNeighbors[2] = caller;
                            else if(iIndex == 1 && low == 0)
                                mChilds[iChild]->mNeighbors[0] = caller;
                            else if(iIndex == 2 && low == 1)
                                mChilds[iChild]->mNeighbors[1] = caller;
                            else
                                LOG_ERROR("[critical] low ist ungueltig!", returnValue);
                        }
                        return returnValue;
                    }
                    if(mChilds[iChild]->mIndices[high] == borderIndices[1])
                    {
                        returnValue = mChilds[iChild];
                        if(caller)
                        {
                            if(iIndex == 0 && high == 1)
                                mChilds[iChild]->mNeighbors[0] = caller;
                            else if(iIndex == 1 && high == 2)
                                mChilds[iChild]->mNeighbors[1] = caller;
                            else if(iIndex == 2 && high == 0)
                                mChilds[iChild]->mNeighbors[2] = caller;
                            else
                                LOG_ERROR("[critical] high ist ungueltig!", returnValue);
                        }
                        return returnValue;
                    }
                    
                }
        }
    }
    return NULL;
}
