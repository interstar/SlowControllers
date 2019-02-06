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
  }

  float TriOsc::next() {
    v = v + dv;
    if (v > max) { v = max; dv = std::min(-dv,dv); }
    if (v < min) { v = min; dv = std::max(-dv,dv); }
    return v;
  }


  // __WaveTable_________________________________________________________________________
  //

  void WaveTable::setup() {
    for (int i=0;i<4096;i++) {
      waves.wx[i] = 0;
      waves.wy[i] = 0;
    }

    waves.writeHead.start(1,4096);
    waves.readHead.start(1,4096);
    ox.setup(0.4,-5,5);
    oy.setup(0.1,-5,5);
    mix = 0.5;
  }

  void WaveTable::setDX(float d) { ox.setDV(d); }
  void WaveTable::setDY(float d) { oy.setDV(d); }
  void WaveTable::setMix(float m) { mix = m; }
  void WaveTable::setScan(float s) {
    waves.readHead.setDX(s);
  }

  void WaveTable::update() {
    int i = waves.writeHead.next();
    waves.wx[i] = ox.next();
    waves.wy[i] = oy.next();
  }

  float WaveTable::x_(int i) { return waves.wx[i]; }
  float WaveTable::y_(int i) { return waves.wy[i]; }
  float WaveTable::z_(int i) { return 0.3*(waves.wx[i]*(1-mix) + waves.wy[i]*mix); }

  float WaveTable::nextScan() {
    return z_(waves.readHead.next());
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
      ONE_OUTPUT,
      NUM_OUTPUTS
    };

    enum LightIds {
      NUM_LIGHTS
    };

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
  waveTable.setDX(params[DX_PARAM].value);
  waveTable.setMix(params[Mix_PARAM].value);
  waveTable.setScan(params[Scan_PARAM].value);
  outputs[ONE_OUTPUT].value = waveTable.nextScan();
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

    addParam(ParamWidget::create<Davies1900hBlackKnob>(Vec(20, 180), module, SlowWaveTable::DX_PARAM, 0.01, 10.0, 0.3));
    addParam(ParamWidget::create<Davies1900hBlackKnob>(Vec(60, 180), module, SlowWaveTable::DY_PARAM, 0.01, 10.0, 0.3));
    addParam(ParamWidget::create<Davies1900hBlackKnob>(Vec(100, 180), module, SlowWaveTable::Mix_PARAM, 0, 1, 0.5));
    addParam(ParamWidget::create<Davies1900hBlackKnob>(Vec(140, 180), module, SlowWaveTable::Scan_PARAM, 0.1, 100, 10));


    addOutput(Port::create<PJ301MPort>(Vec(234, 200), Port::OUTPUT, module, SlowWaveTable::ONE_OUTPUT));

  }

};


Model *modelSlowWaveTable = Model::create<SlowWaveTable, SlowWaveTableWidget>("SlowWaveTable","SlowWaveTable","SlowWaveTable", OSCILLATOR_TAG);
