#pragma once

#include "ofMain.h"
#include "ofxSvg.h"
#include "ofxGui.h"
#include "textButton.h"
#include "objExporter.h"

class svgToObj : public ofBaseApp
{
	public:
		void			setup();
		void			update();
		void			draw();
		void			mousePressed(  int x, int y, int button );
	
		int				svgIndex;
		bool			isProcessing;
		ofDirectory		dir;
		objExporter		exporter;
		ofTessellator	t;
		vector<ofMesh*> meshes;
		vector<ofColor> colors;
	
		ofTrueTypeFont	font;
	
		textButton		goButton;
		textButton		button;
		vector<textButton> buttons;
		ofxIntSlider	spacingSlider;
		ofxPanel		gui;
		
	
};
