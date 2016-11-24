#include "ofApp.h"
//--------------------------------------------------------------
//----------------------  Params -------------------------------
//--------------------------------------------------------------
Params param;        //Definition of global variable
void Params::setup() {
	eCenter = ofPoint(ofGetWidth() / 2, ofGetHeight() / 2);
	eRad = 300;
	velRad = 0;
	lifeTime = 2.0;
	rotate = 0;
	force = 0;
	spinning = 0;
	friction = 0.05;
	size = 5;
	shape = 0;
	lineWidth = 2;
	fill = 0;
	redChannel = 255;
	greenChannel = 0;
	blueChannel = 255;
	
}

//--------------------------------------------------------------
//----------------------  Particle  ----------------------------
//--------------------------------------------------------------
Particle::Particle() {
	live = false;
}

//--------------------------------------------------------------
ofPoint randomPointInCircle(float maxRad) {
	ofPoint pnt;
	float rad = ofRandom(0, maxRad);
	float angle = ofRandom(0, M_TWO_PI);
	pnt.x = cos(angle) * rad;
	pnt.y = sin(angle) * rad;
	return pnt;
}

//--------------------------------------------------------------
void Particle::setup() {
	pos = param.eCenter + randomPointInCircle(param.eRad);
	vel = randomPointInCircle(param.velRad);
	time = 0;
	lifeTime = param.lifeTime;
	live = true;
}

//--------------------------------------------------------------
void Particle::update(float dt) {
	if (live) {
		//Rotate vel
		vel.rotate(0, 0, param.rotate * dt);
		ofPoint acc;         //Acceleration
		ofPoint delta = pos - param.eCenter;
		float len = delta.length();
		if (ofInRange(len, 0, param.eRad)) {
			delta.normalize();

			//Attraction/repulsion force 
			acc += delta * param.force;

			//Spinning force
			acc.x += -delta.y * param.spinning;
			acc.y += delta.x * param.spinning;
		}
		vel += acc * dt;            //Euler method
		vel *= (1 - param.friction);  //Friction

									  //Update pos
		pos += vel * dt;    //Euler method
							//Update time and check if particle should die
		time += dt;
		if (time >= lifeTime) {
			live = false;   //Particle is now considered as died
		}
	}
}

//--------------------------------------------------------------
void Particle::draw() {
	if (live) {
		//Compute size
		/*param.size = ofMap(
			fabs(time - lifeTime / 2), 0, lifeTime / 2, 3, 1);*/

			//Compute color
		ofColor color = ofColor(param.redChannel,param.greenChannel,param.blueChannel);
		float hue = ofMap(time, 0, lifeTime, 0, 255);
		color.setHue(hue);
		ofSetColor(color);
		if (param.shape == 0) { ofCircle(pos, param.size); } //Draw circle
		if (param.shape == 1) { ofRect(pos - param.size, param.size * 2, param.size * 2); } //Draw Rectangle
		//if (param.shape == 2) {	ofCircle(pos, param.size * 10);		}
	}
}
//--------------------------------------------------------------
void ofApp::setup() {
	receive.setup(PORT);
	ofSetWindowTitle("OSC Lemur Particle");
	ofSetFrameRate(60);
	//Allocate drawing buffer
	int w = ofGetWidth();
	int h = ofGetHeight();
	fbo.allocate(w, h, GL_RGB32F_ARB);

	//Fill buffer with color
	fbo.begin();
	ofBackground(ofRandom(0), ofRandom(0), ofRandom(0));
	fbo.end();

	//Set up parameters
	param.setup();		//Global parameters
	history = 0.9;
	bornRate = 300;
	interfPreset = 0;
	bornCount = 0;
	pulsate = 0;
	time0 = ofGetElapsedTimef();
	counter = 0;

	//GUI
	interf.setup();
	interf.addSlider("bornRate", &bornRate, 0, 700);
	interf.addSlider("lifeTime", &param.lifeTime, 0, 5);
	interf.addSlider("history", &history, 0, 1);
	interf.addSlider("eRad", &param.eRad, 0, 800);
	interf.addSlider("velRad", &param.velRad, 0, 400);
	interf.addSlider("rotate", &param.rotate, -500, 500);
	interf.addSlider("spinning", &param.spinning, -1000, 1000);
	interf.addSlider("force", &param.force, -1000, 1000);
	interf.addSlider("friction", &param.friction, 0, 0.1);
	interf.addSlider("fill", &param.fill, 0, 1);
	interf.addSlider("lineWidth", &param.lineWidth, 1, 8);
	interf.addSlider("pulsate", &pulsate, 0, 1);
	interf.addSlider("shape", &param.shape, 0, 1);
	interf.addSlider("size", &param.size, 2, 50);
	interf.addSlider("redChannel", &param.redChannel, 0, 255);
	interf.addSlider("greenChannel", &param.greenChannel, 255, 0);
	interf.addSlider("blueChannel", &param.blueChannel, 0, 255);


	drawInterface = true;
}

//--------------------------------------------------------------
void ofApp::update() {

	counter = counter + 0.033f;
	while (receive.hasWaitingMessages())
	{
		ofxOscMessage m;
		receive.getNextMessage(&m);

		if (m.getAddress() == "/bornRate/x")
		{
			float bornRateScaled = m.getArgAsFloat(0);
			bornRate = ofMap(bornRateScaled, 0, 1, 0, 700);
		}
		if (m.getAddress() == "/lifeTime/x")
		{
			float lifeTimeScaled = m.getArgAsFloat(0);
			param.lifeTime = ofMap(lifeTimeScaled, 0, 1, 0, 5);
		}
		if (m.getAddress() == "/history/x")
		{
			float historyScaled = m.getArgAsFloat(0);
			history = ofMap(historyScaled, 0, 1, 0, 1);
		}
		if (m.getAddress() == "/eRad/x")
		{
			float eRadScaled = m.getArgAsFloat(0);
			param.eRad = ofMap(eRadScaled, 0, 1, 0, 800);
		}
		if (m.getAddress() == "/velRad/x")
		{
			float velRadScaled = m.getArgAsFloat(0);
			param.velRad = ofMap(velRadScaled, 0, 1, 0, 400);

		}
		if (m.getAddress() == "/rotate/x")
		{
			float rotateScaled = m.getArgAsFloat(0);
			param.rotate = ofMap(rotateScaled, 0, 1, -500, 500);
		}
		if (m.getAddress() == "/spinning/x")
		{
			float spinningScaled = m.getArgAsFloat(0);
			param.spinning = ofMap(spinningScaled, 0, 1, -1000, 1000);
		}
		if (m.getAddress() == "/force/x")
		{
			float forceScaled = m.getArgAsFloat(0);
			param.force = ofMap(forceScaled, 0, 1, -1000, 1000);
		}
		if (m.getAddress() == "/friction/x")
		{
			float frictionScaled = m.getArgAsFloat(0);
			param.friction = ofMap(frictionScaled, 0, 1, 0, 0.10);
		}
		if (m.getAddress() == "/fill/x")
		{
			float fillScaled = m.getArgAsFloat(0);
			param.fill = ofMap(fillScaled, 0, 1, 0, 1);
		}
		if (m.getAddress() == "/lineWidth/x") {
			float lineWidthScaled = m.getArgAsFloat(0);
			param.lineWidth = ofMap(lineWidthScaled, 0, 1, 1, 8);
		}
		if (m.getAddress() == "/shape/x")
		{
			float shapeScaled = m.getArgAsFloat(0);
			param.shape = ofMap(shapeScaled, 0, 1, 0, 1);
		}
		if (m.getAddress() == "/size/x")
		{
			float sizeScaled = m.getArgAsFloat(0);
			param.size = ofMap(sizeScaled, 0, 1, 2, 50);
		}
		if (m.getAddress() == "/redChannel/x") {
			float redChannelScaled = m.getArgAsFloat(0);
			param.redChannel = ofMap(redChannelScaled, 0, 1, 0, 255);
		}
		if (m.getAddress() == "/greenChannel/x") {
			float greenChannelScaled = m.getArgAsFloat(0);
			param.greenChannel = ofMap(greenChannelScaled, 0, 1, 0, 255);
		}
		if (m.getAddress() == "/blueChannel/x") {
			float blueChannelScaled = m.getArgAsFloat(0);
			param.blueChannel = ofMap(blueChannelScaled, 0, 1, 0, 255);
		}
		if (m.getAddress() == "/pulsate/x") {
			pulsate = m.getArgAsFloat(0);
		}
	}
	//Compute dt
	float time = ofGetElapsedTimef();
	float dt = ofClamp(time - time0, 0, 0.1);
	time0 = time;

	//Delete inactive particles
	int i = 0;
	while (i < p.size()) {
		if (!p[i].live) {
			p.erase(p.begin() + i);
		}
		else {
			i++;
		}
	}

	//Born new particles
	bornCount += dt * bornRate;      //Update bornCount value
	if (bornCount >= 1) {          //It's time to born particle(s)
		int bornN = int(bornCount);//How many born
		bornCount -= bornN;          //Correct bornCount value
		for (int i = 0; i<bornN; i++) {
			Particle newP;
			newP.setup();            //Start a new particle
			p.push_back(newP);     //Add this particle to array
		}
	}

	//Update the particles
	for (int i = 0; i<p.size(); i++) {
		p[i].update(dt);
	}
}

void ofApp::draw() {
	ofBackground(ofRandom(0,255), ofRandom(0,255), ofRandom(0,255));  //Set white background

								  //1. Drawing to buffer
	fbo.begin();

	//Draw semi-transparent white rectangle
	//to slightly clearing a buffer (depends on history value)

	ofEnableAlphaBlending();         //Enable transparency

	float alpha = (1 - history) * 255;
	ofSetColor(0,0,0, alpha);
	ofFill();
	ofRect(0, 0, ofGetWidth(), ofGetHeight());

	ofDisableAlphaBlending();        //Disable transparency

									 //Draw the particles
	
	param.shape = round(param.shape); //round shape to an int
	param.fill = round(param.fill);
	pulsate = round(pulsate);
	
	for (int i = 0; i<p.size(); i++) {
		if (pulsate) {
			float radius = param.size + ofRandom(5) * sin(time0);
			float sizeadjust = ofMap(radius, -150000, 150000, 0, 200,true);
			param.size = ofMap(sizeadjust,0,200,2,70)*ofRandom(0,1);
		}
		if (param.fill == 1) {
			ofEnableAlphaBlending();
			ofFill();
			ofSetColor(param.redChannel, param.greenChannel, param.blueChannel, ofRandom(255));
			p[i].draw();
			ofDisableAlphaBlending();
		}
		ofNoFill();
		ofSetColor(param.redChannel, param.greenChannel, param.blueChannel);
		ofSetLineWidth(param.lineWidth);
		p[i].draw();
	
	}

	fbo.end();

	//2. Draw buffer on the screen
	ofSetColor(255, 255, 255);
	fbo.draw(0, 0);

	//GUI
	if (drawInterface) {
		//Draw text
		ofSetColor(param.redChannel, param.greenChannel, param.blueChannel);
		ofDrawBitmapString("Particles: " + ofToString(p.size()), 20, 20);
		ofDrawBitmapString("Press Enter to hide this GUI", ofGetWidth()-240, 20);
		//Draw sliders
		interf.draw();

		//Draw emitter as a circle
		ofSetCircleResolution(50);
		ofNoFill();
		ofSetColor(128, 128, 128);
		ofCircle(param.eCenter, param.eRad);
		ofSetCircleResolution(20);
	}
}
void Interface::setup() {
	selected = -1;
}
void Interface::addSlider(string title, float *value, float minV, float maxV) {
	Slider s;
	s.title = title;
	s.rect = ofRectangle(20, 40 + slider.size() * 40, 150, 30);
	s.value = value;
	s.minV = minV;
	s.maxV = maxV;
	slider.push_back(s);
}

void Interface::draw() {
	for (int i = 0; i<slider.size(); i++) {
		Slider &s = slider[i];
		ofRectangle r = s.rect;
		ofFill();
		ofSetColor(0, 0, 0);
		ofRect(r);
		ofSetColor(255, 255, 255);
		ofNoFill();
		ofRect(r);
		ofFill();
		float w = ofMap(*s.value, s.minV, s.maxV, 0, r.width);
		ofRect(r.x, r.y + 15, w, r.height - 15);
		ofDrawBitmapString(s.title + " " + ofToString(*s.value, 2), r.x + 5, r.y + 12);
	}
}

void Interface::mousePressed(int x, int y) {
	for (int i = 0; i<slider.size(); i++) {
		Slider &s = slider[i];
		ofRectangle r = s.rect;
		if (ofInRange(x, r.x, r.x + r.width) && ofInRange(y, r.y, r.y + r.height)) {
			selected = i;
			*s.value = ofMap(x, r.x, r.x + r.width, s.minV, s.maxV, true);
		}
	}
}

void Interface::mouseDragged(int x, int y) {
	if (selected >= 0) {
		Slider &s = slider[selected];
		ofRectangle r = s.rect;
		*s.value = ofMap(x, r.x, r.x + r.width, s.minV, s.maxV, true);
	}
}

void Interface::mouseReleased(int x, int y) {
	selected = -1;
}
void Interface::save(int index)
{
	vector<string> list;
	for (int i = 0; i<slider.size(); i++) {
		list.push_back(ofToString(*slider[i].value));
	}
	string text = ofJoinString(list, " ");
	string fileName = "preset" + ofToString(index) + ".txt";
	ofBuffer buffer = ofBuffer(text);
	ofBufferToFile(fileName, buffer);
}

//--------------------------------------------------------------
void Interface::load(int index)
{
	string fileName = "preset" + ofToString(index) + ".txt";
	string text = string(ofBufferFromFile(fileName));
	vector<string> list = ofSplitString(text, " ");

	if (list.size() == slider.size()) {
		for (int i = 0; i<slider.size(); i++) {
			*slider[i].value = ofToFloat(list[i]);
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	if (key == OF_KEY_RETURN) {	//Hide/show GUI
		drawInterface = !drawInterface;
	}

	if (key == ' ') {		//Grab the screen image to file
		ofImage image;
		image.grabScreen(0, 0, ofGetWidth(), ofGetHeight());

		//Select random file name
		int n = ofRandom(0, 1000);
		string fileName = "screen" + ofToString(n) + ".png";

		image.saveImage(fileName);
	}

	//Load presets
	if (key == '1') { interf.load(1); }
	if (key == '2') { interf.load(2); }
	if (key == '3') { interf.load(3); }
	if (key == '4') { interf.load(4); }
	if (key == '5') { interf.load(5); }
	if (key == '6') { interf.load(6); }
	if (key == '7') { interf.load(7); }
	if (key == '8') { interf.load(8); }
	if (key == '9') { interf.load(9); }

	//Save presets
	if (key == '!') { interf.save(1); }
	if (key == '@') { interf.save(2); }
	if (key == '#') { interf.save(3); }
	if (key == '$') { interf.save(4); }
	if (key == '%') { interf.save(5); }
	if (key == '^') { interf.save(6); }
	if (key == '&') { interf.save(7); }
	if (key == '*') { interf.save(8); }
	if (key == '(') { interf.save(9); }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	if (drawInterface) {
		interf.mouseDragged(x, y);
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	if (drawInterface) {
		interf.mousePressed(x, y);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	interf.mouseReleased(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
