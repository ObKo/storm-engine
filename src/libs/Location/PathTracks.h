#ifndef _PathTracks_h_
#define _PathTracks_h_

#include "CameraTracksFile.h"
#include "dx8render.h"
#include "math3D.h"

class PathTracks
{
#pragma pack(push, 1)
    struct Point
    {
        Quaternion q;
        CVECTOR p;
        CVECTOR s;
    };
#pragma pack(pop)

    //--------------------------------------------------------------------------------------------
  public:
    PathTracks();
    virtual ~PathTracks();

    //--------------------------------------------------------------------------------------------
  public:
    //Загрузить трек в PathTracks
    bool Load(const char *fileName);
    //Нарисовать трек
    void Draw(VDX8RENDER *render);
    //Получить точку трека
    bool GetPoint(float index, CVECTOR &cp, Quaternion &cq);

    //--------------------------------------------------------------------------------------------
  private:
    Point *point;
    long numPoints;
};

#endif