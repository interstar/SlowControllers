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

  void TriOsc::setup(float d, float m, float M) {
    v = 0;
    dv = d;
    min = m;
    max = M;
  }

  void TriOsc::setDV(float d) {
    //if (d > (max - min)) { return; }
    dv = d;
    v = 0;
  }

  float TriOsc::next() {
    v = v + dv;
    if (v > max) {
      v = max;
      dv = std::min(-dv,dv);
      return v;
    }
    if (v < min) {
      v = min; dv = std::max(-dv,dv);
    }
    return v;
  }


  // __WaveTable_________________________________________________________________________
  //

  void WaveTable::setup() {
    for (int i=0;i<4096;i++) {
      wx[i] = 0;
      wy[i] = 0;
    }

    writeHead.start(1,4096);
    readHead.start(1,4096);
    ox.setup(0.2,-5,5);
    oy.setup(0.05,-5,5);
    mix = 0.5;
  }

  void WaveTable::setDX(float d) { ox.setDV(d); }
  void WaveTable::setDY(float d) { oy.setDV(d); }
  void WaveTable::setMix(float m) { mix = m; }

  void WaveTable::setScan(float s) {
    readHead.setDX(s);
  }

  void WaveTable::setFrozen(float fr) {
    frozen = (fr < 0.5) ? false : true;
  }

  void WaveTable::update() {
    if (!frozen) {
      int i = writeHead.next();
      wx[i] = ox.next();
      wy[i] = oy.next();
    }
  }

  float WaveTable::x() { return xx; }
  float WaveTable::y() { return yy; }
  float WaveTable::z() { return zz; }

  float WaveTable::x_(int i) { return wx[i]; }
  float WaveTable::y_(int i) { return wy[i]; }
  float WaveTable::z_(int i) { return 0.6 * (wx[i]*(1-mix) + wy[i]*mix); }

  void WaveTable::nextScan() {
    int i = readHead.next();
    xx = wx[i];
    yy = wy[i];
    zz = 0.6 * (xx * (1-mix) + yy * mix);
  }

  // __WaveTableWidget___________________________________________________________________
  // the actual widget, not the "pseudo" widget of the module's front panel

  void WaveTableWidget::setup(float ex, float wy, WaveTable *wtp) {
    x = ex;
    y = wy;
    w = 350;
    h = 130;
    waveTable = wtp;
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
      nvgLineTo(vg, x+10+(i/13), y+30+waveTable->x_(i)*2);
    }
    nvgStroke(vg);

    nvgBeginPath(vg);
    nvgStrokeColor(vg, nvgRGBA(60,180,250,255));
    nvgStrokeWidth(vg,1.0f);
    nvgMoveTo(vg,10,y+70);
    for (int i=0;i<4096;i=i+1) {
      nvgLineTo(vg, x+10+(i/13), y+70+waveTable->y_(i)*2);
    }
    nvgStroke(vg);

    nvgBeginPath(vg);
    nvgStrokeColor(vg, nvgRGBA(60,180,250,255));
    nvgStrokeWidth(vg,1.0f);
    nvgMoveTo(vg,10,y+110);
    for (int i=0;i<4096;i=i+1) {
      nvgLineTo(vg, x+10+(i/13), y+110+waveTable->z_(i)*2);
    }
    nvgStroke(vg);



  }

  void WaveTableWidget::update() {
    waveTable->update();
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
      X_OUTPUT,
      Y_OUTPUT,
      Mix_OUTPUT,
      NUM_OUTPUTS
    };

    enum LightIds {
      NUM_LIGHTS
    };

    float oldDX, oldDY;
    WaveTable waveTable;
    SlowWaveTable() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
      waveTable.setup();
    }

    void step() override;

    WaveTable* exposeWT() { return &waveTable; }
  };

};


void SlowWaveTable::step() {
  // fill in how we handle inputs and outputs here
  float dx = params[DX_PARAM].value;
  if (dx != oldDX) {
    waveTable.setDX(log10(dx));
    oldDX = dx;
  }
  float dy = params[DY_PARAM].value;
  if (dy != oldDY) {
    waveTable.setDY(log10(dy));
    oldDY = dy;
  }
  waveTable.setFrozen(params[Freeze_PARAM].value);
  waveTable.setMix(params[Mix_PARAM].value);
  waveTable.setScan(params[Scan_PARAM].value);

  waveTable.nextScan();
  outputs[X_OUTPUT].value = waveTable.x();
  outputs[Y_OUTPUT].value = waveTable.y();
  outputs[Mix_OUTPUT].value = waveTable.z();
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
    wtw->setup(0,0,module->exposeWT());
    addChild(wtw);

    addParam(ParamWidget::create<CKSS>(Vec(234, 200), module, SlowWaveTable::Freeze_PARAM, 0.0f, 1.0f, 1.0f));

    addParam(ParamWidget::create<RoundHugeBlackKnob>(Vec(30, 180), module, SlowWaveTable::DX_PARAM, 0.01, 1.0, 0.3));
    addParam(ParamWidget::create<RoundHugeBlackKnob>(Vec(130, 180), module, SlowWaveTable::DY_PARAM, 0.01, 1.0, 0.3));
    addParam(ParamWidget::create<RoundHugeBlackKnob>(Vec(30, 270), module, SlowWaveTable::Mix_PARAM, 0, 1, 0.5));
    addParam(ParamWidget::create<RoundHugeBlackKnob>(Vec(130, 270), module, SlowWaveTable::Scan_PARAM, 0.01, 10, 1));


    addOutput(Port::create<PJ301MPort>(Vec(334, 200), Port::OUTPUT, module, SlowWaveTable::X_OUTPUT));
    addOutput(Port::create<PJ301MPort>(Vec(334, 240), Port::OUTPUT, module, SlowWaveTable::Y_OUTPUT));
    addOutput(Port::create<PJ301MPort>(Vec(334, 280), Port::OUTPUT, module, SlowWaveTable::Mix_OUTPUT));
  }

};


Model *modelSlowWaveTable = Model::create<SlowWaveTable, SlowWaveTableWidget>("SlowWaveTable","SlowWaveTable","SlowWaveTable", OSCILLATOR_TAG);
