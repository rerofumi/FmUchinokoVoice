/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 6.0.7

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "PluginView.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
void PluginView::setProcesser(Fm_uchinoko_voiceAudioProcessor& p)
{
    processer = &p;
    BaseFreqSlider.get()->setValue(processer->getBaseFrequency());
}
//[/MiscUserDefs]

//==============================================================================
PluginView::PluginView ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    PitchSlider.reset (new juce::Slider ("PitchShift"));
    addAndMakeVisible (PitchSlider.get());
    PitchSlider->setTooltip (TRANS("Pitch"));
    PitchSlider->setRange (0, 440, 0);
    PitchSlider->setSliderStyle (juce::Slider::LinearHorizontal);
    PitchSlider->setTextBoxStyle (juce::Slider::TextBoxLeft, true, 80, 20);
    PitchSlider->setColour (juce::Slider::backgroundColourId, juce::Colour (0xff2d2308));
    PitchSlider->setColour (juce::Slider::thumbColourId, juce::Colours::coral);
    PitchSlider->addListener (this);

    PitchSlider->setBounds (72, 280, 448, 24);

    FormantSlider.reset (new juce::Slider ("FormantSlider"));
    addAndMakeVisible (FormantSlider.get());
    FormantSlider->setRange (-1, 1, 0);
    FormantSlider->setSliderStyle (juce::Slider::Rotary);
    FormantSlider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    FormantSlider->addListener (this);

    FormantSlider->setBounds (24, 376, 112, 112);

    FreqAnalyzerSw.reset (new juce::ToggleButton ("FreqAnalyzerSw"));
    addAndMakeVisible (FreqAnalyzerSw.get());
    FreqAnalyzerSw->setButtonText (TRANS("Freq Analyze"));
    FreqAnalyzerSw->addListener (this);

    FreqAnalyzerSw->setBounds (408, 312, 112, 24);

    BaseFreqSlider.reset (new juce::Slider ("BaseFreqSlider"));
    addAndMakeVisible (BaseFreqSlider.get());
    BaseFreqSlider->setRange (60, 440, 0);
    BaseFreqSlider->setSliderStyle (juce::Slider::LinearHorizontal);
    BaseFreqSlider->setTextBoxStyle (juce::Slider::TextBoxLeft, false, 80, 20);
    BaseFreqSlider->addListener (this);

    BaseFreqSlider->setBounds (72, 312, 328, 24);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

PluginView::~PluginView()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    PitchSlider = nullptr;
    FormantSlider = nullptr;
    FreqAnalyzerSw = nullptr;
    BaseFreqSlider = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void PluginView::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    if (processer->isUpdateParameter()) {
        PitchSlider.get()->setValue(processer->getFrequencyShift());
        FormantSlider.get()->setValue(processer->getFormantShift());
        BaseFreqSlider.get()->setValue(processer->getBaseFrequency());
        FreqAnalyzerSw.get()->setToggleState(processer->getBaseAnalyze(), juce::dontSendNotification);
    }
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff323e44));

    {
        int x = 12, y = 12, width = 512, height = 256;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        g.drawImage(processer->getSpectrumView(), x, y, width, height, 0, 0, width, height);
        //[/UserPaintCustomArguments]
    }

    {
        int x = 12, y = 284, width = 60, height = 20;
        juce::String text (TRANS("FreqShift"));
        juce::Colour fillColour = juce::Colours::azure;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centred, true);
    }

    {
        int x = 12, y = 316, width = 60, height = 20;
        juce::String text (TRANS("BaseFreq"));
        juce::Colour fillColour = juce::Colours::azure;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centred, true);
    }

    {
        int x = 36, y = 356, width = 92, height = 20;
        juce::String text (TRANS("Formant Shift"));
        juce::Colour fillColour = juce::Colours::azure;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centred, true);
    }

    //[UserPaint] Add your own custom painting code here..
    if (FreqAnalyzerSw.get()->getToggleState()) {
        auto freq = processer->getBaseFrequency();
        BaseFreqSlider.get()->setValue(freq);
    }
    //[/UserPaint]
}

void PluginView::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void PluginView::sliderValueChanged (juce::Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == PitchSlider.get())
    {
        //[UserSliderCode_PitchSlider] -- add your slider handling code here..
        processer->setFrequencyShift((float)PitchSlider.get()->getValue());
        //[/UserSliderCode_PitchSlider]
    }
    else if (sliderThatWasMoved == FormantSlider.get())
    {
        //[UserSliderCode_FormantSlider] -- add your slider handling code here..
        processer->setFormantShift((float)FormantSlider.get()->getValue());
        //[/UserSliderCode_FormantSlider]
    }
    else if (sliderThatWasMoved == BaseFreqSlider.get())
    {
        //[UserSliderCode_BaseFreqSlider] -- add your slider handling code here..
        processer->setBaseFrequency((float)BaseFreqSlider.get()->getValue());
        //[/UserSliderCode_BaseFreqSlider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void PluginView::buttonClicked (juce::Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == FreqAnalyzerSw.get())
    {
        //[UserButtonCode_FreqAnalyzerSw] -- add your button handler code here..
        processer->setFrequencyAnalyzer(FreqAnalyzerSw.get()->getToggleState());
        //[/UserButtonCode_FreqAnalyzerSw]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="PluginView" componentName=""
                 parentClasses="public juce::Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ff323e44">
    <IMAGE pos="12 12 512 256" resource="" opacity="1.0" mode="0"/>
    <TEXT pos="12 284 60 20" fill="solid: fff0ffff" hasStroke="0" text="FreqShift"
          fontname="Default font" fontsize="15.0" kerning="0.0" bold="0"
          italic="0" justification="36"/>
    <TEXT pos="12 316 60 20" fill="solid: fff0ffff" hasStroke="0" text="BaseFreq"
          fontname="Default font" fontsize="15.0" kerning="0.0" bold="0"
          italic="0" justification="36"/>
    <TEXT pos="36 356 92 20" fill="solid: fff0ffff" hasStroke="0" text="Formant Shift"
          fontname="Default font" fontsize="15.0" kerning="0.0" bold="0"
          italic="0" justification="36"/>
  </BACKGROUND>
  <SLIDER name="PitchShift" id="e45884d728ca3120" memberName="PitchSlider"
          virtualName="" explicitFocusOrder="0" pos="72 280 448 24" tooltip="Pitch"
          bkgcol="ff2d2308" thumbcol="ffff7f50" min="0.0" max="440.0" int="0.0"
          style="LinearHorizontal" textBoxPos="TextBoxLeft" textBoxEditable="0"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="FormantSlider" id="fc60e961852b2318" memberName="FormantSlider"
          virtualName="" explicitFocusOrder="0" pos="24 376 112 112" min="-1.0"
          max="1.0" int="0.0" style="Rotary" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <TOGGLEBUTTON name="FreqAnalyzerSw" id="8b09a9d253dfa6f3" memberName="FreqAnalyzerSw"
                virtualName="" explicitFocusOrder="0" pos="408 312 112 24" buttonText="Freq Analyze"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <SLIDER name="BaseFreqSlider" id="ec8787278127db06" memberName="BaseFreqSlider"
          virtualName="" explicitFocusOrder="0" pos="72 312 328 24" min="60.0"
          max="440.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

