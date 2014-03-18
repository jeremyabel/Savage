#include "svgToObj.h"
#include "objExporter.h"

//--------------------------------------------------------------
void svgToObj::setup()
{
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
    ofEnableSmoothing();
	ofBackground(ofColor(41, 49, 52) * 0.5f);
	ofSetColor(255);
	
	isProcessing = false;
	
	// GUI Stuff
	font.loadFont( "fonts/Inconsolata.otf", 14, true, false, false, 0.3, 90 );
	
	// Input directory
	button.font = &font;
	button.secondFont = &font;
	button.bDrawLong = false;
	button.prefix = "Source Dir: ";
	button.setText("NONE");
	button.topLeftAnchor.set( 46, 60 );
	buttons.push_back(button);
	button.calculateRect();
	
	// Output dir
	button.prefix = "Output Dir: ";
	button.setText("NONE");
	button.topLeftAnchor.set( button.topLeftAnchor.x, button.topLeftAnchor.y + button.rect.height + 10 );
	buttons.push_back(button);
	button.calculateRect();
	
	// Output file name
	button.prefix = "File Name: ";
	button.setText("NONE");
	button.topLeftAnchor.set( button.topLeftAnchor.x, button.topLeftAnchor.y + button.rect.height + 10 );
	buttons.push_back(button);
	button.calculateRect();
	
	spacingSlider.setup("Spacing", 4, 1, 10);
	spacingSlider.setBackgroundColor(ofColor(0));
	spacingSlider.setUseTTF(true);
	spacingSlider.loadFont("fonts/Inconsolata.otf", 12);
	spacingSlider.setSize(156, 30);
	spacingSlider.setTextColor(ofColor(74,255,203));
	spacingSlider.setFillColor(ofColor(60));
	spacingSlider.setPosition( 34, button.topLeftAnchor.y + 22 );
	
	// Go button
	goButton = button;
	goButton.prefix = "PROCESS";
	//goButton.bSelectable = false;
	goButton.setText("");
	goButton.topLeftAnchor.set( button.topLeftAnchor.x, button.topLeftAnchor.y + button.rect.height * 2 + 14 );
	goButton.calculateRect();
}

//--------------------------------------------------------------
void svgToObj::update()
{
	// Update buttons
	for ( int i = 0; i < buttons.size(); i++ )
	{
		buttons[i].calculateRect();
		buttons[i].checkMousePressed(ofPoint(mouseX, mouseY));
	}
	
	goButton.calculateRect();
	goButton.checkMousePressed(ofPoint(mouseX, mouseY));
	
	// Process frames
	if ( isProcessing && svgIndex < dir.numFiles() )
	{
		ofLog() << "processing " << dir.getPath(svgIndex) << "...";
	
		ofMesh* mesh = new ofMesh();
		ofxSVG* svg  = new ofxSVG();
		vector<ofPolyline> outlines;
	
		if ( svg->load( dir.getPath(svgIndex) ) )
		{
			for ( int i = 0; i < svg->getNumPath(); i++ )
			{
				ofPath p = svg->getPathAt(i);
				p.setPolyWindingMode(OF_POLY_WINDING_ODD);
				vector<ofPolyline>& lines = p.getOutline();
				for ( int j = 0; j < (int)lines.size(); j++ )
				{
					// Resample
					lines[j] = lines[j].getResampledBySpacing(spacingSlider);
					outlines.push_back(lines[j]);
				}
			}
	
			// Triangulate and export
			t.tessellateToMesh( outlines, OF_POLY_WINDING_ODD, *mesh, true );
			mesh->setMode( OF_PRIMITIVE_TRIANGLES );
			exporter.exportToObj( mesh, svgIndex );
			
			meshes.push_back(mesh);
			colors.push_back(ofColor(ofRandom(0, 255),ofRandom(0, 255),ofRandom(0, 255)));
		}
		
		svgIndex++;
		
		if ( svgIndex == dir.numFiles() )
		{
			ofLog() << "DONE";
			exporter.close();
			isProcessing = false;
		}
	}
}

//--------------------------------------------------------------
void svgToObj::draw()
{
	// Output meshes
	for ( int i = 0; i < meshes.size(); i++ )
	{
		ofSetColor(colors[i]);
		meshes[i]->draw();
	}
	
	// Buttons
	for ( int i = 0; i < buttons.size(); i++ )
	{
		buttons[i].draw();
	}
	
	goButton.draw();
	spacingSlider.draw();
}

//--------------------------------------------------------------
void svgToObj::mousePressed( int x, int y, int button )
{
	if ( !isProcessing )
	{
		for ( int i = 0; i < buttons.size(); i++ )
			buttons[i].checkMousePressed(ofPoint(x, y));
				
		goButton.checkMousePressed(ofPoint(x, y));

		
		// Input dir button
		if ( buttons[0].bMouseOver == true )
		{
			ofFileDialogResult dirResult = ofSystemLoadDialog( "Select an input directory", true );
			
			if ( dirResult.bSuccess )
			{
				// Get svg files in directory
				dir.open( dirResult.filePath );
				dir.allowExt("svg");
				dir.listDir();
				
				if ( dir.numFiles() <= 0 )
					ofSystemAlertDialog( "Please select a directory containing at least one .svg file." );
				else
					buttons[0].setText( dirResult.filePath );
			}
			else
				buttons[0].setText( "NONE" );
		}
		
		
		// Output file button
		if ( buttons[1].bMouseOver == true )
		{
			ofFileDialogResult dirResult = ofSystemLoadDialog( "Select an output destination directory.", true );
			
			if ( dirResult.bSuccess )
				buttons[1].setText( dirResult.filePath );
			else
				buttons[1].setText( "NONE" );
		}
		
		
		// File name button
		if ( buttons[2].bMouseOver == true )
		{
			string text = ofSystemTextBoxDialog( "Name for output .obj file:", buttons[2].myText );
			buttons[2].setText(text);
		}
		
		
		// Check if we can proceed
		if ( goButton.bMouseOver == true )
		{
			// Check output directory
			if ( dir.doesDirectoryExist( buttons[1].myText, false ) )
			{
				dir.open( buttons[1].myText );

				if ( !dir.isDirectory() )
				{
					ofSystemAlertDialog( "Please select a valid directory, or allow write permission." );
					return;
				}
			}
			else
			{
				ofSystemAlertDialog( "Output directory does not exist." );
				return;
			}
			
			// Get svg files in directory
			if ( dir.doesDirectoryExist( buttons[0].myText, false ) )
			{
				dir.open( buttons[0].myText );
				dir.allowExt("svg");
				dir.listDir();
				
				// Check input directory
				if ( dir.numFiles() <= 0 )
				{
					ofSystemAlertDialog( "Please select a directory containing at least one .svg file." );
					return;
				}
			}
			else
			{
				ofSystemAlertDialog( "Input directory does not exist." );
				return;
			}

			ofLog() << "Ready to go!";
			meshes.clear();
			colors.clear();
			isProcessing = true;
		}
	}
}