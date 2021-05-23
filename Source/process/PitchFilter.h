#pragma once
#pragma once
#include "FFTFilter.h"
#include <JuceHeader.h>

class PitchFilter :
	public FFTFilter
{
public:
	PitchFilter(int segment_length, int buffer_length);
	virtual ~PitchFilter();

	/* --- method*/
	void setFrequencyShift(float pitch, float if_samplerate);
	void setPitchShift(float slide, float if_samplerate);
	void setFormantShift(float slide, float if_samplerate);
	std::shared_ptr<float> getFormantShift(void);
	void setBaseFrequency(float freq);
	float getBaseFrequency();
	void setFreqAnalyzer(bool sw);

	/* --- member */
private:
	/* --- method*/
	void update_table();
	void effect(const juce::HeapBlock<juce::dsp::Complex<float>>& in_data, juce::HeapBlock<juce::dsp::Complex<float>>& out_data, int size, int side) override;
	float analyze_basefreq();

	/* --- member */
	std::shared_ptr<int> pitch_table;
	std::shared_ptr<int> formant_table;
	std::shared_ptr<int> shift_table;
	std::shared_ptr<float> level;
	std::shared_ptr<float> analyze_buffer;
	float shift_freq;
	float pitch_shift;
	float formant_shift;
	float sample_rate;
	float voice_basefreq;
	bool basefreq_analyzer_sw;

	const int base_analyzer_filter_size = 16;
	int base_freq_filter_count;
	float base_freq_filter[16];
};

