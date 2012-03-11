/* 
 * File:   DRRandom.h
 * Author: dario
 *
 * Created on 25. August 2011, 13:28
 */

#ifndef DR_CORE2_RANDOM_H
#define	DR_CORE2_RANDOM_H


class CORE2_API DRRandom
{
public:
    DRRandom() {/*srand(time(NULL));*/}
    
    static long core2_rand();
    static double core2_randf();
    static u64 r64();// { return rand();}// + ((long long)rand()) << 32;}
    static double rDouble(double max, double min); 
    static DRReal rReal(DRReal fMax, DRReal fMin)
    {
         return static_cast<DRReal>(rDouble(fMax, fMin));
    }
    static int rInt(int max, int min);
    static DRVector3 rVector3(DRReal maxLengthSq)
    {
            DRVector3 ret;
            ret.x = rReal(maxLengthSq/2.0f, -maxLengthSq/2.0f);
            ret.y = rReal((maxLengthSq - ret.x)/1.5f, -((maxLengthSq - ret.x)/1.5f));
            ret.z = rReal(maxLengthSq - ret.x - ret.y, -(maxLengthSq -ret.x - ret.y));
            return ret;
    }
    static DRVector2 rVector2(DRReal maxLengthSq)
    {
            DRVector2 ret;
            ret.x = rReal(maxLengthSq/2.0f, -maxLengthSq/2.0f);
            ret.y = rReal(maxLengthSq - ret.x, -(maxLengthSq -ret.x));
            return ret;
    }    
    static void seed(long seed); 
    static void seedf(long seed);
private:
};


#endif	/* DR_CORE2_RANDOM_H */

