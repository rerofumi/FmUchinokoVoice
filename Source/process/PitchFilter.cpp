#pragma once
#include "PitchFilter.h"


PitchFilter::PitchFilter(int segment_length, int buffer_length) :
	FFTFilter(segment_length, buffer_length)
	, pitch_table(new int[segment_length])
	, formant_table(new int[segment_length])
	, shift_table(new int[segment_length])
	, level(new float[spectrumNum])
	, shift_freq(0.0)
	, pitch_shift(0.0)
	, formant_shift(0.0)
	, sample_rate(48000.0)
	, voice_basefreq(80.0)
	, basefreq_analyzer_sw(false)
	, base_freq_filter_count(0)
{
	for (int i = 0; i < segment_length; i++) {
		pitch_table.get()[i] = i;
		formant_table.get()[i] = i;
		shift_table.get()[i] = i;
	}
	for (auto i = 0; i < base_analyzer_filter_size; i++) {
		base_freq_filter[i] = 80.0f;
	}
}

PitchFilter::~PitchFilter()
{
}

void PitchFilter::setFrequencyShift(float pitch, float if_samplerate)
{
	shift_freq = pitch;
	sample_rate = if_samplerate;
	update_table();
}

void PitchFilter::setPitchShift(float slide, float if_samplerate)
{
	pitch_shift = slide;
	sample_rate = if_samplerate;
	update_table();
}

void PitchFilter::setFormantShift(float slide, float if_samplerate)
{
	formant_shift = slide;
	sample_rate = if_samplerate;
	update_table();
}

void PitchFilter::setBaseFrequency(float freq)
{
	voice_basefreq = freq;
	update_table();
}

float PitchFilter::getBaseFrequency()
{
	if (basefreq_analyzer_sw) {
		auto fq = analyze_basefreq();
		if ((fq > 60.0f) && (std::fabs(fq - voice_basefreq) < 50.0f)) {
			voice_basefreq = fq;
			update_table();
		}
	}
	return voice_basefreq;
}

void PitchFilter::setFreqAnalyzer(bool sw)
{
	basefreq_analyzer_sw = sw;
}

std::shared_ptr<float> PitchFilter::getFormantShift(void)
{
	for (int i = 0; i < spectrumNum; i++) {
		auto pos = (int)(segment_length * i / spectrumNum);
		auto power = juce::jmap(
			juce::jlimit(0.0f, (float)FFTFilter::fftSize, (float)formant_table.get()[pos]),
			0.0f, (float)FFTFilter::fftSize, 0.0f, 1.0f
		);
		level.get()[i] = power;
	}
	return level;
}


void PitchFilter::update_table()
{
	int fft_size = FFTFilter::fftSize;
	int fq_size = fft_size / 2;
	float step_fq = (float)((sample_rate / 2.0) / fq_size);
	for (auto i = 0; i < fft_size; i++) {
		shift_table.get()[i] = i;
		formant_table.get()[i] = i;
	}
	// -----------------------------
	// frequency shift
	// -----------------------------
	int index = 0;
	for (auto i = 0; i < fq_size; i++) {
		if (((step_fq * i) < shift_freq) && (i > 0)) {
			shift_table.get()[i] = -1;
		}
		else if (((i % 2) == 0) && (index == 1)) {
			shift_table.get()[i] = -1;
		}
		else {
			shift_table.get()[i] = index;
			index++;
		}
	}
	// -----------------------------
	// pitch shift
	// -----------------------------
	float step_shift1 = step_fq;
	float step_shift2 = step_fq;
	if (pitch_shift < 0) {
		// shift to lower
		step_shift1 = (float)(step_fq * (1.0f - pitch_shift));
	}
	else {
		// shift to higher
		step_shift1 = (float)(step_fq / (1.0f + pitch_shift));
	}
	if (formant_shift < 0) {
		// shift to lower
		step_shift2 = (float)(step_shift1 * (1.0f - formant_shift));
	}
	else {
		// shift to higher
		step_shift2 = (float)(step_shift1 / (1.0f + formant_shift));
	}
	// - build table
	{
		float fq = 0.0f;
		int post_index = 0;
		for (auto i = 0; i < fq_size; i++) {
			if (i == 0) {
				formant_table.get()[i] = 0;
				fq += step_fq;
			}
			//			else if ((step_fq * i) < (voice_basefreq + shift_freq)) {
			//				formant_table.get()[i] = -1;
			//				fq += step_fq;
			//			}
			else if (fq < voice_basefreq) {
				int new_index = (int)(fq / step_fq);
				if ((i % 2) != (new_index % 2)) {
					new_index += 1;
				}
				formant_table.get()[i] = shift_table.get()[new_index];
				post_index = new_index;
				fq += step_shift1;
			}
			else if (fq < (sample_rate / 2.0)) {
				int new_index = (int)(fq / step_fq);
				if ((i % 2) != (new_index % 2)) {
					new_index += 1;
				}
				formant_table.get()[i] = shift_table.get()[new_index];
				post_index = new_index;
				fq += step_shift2;
			}
			else {
				formant_table.get()[i] = -1;
			}
		}
		formant_table.get()[fq_size] = shift_table.get()[fq_size];
	}
	// -----------------------------
	// cpmplement table to complex conjugate
	// -----------------------------
	formant_table.get()[0] = 0;
	formant_table.get()[fq_size] = fq_size;
	for (auto i = 0; i < fq_size - 1; i++) {
		int sindex = formant_table.get()[i + 1];
		if (sindex == -1) {
			formant_table.get()[fft_size - 1 - i] = sindex;
		}
		else {
			formant_table.get()[fft_size - 1 - i] = fft_size - sindex;
		}
	}
	// -----------------------------
	// update pitch table
	// -----------------------------
	{
		std::lock_guard<std::mutex> lock(flag_lock);
		for (auto i = 0; i < fft_size; i++) {
			pitch_table.get()[i] = formant_table.get()[i];
		}
	}
}

float PitchFilter::analyze_basefreq()
{
	float freq = 60.0f;
	float filtered_freq = 60.0f;
	float peak_gain = 0.0f;
	int peak = 0;

	for (auto i = segment_length / 4; i > 1; i--) {
		if (fftbuf_in.get()[i] > 50.0) {
			if (fftbuf_in.get()[i] > peak_gain) {
				peak = i;
				peak_gain = fftbuf_in.get()[i];
			}
		}
	}
	if (peak > 0) {
		auto freq1 = (peak) * ((sample_rate / 2.0) / (segment_length / 2.0));
		auto freq2 = (peak + 1) * ((sample_rate / 2.0) / (segment_length / 2.0));
		auto pow1 = fftbuf_in.get()[peak];
		auto pow2 = fftbuf_in.get()[peak + 1];
		freq = (float)((freq1 * pow1 + freq2 * pow2) / ((double)pow1 + (double)pow2));
		freq = juce::jmax(freq, 60.0f);
	}
	if (freq > 60.0f) {
		base_freq_filter[base_freq_filter_count] = freq;
		base_freq_filter_count = (base_freq_filter_count + 1) % base_analyzer_filter_size;
		filtered_freq = 0.0f;
		for (auto i = 0; i < base_analyzer_filter_size; i++) {
			filtered_freq += base_freq_filter[i];
		}
		filtered_freq /= base_analyzer_filter_size;
	}
	return filtered_freq;
}


void PitchFilter::effect(const juce::HeapBlock<juce::dsp::Complex<float>>& in_data, juce::HeapBlock<juce::dsp::Complex<float>>& out_data, int size, int side)
{
	{
		std::lock_guard<std::mutex> lock(flag_lock);
		// pitch shifting
		int target;
		for (int i = 0; i < size; i++) {
			if (pitch_table.get()[i] < 0) {
				out_data.get()[i].real(0);
				out_data.get()[i].imag(0);
			}
			else {
				target = pitch_table.get()[i];
				out_data.get()[i].real(in_data.get()[target].real());
				out_data.get()[i].imag(in_data.get()[target].imag());
			}
		}
	}
}