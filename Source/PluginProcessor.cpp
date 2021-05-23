/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Fm_uchinoko_voiceAudioProcessor::Fm_uchinoko_voiceAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	)
#endif
	, spectrum(juce::Image::RGB, 512, 256, true)
	, myFilter(PitchFilter::fftSize, PitchFilter::fftSize * 3)
	, if_samplerate(48000.0)
	, if_samplesPerBlock(512)
	, update_parameter_flag(false)
{
	// plugin parameter
	addParameter(prm_frequency_shift = new juce::AudioParameterFloat("PrmFreqShift", "FrequencyShift", 0.0f, 440.0f, 0.0f));
	addParameter(prm_base_frequency = new juce::AudioParameterFloat("PrmBaseFreq", "BaseFrequency", 60.0f, 4000.0f, 400.0f));
	addParameter(prm_pitch_shift = new juce::AudioParameterFloat("PrmPitchShift", "PitchShift", -1.0f, 1.0f, 0.0f));
	addParameter(prm_formant_shift = new juce::AudioParameterFloat("PrmFormantShift", "FormantShift", -1.0f, 1.0f, 0.0f));
	addParameter(prm_base_analyze = new juce::AudioParameterBool("PrmBaseAnalyze", "BaseAnalyze", false));
	post_frequency_shift = *prm_frequency_shift;
	post_base_frequency = *prm_base_frequency;
	post_pitch_shift = *prm_pitch_shift;
	post_formant_shift = *prm_formant_shift;
	post_base_analyze = *prm_base_analyze;
}

Fm_uchinoko_voiceAudioProcessor::~Fm_uchinoko_voiceAudioProcessor()
{
}

//==============================================================================
const juce::String Fm_uchinoko_voiceAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool Fm_uchinoko_voiceAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool Fm_uchinoko_voiceAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool Fm_uchinoko_voiceAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double Fm_uchinoko_voiceAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int Fm_uchinoko_voiceAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int Fm_uchinoko_voiceAudioProcessor::getCurrentProgram()
{
	return 0;
}

void Fm_uchinoko_voiceAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String Fm_uchinoko_voiceAudioProcessor::getProgramName(int index)
{
	return {};
}

void Fm_uchinoko_voiceAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void Fm_uchinoko_voiceAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	if_samplerate = sampleRate;
	if_samplesPerBlock = samplesPerBlock;
}

void Fm_uchinoko_voiceAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Fm_uchinoko_voiceAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	// Some plugin hosts, such as certain GarageBand versions, will only
	// load plugins that support stereo bus layouts.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

void Fm_uchinoko_voiceAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	/* --- my processing code*/
	juce::ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	auto data_size = buffer.getNumSamples();
	float* channel_1 = buffer.getWritePointer(0);
	float* channel_2 = buffer.getWritePointer(1);
	// do work -----
	myFilter.process(channel_1, channel_2, data_size);
	myFilter.output(channel_1, channel_2, data_size);
	// -----
}

//==============================================================================
bool Fm_uchinoko_voiceAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Fm_uchinoko_voiceAudioProcessor::createEditor()
{
	return new Fm_uchinoko_voiceAudioProcessorEditor(*this);
}

//==============================================================================
void Fm_uchinoko_voiceAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// store parameter
	std::unique_ptr<juce::XmlElement> xml(new juce::XmlElement("ParamUchinokoVoice"));
	xml->setAttribute("frequency_shift", (double)*prm_frequency_shift);
	xml->setAttribute("base_frequency", (double)*prm_base_frequency);
	xml->setAttribute("pitch_shift", (double)*prm_pitch_shift);
	xml->setAttribute("formant_shift", (double)*prm_formant_shift);
	xml->setAttribute("base_analyze", (bool)*prm_base_analyze);
	copyXmlToBinary(*xml, destData);
}

void Fm_uchinoko_voiceAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// read parameter
	std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
	if (xmlState.get() != nullptr) {
		if (xmlState->hasTagName("ParamUchinokoVoice")) {
			post_frequency_shift = *prm_frequency_shift = (float)xmlState->getDoubleAttribute("frequency_shift", 0.0);
			post_base_frequency = *prm_base_frequency = (float)xmlState->getDoubleAttribute("base_frequency", 80.0);
			post_pitch_shift = *prm_pitch_shift = (float)xmlState->getDoubleAttribute("pitch_shift", 0.0);
			post_formant_shift = *prm_formant_shift = (float)xmlState->getDoubleAttribute("formant_shift", 0.0);
			post_base_analyze = *prm_base_analyze = (bool)xmlState->getBoolAttribute("base_analyze", false);
		}
	}
	post_frequency_shift = 0.0;
	post_base_frequency = 80.0;
	post_pitch_shift = 0.0;
	post_formant_shift = 0.0;
	post_base_analyze = false;
	sync_plugin_parameter();
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new Fm_uchinoko_voiceAudioProcessor();
}



//==============================================================================
// add my methods
//==============================================================================
/*---
* change pitch shift table
*/
void Fm_uchinoko_voiceAudioProcessor::setFrequencyShift(float pitch)
{
	myFilter.setFrequencyShift(pitch, (float)if_samplerate);
	*prm_frequency_shift = pitch;
	post_frequency_shift = pitch;
}
void Fm_uchinoko_voiceAudioProcessor::setPitchShift(float slide)
{
	myFilter.setPitchShift(slide, (float)if_samplerate);
	*prm_pitch_shift = slide;
	post_pitch_shift = slide;
}
void Fm_uchinoko_voiceAudioProcessor::setFormantShift(float slide)
{
	myFilter.setFormantShift(slide, (float)if_samplerate);
	*prm_formant_shift = slide;
	post_formant_shift = slide;
}
void Fm_uchinoko_voiceAudioProcessor::setBaseFrequency(float value)
{
	myFilter.setBaseFrequency(value);
	*prm_base_frequency = value;
	post_base_frequency = value;
}
void Fm_uchinoko_voiceAudioProcessor::setFrequencyAnalyzer(bool sw)
{
	myFilter.setFreqAnalyzer(sw);
	*prm_base_analyze = sw;
	post_base_analyze = sw;
}
float Fm_uchinoko_voiceAudioProcessor::getBaseFrequency()
{
	return myFilter.getBaseFrequency();
}
float Fm_uchinoko_voiceAudioProcessor::getFrequencyShift()
{
	return *prm_frequency_shift;
}
float Fm_uchinoko_voiceAudioProcessor::getPitchShift()
{
	return *prm_pitch_shift;
}
float Fm_uchinoko_voiceAudioProcessor::getFormantShift()
{
	return *prm_formant_shift;
}
bool Fm_uchinoko_voiceAudioProcessor::getBaseAnalyze()
{
	return *prm_base_analyze;
}
bool Fm_uchinoko_voiceAudioProcessor::isUpdateParameter()
{
	bool result = update_parameter_flag;
	update_parameter_flag = false;
	return result;
}


void Fm_uchinoko_voiceAudioProcessor::sync_plugin_parameter()
{
	if (*prm_frequency_shift != post_frequency_shift) {
		myFilter.setFrequencyShift(*prm_frequency_shift, (float)if_samplerate);
		post_frequency_shift = *prm_frequency_shift;
		update_parameter_flag = true;
	}
	if (*prm_base_frequency != post_base_frequency) {
		myFilter.setBaseFrequency(*prm_base_frequency);
		post_base_frequency = *prm_base_frequency;
		update_parameter_flag = true;
	}
	if (*prm_pitch_shift != post_pitch_shift) {
		myFilter.setPitchShift(*prm_pitch_shift, (float)if_samplerate);
		post_pitch_shift = *prm_pitch_shift;
		update_parameter_flag = true;
	}
	if (*prm_formant_shift != post_formant_shift) {
		myFilter.setFormantShift(*prm_formant_shift, (float)if_samplerate);
		post_formant_shift = *prm_formant_shift;
		update_parameter_flag = true;
	}
	if (*prm_base_analyze != post_base_analyze) {
		myFilter.setFreqAnalyzer(*prm_base_analyze);
		post_base_analyze = *prm_base_analyze;
		update_parameter_flag = true;
	}
}

/* ---
* rendering spectrum view
*/
juce::Image& Fm_uchinoko_voiceAudioProcessor::getSpectrumView()
{
	if (myFilter.getProcessFlag()) {
		spectrum.clear(juce::Rectangle<int>(512, 256), juce::Colour(0, 0, 0));
		// base freq line
		drawVoiceBaseFrequency(spectrum, myFilter.getBaseFrequency());
#ifdef DEBUG
		// formant shift table
		auto level_formant = myFilter.getFormantShift();
		drawSpectrumGraph(spectrum, level_formant, juce::Colour(200, 200, 0), false);
#endif
		// input level
		auto level = myFilter.getSpectrumInput();
		drawSpectrumGraph(spectrum, level, juce::Colour(0, 200, 0), true);
		// output level
		auto level_out = myFilter.getSpectrumOutput();
		drawSpectrumGraph(spectrum, level_out, juce::Colour(200, 0, 80), true);
		//
		myFilter.setProcessFlag(false);
	}
	return spectrum;
}

/* --- base freq line */
void Fm_uchinoko_voiceAudioProcessor::drawVoiceBaseFrequency(juce::Image view, float freq)
{
	float line_x = 0.0;
	float target_index = (freq / (if_samplerate / 2.0)) * (float)PitchFilter::spectrumNum;
	for (int x = 1; x < 512; x++) {
		float skewedProportionX = 0.0f;
		skewedProportionX = 1.0f - std::exp(std::log(1.0f - (float)x / 512.0f) * 0.2f);
		line_x = x;
		float index = juce::jlimit(0, (int)PitchFilter::spectrumNum, (int)(skewedProportionX * (float)PitchFilter::spectrumNum));
		if (index > target_index) break;
	}
	juce::Graphics g(view);
	g.setColour(juce::Colour(160, 160, 160));
	g.setOpacity(1.0);
	g.drawLine(line_x, 0.0f, line_x, 255.0f);
}

/* --- draw graph */
void Fm_uchinoko_voiceAudioProcessor::drawSpectrumGraph(juce::Image view, std::shared_ptr<float> power, juce::Colour color, bool isLog)
{
	int post_point = 0;
	float post_level = 0.0f;
	juce::Graphics g(view);
	for (int x = 1; x < 512; x++) {
		float skewedProportionX = 0.0f;
		if (isLog) {
			skewedProportionX = 1.0f - std::exp(std::log(1.0f - (float)x / 512.0f) * 0.2f);
		}
		else {
			skewedProportionX = (float)x / 512.0f;
		}
		auto fftDataIndex = juce::jlimit(0, (int)PitchFilter::spectrumNum, (int)(skewedProportionX * (float)PitchFilter::spectrumNum));
		auto lv = power.get()[fftDataIndex];
		if ((std::fabs(post_level - lv) > 0.0001) || (x == 511) || (!isLog)) {
			g.setColour(color);
			g.setOpacity(1.0);
			g.drawLine(
				(float)post_point,
				juce::jmap(post_level, 0.0f, 1.0f, 256.0f, 0.0f),
				(float)x,
				juce::jmap(lv, 0.0f, 1.0f, 256.0f, 0.0f)
			);
			{
				g.setOpacity(0.5);
				juce::Path pen;
				pen.startNewSubPath(juce::Point<float>((float)post_point, juce::jmap(post_level, 0.0f, 1.0f, 256.0f, 0.0f)));
				pen.lineTo(juce::Point<float>((float)x, juce::jmap(lv, 0.0f, 1.0f, 256.0f, 0.0f)));
				pen.lineTo(juce::Point<float>((float)x, 256.0f));
				pen.lineTo(juce::Point<float>((float)post_point, 256.0f));
				pen.closeSubPath();
				g.fillPath(pen);
			}
			post_point = x;
			post_level = lv;
		}
	}
	g.setOpacity(1.0);
	//
	// update plugin parameter
	sync_plugin_parameter();
}

