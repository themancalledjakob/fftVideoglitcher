#pragma once

#include "ofMain.h"
#include "ofxFft.h"

#define MIC 0
#define NOISE 1
#define SINE 2

#define EQSIZE 32

struct FftData {
	float eq[EQSIZE];
	float amp = 0;
	float pitch = 0;
	float dirty = 0;
};

class fftHandler : public ofBaseApp, public ofThread {
public:
	void setup();
	void plot(float* array, int length, float scale, float offset);
    void plotColorRect();
    void plotColorRect(int x, int y, int w, int h);
    void plotColorRect(float* array, int length, float x, float y, float width, float height);
    void plotColorRectInverted();
    void plotColorRectInverted(float* array, int length, float x, float y, float width, float height);
	void audioReceived(float* input, int bufferSize, int nChannels);
	void audioOut(float* input, int bufferSize, int nChannels);
	void draw();
	void drawInverted();
	void keyPressed(int key);
    void mousePressed(int x, int y);
    FftData getData();
    
	int plotHeight, bufferSize;
    
	ofxFft* fft;
    
	float* audioInput;
	float* fftOutput;
	float* eqFunction;
	float* eqOutput;
	float* ifftOutput;
    
	float appWidth;
	float appHeight;
    
	int mode;
    
    float totalAmplitude;
    float eqAmplitudes[EQSIZE];
    float pitch;
    float dirty;
    
    bool useInput; // trigger calculation
    bool useOutput; // trigger calculation
    
    ofColor drawColor;
    ofFbo fbo;
    ofFbo fboi;
    
    float intensity;
};
