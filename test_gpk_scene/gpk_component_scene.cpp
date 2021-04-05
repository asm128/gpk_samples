#include "gpk_component_scene.h"
#include "gpk_storage.h"
#include "gpk_stdstring.h"

static ::gpk::error_t					createFromSTL			(::gpk::SComponentScene & scene, ::gpk::SSTLFile & stlFile, ::gpk::view_const_string componentName)  {
	::gpk::SNodeRenderer						& renderer				= scene.Renderer;
	int32_t										indexNewNode			= renderer.Nodes.push_back(componentName, {});
	::gpk::SRenderNode							& newNode				= renderer.Nodes[indexNewNode];

	//::gpk::array_pod<char_t>					propertyName;

	::gpk::SNodeRenderer::TVertexBuffer			& bufferVertices		= renderer.Vertices			[newNode.Vertices		= renderer.Vertices			.push_back(componentName, {})];
	::gpk::SNodeRenderer::TNormalBuffer			& bufferNormals			= renderer.Normals			[newNode.Normals		= renderer.Normals			.push_back(componentName, {})];
	::gpk::SNodeRenderer::TVertexColorBuffer	& bufferVertexColors	= renderer.VertexColors		[newNode.VertexColor	= renderer.VertexColors		.push_back(componentName, {})];
	::gpk::SMaterial							& material				= renderer.Materials		[newNode.Material		= renderer.Materials		.push_back(componentName, {})];
	::gpk::SRenderNodeTransform					& transform				= renderer.Transforms		[newNode.Transform		= renderer.Transforms		.push_back(componentName, {})];
	::gpk::SBoundingVolume						& boundingVolume		= renderer.BoundingVolumes	[newNode.BoundingVolume	= renderer.BoundingVolumes	.push_back(componentName, {})];

	transform.SetIdentity();

	material.Ambient						= ::gpk::DARKGRAY * .5	;
	material.Diffuse						= ::gpk::WHITE			;
	material.Specular						= ::gpk::GRAY			;
	material.Emissive						= ::gpk::BLACK			;
	material.SpecularFactor					= 10.0f					;

	bufferVertices		.resize(stlFile.Triangles.size() * 3);
	bufferNormals		.resize(stlFile.Triangles.size());
	bufferVertexColors	.resize(stlFile.Triangles.size());
	newNode.PerFaceColor					= 1;
	newNode.PerFaceNormal					= 1;
	for(uint32_t iTriangle = 0; iTriangle < stlFile.Triangles.size(); ++iTriangle) {
		const ::gpk::SSTLTriangle					& triangle				= stlFile.Triangles[iTriangle];
		bufferVertices		[iTriangle * 3 + 0]	= {triangle.Triangle.A.x, triangle.Triangle.A.z, triangle.Triangle.A.y};
		bufferVertices		[iTriangle * 3 + 2]	= {triangle.Triangle.B.x, triangle.Triangle.B.z, triangle.Triangle.B.y};
		bufferVertices		[iTriangle * 3 + 1]	= {triangle.Triangle.C.x, triangle.Triangle.C.z, triangle.Triangle.C.y};
		bufferNormals		[iTriangle]			= {triangle.Normal.x, triangle.Normal.z, triangle.Normal.y};
		bufferVertexColors	[iTriangle].FromBGR16(triangle.Attribute);
		boundingVolume.Limits.Min.x				= ::gpk::min(triangle.Triangle.A.x, ::gpk::min(triangle.Triangle.B.x, ::gpk::min(triangle.Triangle.C.x, boundingVolume.Limits.Min.x)));
		boundingVolume.Limits.Min.y				= ::gpk::min(triangle.Triangle.A.z, ::gpk::min(triangle.Triangle.B.z, ::gpk::min(triangle.Triangle.C.z, boundingVolume.Limits.Min.y)));
		boundingVolume.Limits.Min.z				= ::gpk::min(triangle.Triangle.A.y, ::gpk::min(triangle.Triangle.B.y, ::gpk::min(triangle.Triangle.C.y, boundingVolume.Limits.Min.z)));
		boundingVolume.Limits.Max.x				= ::gpk::max(triangle.Triangle.A.x, ::gpk::max(triangle.Triangle.B.x, ::gpk::max(triangle.Triangle.C.x, boundingVolume.Limits.Max.x)));
		boundingVolume.Limits.Max.y				= ::gpk::max(triangle.Triangle.A.z, ::gpk::max(triangle.Triangle.B.z, ::gpk::max(triangle.Triangle.C.z, boundingVolume.Limits.Max.y)));
		boundingVolume.Limits.Max.z				= ::gpk::max(triangle.Triangle.A.y, ::gpk::max(triangle.Triangle.B.y, ::gpk::max(triangle.Triangle.C.y, boundingVolume.Limits.Max.z)));
	}
	boundingVolume.Center					= boundingVolume.Limits.Min + (boundingVolume.Limits.Max - boundingVolume.Limits.Min) / 2;
	boundingVolume.Radius					= ::gpk::max(boundingVolume.Limits.Min.Length(), boundingVolume.Limits.Max.Length()) / 2;
	boundingVolume.Type						= ::gpk::BOUNDINGPRIMITIVE_TYPE_SPHERE;
	for(uint32_t iVertex = 0; iVertex < bufferVertices.size(); ++iVertex)
		bufferVertices[iVertex]					-= boundingVolume.Center;

	::gpk::SComponentData						newComponentGroup		= {};
	newComponentGroup.IndexParent			= -1;
	newComponentGroup.RenderNodes.push_back(indexNewNode);
	return scene.Components.push_back(componentName, newComponentGroup);
}

static ::gpk::error_t					createFromMTL			(::gpk::SComponentScene & scene, ::gpk::view_const_char filename, ::gpk::SKeyedArrayPOD<int16_t> & indices)  {
	::gpk::array_pod<byte_t>					rawMat					= {};
	::gpk::fileToMemory(filename, rawMat);

	::gpk::array_obj<::gpk::view_const_char>	matFileLines			= {};
	::gpk::split(::gpk::vcc{rawMat}, '\n', matFileLines);

	int32_t										countMaterials			= 0;
	::gpk::array_pod<char_t>					materialPath			= {};
	::gpk::label								materialName			= {};
	::gpk::SMaterial							newMaterial				= {};
	for(uint32_t iLine = 0; iLine < matFileLines.size(); ++iLine) {
		::gpk::view_const_char & line = matFileLines[iLine];
		::gpk::trim(line, line);
		if(0 == line.size() || line[0] == '#')
			continue;
		info_printf("Line %u: '%s'.", iLine, ::gpk::toString(line).begin());
		::gpk::array_obj<::gpk::view_const_char>	lineValues				= {};
		::gpk::split(line, ::gpk::vcs{" \t"}, lineValues);
		for(uint32_t iValue = 0; iValue < lineValues.size(); ++iValue)
			::gpk::trim(lineValues[iValue], lineValues[iValue]);

		const ::gpk::vcc command = lineValues[0];
		if(command == ::gpk::vcs{"newmtl"}) {
				 if(countMaterials) {
					materialPath = filename;
					materialPath.push_back('/');
					materialPath.append(materialName);
					indices.push_back(materialName, (int16_t)scene.Renderer.Materials.push_back(::gpk::label(::gpk::vcc{materialPath}), newMaterial));
				 }
				 materialName = {lineValues[1].begin(), lineValues[1].size()};
				 ++countMaterials;
			 }
		else if(command == ::gpk::vcs{"Ka"	}) { newMaterial.Ambient; }
		else if(command == ::gpk::vcs{"Kd"	}) { newMaterial.Diffuse; }
		else if(command == ::gpk::vcs{"d"		}) { newMaterial.Transparency; }
		else if(command == ::gpk::vcs{"illum"	}) { }
	}
	if(countMaterials) {
		materialPath = filename;
		materialPath.push_back('/');
		materialPath.append(materialName);
		indices.push_back(::gpk::label(materialName), (int16_t)scene.Renderer.Materials.push_back(::gpk::label(::gpk::vcc{materialPath}), newMaterial));
	}
	return 0;
}

static ::gpk::error_t					createFromOBJ			(::gpk::SComponentScene & scene, ::gpk::view_const_char filename)  {
	::gpk::array_pod<byte_t>					rawObj					= {};
	::gpk::fileToMemory(filename, rawObj);

	::gpk::SKeyedArrayPOD<int16_t>				materialIndices			= {};

	//::gpk::SComponentData						newObject				= {};
	//::gpk::SComponentData						newGroup				= {};
	//uint32_t									countObjects			= 0;
	//uint32_t									countGroups				= 0;


	::gpk::array_obj<::gpk::view_const_char>	objFileLines			= {};
	::gpk::split(::gpk::vcc{rawObj}, '\n', objFileLines);
	for(uint32_t iLine = 0; iLine < objFileLines.size(); ++iLine) {
		::gpk::view_const_char & line = objFileLines[iLine];
		::gpk::trim(line, line);
		if(0 == line.size() || line[0] == '#')
			continue;
		info_printf("Line %u: '%s'.", iLine, ::gpk::toString(line).begin());

		::gpk::array_obj<::gpk::view_const_char>	lineValues				= {};
		::gpk::split(line, ::gpk::vcs{" \t"}, lineValues);
		for(uint32_t iValue = 0; iValue < lineValues.size(); ++iValue) {
			::gpk::view_const_char						& value					= lineValues[iValue];
			::gpk::trim(value, value);
		}
		const ::gpk::vcc command = lineValues[0];
		if(command == ::gpk::vcs{"mtllib"}) {
			::gpk::array_pod<char_t>					matFilename				= {};
			int32_t										pathStop				= ::gpk::rfind('/', filename);
			if(pathStop == -1) {
				matFilename.append_string("./");
				matFilename.append_string(lineValues[1]);
			}
			else {
				::gpk::view_const_char						filepath				= {};
				gpk_necall(filename.slice(filepath, 0, pathStop), "%s", "");
				matFilename.append(filepath);
				matFilename.push_back('/');
				matFilename.append(lineValues[1]);
			}
			e_if(::createFromMTL(scene, matFilename, materialIndices), "Failed to load material file '%s'.", ::gpk::toString(matFilename).begin());
		}
		else if(command == ::gpk::vcs{"o"}) {

		}
		else if(command == ::gpk::vcs{"g"}) {

		}
		else if(command == ::gpk::vcs{"v"}) {

		}
		else if(command == ::gpk::vcs{"vt"}) {

		}
		else if(command == ::gpk::vcs{"vn"}) {

		}
		else if(command == ::gpk::vcs{"vp"}) {

		}
		else if(command == ::gpk::vcs{"f"}) {

		}
	}
	(void)scene;
	return 0;
}

::gpk::error_t	gpk::SComponentScene::CreateFromFile		(::gpk::view_const_string filename)		{
	::gpk::view_const_char					extension				= {};
	::gpk::array_pod<char_t>				ext_lwr					= {};
	gpk_necall(filename.slice(extension, filename.size() - 4, 4), "File extension not supported for file '%s'", filename.begin());
	gpk_necall(ext_lwr.append(extension), "%s", "");
	::gpk::tolower(ext_lwr);
	static const ::gpk::vcs					str_stl					= ".stl";
	static const ::gpk::vcs					str_obj					= ".obj";
	if(ext_lwr == str_stl) {
		::gpk::SSTLFile							stlFile					= {};
		gpk_necall(::gpk::stlFileLoad(filename, stlFile), "Failed to load file '%s'.", ::gpk::toString(filename).begin());
		return ::createFromSTL(*this, stlFile, filename);
	}
	else if(ext_lwr == str_obj) {
		return ::createFromOBJ(*this, filename);
	}
	else {
		error_printf("File format not supported for file '%s'", ::gpk::toString(filename).begin());
		return -1;
	}
	return 0;
}
