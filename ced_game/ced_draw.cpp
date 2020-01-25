#include "ced_draw.h"
#include <algorithm>

int								ced::setPixel			(::gpk::view_grid<::gpk::SColorBGRA> pixels, ::gpk::SCoord2<int32_t> position, ::gpk::SColorBGRA color)	{
	if( (position.x >= 0 && position.x < (int32_t)pixels.metrics().x)
	 && (position.y >= 0 && position.y < (int32_t)pixels.metrics().y)
	)
		pixels[position.y][position.x]	= color;
	return 0;
}

int								ced::drawRectangle		(::gpk::view_grid<::gpk::SColorBGRA> pixels, ::gpk::SRectangle2D<int32_t> rectangle, ::gpk::SColorBGRA color)	{
	for(int32_t y = 0; y < (int32_t)rectangle.Size.y; ++y)
	for(int32_t x = 0; x < (int32_t)rectangle.Size.x; ++x)
		::ced::setPixel(pixels, {rectangle.Offset.x + x, rectangle.Offset.y + y}, color);
	return 0;
}

int								ced::drawCircle			(::gpk::view_grid<::gpk::SColorBGRA> pixels, ::gpk::SCircle2D<int32_t> circle, ::gpk::SColorBGRA color)	{
	for(int32_t y = (int32_t)-circle.Radius; y < (int32_t)circle.Radius; ++y)
	for(int32_t x = (int32_t)-circle.Radius; x < (int32_t)circle.Radius; ++x) {
		::gpk::SCoord2<int32_t>						position			= {x, y};
		if(position.Length() <= circle.Radius)
			::ced::setPixel(pixels, {circle.Center.x + x, circle.Center.y + y}, color);
	}
	return 0;
}

// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
int								ced::drawLine       	(::gpk::view_grid<::gpk::SColorBGRA> pixels, ::gpk::SLine2<int32_t> line, ::gpk::SColorBGRA color)	{
	int32_t								dx						= (int32_t)abs(line.B.x - line.A.x);
	int32_t								sx						= (int32_t)line.A.x < line.B.x ? 1 : -1;
	int32_t								dy						= (int32_t)-abs(line.B.y - line.A.y );
	int32_t								sy						= (int32_t)line.A.y < line.B.y ? 1 : -1;
	int32_t								err						= dx + dy;  /* error value e_xy */
	while (true) {   /* loop */
		if (line.A.x == line.B.x && line.A.y == line.B.y)
			break;
		int32_t								e2						= 2 * err;
		if (e2 >= dy) {
			err								+= dy; /* e_xy+e_x > 0 */
			line.A.x						+= sx;
			setPixel(pixels, {line.A.x, line.A.y}, color);
		}
		if (e2 <= dx) { /* e_xy+e_y < 0 */
			err								+= dx;
			line.A.y						+= sy;
			setPixel(pixels, {line.A.x, line.A.y}, color);
		}
	}
	return 0;
}


// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
int								ced::drawLine       	(::gpk::view_grid<::gpk::SColorBGRA> pixels, ::gpk::SLine2<int32_t> line, ::gpk::array_pod<::gpk::SCoord2<int32_t>> & pixelCoords)	{
	int32_t								dx						= (int32_t)abs(line.B.x - line.A.x);
	int32_t								sx						= (int32_t)line.A.x < line.B.x ? 1 : -1;
	int32_t								dy						= (int32_t)-abs(line.B.y - line.A.y );
	int32_t								sy						= (int32_t)line.A.y < line.B.y ? 1 : -1;
	int32_t								err						= dx + dy;  /* error value e_xy */
	pixelCoords.push_back({line.A.x, line.A.y});
	while (true) {   /* loop */
		if (line.A.x == line.B.x && line.A.y == line.B.y)
			break;
		int32_t								e2						= 2 * err;
		if (e2 >= dy) {
			err								+= dy; /* e_xy+e_x > 0 */
			line.A.x						+= sx;
			if( line.A.x >= 0 && line.A.x < (int32_t)pixels.metrics().x
			 && line.A.y >= 0 && line.A.y < (int32_t)pixels.metrics().y
			)
				pixelCoords.push_back({line.A.x, line.A.y});

		}
		if (e2 <= dx) { /* e_xy+e_y < 0 */
			err								+= dx;
			line.A.y						+= sy;
			if( line.A.x >= 0 && line.A.x < (int32_t)pixels.metrics().x
			 && line.A.y >= 0 && line.A.y < (int32_t)pixels.metrics().y
			)
				pixelCoords.push_back({line.A.x, line.A.y});
		}

	}
	return 0;
}


// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
int								ced::drawLine
	( ::gpk::view_grid<::gpk::SColorBGRA>			pixels
	, const ::gpk::SLine3<float>					& lineFloat
	, ::gpk::array_pod<::gpk::SCoord3<float>>		& pixelCoords
	, ::gpk::view_grid<uint32_t>					depthBuffer
	) {
	::gpk::SLine2<int32_t>				line					= {{(int32_t)lineFloat.A.x, (int32_t)lineFloat.A.y}, {(int32_t)lineFloat.B.x, (int32_t)lineFloat.B.y}};
	int32_t								dx						= (int32_t)abs(line.B.x - line.A.x);
	int32_t								sx						= (int32_t)line.A.x < line.B.x ? 1 : -1;
	int32_t								dy						= (int32_t)-abs(line.B.y - line.A.y );
	int32_t								sy						= (int32_t)line.A.y < line.B.y ? 1 : -1;
	int32_t								err						= dx + dy;  /* error value e_xy */

	double								xDiff					= (line.B.x - line.A.x);
	double								yDiff					= (line.B.y - line.A.y);
	bool								yAxis					= yDiff > xDiff;
	uint32_t							intZ					= uint32_t(0xFFFFFFFFU * lineFloat.A.z);
	if( line.A.x >= 0 && line.A.x < (int32_t)pixels.metrics().x
	 && line.A.y >= 0 && line.A.y < (int32_t)pixels.metrics().y
	) {
		if( depthBuffer[line.A.y][line.A.x] > intZ ) {
			depthBuffer[line.A.y][line.A.x]	= intZ;
			//pixelCoords.push_back({(int32_t)line.A.x, (int32_t)line.A.y});
			pixelCoords.push_back(lineFloat.A);
		}
	}

	while (true) {   /* loop */
		if (line.A.x == line.B.x && line.A.y == line.B.y)
			break;
		int32_t								e2						= 2 * err;
		if (e2 >= dy) {
			err								+= dy; /* e_xy+e_x > 0 */
			line.A.x						+= sx;
			if( line.A.x >= 0 && line.A.x < (int32_t)pixels.metrics().x
			 && line.A.y >= 0 && line.A.y < (int32_t)pixels.metrics().y
			) {
				const double					factor					= yAxis ? 1.0 / yDiff * line.A.y : 1.0 / xDiff * line.A.x;
				const double					finalZ					= lineFloat.B.z * factor + (lineFloat.A.z * (1.0 - factor));
				intZ						= uint32_t(0xFFFFFFFFU * (finalZ));
				if(depthBuffer[line.A.y][line.A.x] <= intZ)
					continue;

				depthBuffer[line.A.y][line.A.x]	= intZ;
				//pixelCoords.push_back({(int32_t)line.A.x, (int32_t)line.A.y});
				pixelCoords.push_back({(float)line.A.x, (float)line.A.y, (float)finalZ});
			}
		}
		if (e2 <= dx) { /* e_xy+e_y < 0 */
			err								+= dx;
			line.A.y						+= sy;
			if( line.A.x >= 0 && line.A.x < (int32_t)pixels.metrics().x
			 && line.A.y >= 0 && line.A.y < (int32_t)pixels.metrics().y
			) {
				const double					factor					= 1.0 / (yAxis ? yDiff * line.A.y : xDiff * line.A.x);
				const double					finalZ					= lineFloat.B.z * factor + (lineFloat.A.z * (1.0 - factor));
				intZ						= uint32_t((0xFFFFFFFFU) * (finalZ));
				if(depthBuffer[line.A.y][line.A.x] <= intZ)
					continue;

				depthBuffer[line.A.y][line.A.x]	= intZ;
				//pixelCoords.push_back({(int32_t)line.A.x, (int32_t)line.A.y});
				pixelCoords.push_back({(float)line.A.x, (float)line.A.y, (float)finalZ});

			}
		}

	}
	return 0;
}

//https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/
double									orient2d				(const ::gpk::SLine2<int32_t>	& segment, const ::gpk::SCoord2<int32_t>& point)		{ return (segment.B.x - segment.A.x) * (point.y - segment.A.y) - (segment.B.y - segment.A.y) * (point.x - segment.A.x); }
double									orient2d				(const ::gpk::SLine3<int32_t>	& segment, const ::gpk::SCoord2<int32_t>& point)		{ return (segment.B.x - segment.A.x) * (point.y - segment.A.y) - (segment.B.y - segment.A.y) * (point.x - segment.A.x); }
double									orient2d				(const ::gpk::SLine3<float>		& segment, const ::gpk::SCoord2<int32_t>& point)		{ return (segment.B.x - segment.A.x) * (point.y - segment.A.y) - (segment.B.y - segment.A.y) * (point.x - segment.A.x); }

template <typename _tValue>	_tValue 	max3					(_tValue & a, _tValue & b, _tValue & c)			{ return ::std::max(::std::max(a, b), c); }
template <typename _tValue>	_tValue 	min3					(_tValue & a, _tValue & b, _tValue & c)			{ return ::std::min(::std::min(a, b), c); }

int								ced::drawTriangle		(::gpk::view_grid<::gpk::SColorBGRA> pixels, ::gpk::STriangle2<int32_t> triangle, ::gpk::SColorBGRA color){
	// Compute triangle bounding box
	int32_t								minX					= ::min3(triangle.A.x, triangle.B.x, triangle.C.x);
	int32_t								minY					= ::min3(triangle.A.y, triangle.B.y, triangle.C.y);
	int32_t								maxX					= ::max3(triangle.A.x, triangle.B.x, triangle.C.x);
	int32_t								maxY					= ::max3(triangle.A.y, triangle.B.y, triangle.C.y);

	// Clip against screen bounds
	minX							= ::std::max(minX, 0);
	minY							= ::std::max(minY, 0);
	maxX							= ::std::min(maxX, (int32_t)pixels.metrics().x	- 1);
	maxY							= ::std::min(maxY, (int32_t)pixels.metrics().y	- 1);

	// Rasterize
	::gpk::SCoord2<int32_t>				p;
	for (p.y = minY; p.y <= maxY; p.y++) {
		for (p.x = minX; p.x <= maxX; p.x++) {
			// Determine barycentric coordinates
			double									w0						= ::orient2d({triangle.B, triangle.C}, p);
			double									w1						= ::orient2d({triangle.C, triangle.A}, p);
			double									w2						= ::orient2d({triangle.A, triangle.B}, p);
			// If p is on or inside all edges, render pixel.
			if (w0 < 0 || w1 < 0 || w2 < 0)
				continue;
			::ced::setPixel(pixels, p, color);
		}
	}
	return 0;
}

int								ced::drawTriangle
	( const ::gpk::SCoord2<uint32_t>					targetSize
	, const ::gpk::STriangle3<float>					triangle
	, ::gpk::array_pod<::gpk::SCoord2<int32_t>>			& pixelCoords
	, ::gpk::array_pod<::gpk::STriangleWeights<double>>	& proportions
	, ::gpk::view_grid<uint32_t>						depthBuffer
	)	{
	// Compute triangle bounding box
	int32_t								minX					= (int32_t)::min3(triangle.A.x, triangle.B.x, triangle.C.x);
	int32_t								minY					= (int32_t)::min3(triangle.A.y, triangle.B.y, triangle.C.y);
	int32_t								maxX					= (int32_t)::max3(triangle.A.x, triangle.B.x, triangle.C.x);
	int32_t								maxY					= (int32_t)::max3(triangle.A.y, triangle.B.y, triangle.C.y);

	// Clip against screen bounds
	minX							= ::std::max(minX, 0);
	minY							= ::std::max(minY, 0);
	maxX							= ::std::min(maxX, (int32_t)targetSize.x - 1);
	maxY							= ::std::min(maxY, (int32_t)targetSize.y - 1);

	// Rasterize
	::gpk::SCoord2<int32_t> p;
	for (p.y = minY; p.y <= maxY; ++p.y)
	for (p.x = minX; p.x <= maxX; ++p.x) {
		// Determine barycentric coordinates
		double								w0						= ::orient2d({triangle.B, triangle.C}, p);
		double								w1						= ::orient2d({triangle.C, triangle.A}, p);
		double								w2						= ::orient2d({triangle.A, triangle.B}, p);
		// If p is on or inside all edges, render pixel.
		if (w0 < 0 || w1 < 0 || w2 < 0)
			continue;

		double								proportionABC			= w0 + w1 + w2;
		if(proportionABC == 0)
			continue;
		double								proportionA				= w0 / proportionABC;
		double								proportionB				= w1 / proportionABC;
		double								proportionC				= 1.0 - (proportionA + proportionB);

		// Calculate interpolated depth
		double								finalZ					= triangle.A.z * proportionA;
		finalZ							+= triangle.B.z * proportionB;
		finalZ							+= triangle.C.z * proportionC;
		if(finalZ >= 1.0 || finalZ <= 0)
			continue;

		uint32_t							intZ					= uint32_t(0xFFFFFFFFU * finalZ);
		if(depthBuffer[p.y][p.x] < intZ)
			continue;

		depthBuffer[p.y][p.x] = intZ;
		pixelCoords.push_back(p);
		proportions.push_back({proportionA, proportionB, proportionC});

	}
	return 0;
}

int													ced::drawQuadTriangle
	( const ::gpk::view_grid<::gpk::SColorBGRA>			targetPixels
	, const ::ced::SGeometryQuads						& geometry
	, const int											iTriangle
	, const ::gpk::SMatrix4<float>						& matrixTransform
	, const ::gpk::SMatrix4<float>						& matrixView
	, const ::gpk::SCoord3<float>						& lightVector
	, ::gpk::array_pod<::gpk::SCoord2<int32_t>>			& pixelCoords
	, ::gpk::array_pod<::gpk::STriangleWeights<double>>	& pixelVertexWeights
	, ::gpk::view_grid<::gpk::SColorBGRA>				textureImage
	, ::gpk::array_pod<::gpk::SCoord3<float>>			& lightPoints
	, ::gpk::array_pod<::gpk::SColorBGRA>				& lightColors
	, ::gpk::view_grid<uint32_t>						depthBuffer
	) {
	::gpk::STriangle3	<float>								triangleWorld		= geometry.Triangles	[iTriangle];
	::gpk::STriangle3	<float>								triangle			= triangleWorld;
	::gpk::SCoord3		<float>								normal				= geometry.Normals		[iTriangle / 2];

	triangleWorld.A										= matrixTransform.Transform(triangleWorld.A);
	triangleWorld.B										= matrixTransform.Transform(triangleWorld.B);
	triangleWorld.C										= matrixTransform.Transform(triangleWorld.C);

	triangle											= triangleWorld;
	triangle.A											= matrixView.Transform(triangle.A);
	triangle.B											= matrixView.Transform(triangle.B);
	triangle.C											= matrixView.Transform(triangle.C);
	if(triangle.A.z < 0 || triangle.A.z >= 1) return 0;
	if(triangle.B.z < 0 || triangle.B.z >= 1) return 0;
	if(triangle.C.z < 0 || triangle.C.z >= 1) return 0;

	normal												= matrixTransform.TransformDirection(normal).Normalize();

	::ced::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);
	::gpk::SCoord2<float>										imageUnit				= {textureImage.metrics().x - 1.0f, textureImage.metrics().y - 1.0f};
	const ::gpk::STriangle2	<float>								& triangleTexCoords		= geometry.TextureCoords	[iTriangle];
	double														lightFactorDirectional	= normal.Dot(lightVector);
	(void)lightFactorDirectional;
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		::gpk::SCoord2<int32_t>										pixelCoord				= pixelCoords		[iPixelCoord];
		const ::gpk::STriangleWeights<double>						& vertexWeights			= pixelVertexWeights[iPixelCoord];
		::gpk::SCoord2<float>										texCoord				= triangleTexCoords.A * vertexWeights.A;
		texCoord												+= triangleTexCoords.B * vertexWeights.B;
		texCoord												+= triangleTexCoords.C * vertexWeights.C;
		::gpk::SCoord3<float>										position				= triangleWorld.A * vertexWeights.A;
		position												+= triangleWorld.B * vertexWeights.B;
		position												+= triangleWorld.C * vertexWeights.C;
		::gpk::SColorFloat												texelColor				= textureImage[(uint32_t)(texCoord.y * imageUnit.y)][(uint32_t)(texCoord.x * imageUnit.x)];
		::gpk::SColorFloat												fragmentColor			= {};
		for(uint32_t iLight = 0; iLight < lightPoints.size(); ++iLight) {
			::gpk::SCoord3<float>										lightToPoint		= lightPoints[iLight] - position;
			::gpk::SCoord3<float>										vectorToLight		= lightToPoint;
			double														lightFactor			= vectorToLight.Dot(normal.Normalize());
			if(lightToPoint.Length() > 5 || lightFactor <= 0)
				continue;
			double														range				= 10;
			double														invAttenuation		= ::std::max(0.0, 1.0 - (lightToPoint.Length() / range));
			fragmentColor											+= ::gpk::SColorFloat{texelColor * lightColors[iLight] * invAttenuation * .5};
		}
		::ced::setPixel(targetPixels, pixelCoord, texelColor *.5 + fragmentColor);
		(void)lightVector;
	}
	return 0;
}

int													ced::drawQuadTriangle
	( const ::gpk::view_grid<::gpk::SColorBGRA>			targetPixels
	, const ::ced::SGeometryQuads						& geometry
	, const int											iTriangle
	, const ::gpk::SMatrix4<float>						& matrixTransform
	, const ::gpk::SMatrix4<float>						& matrixView
	, const ::gpk::SCoord3<float>						& lightVector
	, const ::gpk::SColorBGRA							color
	, ::gpk::array_pod<::gpk::SCoord2<int32_t>>			& pixelCoords
	, ::gpk::array_pod<::gpk::STriangleWeights<double>>	& pixelVertexWeights
	, ::gpk::view_grid<uint32_t>						depthBuffer
	) {
	::gpk::STriangle3	<float>								triangle			= geometry.Triangles	[iTriangle];
	::gpk::SCoord3		<float>								normal				= geometry.Normals		[iTriangle / 2];
	normal												= matrixTransform.TransformDirection(normal).Normalize();

	triangle.A											= matrixTransform.Transform(triangle.A);
	triangle.B											= matrixTransform.Transform(triangle.B);
	triangle.C											= matrixTransform.Transform(triangle.C);

	triangle.A											= matrixView.Transform(triangle.A);
	triangle.B											= matrixView.Transform(triangle.B);
	triangle.C											= matrixView.Transform(triangle.C);
	if(triangle.A.z < 0 || triangle.A.z >= 1) return 0;
	if(triangle.B.z < 0 || triangle.B.z >= 1) return 0;
	if(triangle.C.z < 0 || triangle.C.z >= 1) return 0;

	double													lightFactor			= normal.Dot(lightVector);

	::ced::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		::gpk::SCoord2<int32_t>									pixelCoord			= pixelCoords[iPixelCoord];
		//color.r										= (uint8_t)(0xFF * pixelVertexWeights[iPixelCoord].A);
		//color.g										= (uint8_t)(0xFF * pixelVertexWeights[iPixelCoord].B);
		//color.b										= (uint8_t)(0xFF * pixelVertexWeights[iPixelCoord].C);
		::ced::setPixel(targetPixels, pixelCoord, (color * 0.1) + color * lightFactor);
	}
	return 0;
}

int													ced::drawTriangle
	( const ::gpk::view_grid<::gpk::SColorBGRA>			targetPixels
	, const ::ced::SGeometryTriangles					& geometry
	, const int											iTriangle
	, const ::gpk::SMatrix4<float>						& matrixTransform
	, const ::gpk::SMatrix4<float>						& matrixView
	, const ::gpk::SCoord3<float>						& lightVector
	, const ::gpk::SColorBGRA							color
	, ::gpk::array_pod<::gpk::SCoord2<int32_t>>			& pixelCoords
	, ::gpk::array_pod<::gpk::STriangleWeights<double>>	& pixelVertexWeights
	, ::gpk::view_grid<uint32_t>						depthBuffer
	) {
	::gpk::STriangle3			<float>						triangle			= geometry.Triangles	[iTriangle];
	const ::gpk::STriangle3	<float>						& triangleNormals	= geometry.Normals		[iTriangle];
	triangle.A											= matrixTransform.Transform(triangle.A);
	triangle.B											= matrixTransform.Transform(triangle.B);
	triangle.C											= matrixTransform.Transform(triangle.C);

	triangle.A											= matrixView.Transform(triangle.A);
	triangle.B											= matrixView.Transform(triangle.B);
	triangle.C											= matrixView.Transform(triangle.C);
	if(triangle.A.z < 0 || triangle.A.z >= 1) return 0;
	if(triangle.B.z < 0 || triangle.B.z >= 1) return 0;
	if(triangle.C.z < 0 || triangle.C.z >= 1) return 0;

	::ced::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		::gpk::SCoord2<int32_t>									pixelCoord				= pixelCoords		[iPixelCoord];
		const ::gpk::STriangleWeights<double>					& vertexWeights			= pixelVertexWeights[iPixelCoord];
		::gpk::SCoord3<float>									normal					= triangleNormals.A * vertexWeights.A;
		normal												+= triangleNormals.B * vertexWeights.B;
		normal												+= triangleNormals.C * vertexWeights.C;
		normal												= matrixTransform.TransformDirection(normal).Normalize();
		double													lightFactor			= normal.Dot(lightVector);
		//color.r										= (uint8_t)(0xFF * pixelVertexWeights[iPixelCoord].A);
		//color.g										= (uint8_t)(0xFF * pixelVertexWeights[iPixelCoord].B);
		//color.b										= (uint8_t)(0xFF * pixelVertexWeights[iPixelCoord].C);
		::ced::setPixel(targetPixels, pixelCoord, color * lightFactor);
	}
	return 0;
}

int													ced::drawTriangle
	( const ::gpk::view_grid<::gpk::SColorBGRA>			targetPixels
	, const ::ced::SGeometryTriangles					& geometry
	, const int											iTriangle
	, const ::gpk::SMatrix4<float>						& matrixTransform
	, const ::gpk::SMatrix4<float>						& matrixView
	, const ::gpk::SCoord3<float>						& lightVector
	, ::gpk::array_pod<::gpk::SCoord2<int32_t>>			& pixelCoords
	, ::gpk::array_pod<::gpk::STriangleWeights<double>>	& pixelVertexWeights
	, ::gpk::view_grid<::gpk::SColorBGRA>				textureImage
	, ::gpk::view_grid<uint32_t>						depthBuffer
	) {
	::gpk::STriangle3		<float>								triangle			= geometry.Triangles		[iTriangle];
	const ::gpk::STriangle3	<float>								& triangleNormals	= geometry.Normals			[iTriangle];
	const ::gpk::STriangle2	<float>								& triangleTexCoords	= geometry.TextureCoords	[iTriangle];
	triangle.A											= matrixTransform.Transform(triangle.A);
	triangle.B											= matrixTransform.Transform(triangle.B);
	triangle.C											= matrixTransform.Transform(triangle.C);

	triangle.A											= matrixView.Transform(triangle.A);
	triangle.B											= matrixView.Transform(triangle.B);
	triangle.C											= matrixView.Transform(triangle.C);
	if(triangle.A.z < 0 || triangle.A.z >= 1)
		return 0;
	if(triangle.B.z < 0 || triangle.B.z >= 1)
		return 0;
	if(triangle.C.z < 0 || triangle.C.z >= 1)
		return 0;

	::ced::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);

	::gpk::SCoord2<float>										imageUnit			= {textureImage.metrics().x - 1.0f, textureImage.metrics().y - 1.0f};
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		::gpk::SCoord2<int32_t>										pixelCoord				= pixelCoords		[iPixelCoord];
		const ::gpk::STriangleWeights<double>						& vertexWeights			= pixelVertexWeights[iPixelCoord];
		::gpk::SCoord3<float>										normal					= triangleNormals.A * vertexWeights.A;
		normal													+= triangleNormals.B * vertexWeights.B;
		normal													+= triangleNormals.C * vertexWeights.C;
		normal													= matrixTransform.TransformDirection(normal).Normalize();

		double														lightFactor				= normal.Dot(lightVector);

		::gpk::SCoord2<float>										texCoord				= triangleTexCoords.A * vertexWeights.A;
		texCoord												+= triangleTexCoords.B * vertexWeights.B;
		texCoord												+= triangleTexCoords.C * vertexWeights.C;
		::gpk::SColorBGRA												texelColor				= textureImage[(uint32_t)(texCoord.y * imageUnit.y)][(uint32_t)(texCoord.x * imageUnit.x)];
		::ced::setPixel(targetPixels, pixelCoord, (texelColor * .3) + texelColor * lightFactor);
	}
	return 0;
}
