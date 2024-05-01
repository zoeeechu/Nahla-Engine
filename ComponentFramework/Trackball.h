#ifndef TRACKBALL_H
#define	TRACKBALL_H
#include "MMath.h"
union SDL_Event;

	using namespace MATH;	

	class Trackball {
	private:
		bool mouseDown;
		Quaternion mouseRotationQuat;
		// UN - keep track of the orientation before we start rotating things
		Quaternion prevQuat;
		Matrix4 invNDC;			/// the inverse of the viewportNDC matrix
		Vec3 beginV, endV;		/// Begin and end points after being transformed by invNDC
	public:
		Trackball(); 
		~Trackball();
		Trackball(const Trackball&) = delete;
		Trackball(Trackball&&) = delete;
		Trackball& operator=(const Trackball&) = delete;
		Trackball& operator=(Trackball&&) = delete;

		const Quaternion getQuat() const { return mouseRotationQuat; }
		void HandleEvents(const SDL_Event &sdlEvent);
		void setWindowDimensions();
	private:
		/// Just some functions for internal use only thus private
		const Vec3 getMouseVector(int x, int y);
		///Any time the window dimensions change like stretching it we will
		/// need to rebuild the invNDC matrix
		void onLeftMouseDown(int x, int y);
		void onLeftMouseUp(int x, int y);
		void onMouseMove(int x, int y);
	};

#endif

