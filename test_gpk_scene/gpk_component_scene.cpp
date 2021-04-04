#include "gpk_component_scene.h"
#include "gpk_storage.h"
#include "gpk_stdstring.h"

static ::gpk::error_t					createFromSTL			(::gpk::SComponentScene & scene, ::gpk::SSTLFile & stlFile, ::gpk::view_const_string componentName)  {
	::gpk::SComponentData						newComponentGroup		= {};
	::gpk::array_obj<::gpk::SComponentData>		newComponentNodes		= {};
	newComponentGroup.IndexParent	= -1;
	::gpk::SRenderNode							newNode					= {};
	::gpk::SNodeRenderer						& renderer				= scene.Renderer;
	::gpk::SNodeRenderer::TVertexBuffer			& bufferVertices		= renderer.Vertices		[newNode.Vertices		= renderer.Vertices		.push_back({})];
	::gpk::SNodeRenderer::TNormalBuffer			& bufferNormals			= renderer.Normals		[newNode.Normals		= renderer.Normals		.push_back({})];
	::gpk::SNodeRenderer::TVertexColorBuffer	& bufferVertexColors	= renderer.VertexColors	[newNode.VertexColor	= renderer.VertexColors	.push_back({})];
	bufferVertices		.resize(stlFile.Triangles.size() * 3);
	bufferNormals		.resize(stlFile.Triangles.size());
	bufferVertexColors	.resize(stlFile.Triangles.size());
	newNode.PerFaceColor					= 1;
	newNode.PerFaceNormal					= 1;
	for(uint32_t iTriangle = 0; iTriangle < stlFile.Triangles.size(); ++iTriangle) {
		bufferVertices		[iTriangle * 3 + 0]	= stlFile.Triangles[iTriangle].Triangle.A;
		bufferVertices		[iTriangle * 3 + 1]	= stlFile.Triangles[iTriangle].Triangle.B;
		bufferVertices		[iTriangle * 3 + 2]	= stlFile.Triangles[iTriangle].Triangle.C;
		bufferNormals		[iTriangle]			= stlFile.Triangles[iTriangle].Normal;
		bufferVertexColors	[iTriangle].FromBGR16(stlFile.Triangles[iTriangle].Attribute);
	}
	newComponentGroup.RenderNodes.push_back(renderer.Nodes.push_back(newNode));
	scene.ComponentNames.push_back(componentName);
	return scene.Components.push_back(newComponentGroup);
}

::gpk::error_t	gpk::SComponentScene::CreateFromFile		(::gpk::view_const_string filename)		{
	::gpk::view_const_char					extension				= {};
	::gpk::array_pod<char_t>				ext_lwr					= {};
	gpk_necall(filename.slice(extension, filename.size() - 4, 4), "File extension not supported for file '%s'", filename.begin());
	gpk_necall(ext_lwr.append(extension), "%s", "");
	::gpk::tolower(ext_lwr);
	static const ::gpk::vcs					str_stl					= ".stl";
	if(ext_lwr == str_stl) {
		::gpk::SSTLFile							stlFile					= {};
		gpk_necall(::gpk::stlFileLoad(filename, stlFile), "Failed to load file '%s'.", ::gpk::toString(filename).begin());
		return ::createFromSTL(*this, stlFile, filename);
	}
	else {
		error_printf("File format not supported for file '%s'", ::gpk::toString(filename).begin());
		return -1;
	}
	return 0;
}
