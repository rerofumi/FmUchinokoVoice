/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "process/PitchFilter.h"

//==============================================================================
/**
*/
class Fm_uchinoko_voiceAudioProcessor : public juce::AudioProcessor
{
public:
	//==============================================================================
	Fm_uchinoko_voiceAudioProcessor();
	~Fm_uchinoko_voiceAudioProcessor() override;

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

	//==============================================================================
	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	//==============================================================================
	const juce::String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const juce::String getProgramName(int index) override;
	void changeProgramName(int index, const juce::String& newName) override;

	//==============================================================================
	void getStateInformation(juce::MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

	//===== mycode
	juce::Image& getSpectrumView();
	void setFrequencyShift(float pitch);
	void setPitchShift(float slide);
	void setFormantShift(float slide);
	void setBaseFrequency(float value);
	void setFrequencyAnalyzer(bool sw);
	float getFrequencyShift();
	float getPitchShift();
	float getFormantShift();
	float getBaseFrequency();
	bool getBaseAnalyze();
	bool isUpdateParameter();

private:
	//===== plugin parameter
	juce::AudioParameterFloat* prm_frequency_shift;
	juce::AudioParameterFloat* prm_base_frequency;
	juce::AudioParameterFloat* prm_pitch_shift;
	juce::AudioParameterFloat* prm_formant_shift;
	juce::AudioParameterBool* prm_base_analyze;
	float post_frequency_shift;
	float post_base_frequency;
	float post_pitch_shift;
	float post_formant_shift;
	bool post_base_analyze;
	bool update_parameter_flag;
	//===== mycode
	void drawSpectrumGraph(juce::Image view, std::shared_ptr<float> level, juce::Colour color, bool isLog);
	void drawVoiceBaseFrequency(juce::Image view, float freq);
	void sync_plugin_parameter();
	juce::Image spectrum;
	PitchFilter myFilter;
	double if_samplerate;
	int if_samplesPerBlock;

	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Fm_uchinoko_voiceAudioProcessor)
};
