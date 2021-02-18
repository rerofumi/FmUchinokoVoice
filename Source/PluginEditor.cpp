/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Fm_uchinoko_voiceAudioProcessorEditor::Fm_uchinoko_voiceAudioProcessorEditor (Fm_uchinoko_voiceAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(536, 512);
    addAndMakeVisible(gui_component);
    startTimerHz(10);
    gui_component.setSize(this->getWidth(), this->getHeight());
    gui_component.setProcesser(p);
}

Fm_uchinoko_voiceAudioProcessorEditor::~Fm_uchinoko_voiceAudioProcessorEditor()
{
}

//==============================================================================
void Fm_uchinoko_voiceAudioProcessorEditor::paint (juce::Graphics& g)
{
}

void Fm_uchinoko_voiceAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void Fm_uchinoko_voiceAudioProcessorEditor::timerCallback()
{
    repaint();
}