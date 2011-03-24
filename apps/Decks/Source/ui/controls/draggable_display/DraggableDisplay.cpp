/*
 *  DraggableDisplay.cpp
 *  Decks
 *
 *  Created by David Rowland on 05/03/2010.
 *  Copyright 2010 dRowAudio. All rights reserved.
 *
 */

#include "DraggableDisplay.h"

DraggableDisplay::DraggableDisplay()
:	noDecks(Settings::getInstance()->getPropertyOfChild("noChannels", "noChannels")),
	settings(DeckManager::getInstance())
{
	Settings::getInstance()->getValueTreePointer()->addListener(this);
	thumbnailCache = new MultipleAudioThumbnailCache(noDecks);
	
	for (int i = 0; i < noDecks; i++)
	{
		draggableWaveDisplays.add(new SwitchableDraggableWaveDisplay(DeckManager::getInstance()->getDeck(i)->getMainFilePlayer(), thumbnailCache));
		addAndMakeVisible(draggableWaveDisplays[i]);
	}
	
	// create the zoom & playhead sliders
	addAndMakeVisible( zoomSlider = new Slider("zoomSlider") );
	zoomSlider->setSliderStyle(Slider::LinearVertical);
	zoomSlider->setRange(-20.0, 0.999999, 0.0001);
//	zoomSlider->setValue(0.0f);
	zoomSlider->setSkewFactorFromMidPoint(0.0);	
	zoomSlider->getValueObject().referTo(Settings::getInstance()->getPropertyOfChildAsValue(UISettings::DraggableDisplaySettings::SectionName, DRAGGABLE_SETTING(zoom)));
	zoomSlider->addListener(this);
	
	addAndMakeVisible( playheadPosSlider = new Slider("playheadPosSlider") );
	playheadPosSlider->setSliderStyle(Slider::LinearVertical);
	playheadPosSlider->setRange(0.0, 1.0, 0.0001);
//	playheadPosSlider->setValue(0.5);
	playheadPosSlider->getValueObject().referTo(Settings::getInstance()->getPropertyOfChildAsValue(UISettings::DraggableDisplaySettings::SectionName, DRAGGABLE_SETTING(centrePos)));
	playheadPosSlider->addListener(this);
}

DraggableDisplay::~DraggableDisplay()
{		
	Settings::getInstance()->getValueTreePointer()->removeListener(this);
	zoomSlider->removeListener(this);
	playheadPosSlider->removeListener(this);
	
	draggableWaveDisplays.clear();
	
	deleteAllChildren();
	DBG("DraggableDisplay deleted");
}

//================================================================
void DraggableDisplay::resized()
{
	const int height = getHeight();
	const int width = getWidth();
	const int margin = 3;
	const int sliderWidth = 20;
	
	zoomSlider->setBounds(margin, margin,
						  sliderWidth, height-2*margin);
	
	playheadPosSlider->setBounds(width-margin-sliderWidth, margin,
								 sliderWidth, height-2*margin);				
	
	
	//================================================================
	const int noDecks = Settings::getInstance()->getPropertyOfChild("noChannels", "noChannels");
	int noEnabledDecks = DeckManager::getNoEnabledDecks();
	
	if (noEnabledDecks)
	{
		Value tempTrue(true);
		const int waveDisplayWidth = (playheadPosSlider->getX()-zoomSlider->getRight())-2*margin;
		const int waveDisplayHeight = (height - (noEnabledDecks+1)*margin)/noEnabledDecks;
		
		int pos = 0;
		for (int i = 0; i < noDecks; i++)
		{
			if (Settings::getInstance()->getPropertyOfChannel(i, CHANNEL_SETTING(on)) == tempTrue) {
				draggableWaveDisplays[i]->setVisible(true);
				draggableWaveDisplays[i]->setBounds(zoomSlider->getRight()+margin, margin+(pos*(waveDisplayHeight+margin)), waveDisplayWidth, waveDisplayHeight);
				pos++;
			}
			else
				draggableWaveDisplays[i]->setVisible(false);
		}
	}
	else {
		for (int i = 0; i < noDecks; i++)
			draggableWaveDisplays[i]->setVisible(false);
	}
	//================================================================
	
	// set up first display for other to run off
	//		int firstDisplayNo = 0;
	//		for (int i = 0; i < noDecks; i++) {
	//			if (draggableWaveDisplays[i])
	//				firstDisplayNo = i;
	//			break;
	//		}
	//		
	//		draggableWaveDisplays[firstDisplayNo]->setBounds(zoomSlider->getRight()+margin, margin,
	//														 (playheadPosSlider->getX()-zoomSlider->getRight())-2*margin,
	//														 waveDisplayHeight);
	//		int nextDisplayNo = firstDisplayNo;
	//		for (int i = firstDisplayNo+1; i < noDecks; i++)
	//		{
	//			if (draggableWaveDisplays[i])
	//				draggableWaveDisplays[i]->setBounds(zoomSlider->getRight()+margin, draggableWaveDisplays[nextDisplayNo]->getBottom()+margin,
	//													(playheadPosSlider->getX()-zoomSlider->getRight())-2*margin, waveDisplayHeight);
	//			nextDisplayNo = i;
	//		}
}

void DraggableDisplay::paint (Graphics &g)
{
	g.setColour(DecksColours::getInstance()->getColour(DecksColours::panelColour));
	g.fillAll();
}

//================================================================
void DraggableDisplay::sliderValueChanged(Slider *slider)
{
	if (slider == zoomSlider)
	{
		const double zoomFactor = 1.0f - zoomSlider->getValue();
		for (int i = 0; i < noDecks; i++)
			draggableWaveDisplays[i]->setZoomFactor(zoomFactor);
	}
	else if (slider == playheadPosSlider)
	{
		for (int i = 0; i < noDecks; i++)
			draggableWaveDisplays[i]->setPlayheadPosition(playheadPosSlider->getValue());
	}
}

void DraggableDisplay::valueTreePropertyChanged (ValueTree  &treeWhosePropertyHasChanged, const var::identifier  &property)
{
	for (int i = 0; i < int(Settings::getInstance()->getPropertyOfChild("noChannels", "noChannels")); i++) {
		if (treeWhosePropertyHasChanged.getProperty(property) == Settings::getInstance()->getPropertyOfChannelAsValue(i, CHANNEL_SETTING(on))) {
			resized();
		}
//		else if (treeWhosePropertyHasChanged.getProperty(property) == Settings::getInstance()->getPropertyOfChannelAsValue(i, CHANNEL_SETTING(on))) {
//			resized();
//		}
	}
}

void DraggableDisplay::valueTreeParentChanged (ValueTree &treeWhoseParentHasChanged)
{
}
