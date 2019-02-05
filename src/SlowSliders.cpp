#include "SlowControllers.hpp"


struct SlowSliders : Module {
	enum ParamIds {
		PITCH_PARAM,
                SLOW1_PARAM,
                SLOW2_PARAM,
                SLOW3_PARAM,
                SLOW4_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		PITCH_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		CTRL1_OUTPUT,
                CTRL2_OUTPUT,
                CTRL3_OUTPUT,
                CTRL4_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		BLINK_LIGHT,
		NUM_LIGHTS
	};

	float phase = 0.0;
	float blinkPhase = 0.0;

	SlowSliders() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};


void SlowSliders::step() {
  outputs[CTRL1_OUTPUT].value = params[SLOW1_PARAM].value;
  outputs[CTRL2_OUTPUT].value = params[SLOW2_PARAM].value;
  outputs[CTRL3_OUTPUT].value = params[SLOW3_PARAM].value;
  outputs[CTRL4_OUTPUT].value = params[SLOW4_PARAM].value;
}


struct SlowSlidersWidget : ModuleWidget {
  SlowSlidersWidget(SlowSliders *module) : ModuleWidget(module) {
    setPanel(SVG::load(assetPlugin(plugin, "res/SlowSliders.svg")));

    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));


    SlowSliderWidget *ssw;

    ssw= ParamWidget::create<SlowSliderWidget>(Vec(10,30), module, SlowSliders::SLOW1_PARAM, -3.0, 3.0, 0);
    ssw->setup(0,0,-5.0,5.0);
    ssw->setDestination(30,10);
    addParam(ssw);

    addOutput(Port::create<PJ301MPort>(Vec(164, 63), Port::OUTPUT, module, SlowSliders::CTRL1_OUTPUT));


    ssw= ParamWidget::create<SlowSliderWidget>(Vec(10,200), module, SlowSliders::SLOW2_PARAM, -3.0, 3.0, 0);
    ssw->setup(0,0,-5.0,5.0);
    ssw->setDestination(30,40);
    addParam(ssw);

    addOutput(Port::create<PJ301MPort>(Vec(164, 129), Port::OUTPUT, module, SlowSliders::CTRL2_OUTPUT));


    ssw= ParamWidget::create<SlowSliderWidget>(Vec(90,30), module, SlowSliders::SLOW3_PARAM, -3.0, 3.0, 0);
    ssw->setup(0,0,-5.0,5.0);
    ssw->setDestination(138,1);
    addParam(ssw);

    addOutput(Port::create<PJ301MPort>(Vec(164, 196), Port::OUTPUT, module, SlowSliders::CTRL3_OUTPUT));


    ssw= ParamWidget::create<SlowSliderWidget>(Vec(90,200), module, SlowSliders::SLOW4_PARAM, -3.0, 3.0, 0);
    ssw->setup(0,0,-5.0,5.0);
    ssw->setDestination(120,10);
    addParam(ssw);

    addOutput(Port::create<PJ301MPort>(Vec(164, 262), Port::OUTPUT, module, SlowSliders::CTRL4_OUTPUT));

  }
};


// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelSlowSliders = Model::create<SlowSliders, SlowSlidersWidget>("SlowSliders", "SlowSliders", "SlowSliders", CONTROLLER_TAG);
