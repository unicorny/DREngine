#ifndef __DR_TEXTUR__
#define __DR_TEXTUR__

class ENGINE_API DRTextur
{
public:
    DRTextur();
    DRTextur(const char* filename, bool keepImage = false);
    virtual ~DRTextur();

    DRReturn load(const char* filename, bool keepImage = false);
    void unload();

    DRReturn bind();

protected:
    DRIImage* mParent;
    unsigned mTexturID;

};

#endif //__DR_TEXTUR__
