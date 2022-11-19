// An example of processing mouse events in an OpenGL program using


#include "test_gpk_opengl.h"	//

#include <GL/gl.h>			/* OpenGL header file */
#include <GL/glu.h>			/* OpenGL utilities header file */
#include <stdio.h>


enum { 
	PAN = 1,				/* pan state bit */
	ROTATE,				/* rotate state bits */
	ZOOM				/* zoom state bit */
};


HPALETTE	hPalette = 0;			/* custom palette (if needed) */
GLfloat		trans[3];			/* current translation */
GLfloat		rot[2];				/* current rotation */


static void update(int state, int ox, int nx, int oy, int ny) {
	int dx = ox - nx;
	int dy = ny - oy;

	switch(state) {
	case PAN:
		trans[0] -= dx / 100.0f;
		trans[1] -= dy / 100.0f;
		break;
	case ROTATE:
		rot[0] += (dy * 180.0f) / 500.0f;
		rot[1] -= (dx * 180.0f) / 500.0f;
#define clamp(x) x = x > 360.0f ? x-360.0f : x < -360.0f ? x+=360.0f : x
		clamp(rot[0]);
		clamp(rot[1]);
		break;
	case ZOOM:
		trans[2] -= (dx+dy) / 100.0f;
		break;
	}
}

static void reshape(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (float)width/height, 0.001, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.0f);
}

static void display(HDC hDC) {
	/* rotate a triangle around */
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glTranslatef(trans[0], trans[1], trans[2]);
	glRotatef(rot[0], 1.0f, 0.0f, 0.0f);
	glRotatef(rot[1], 0.0f, 1.0f, 0.0f);
	glBegin(GL_TRIANGLES);

#define TOP glIndexi(1); glColor3f(1.0f, 0.0f, 0.0f); glVertex3i(0, 1, 0)
#define FR  glIndexi(2); glColor3f(0.0f, 1.0f, 0.0f); glVertex3i(1, -1, 1)
#define FL  glIndexi(3); glColor3f(0.0f, 0.0f, 1.0f); glVertex3i(-1, -1, 1)
#define BR  glIndexi(3); glColor3f(0.0f, 0.0f, 1.0f); glVertex3i(1, -1, -1)
#define BL  glIndexi(2); glColor3f(0.0f, 1.0f, 0.0f); glVertex3i(-1, -1, -1)

	TOP; FL; FR;
	TOP; FR; BR;
	TOP; BR; BL;
	TOP; BL; FL;
	FR; FL; BL;
	BL; BR; FR;
	
	glEnd();
	glPopMatrix();
	glFlush();
	SwapBuffers(hDC);			/* nop if singlebuffered */
}


LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { 
	static PAINTSTRUCT ps;
	static GLboolean left  = GL_FALSE;	/* left button currently down? */
	static GLboolean right = GL_FALSE;	/* right button currently down? */
	static GLuint    state   = 0;	/* mouse state flag */
	static int omx, omy, mx, my;

	switch(uMsg) {
	case WM_PAINT:
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		return 0;

	case WM_SIZE:
		reshape(LOWORD(lParam), HIWORD(lParam));
		PostMessage(hWnd, WM_PAINT, 0, 0);
		return 0;

	case WM_CHAR:
		switch (wParam) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
	return 0;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	/* if we don't set the capture we won't get mouse move
		   messages when the mouse moves outside the window. */
	SetCapture(hWnd);
	mx = LOWORD(lParam);
	my = HIWORD(lParam);
	if (uMsg == WM_LBUTTONDOWN)
		state |= PAN;
	if (uMsg == WM_RBUTTONDOWN)
		state |= ROTATE;
	return 0;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	/* remember to release the capture when we are finished. */
	ReleaseCapture();
	state = 0;
	return 0;

	case WM_MOUSEMOVE:
	if (state) {
		omx = mx;
		omy = my;
		mx = LOWORD(lParam);
		my = HIWORD(lParam);
		/* Win32 is pretty braindead about the x, y position that
		   it returns when the mouse is off the left or top edge
		   of the window (due to them being unsigned). therefore,
		   roll the Win32's 0..2^16 pointer co-ord range to the
		   more amenable (and useful) 0..+/-2^15. */
		if(mx & 1 << 15) mx -= (1 << 16);
		if(my & 1 << 15) my -= (1 << 16);
		update(state, omx, mx, omy, my);
		PostMessage(hWnd, WM_PAINT, 0, 0);
	}
	return 0;

	case WM_PALETTECHANGED:
	if (hWnd == (HWND)wParam)
		break;
	/* fall through to WM_QUERYNEWPALETTE */

	case WM_QUERYNEWPALETTE:
	if (hPalette) {
		HDC hDC = GetDC(hWnd);
		UnrealizeObject(hPalette);
		SelectPalette(hDC, hPalette, FALSE);
		RealizePalette(hDC);
		ReleaseDC(hWnd, hDC);
		return TRUE;
	}
	return FALSE;

	case WM_CLOSE:
	PostQuitMessage(0);
	return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam); 
} 

HWND IntiOpenGLForWindow(HWND hWnd, BYTE type, DWORD flags) {
	HDC							hDC		= GetDC(hWnd);

	/* there is no guarantee that the contents of the stack that become
	   the pfd are zeroed, therefore _make sure_ to clear these bits. */
	PIXELFORMATDESCRIPTOR		pfd		= {sizeof(pfd)};
	pfd.nVersion			= 1;
	pfd.dwFlags				= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | flags;
	pfd.iPixelType			= type;
	pfd.cDepthBits			= 32;
	pfd.cColorBits			= 32;

	int							pf = ChoosePixelFormat(hDC, &pfd);
	if (pf == 0) {
		MessageBox(NULL, TEXT("ChoosePixelFormat() failed: Cannot find a suitable pixel format."), TEXT("Error"), MB_OK); 
		return 0;
	} 
 
	if (SetPixelFormat(hDC, pf, &pfd) == FALSE) {
		MessageBox(NULL, TEXT("SetPixelFormat() failed: Cannot set format specified."), TEXT("Error"), MB_OK);
		return 0;
	} 

	DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	ReleaseDC(hWnd, hDC);
	return hWnd;
}    

#include "gpk_bitmap_target.h"
#include "gpk_grid_copy.h"
#include "gpk_grid_scale.h"
#include "gpk_view_bit.h"
#include "gpk_matrix.h"
#include "gpk_png.h"

#include "gpk_app_impl.h"

static constexpr	const uint32_t										ASCII_SCREEN_WIDTH							= 132	;
static constexpr	const uint32_t										ASCII_SCREEN_HEIGHT							= 50	;

GPK_DEFINE_APPLICATION_ENTRY_POINT(::SApplication, "Title");

					::SApplication										* g_app						= 0;

static				::gpk::error_t										updateSizeDependentResources				(::SApplication& app)											{
	const ::gpk::SCoord2<uint32_t>												newSize										= app.Framework.MainDisplay.Size;
	//::gpk::updateSizeDependentTarget(app.Framework.MainDisplayOffscreen->Color, newSize);
	//::gpk::updateSizeDependentTarget(app.Framework.MainDisplayOffscreen->DepthStencil, newSize);

	return 0;
}

// --- Cleanup application resources.
					::gpk::error_t										cleanup										(::SApplication& app)											{
	::gpk::SWindowPlatformDetail												& displayDetail								= app.Framework.MainDisplay.PlatformDetail;
	::gpk::mainWindowDestroy(app.Framework.MainDisplay);
	::UnregisterClass(displayDetail.WindowClassName, displayDetail.WindowClass.hInstance);
	g_app													= 0;
	return 0;
}

// Vertex coordinates for cube faces
static constexpr const ::gpk::STriangle3<float>						geometryCube	[12]						=
	{ {{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}	// Right	- first			?? I have no idea if this is correct lol
	, {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}}	// Right	- second		?? I have no idea if this is correct lol

	, {{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}	// Back		- first			?? I have no idea if this is correct lol
	, {{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}	// Back		- second		?? I have no idea if this is correct lol

	, {{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}	// Bottom	- first			?? I have no idea if this is correct lol
	, {{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}	// Bottom	- second		?? I have no idea if this is correct lol

	, {{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}	// Left		- first
	, {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}}	// Left		- second

	, {{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}}	// Front	- first
	, {{1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}}	// Front	- second

	, {{1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}}	// Top		- first
	, {{1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}}	// Top		- second
	};

					::gpk::error_t										setup										(::SApplication& app)											{
	g_app													= &app;
	::gpk::SFramework															& framework									= app.Framework;
	::gpk::SWindow																& mainWindow								= framework.MainDisplay;
	mainWindow.Size														= {640, 480};
	gerror_if(errored(::gpk::mainWindowCreate(mainWindow, framework.RuntimeValues.PlatformDetail, framework.Input)), "Failed to create main window why?!");

	static constexpr const ::gpk::SCoord3<float>								cubeCenter									= {0.5f, 0.5f, 0.5f};
	for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) {
		::gpk::STriangle3<float>													& transformedTriangle						= app.CubePositions[iTriangle];
		transformedTriangle														= geometryCube[iTriangle];
		transformedTriangle.A													-= cubeCenter;
		transformedTriangle.B													-= cubeCenter;
		transformedTriangle.C													-= cubeCenter;
	}
	ree_if(errored(::updateSizeDependentResources	(app)), "Cannot update offscreen and textures and this could cause an invalid memory access later on.");

	::IntiOpenGLForWindow(mainWindow.PlatformDetail.WindowHandle, PFD_TYPE_RGBA, PFD_DOUBLEBUFFER);
	framework.MainDisplayOffscreen = {};

	// create a rendering context  
	app.DrawingContext				= GetDC(framework.MainDisplay.PlatformDetail.WindowHandle);
	app.GLRenderContext				= wglCreateContext(app.DrawingContext); 
 
	// make it the calling thread's current rendering context 
	wglMakeCurrent (app.DrawingContext, app.GLRenderContext);
	return 0;
}

					::gpk::error_t										update										(::SApplication& app, bool systemRequestedExit)					{
	retval_ginfo_if(1, systemRequestedExit, "Exiting because the runtime asked for close. We could also ignore this value and just continue execution if we don't want to exit.");
	::gpk::error_t																frameworkResult								= ::gpk::updateFramework(app.Framework);
	ree_if(errored(frameworkResult), "Unknown error.");
	rvi_if(1, frameworkResult == 1, "Framework requested close. Terminating execution.");
	ree_if(errored(::updateSizeDependentResources(app)), "Cannot update offscreen and this could cause an invalid memory access later on.");
	//-----------------------------
	::gpk::STimer																& timer										= app.Framework.Timer;
	::gpk::SWindow																& mainWindow								= app.Framework.MainDisplay;
	char																		buffer		[256]							= {};
	sprintf_s(buffer, "[%u x %u]. FPS: %g. Last frame seconds: %g.", mainWindow.Size.x, mainWindow.Size.y, 1 / timer.LastTimeSeconds, timer.LastTimeSeconds);
	::HWND																		windowHandle								= mainWindow.PlatformDetail.WindowHandle;
	SetWindowTextA(windowHandle, buffer);
	

	HDC hDC = GetDC(mainWindow.PlatformDetail.WindowHandle);
	display(hDC);
	ReleaseDC(mainWindow.PlatformDetail.WindowHandle, hDC);

	return 0;
}

static constexpr const ::gpk::SCoord3<float>						geometryCubeNormals	[12]						=
	{ {0.0f, 0.0f, -1.0f}	// Right	- first			?? I have no idea if this is correct lol
	, {0.0f, 0.0f, -1.0f}	// Right	- second		?? I have no idea if this is correct lol

	, {-1.0f, 0.0f, 0.0f}	// Back		- first			?? I have no idea if this is correct lol
	, {-1.0f, 0.0f, 0.0f}	// Back		- second		?? I have no idea if this is correct lol

	, {0.0f, -1.0f, 0.0f}	// Bottom	- first			?? I have no idea if this is correct lol
	, {0.0f, -1.0f, 0.0f}	// Bottom	- second		?? I have no idea if this is correct lol

	, {0.0f, 0.0f, 1.0f}	// Left		- first
	, {0.0f, 0.0f, 1.0f}	// Left		- second

	, {1.0f, 0.0f, 0.0f}	// Front	- first
	, {1.0f, 0.0f, 0.0f}	// Front	- second

	, {0.0f, 1.0f, 0.0f}	// Top		- first
	, {0.0f, 1.0f, 0.0f}	// Top		- second
	};

struct SCamera {
						::gpk::SCoord3<float>								Position, Target;
};

					::gpk::error_t										draw										(::SApplication& app)											{	// --- This function will draw some coloured symbols in each cell of the ASCII screen.
	(void)app;
	//::gpk::SFramework															& framework									= app.Framework;
	//
	//::gpk::ptr_obj<::gpk::SRenderTarget<::gpk::SColorBGRA, uint32_t>>			backBuffer;
	//backBuffer->resize(framework.MainDisplayOffscreen->Color.metrics(), 0xFF000080, (uint32_t)-1);
	//
	////------------------------------------------------
	//::gpk::array_pod<::gpk::STriangle3<float>>									triangle3dList								= {};
	//::gpk::array_pod<::gpk::SColorBGRA>											triangle3dColorList							= {};
	//triangle3dList.resize(12);
	//triangle3dColorList.resize(12);
	//::gpk::SMatrix4<float>														projection									= {};
	//::gpk::SMatrix4<float>														viewMatrix									= {};
	//projection.Identity();
	//::gpk::SFrameInfo															& frameInfo									= framework.FrameInfo;
	//const ::gpk::SCoord3<float>													tilt										= {10, };	// ? cam't remember what is this. Radians? Eulers?
	//const ::gpk::SCoord3<float>													rotation									= {0, (float)frameInfo.FrameNumber / 100, 0};
	//
	//::gpk::SNearFar																nearFar										= {0.01f , 1000.0f};
	//
	//static constexpr const ::gpk::SCoord3<float>								cameraUp									= {0, 1, 0};	// ? cam't remember what is this. Radians? Eulers?
	//::SCamera																	camera										= {{10, 5, 0}, {}};
	//::gpk::SCoord3<float>														lightPos									= {10, 5, 0};
	//static float																cameraRotation								= 0;
	//cameraRotation															+= (float)framework.Input->MouseCurrent.Deltas.x / 5.0f;
	////camera.Position	.RotateY(cameraRotation);
	//camera.Position	.RotateY(frameInfo.Microseconds.Total / 1000000.0f);
	//lightPos		.RotateY(frameInfo.Microseconds.Total /  500000.0f * -2);
	//viewMatrix.LookAt(camera.Position, camera.Target, cameraUp);
	//const ::gpk::SCoord2<uint32_t>												& offscreenMetrics							= backBuffer->Color.View.metrics();
	//projection.FieldOfView(.25 * ::gpk::math_pi, offscreenMetrics.x / (double)offscreenMetrics.y, nearFar.Near, nearFar.Far );
	//projection																= viewMatrix * projection;
	//lightPos.Normalize();
	//
	//::gpk::SMatrix4<float>														viewport									= {};
	//viewport._11															= 2.0f / offscreenMetrics.x;
	//viewport._22															= 2.0f / offscreenMetrics.y;
	//viewport._33															= 1.0f / (float)(nearFar.Far - nearFar.Near);
	//viewport._43															= (float)(-nearFar.Near * ( 1.0f / (nearFar.Far - nearFar.Near) ));
	//viewport._44															= 1.0f;
	//projection																= projection * viewport.GetInverse();
	//for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) {
	//	::gpk::STriangle3<float>													& transformedTriangle						= triangle3dList[iTriangle];
	//	transformedTriangle														= app.CubePositions[iTriangle];
	//	::gpk::transform(transformedTriangle, projection);
	//}
	//::gpk::array_pod<::gpk::STriangle2<int32_t>>								triangle2dList								= {};
	//triangle2dList.resize(12);
	//const ::gpk::SCoord2<int32_t>												screenCenter								= {(int32_t)offscreenMetrics.x / 2, (int32_t)offscreenMetrics.y / 2};
	//for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) { // Maybe the scale
	//	::gpk::STriangle3<float>													& transformedTriangle3D						= triangle3dList[iTriangle];
	//	::gpk::STriangle2<int32_t>													& transformedTriangle2D						= triangle2dList[iTriangle];
	//	transformedTriangle2D.A													= {(int32_t)transformedTriangle3D.A.x, (int32_t)transformedTriangle3D.A.y};
	//	transformedTriangle2D.B													= {(int32_t)transformedTriangle3D.B.x, (int32_t)transformedTriangle3D.B.y};
	//	transformedTriangle2D.C													= {(int32_t)transformedTriangle3D.C.x, (int32_t)transformedTriangle3D.C.y};
	//	::gpk::translate(transformedTriangle2D, screenCenter);
	//}
	//
	//for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) {
	//	double																		lightFactor									= geometryCubeNormals[iTriangle].Dot(lightPos);
	//	triangle3dColorList[iTriangle]											= (::gpk::RED * lightFactor).Clamp();
	//}
	//::gpk::array_pod<::gpk::SCoord2<int32_t>>									trianglePixelCoords;
	//::gpk::array_pod<::gpk::SCoord2<int32_t>>									wireframePixelCoords;
	//::gpk::SCoord3<float> cameraFront = (camera.Target - camera.Position).Normalize();
	//for(uint32_t iTriangle = 0; iTriangle < 12; ++iTriangle) {
	//	double																		lightFactor									= geometryCubeNormals[iTriangle].Dot(cameraFront);
	//	if(lightFactor > 0)
	//		continue;
	//	gerror_if(errored(::gpk::drawTriangle(backBuffer->Color.View, triangle3dColorList[iTriangle], triangle2dList[iTriangle])), "Not sure if these functions could ever fail");
	//	//::gpk::drawLine(backBuffer->Color.View, (::gpk::SColorBGRA)::gpk::GREEN	, ::gpk::SLine2<int32_t>{triangle2dList[iTriangle].A, triangle2dList[iTriangle].B});
	//	//::gpk::drawLine(backBuffer->Color.View, (::gpk::SColorBGRA)::gpk::BLUE	, ::gpk::SLine2<int32_t>{triangle2dList[iTriangle].B, triangle2dList[iTriangle].C});
	//	//::gpk::drawLine(backBuffer->Color.View, (::gpk::SColorBGRA)::gpk::RED	, ::gpk::SLine2<int32_t>{triangle2dList[iTriangle].C, triangle2dList[iTriangle].A});
	//	//trianglePixelCoords.clear(); ::gpk::drawTriangle(offscreenMetrics, triangle2dList[iTriangle], trianglePixelCoords);
	//	//for(uint32_t iCoord = 0; iCoord < trianglePixelCoords.size(); ++iCoord)
	//	//	::gpk::drawPixelLight(backBuffer->Color.View, trianglePixelCoords[iCoord], (::gpk::SColorBGRA)::gpk::BLUE, 0.05f, 2.5);
	//	//wireframePixelCoords.clear(); ::gpk::drawLine(offscreenMetrics, ::gpk::SLine2<int32_t>{triangle2dList[iTriangle].A, triangle2dList[iTriangle].B}, wireframePixelCoords);
	//	//for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord)
	//	//	::gpk::drawPixelLight(backBuffer->Color.View, wireframePixelCoords[iCoord], (::gpk::SColorBGRA)::gpk::GREEN, 0.05f, 1.5);
	//	//wireframePixelCoords.clear(); ::gpk::drawLine(offscreenMetrics, ::gpk::SLine2<int32_t>{triangle2dList[iTriangle].B, triangle2dList[iTriangle].C}, wireframePixelCoords);
	//	//for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord)
	//	//	::gpk::drawPixelLight(backBuffer->Color.View, wireframePixelCoords[iCoord], (::gpk::SColorBGRA)::gpk::CYAN, 0.05f, 1.5);
	//	//wireframePixelCoords.clear(); ::gpk::drawLine(offscreenMetrics, ::gpk::SLine2<int32_t>{triangle2dList[iTriangle].C, triangle2dList[iTriangle].A}, wireframePixelCoords);
	//	//for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord)
	//	//	::gpk::drawPixelLight(backBuffer->Color.View, wireframePixelCoords[iCoord], (::gpk::SColorBGRA)::gpk::RED, 0.05f, 1.5);
	//	::gpk::drawLine(offscreenMetrics, ::gpk::SLine2<int32_t>{triangle2dList[iTriangle].A, triangle2dList[iTriangle].B}, wireframePixelCoords);
	//	::gpk::drawLine(offscreenMetrics, ::gpk::SLine2<int32_t>{triangle2dList[iTriangle].B, triangle2dList[iTriangle].C}, wireframePixelCoords);
	//	::gpk::drawLine(offscreenMetrics, ::gpk::SLine2<int32_t>{triangle2dList[iTriangle].C, triangle2dList[iTriangle].A}, wireframePixelCoords);
	//}
	//for(uint32_t iCoord = 0; iCoord < wireframePixelCoords.size(); ++iCoord)
	//	::gpk::drawPixelLight(backBuffer->Color.View, wireframePixelCoords[iCoord], (::gpk::SColorBGRA)::gpk::GREEN, 0.05f, 1.5);
	//
	//::gpk::grid_mirror_y(framework.MainDisplayOffscreen->Color.View, backBuffer->Color.View);
	////framework.MainDisplayOffscreen = backBuffer;
	////------------------------------------------------
	return 0;
}

