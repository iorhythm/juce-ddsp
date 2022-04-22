/*
  ==============================================================================

	CustomLookAndFeel.h
	Created: 18 Nov 2020 11:52:50am
	Author:  Erik Frej Knudsen

  ==============================================================================
*/
#pragma once

#include <JuceHeader.h>


class OtherLookAndFeel : public LookAndFeel_V4
{
public:
	OtherLookAndFeel()
	{
	}


	void drawRotarySlider (Graphics &g, int x, int y, int width, int height, 
		float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, 
		Slider &slider) override
	{
		// values to feed the method
		const auto radius = (float) jmin (width / 2, height / 2) - 4.0f;
		const auto centreX = (float) x + (float) width  * 0.5f;
		const auto centreY = (float) y + (float) height * 0.5f;
		const auto rx = centreX - radius;
		const auto ry = centreY - radius;
		const auto rw = radius * 2.0f;
		const auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
		
		// fill
		g.setColour (Colours::grey);
		g.fillEllipse (rx, ry, rw, rw);
 
		// outline
		g.setColour (Colours::white);
		g.drawEllipse (rx, ry, rw, rw, 1.7f);
		
		// slider pointer
		const auto pointerLength = radius * 0.50f;
		const auto pointerThickness = 3.0f;

		Path p;
		p.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
		p.applyTransform (AffineTransform::rotation (angle).translated (centreX, centreY));
		
		g.setColour (Colours::white);
		g.fillPath (p);
	}
	
	void drawRoundThumb (Graphics& g, float x, float y, float diameter, Colour colour, float outlineThickness)
	{
		auto halfThickness = outlineThickness * 0.5f;

		Path p;
		p.addEllipse (x + halfThickness,
					  y + halfThickness,
					  diameter - outlineThickness,
					  diameter - outlineThickness);

		g.setColour (Colours::white);
		g.fillPath (p);

		g.setColour (colour.brighter());
		g.strokePath(p, PathStrokeType(outlineThickness));
	}
	
	void drawTickBox (Graphics& g, Component& component,
					  float x, float y, float w, float h,
					  bool ticked,
					  bool isEnabled,
					  bool isMouseOverButton,
					  bool isButtonDown) override
	{
		auto boxSize = w * 0.7f;

		auto isDownOrDragging = component.isEnabled() && (component.isMouseOverOrDragging() || component.isMouseButtonDown());

		auto colour = component.findColour (TextButton::buttonColourId)
							   .withMultipliedSaturation ((component.hasKeyboardFocus (false) || isDownOrDragging) ? 1.3f : 0.9f)
							   .withMultipliedAlpha (component.isEnabled() ? 1.0f : 0.7f);

		drawRoundThumb (g, x, y + (h - boxSize) * 0.5f, boxSize, colour,
						isEnabled ? ((isButtonDown || isMouseOverButton) ? 1.1f : 0.5f) : 0.3f);

		if (ticked)
		{
			g.setColour (isEnabled ? findColour (TextButton::buttonOnColourId) : Colours::grey);

			auto scale = 9.0f;
			auto trans = AffineTransform::scale (w / scale, h / scale).translated (x - 2.5f, y + 1.0f);

			g.fillPath (LookAndFeel_V4::getTickShape (6.0f), trans);
		}
	}
};