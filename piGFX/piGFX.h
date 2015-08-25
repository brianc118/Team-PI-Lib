/*
 * piGFX.cpp - graphis library for Team PI
 * 
 * by Brian Chen
 * (C) Team PI 2015
 */

#ifndef PIGFX
#define PIGFX

#include <Arduino.h>
#include <ILI9341_t3.h>
#include <Adafruit_FT6206.h>

class BUTTON{
public:
	bool toggleBorderMode = false;
	bool touched = false;
	bool lTouched = false;
	bool pressed = false;
	bool released = false;
	bool enabled = true;

	BUTTON(ILI9341_t3 *tft, POINT *point){
		myTft = tft;
		myPoint = point;
	}
	void setBounds(uint16_t _x1, uint16_t _y1, uint16_t _x2, uint16_t _y2){
		x1 = _x1;
		x2 = _x2;
		y1 = _y1;
		y2 = _y2;
		width = x2 - x1;
		height = y2 - y1;
	}
	void setText(String _str, uint8_t _textSize, uint16_t _textColour = ILI9341_WHITE){
		str = _str;
		textSize = _textSize;
		textColour = _textColour;

	}
	void setColour(uint16_t _colour){
		if (_colour != currentColour){
			colourToChange = true;
		}
		else{
			colourToChange = false;
		}
		colour = _colour;
	}
	void setBorderColour(uint16_t _borderColour){
		borderColour = _borderColour;
	}
	void draw(bool forceDraw = true){
		if (colourToChange || forceDraw){
			uint16_t textWidth = str.length() * (textSize * 5 + 1);
			uint16_t textHeight = textSize * 8;
			currentColour = colour;
			myTft->fillRect(x1, y1, x2 - x1, y2 - y1, colour);
			myTft->setTextSize(textSize);
			myTft->setTextColor(textColour);
			myTft->setCursor((x1 + x2 - textWidth)/2, (y1 + y2 - textHeight)/2);
			myTft->print(str);
		}
		colourToChange = false;
	}
	void erase(){
		myTft->fillRect(x1, y1, x2 - x1, y2 - y1, backColour);
	}
	void checkTouch(){
		if (enabled){
			touched = false;
			if (myPoint->x != 0 || myPoint->y != 0){
				if (myPoint->x > x1 && myPoint->x < x2
				 && myPoint->y > y1 && myPoint->y < y2){
					// touched!
					touched = true;
				}
			}
			// draw border of touched!
			pressed = (touched && !lTouched);
			released = (!touched && lTouched);
			if (!toggleBorderMode){
				if (pressed){
					drawBorder();
					//Serial.println("pressed");
				}
				else if (released){
					// if just been released remove border
					eraseBorder();
					//Serial.println("released");
				}
			}
			else{
				if (pressed){
					if (borderDrawn){
						eraseBorder();
					}
					else{
						drawBorder();
					}
				}
			}

			// Serial.print(touched);
			// Serial.print('\t');
			// Serial.println(lTouched);

			lTouched = touched;
		}
	}

	void drawBorder(){
		myTft->drawRect(x1, y1, width, height, borderColour);
		borderDrawn = true;
	}
	void eraseBorder(){
		myTft->drawRect(x1, y1, width, height, colour);
		borderDrawn = false;
	}

private:
	ILI9341_t3 *myTft;
	POINT *myPoint;

	uint16_t x1, y1, x2, y2;
	uint16_t width;
	uint16_t height;
	uint16_t colour = ILI9341_WHITE;
	uint16_t currentColour = ILI9341_WHITE;
	bool colourToChange = false;
	uint16_t backColour = ILI9341_BLACK;
	uint8_t textSize = 1;
	uint16_t textColour = ILI9341_GREEN;
	String str;

	uint16_t borderColour = ILI9341_WHITE;

	bool borderDrawn = false;
};

#endif