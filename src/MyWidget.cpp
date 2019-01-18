#include "Template.hpp"


namespace rack {

  void SlowSliderWidget::draw(NVGcontext *vg) {
    update();

    nvgBeginPath(vg);
    nvgFillColor(vg, nvgRGBA(0,0,0,255));
    nvgRect(vg, x, y, w, h);
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgFillColor(vg,nvgRGBA(0,255,0,255));
    nvgMoveTo(vg,x,y+current);
    nvgLineTo(vg,x+w,y+current);
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgFillColor(vg,nvgRGBA(255,0,200,255));
    nvgMoveTo(vg,x,y+destination);
    nvgLineTo(vg,x+w,y+destination);
  }

  void SlowSliderWidget::setup(float ex, float wy, float tmin, float tmax) {
    x = ex;
    y = wy;
    w = 70;
    h = 140;
    targetMin = tmin;
    targetMax = tmax;
    slideBottom = h;
    slideTop = 0;
    current = h/2;
    moving = false;
  }

  bool SlowSliderWidget::hit(float ex, float wy) {
    if (ex < x) { return false; }
    if (ex > x+w) { return false; }
    if (wy < y) { return false; }
    if (wy > y+h) { return false; }
    return true;
  }

  bool SlowSliderWidget::closish(float x, float target, float margin) {
    if (x < target-margin) { return false; }
    if (x > target+margin) { return false; }
    return true;
  }

  void SlowSliderWidget::setDestination(float ex, float wy) {
    float dx = ex - x;
    destination = wy-y;
    if (dx < 1) { dx = 1; }
    dy = ((wy-y) - current) / (dx*100);
    moving = true;
  }


  void SlowSliderWidget::update() {
    if (moving && (!closish(current,destination,1)) && (current<h)) {
      current = current+dy;
    } else {
      moving = false;
    }
  }


  float SlowSliderWidget::map(float value, float istart, float istop, float ostart, float ostop) {
    return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
  }

  float SlowSliderWidget::getValue() {
    return map(current,h,0,targetMin,targetMax);
  }

} // namespace rack
