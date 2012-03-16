#ifndef __DR_IMAGE__
#define __DR_IMAGE__

class IMAGE_API DRImage : public DRIImage
{
public:
    DRImage();
    virtual ~DRImage();

    // Funktionen
    virtual DRReturn loadFromFile(const char* filename);
    virtual DRReturn saveIntoFile(const char* filename);
    virtual GLenum getImageFormat();
    virtual void setImageFormat(GLenum format);
    
    virtual u32 getWidth();
    virtual void setWidth(u32 width);
    virtual u32 getHeight();
    virtual void setHeight(u32 height);
    virtual DRVector2 getSize();
    virtual void setSize(DRVector2 size);
    virtual u8* getPixel();
    virtual void getPixel(DRColor* buffer);
    virtual void setPixel(u8* pixel);
    virtual void setPixel(DRColor* pixel);

protected:
    //Hilfsfunktionen
    const char* colorTypeName(FREE_IMAGE_COLOR_TYPE type);
    const char* imageFormatName(FREE_IMAGE_FORMAT type);

private:
    FIBITMAP* mImage;
    std::string mFilename;
    //daten für selbst angelegte Bilder
    DRVector2 mSize;
    GLuint mImageFormat;

    bool      mLoadedSucessfully;



};
//*/
#endif //__DR_IMAGE__
