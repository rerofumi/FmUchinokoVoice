#include "FilterBase.h"

FilterBase::FilterBase(int segment_length, int buffer_length) :
	segment_length(segment_length),
	buffer_length(buffer_length),
	input_pointer(0),
	output_buffer_pointer(0),
	output_pointer(0)
{
	input_buffer[0] = new float[segment_length];
	input_buffer[1] = new float[segment_length];
	output_buffer[0] = new float[buffer_length];
	output_buffer[1] = new float[buffer_length];
	filter_buffer[0] = new float[segment_length];
	filter_buffer[1] = new float[segment_length];
}

FilterBase::~FilterBase()
{
	delete[] input_buffer[0];
	delete[] input_buffer[1];
	delete[] output_buffer[0];
	delete[] output_buffer[1];
	delete[] filter_buffer[0];
	delete[] filter_buffer[1];
}

void FilterBase::setProcessFlag(bool flag)
{
	std::lock_guard<std::mutex> lock(flag_lock);
	process_done = flag;
}

bool FilterBase::getProcessFlag()
{
	std::lock_guard<std::mutex> lock(flag_lock);
	return process_done;
}

void FilterBase::process(float* buffer_1, float* buffer_2, int size)
{
	for (size_t copy_counter = 0; copy_counter < size; copy_counter++) {
		input_buffer[0][input_pointer] = buffer_1[copy_counter];
		input_buffer[1][input_pointer] = buffer_2[copy_counter];
		input_pointer = (input_pointer + 1) % segment_length;
		if (input_pointer == 0) {
			filter_process(input_buffer[0], filter_buffer[0], segment_length, 0);
			filter_process(input_buffer[1], filter_buffer[1], segment_length, 1);
			for (size_t i = 0; i < segment_length; i++) {
				output_buffer[0][output_buffer_pointer] = filter_buffer[0][i];
				output_buffer[1][output_buffer_pointer] = filter_buffer[1][i];
				output_buffer_pointer = (output_buffer_pointer + 1) % buffer_length;
			}
			setProcessFlag(true);
		}
	}
}

int FilterBase::output(float* buffer_1, float* buffer_2, int size)
{
	for (size_t counter = 0; counter < size; counter++) {
		buffer_1[counter] = 0;
		buffer_2[counter] = 0;
	}
	int filtered_size = (output_buffer_pointer + buffer_length - output_pointer) % buffer_length;
	if (filtered_size < size) {
		return 0;
	}
	for (size_t counter = 0; counter < size; counter++) {
		buffer_1[counter] = output_buffer[0][output_pointer];
		buffer_2[counter] = output_buffer[1][output_pointer];
		output_pointer = (output_pointer + 1) % buffer_length;
	}
	return size;
}

void FilterBase::filter_process(float *in_data, float *out_data, int size, int side)
{
	// sample: copy only
	for (int i = 0; i < size; i++) {
		out_data[i] = in_data[i];
	}
}

