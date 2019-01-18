#include "rack.hpp"

#include "nanovg.h"

using namespace rack;

// Forward-declare the Plugin, defined in Template.cpp
extern Plugin *plugin;

// Forward-declare each Model, defined in each module source file
extern Model *modelMyModule;

namespace rack {
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
      float getValue();
    };
}
