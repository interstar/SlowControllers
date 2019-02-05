#include "SlowControllers.hpp"

namespace rack {

  // __PhaseCounter__________________________________________________________________________
  //
  void PhaseCounter::start(float d, float m) {
    x=0;
    dx = d;
    max = m;
  }

  void PhaseCounter::setDX(float d) {
    dx = d;
  }

  int PhaseCounter::next() {
    x = x + dx;
    if (x < 0) { x = max; }
    if (x >= max) { x = 0; }
    return (int)x;
  }


  // __TriOsc___________________________________________________________________________
  //

  void TriOsc::setup(float m, float M) {
    v = 0;
    dv = 0.1;
    min = m;
    max = M;
  }

  void TriOsc::setDV(float d) {
    if (d > (max - min)) { return; }
    dv = d;
  }

  float TriOsc::next() {
    v = v + dv;
    if (v > max) { v = max; dv = -dv; }
    if (v < min) { v = min; dv = -dv; }
    return v;
  }

  // __WaveTableWidget___________________________________________________________________
  // the actual widget, not the "pseudo" widget of the module's front panel

  void WaveTableWidget::setup(float ex, float wy) {
    x = ex;
    y = wy;
    w = 350;
    h = 100;

    for (int i=0;i<4096;i++) {
      waves.wx[i] = 0;
      waves.wy[i] = 0;
    }

    waves.writeHead.start(1,4096);
    waves.readHead.start(1,4096);
    ox.setup(-5,5);
    oy.setup(-5,5);
  }

  void WaveTableWidget::draw(NVGcontext *vg) {
    update();

    nvgBeginPath(vg);
    nvgFillColor(vg, nvgRGBA(0,0,66,255));
    nvgRoundedRect(vg, x, y, w, h, 6);
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgStrokeColor(vg, nvgRGBA(250,180,60,255));
    nvgStrokeWidth(vg,1.0f);
    nvgMoveTo(vg,10,y+30);
    for (int i=0;i<4096;i=i+1) {
      nvgLineTo(vg, x+10+(i/13), y+30+waves.wx[i]*2);
    }
    nvgStroke(vg);

    nvgBeginPath(vg);
    nvgStrokeColor(vg, nvgRGBA(60,180,250,255));
    nvgStrokeWidth(vg,1.0f);
    nvgMoveTo(vg,10,y+60);
    for (int i=0;i<4096;i=i+1) {
      nvgLineTo(vg, x+10+(i/13), y+60+waves.wy[i]*2);
    }
    nvgStroke(vg);

  }

  void WaveTableWidget::update() {
    int i = waves.writeHead.next();
    waves.wx[i] = ox.next();
    waves.wy[i] = oy.next();
  }


  struct SlowWaveTable : Module {
    enum ParamIds {
      DX_PARAM,
      DY_PARAM,
      Mix_PARAM,
      Scan_PARAM,
      Freeze_PARAM,
      NUM_PARAMS
    };

    enum InputIds {
      DX_INPUT,
      DY_INPUT,
      Mix_INPUT,
      Scan_INPUT,
      NUM_INPUTS
    };

    enum OutputIds {
      ONE_OUTPUT,
      NUM_OUTPUTS
    };

    enum LightIds {
      NUM_LIGHTS
    };

    SlowWaveTable() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
    void step() override;

  };

};


void SlowWaveTable::step() {
  // fill in how we handle inputs and outputs here
  // params[DX_PARAM].value;
}

struct SlowWaveTableWidget : ModuleWidget {

  WaveTableWidget *wtw;

  SlowWaveTableWidget(SlowWaveTable *module) : ModuleWidget(module) {
    setPanel(SVG::load(assetPlugin(plugin, "res/SlowWaveTables.svg")));

    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    wtw = Widget::create<WaveTableWidget>(Vec(30,30));
    wtw->setup(0,0);
    addChild(wtw);

    addParam(ParamWidget::create<Davies1900hBlackKnob>(Vec(20, 200), module, SlowWaveTable::DX_PARAM, -30.0, 30.0, 0.01));

  }

};


Model *modelSlowWaveTable = Model::create<SlowWaveTable, SlowWaveTableWidget>("SlowWaveTable","SlowWaveTable","SlowWaveTable", OSCILLATOR_TAG);
