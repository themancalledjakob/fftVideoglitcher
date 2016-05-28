#pragma once

#include "ofMain.h"
// #include "ofxFft.h"
#include "fftHandler.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void audioReceived(float* input, int bufferSize, int nChannels);
	void audioOut(float* output, int bufferSize, int nChannels);
	void update();
	void draw();
    
    void keyPressed(int key);
    void mousePressed(int x, int y, int button);

	// int plotHeight, bufferSize;

	// ofxFft* fft;
	
	// ofMutex soundMutex;
	// vector<float> drawBins, middleBins, audioBins;
    fftHandler fft;

    ofShader shader;
    
    ofVideoPlayer	movie;
};
