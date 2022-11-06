
#include <avr/io.h>
#include <util/delay.h>
#include <EEPROM.h>


#define PIXEL_NUM (2)
#define ws2812_resettime  50
#define ws2812_port B     // Data port
#define ws2812_pin  3     // Data out pin

#define MAX_ANIM 5
#define BRIGHTNESS	64
#define DELAYVAL	5

uint8_t i = 50;
uint8_t j = 0;
uint8_t mode = 2;

void ws2812_setleds     (struct cRGB  *ledarray, uint16_t number_of_leds);
void ws2812_setleds_pin (struct cRGB  *ledarray, uint16_t number_of_leds,uint8_t pinmask);
void ws2812_setleds_rgbw(struct cRGBW *ledarray, uint16_t number_of_leds);

struct pixel {
	uint8_t g;
	uint8_t r;
	uint8_t b;
} pixels[PIXEL_NUM];

struct pixel rgb2uint32(byte r, byte g, byte b){
	struct pixel p = {g,r,b};
	return p;
}

struct pixel Wheel(byte WheelPos) {
	if(WheelPos == 0){
		return rgb2uint32(0,0,0);
	}
	else if(WheelPos < 85) {
		return rgb2uint32(WheelPos * 3, 255 - WheelPos * 3, 0);
	} else if(WheelPos < 170) {
		WheelPos -= 85;
		return rgb2uint32(255 - WheelPos * 3, 0, WheelPos * 3);
	} else {
		WheelPos -= 170;
		return rgb2uint32(0, WheelPos * 3, 255 - WheelPos * 3);
	}
}

void dim(uint8_t bright) {
	for (int i=0; i< PIXEL_NUM*3; i++) {
		((uint8_t*)pixels)[i] = (uint16_t)((uint8_t*)pixels)[i] * bright / 255;
		// pixels[i].r = map(pixels[i].r, 0, 255, 0, bright);
		// pixels[i].g = map(pixels[i].g, 0, 255, 0, bright);
		// pixels[i].b = map(pixels[i].b, 0, 255, 0, bright);
	}
}

void show(uint32_t d) {
	ws2812_setleds((struct cRGB*)pixels, (uint16_t)PIXEL_NUM);
	delay(d);
}

void setAllcolor(uint8_t r, uint8_t g, uint8_t b, uint32_t d, uint8_t bright) {
	pixels[0] = { g,r,b };
	pixels[1] = pixels[0];
	dim(bright);
	show(d);
}

void rainbow() {
	for (int i = 0; i < PIXEL_NUM; i++)
		pixels[i] = Wheel(j + i * 10);
	j++;
	dim(BRIGHTNESS);
	ws2812_setleds((struct cRGB*)pixels, (uint16_t)PIXEL_NUM);
	delay(10);
}

void heart_beat(struct pixel color) {
	uint8_t r = color.r;
	uint8_t g = color.g;
	uint8_t b = color.b;
	int x = 3;
	for (int ii = 1; ii < 252; ii = ii = ii + x) {
		setAllcolor(r,g,b,5,ii);
	}

	x = 3;
	for (int ii = 252; ii > 3; ii = ii - x) {
		setAllcolor(r, g, b, 3, ii);
	}
	delay(10);

	x = 6;
	for (int ii = 1; ii < 255; ii = ii + x) {
		setAllcolor(r, g, b, 2, ii);
	}
	x = 6;
	for (int ii = 255; ii > 1; ii = ii - x) {
		setAllcolor(r, g, b, 3, ii);
	}
	delay(50);
}

void setup() {
	mode = EEPROM.read(0);
	mode = mode + 1;
	if (mode > MAX_ANIM) {
		mode = 0;
	}
	EEPROM.write(0, mode);
}

void loop() {
	switch (mode) {
		case 0:
			rainbow();
			break;
		case 1:
			heart_beat(rgb2uint32(255,0,0));
			break;
		case 2:
			heart_beat(rgb2uint32(0, 255, 0));
			break;
		case 3:
			heart_beat(rgb2uint32(0, 0, 255));
			break;
		case 4:
			heart_beat(Wheel(i+=5));
			break;
		case 5:
			setAllcolor(255, 0, 0, 20, 255);
			setAllcolor(0, 0, 0, 2000, 255);
			break;
	}
}
