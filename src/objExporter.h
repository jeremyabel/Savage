//
//  objExporter.h
//  SVGtoFBX
//
//  Created by SmartFoxServer on 3/17/14.
//
//

#ifndef SVGtoFBX_objExporter_h
#define SVGtoFBX_objExporter_h

#include "ofMain.h"
#include <fstream>

class objExporter
{
public:
	
	ofstream obj;
	int prevTotalIndex = 0;
	
	void init( string filename )
	{
		obj.open( ofToDataPath( filename + ".obj").c_str(), ios::out );
		
		// Write header
		obj << "# " << filename + ".obj" << endl;
	}
	
	void exportToObj( ofMesh* mesh, int index = 0 )
	{
		if ( obj.is_open() )
		{
			// Write object group
			obj << "g ";
			
			// Pad with zeros
			if ( index < 10 )
				obj << "0";
			if ( index < 100 )
				obj << "0";
			if ( index < 1000 )
				obj << "0";
			if ( index < 10000 )
				obj << "0";
			if ( index < 100000 )
				obj << "0";
			
			// Write object name
			obj << index << endl;
			
			vector<ofMeshFace> triangles = mesh->getUniqueFaces();
			
			// Write vertices
			for ( int i = 0; i < triangles.size(); i++ )
			{
				ofMeshFace tri = triangles[i];
				obj << "v " << tri.getVertex(0).x << " " << -tri.getVertex(0).y << " " << tri.getVertex(0).z << endl;
				obj << "v " << tri.getVertex(1).x << " " << -tri.getVertex(1).y << " " << tri.getVertex(1).z << endl;
				obj << "v " << tri.getVertex(2).x << " " << -tri.getVertex(2).y << " " << tri.getVertex(2).z << endl;
			}
			
			obj << endl;
			
			// Write  indices
			for ( int i = 0; i < triangles.size() * 3; i += 3 )
			{
				int indices[] = {i + 1 + prevTotalIndex, i + 2 + prevTotalIndex, i + 3 + prevTotalIndex};
				obj << "f " << indices[0] << " " << indices[1] << " " << indices[2] << endl;
			}
			
			// Prep for next one
			prevTotalIndex += triangles.size() * 3;
			obj << endl;
		}
	}
	
	void close()
	{
		obj.close();
	}
};

#endif
