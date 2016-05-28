#include "ofApp.h"
float timer = 0.0f;

void ofApp::setup() {
	ofSetVerticalSync(true);
		
	ofBackground(0, 0, 0);
	ofSetBackgroundAuto(false);
    // fft.startThread();
    fft.setup();


	// movie.load("movies/bambi.mp4");
	movie.load("movies/unicorn.mp4");
	movie.setLoopState(OF_LOOP_NORMAL);
	movie.play();
	// movie.setVolume(0.0);

	shader.load("shadersGL2/shader");
	timer = 0.0;
}

void ofApp::update() {
	fft.update();
	movie.update();
}

void ofApp::draw() {
	timer += fft.totalAmplitude * 10000.0;
    // ofTranslate(-220, -900);
    // ofScale(4.0, 4.0);
    // ofBackground(25,25,25);
	// fft.plotColorRect();

	// movie.setSpeed(1.0);//fft.totalAmplitude * 1000.0 + 0.8);

	shader.begin();

    shader.setUniform4f("lowbands", fft.eqOutput[0], fft.eqOutput[1], fft.eqOutput[2], fft.eqOutput[3]);
    shader.setUniform4f("highbands", fft.eqOutput[4], fft.eqOutput[5], fft.eqOutput[6], fft.eqOutput[7]);

    shader.setUniform1f("time", ofGetElapsedTimeMillis() * 0.1 + timer);
    // we can pass in a single value into the shader by using the setUniform1 function.
    // if you want to pass in a float value, use setUniform1f.
    // if you want to pass in a integer value, use setUniform1i.
    // shader.setUniform1f("mouseRange", 150);
    
    // we can pass in two values into the shader at the same time by using the setUniform2 function.
    // inside the shader these two values are set inside a vec2 object.

	movie.draw(0,0,ofGetWidth(),ofGetHeight());
	shader.end();

	ofDrawBitmapString("POWER!!!" + ofToString(fft.totalAmplitude), 22, 20);
	int amt = sizeof(fft.eqOutput);
	ofSetColor(255,255,0);
	for(int i = 0; i < amt; i++) {
		ofRect(50, i * 20 + 50, fft.eqOutput[i] * 2550.0f + 1, 10);
	} 
}
void ofApp::audioReceived(float* input, int bufferSize, int nChannels) {	
	fft.audioReceived(input, bufferSize, nChannels);
}
void ofApp::audioOut(float* output, int bufferSize, int nChannels) {	
	fft.audioOut(output, bufferSize, nChannels);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    fft.keyPressed(key);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    fft.mousePressed(x, y);
}
