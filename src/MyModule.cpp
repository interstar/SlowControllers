#include "Template.hpp"


struct MyModule : Module {
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

	MyModule() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};


void MyModule::step() {
	// Implement a simple sine oscillator
	//float deltaTime = engineGetSampleTime();

	// Compute the frequency from the pitch parameter and input
	//float pitch = params[PITCH_PARAM].value;
	//pitch += inputs[PITCH_INPUT].value;
	//pitch = clamp(pitch, -4.0f, 4.0f);
	// The default pitch is C4
	//float freq = 261.626f * powf(2.0f, pitch);

	// Accumulate the phase
	//phase += freq * deltaTime;
	//if (phase >= 1.0f)
	//	phase -= 1.0f;

	// Compute the sine output
	//float sine = sinf(2.0f * M_PI * phase);
	//outputs[SINE_OUTPUT].value = 5.0f * sine;

        // Outputs from the controllers
  outputs[CTRL1_OUTPUT].value = params[SLOW1_PARAM].value;
  outputs[CTRL2_OUTPUT].value = params[SLOW2_PARAM].value;
  outputs[CTRL3_OUTPUT].value = params[SLOW3_PARAM].value;
  outputs[CTRL4_OUTPUT].value = params[SLOW4_PARAM].value;


	// Blink light at 1Hz
	//blinkPhase += deltaTime;
	//if (blinkPhase >= 1.0f)
	//	blinkPhase -= 1.0f;
	//lights[BLINK_LIGHT].value = (blinkPhase < 0.5f) ? 1.0f : 0.0f;
}


struct MyModuleWidget : ModuleWidget {
	MyModuleWidget(MyModule *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/MyModule2.svg")));

		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));



		//addInput(Port::create<PJ301MPort>(Vec(33, 186), Port::INPUT, module, MyModule::PITCH_INPUT));
		//addChild(ModuleLightWidget::create<MediumLight<RedLight>>(Vec(41, 59), module, MyModule::BLINK_LIGHT));
                //addParam(ParamWidget::create<Davies1900hBlackKnob>(Vec(28, 87), module, MyModule::PITCH_PARAM, -3.0, 3.0, 0.0));


                SlowSliderWidget *ssw;

                ssw= ParamWidget::create<SlowSliderWidget>(Vec(20,20), module, MyModule::SLOW1_PARAM, -3.0, 3.0, 0);
                ssw->setup(0,0,-5.0,5.0);
                ssw->setDestination(30,10);
		addParam(ssw);

		addOutput(Port::create<PJ301MPort>(Vec(40, 170), Port::OUTPUT, module, MyModule::CTRL1_OUTPUT));


                ssw= ParamWidget::create<SlowSliderWidget>(Vec(20,200), module, MyModule::SLOW2_PARAM, -3.0, 3.0, 0);
                ssw->setup(0,0,-5.0,5.0);
                ssw->setDestination(30,40);
		addParam(ssw);

		addOutput(Port::create<PJ301MPort>(Vec(40, 345), Port::OUTPUT, module, MyModule::CTRL2_OUTPUT));


                ssw= ParamWidget::create<SlowSliderWidget>(Vec(110,20), module, MyModule::SLOW3_PARAM, -3.0, 3.0, 0);
                ssw->setup(0,0,-5.0,5.0);
                ssw->setDestination(138,1);
		addParam(ssw);

		addOutput(Port::create<PJ301MPort>(Vec(130, 170), Port::OUTPUT, module, MyModule::CTRL3_OUTPUT));


                ssw= ParamWidget::create<SlowSliderWidget>(Vec(110,200), module, MyModule::SLOW4_PARAM, -3.0, 3.0, 0);
                ssw->setup(0,0,-5.0,5.0);
                ssw->setDestination(120,10);
		addParam(ssw);

		addOutput(Port::create<PJ301MPort>(Vec(130, 345), Port::OUTPUT, module, MyModule::CTRL4_OUTPUT));

	}
};


// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelMyModule = Model::create<MyModule, MyModuleWidget>("Template", "MyModule", "My Module", OSCILLATOR_TAG);
