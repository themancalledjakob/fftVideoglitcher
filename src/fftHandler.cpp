#include "fftHandler.h"

void fftHandler::setup() {
	plotHeight = ofGetWindowHeight();
	bufferSize = 512;
    
	fft = ofxFft::create(bufferSize, OF_FFT_WINDOW_BARTLETT);
	// To use with FFTW, try:
	// fft = ofxFft::create(bufferSize, OF_FFT_WINDOW_BARTLETT, OF_FFT_FFTW);
    
	audioInput = new float[bufferSize];
	fftOutput = new float[fft->getBinSize()];
	eqFunction = new float[fft->getBinSize()];
	eqOutput = new float[fft->getBinSize()];
	ifftOutput = new float[bufferSize];

	// 0 output channels,
	// 1 input channel
	// 44100 samples per second
	// [bins] samples per buffer
	// 4 num buffers (latency)
    
	// this describes a linear low pass filter
	for(int i = 0; i < fft->getBinSize(); i++){
		eqFunction[i] = (float) (fft->getBinSize() - i) / (float) fft->getBinSize();
        eqFunction[i] *= eqFunction[i];
    }
    
	mode = MIC;
	appWidth = ofGetWidth();
	appHeight = ofGetHeight();
    
	ofSoundStreamSetup(0, 1, this, 44100, bufferSize, 4);
    
	ofBackground(0, 0, 0);
    
    totalAmplitude = 0.0f;
    pitch = 0.0f;
    drawColor = ofColor(255,0,255);
    useInput = true;
    useOutput = false;

    fbo.allocate(ofGetWindowWidth(), ofGetWindowHeight());
    fboi.allocate(ofGetWindowWidth(), ofGetWindowHeight());
    
    intensity = 4;
}

void fftHandler::draw() {
    fbo.begin();
    plotColorRect();
    fbo.end();
//    fbo.draw(0, 0);
}
void fftHandler::drawInverted() {
    fboi.begin();
    plotColorRectInverted();
    fboi.end();
}

void fftHandler::plot(float* array, int length, float scale, float offset) {
	ofNoFill();
	glPushMatrix();
	glTranslatef(0, (plotHeight/2) + offset, 0);
    ofPushStyle();
    
    float step = (float)ofGetWindowWidth() / (float)length;
	for (int i = 0; i < length; i++){
        ofSetColor(255,255,255);
        ofCircle((float)i*step, array[i] * scale,1);
    }
    
    ofPopStyle();
	glPopMatrix();
}

ofColor getColorInSpec(int w){
    
    float R = 0.0;
    float G = 0.0;
    float B = 0.0;
    
    /*
     c      RGB VALUES FOR VISIBLE WAVELENGTHS   by Dan Bruton (astro@tamu.edu)
     c
     c      This program can be found at
     c      http://www.physics.sfasu.edu/astro/color.html
     c      and was last updated on February 20, 1996.
     c
     c      This program will create a ppm (portable pixmap) image of a spectrum.
     c      The spectrum is generated using approximate RGB values for visible
     c      wavelengths between 380 nm and 780 nm.
     c      NetPBM's ppmtogif can be used to convert the ppm image
     c      to a gif.  The red, green and blue values (RGB) are
     c      assumed to vary linearly with wavelength (for GAMMA=1).
     c      NetPBM Software: ftp://ftp.cs.ubc.ca/ftp/archive/netpbm/
     c
     */
    if (w >= 380 &&   w < 440){
        R = -(w - 440.) / (440. - 380.);
        G = 0.0;
        B = 1.0;
    } else if ( w >= 440 and w < 490 ) {
        R = 0.0;
        G = (w - 440.) / (490. - 440.);
        B = 1.0;
    } else if ( w >= 490 and w < 510 ) {
        R = 0.0;
        G = 1.0;
        B = -(w - 510.) / (510. - 490.);
    } else if ( w >= 510 and w < 580 ) {
        R = (w - 510.) / (580. - 510.);
        G = 1.0;
        B = 0.0;
    } else if ( w >= 580 and w < 645 ) {
        R = 1.0;
        G = -(w - 645.) / (645. - 580.);
        B = 0.0;
    } else if ( w >= 645 and w <= 780 ) {
        R = 1.0;
        G = 0.0;
        B = 0.0;
    }
    return ofColor(R*255.0f, G*255.0f, B*255.0f);
}

void fftHandler::plotColorRect() {
    plotColorRect(eqOutput, EQSIZE, 0, 0, ofGetWindowWidth(), ofGetWindowHeight());
}

void fftHandler::plotColorRect(int x, int y, int w, int h) {
    plotColorRect(eqOutput, EQSIZE, x, y, w, h);
}

void fftHandler::plotColorRect(float* array, int length, float x, float y, float width, float height) {
    int cropfactor = 8;
    int crop = fft->getBinSize()/cropfactor;
    
    float pwidth = width / (float)crop;
    ofPushMatrix();
    ofPushStyle();
    for (int i = 0 ; i < crop; i++) {
        float ratio = 400.0f / (float)crop;
        //        ofColor colorise;
        //        colorise.setHsb((i*ratio), min(array[i]*2555.0f,255.0f), min(array[i]*2555.0f,255.0f));
        ofFill();
        ofColor fillColor = getColorInSpec(((int)(i*ratio)+380));
        fillColor.setBrightness( max( min( array[i]*2550.0f*intensity, 255.0f ), 10.0f ) );
        ofSetColor( fillColor );
        ofRect(pwidth*i,0,pwidth,ofGetWindowHeight());
    }
    ofPopStyle();
    ofPopMatrix();
}

void fftHandler::plotColorRectInverted() {
    plotColorRectInverted(eqOutput, EQSIZE, 0, 0, ofGetWindowWidth(), ofGetWindowHeight());
}

void fftHandler::plotColorRectInverted(float* array, int length, float x, float y, float width, float height) {
    int cropfactor = 4;
    int crop = fft->getBinSize()/cropfactor;
    
    float pwidth = width / (float)crop;
    ofPushMatrix();
    ofPushStyle();
    for (int i = 0 ; i < crop; i++) {
        float ratio = 400.0f / (float)crop;
        //        ofColor colorise;
        //        colorise.setHsb((i*ratio), min(array[i]*2555.0f,255.0f), min(array[i]*2555.0f,255.0f));
        ofFill();
        
        int w = ((int)(i*ratio)+380);
        
        /*
        w -= 380;
        w = abs(w - 400);
        w += 380;
        */
        
        ofColor fillColor = getColorInSpec(w);
        fillColor.setBrightness( max( min( array[i]*2550.0f*intensity, 255.0f ), 10.0f ) );
        ofSetColor( fillColor );
        ofRect(pwidth*i,0,pwidth,ofGetWindowHeight());
    }
    ofPopStyle();
    ofPopMatrix();
}

void fftHandler::audioReceived(float* input, int bufferSize, int nChannels) {
    
    if (useInput){
        if (mode == MIC) {
            // store input in audioInput buffer
            memcpy(audioInput, input, sizeof(float) * bufferSize);
        } else if (mode == NOISE) {
            for (int i = 0; i < bufferSize; i++)
                audioInput[i] = ofRandom(-1, 1);
        } else if (mode == SINE) {
            for (int i = 0; i < bufferSize; i++)
                audioInput[i] = sinf(PI * i * mouseX / appWidth);
        }
        
        fft->setSignal(audioInput);
        memcpy(fftOutput, fft->getAmplitude(), sizeof(float) * fft->getBinSize());
        
        for(int i = 0; i < fft->getBinSize(); i++)
            eqOutput[i] = fftOutput[i];// * eqFunction[i];
        
        fft->setPolar(eqOutput, fft->getPhase());
        
        fft->clampSignal();
        memcpy(ifftOutput, fft->getSignal(), sizeof(float) * fft->getSignalSize());
        
        // get amplitudes total & equalized with a couple of bins (amount defined in EQSIZE)
        float totalAmplitude_tmp = 0;
        float pitch_tmp = 0.0f;
        int groupSize = fft->getBinSize() / EQSIZE;
        float eqAmplitudes_tmp[EQSIZE];
        
        // get loudestBin for dirty
        float loudestBin = 0;
        dirty = 0;
        
        // get total Amplitude
        for (int j = 0; j < EQSIZE; j++ ){
            int start_amp = j * groupSize;
            int stop_amp = start_amp + groupSize;
            
            for (int i = start_amp; i < stop_amp; i++){
                // everything here is done as often as there are original bins
                totalAmplitude_tmp += fft->getAmplitude()[i];
                eqAmplitudes_tmp[j] += fft->getAmplitude()[i];
                if (fft->getAmplitude()[i] > loudestBin) // now we seriously get the loudest bin
                    loudestBin = fft->getAmplitude()[i];
                // fucking clever line this one here (1/2)
                pitch_tmp += fft->getAmplitude()[i] * i;
            }
            eqAmplitudes[j] = eqAmplitudes_tmp[j] / groupSize;
        }
        
        totalAmplitude = totalAmplitude_tmp / fft->getBinSize();
        
        // if the signal is clean, the loudes bin equals the totalamplitude. the greater the difference, the dirtier the signal is
        dirty = totalAmplitude_tmp - loudestBin;
        
        // fucking clever line this one here (2/2)
        pitch = pitch_tmp / totalAmplitude_tmp;
    }
}

FftData fftHandler::getData() {
    FftData data;
    memcpy(data.eq, eqOutput, sizeof(float) * sizeof(eqOutput));
    data.amp = totalAmplitude;
    data.pitch = pitch;
    data.dirty = dirty;
    return data;
}


void fftHandler::audioOut(float* output, int bufferSize, int nChannels) {
    
        if (mode == MIC) {
            // store input in audioInput buffer
            memcpy(audioInput, output, sizeof(float) * bufferSize);
        } else if (mode == NOISE) {
            for (int i = 0; i < bufferSize; i++)
                audioInput[i] = ofRandom(-1, 1);
        } else if (mode == SINE) {
            for (int i = 0; i < bufferSize; i++)
                audioInput[i] = sinf(PI * i * mouseX / appWidth);
        }
        
        fft->setSignal(audioInput);
        memcpy(fftOutput, fft->getAmplitude(), sizeof(float) * fft->getBinSize());
        
        for(int i = 0; i < fft->getBinSize(); i++)
            eqOutput[i] = fftOutput[i] * eqFunction[i];
        
        fft->setPolar(eqOutput, fft->getPhase());
        
        fft->clampSignal();
        memcpy(ifftOutput, fft->getSignal(), sizeof(float) * fft->getSignalSize());
        
        // get amplitudes total & equalized with a couple of bins (amount defined in EQSIZE)
        float totalAmplitude_tmp = 0;
        float pitch_tmp = 0.0f;
        int groupSize = fft->getBinSize() / EQSIZE;
        float eqAmplitudes_tmp[EQSIZE];
        
        // get loudestBin for dirty
        float loudestBin = 0;
        dirty = 0;
        
        // get total Amplitude
        for (int j = 0; j < EQSIZE; j++ ){
            int start_amp = j * groupSize;
            int stop_amp = start_amp + groupSize;
            
            for (int i = start_amp; i < stop_amp; i++){
                // everything here is done as often as there are original bins
                totalAmplitude_tmp += fft->getAmplitude()[i];
                eqAmplitudes_tmp[j] += fft->getAmplitude()[i];
                if (fft->getAmplitude()[i] > loudestBin) // now we seriously get the loudest bin
                    loudestBin = fft->getAmplitude()[i];
                // fucking clever line this one here (1/2)
                pitch_tmp += fft->getAmplitude()[i] * i;
            }
            eqAmplitudes[j] = eqAmplitudes_tmp[j] / groupSize;
        }
        
        totalAmplitude = totalAmplitude_tmp / fft->getBinSize();
        
        // if the signal is clean, the loudes bin equals the totalamplitude. the greater the difference, the dirtier the signal is
        dirty = totalAmplitude_tmp - loudestBin;
        
        // fucking clever line this one here (2/2)
        pitch = pitch_tmp / totalAmplitude_tmp;
}

void fftHandler::keyPressed(int key) {
	switch (key) {
        case 'm':
            mode = MIC;
            break;
        case 'n':
            mode = NOISE;
            break;
        case 's':
            mode = SINE;
            break;
	}
}


void fftHandler::mousePressed(int x, int y){
    intensity = int(0.025 * (float)x);
}