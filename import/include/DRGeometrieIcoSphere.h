/* 
 * File:   DRGeometrieIcoSphere.h
 * Author: dario
 *
 * Created on 26. August 2011, 08:59
 */

#ifndef __DR_ENGINE_GEOMETRIE_ICO_SPHERE_H
#define	__DR_ENGINE_GEOMETRIE_ICO_SPHERE_H


class ENGINE_API DRGeometrieIcoSphere : public DRGeometrieSphere
{
public:
    //! \param maxFaceBuffer count of keeping free memory for IceSphereFace
    DRGeometrieIcoSphere(GLuint maxFaceBuffer = 100);
    virtual ~DRGeometrieIcoSphere();
    
    /*! \param maxEbene höchste zu erstellende Ebene <br> \
     *  (z.B. VY Canis Majoris = 51, Erde = 23)
     * 
     */
    DRReturn initIcoSphere(u8 maxEbene = 0);
    
private:
    DRGeometrieIcoSphere(const DRGeometrieIcoSphere& orig) {LOG_WARNING("Not exist");}
    
    struct IcoSphereFace
    {
        IcoSphereFace();
        void reset();
        
        IcoSphereFace* mNeighbors[3];
        IcoSphereFace* mChilds[4];
        IcoSphereFace* mParent;
        GLuint         mIndices[3];
    };
    
    IcoSphereFace* addNewFace(GLuint index1, GLuint index2, GLuint index3); 
    IcoSphereFace* addNewFace(GLuint index[3]); 
    IcoSphereFace* newFace();
    void deleteFace(IcoSphereFace* face);
    
    DRReturn addNewFacesAtBorder();
    DRReturn grabIndicesFromFaces();
    void reset();
    
    
    std::list<IcoSphereFace*>           mFreeIcoFaceMemory;
    IcoSphereFace                       mRootSphereFaces[20];
    u8                                  mMaxEbene;
    GLuint                              mMaxFaceBuffer;

};
//*/
#endif	/* __DR_ENGINE_GEOMETRIE_ICO_SPHERE_H */

