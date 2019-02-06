#include "rack.hpp"

#include "nanovg.h"

using namespace rack;

// Forward-declare the Plugin, defined in Template.cpp
extern Plugin *plugin;

// Forward-declare each Model, defined in each module source file
extern Model *modelSlowSliders;
extern Model *modelSlowWaveTable;

namespace rack {

  // Slow Slider Widget

  struct SlowSliderWidget : public ParamWidget {
    float x,y,w,h;
    float slideBottom, slideTop, targetMin, targetMax;
    float current, destination; // offsets from y
    float dy;
    bool moving;
    void setup(float ex, float wy, float tmin, float tmax);
    void draw(NVGcontext *vg) override;
    bool hit(float ex, float wy);
    bool closish(float x, float target, float margin);
    void setDestination(float ex, float wy);
    void update();
    float map(float value, float istart, float istop, float ostart, float ostop);
    float getSliderValue();
    void onMouseDown(EventMouseDown &e) override;
  };


  // Slow WaveTable Widget


  struct PhaseCounter {
  public :
    float x, dx, max;
    void start(float d, float m);
    int next();
    void setDX(float d);
  };


  struct TriOsc {

    float v,dv,max,min;
    void setup(float dv, float m, float M);
    void setDV(float d);
    float next();
  };

  struct Waves {
  public:
    float wx[4096];
    float wy[4096];


    PhaseCounter writeHead;
    PhaseCounter readHead;

    float writeSpeed;
  };

  struct WaveTable {
    Waves waves;
    TriOsc ox, oy;
    float mix;
    bool frozen;

    void setup();
    void update();

    // controllers
    void setDX(float dx);
    void setDY(float dy);
    void setMix(float mix);
    void setScan(float scan);
    void setFrozen(float fr);

    float x_(int i);
    float y_(int i);
    float z_(int i);

    float nextScan();
  };

  struct WaveTableWidget : public OpaqueWidget {

    int x,y,w,h;
    WaveTable *waveTable;
    void setup(float ex, float wy, WaveTable *wtp );
    void draw(NVGcontext *vg) override;
    void update();
  };

}
