#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(30);
	ofSetWindowTitle("openframeworks");

	ofBackground(0);

	this->cap_size = cv::Size(1280, 720);

	this->cap.open(0);
	this->cap.set(cv::CAP_PROP_FPS, 30);
	this->cap.set(cv::CAP_PROP_FRAME_WIDTH, this->cap_size.width);
	this->cap.set(cv::CAP_PROP_FRAME_HEIGHT, this->cap_size.height);

	this->image.allocate(this->cap_size.width, this->cap_size.height, OF_IMAGE_COLOR);
	this->frame = cv::Mat(cv::Size(this->image.getWidth(), this->image.getHeight()), CV_MAKETYPE(CV_8UC3, this->image.getPixels().getNumChannels()), this->image.getPixels().getData(), 0);
	this->face_cascade.load("D:\\opencv_4.1.2\\build\\install\\etc\\haarcascades\\haarcascade_frontalface_alt2.xml");

	this->fbo.allocate(this->cap_size.width, this->cap_size.height);
	this->font.loadFont("fonts/Kazesawa-Bold.ttf", 15, true, true, true);
}

//--------------------------------------------------------------
void ofApp::update() {

	this->cap >> this->frame;
	cv::cvtColor(this->frame, this->frame, cv::COLOR_BGR2RGB);
	cv::flip(this->frame, this->frame, 1);
	this->image.update();

	cv::Mat gray_frame, small_frame;
	cv::cvtColor(this->frame, gray_frame, cv::COLOR_RGB2GRAY);
	cv::resize(gray_frame, small_frame, cv::Size(this->cap_size.width * 0.5, this->cap_size.height * 0.5));

	vector<cv::Rect> faces;
	this->face_cascade.detectMultiScale(small_frame, faces);

	int face_count = 0;
	glm::vec2 face_location, face_size;
	float max_area = 0.f;
	for (cv::Rect r : faces) {

		auto tmp_face_location = glm::vec2(r.x * 2.f, r.y * 2.f);
		auto tmp_face_size = glm::vec2(r.width * 2, r.height * 2);
		auto area = tmp_face_size.x * tmp_face_size.y;
		if (area > max_area) {

			face_location = tmp_face_location;
			face_size = tmp_face_size;
		}
	}

	this->fbo.begin();
	ofClear(0);

	for (int x = face_location.x; x < face_location.x + face_size.x; x += 15) {

		for (int y = face_location.y; y < face_location.y + face_size.y; y += 15) {

			char moji = ofMap(ofNoise(x * 0.005, y * 0.005, ofGetFrameNum() * 0.05), 0, 1, '0', 'z');
			this->font.drawString(string{ moji }, x, y);
		}
	}

	ofDrawRectangle(0, 0, this->fbo.getWidth(), face_location.y);
	ofDrawRectangle(0, 0, face_location.x, this->fbo.getHeight());
	ofDrawRectangle(0, face_location.y + face_size.y, this->fbo.getWidth(), this->fbo.getHeight());
	ofDrawRectangle(face_location.x + face_size.x, 0, this->fbo.getWidth(), this->fbo.getHeight());

	this->fbo.end();

	this->image.getTextureReference().setAlphaMask(this->fbo.getTexture());
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->image.draw(0, 0);
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(1280, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}