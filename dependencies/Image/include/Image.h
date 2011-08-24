#ifndef __DR_IMAGE__
#define __DR_IMAGE__

class IMAGE_API DRImage : public DRIImage
{
public:
    DRImage();
    virtual ~DRImage();

    // Funktionen
    virtual DRReturn loadFromFile(const char* filename);
    virtual GLenum getImageFormat();
    virtual unsigned int getWidth();
    virtual unsigned int getHeight();
    virtual unsigned char* getPixel();

protected:
    //Hilfsfunktionen
    const char* colorTypeName(FREE_IMAGE_COLOR_TYPE type);

private:
    FIBITMAP* mImage;
    std::string mFilename;

    bool      mLoadedSucessfully;



};
//*/
#endif //__DR_IMAGE__
