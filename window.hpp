/*
Copyright 2007-2011 Jan Niklas Hasse <jhasse@gmail.com>

This file is part of JNGL.

JNGL is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

JNGL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with JNGL.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "jngl.hpp"
#include "freetype.hpp"
#include "opengl.hpp"

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/type_traits.hpp>
#include <boost/array.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include <map>
#include <stack>

#ifdef __linux
	#ifdef WIZ

	#else
		#include <GL/glx.h>
		#include <GL/glu.h>
		#include <X11/extensions/xf86vmode.h>
		#include <X11/keysym.h>
	#endif
#else
    #ifdef __APPLE__
        #include <SDL.h>
    #else
        #include <windows.h>
    #endif
#endif

namespace jngl
{
	bool Init(const int width, const int height); // Definied in main.cpp
	class Window
	{
	public:
		Window(const std::string& title, const int width, const int height, const bool fullscreen);
		~Window();
		bool Running();
		void BeginDraw();
		void EndDraw();
		void Quit();
		void Continue();
		void SetMouseVisible(const bool visible);
		bool GetMouseVisible() const;
		int MouseX();
		int MouseY();
		int GetWidth() const;
		int GetHeight() const;
		double GetTextWidth(const std::string&);
		int GetLineHeight();
		bool GetFullscreen() const;
		bool KeyDown(const key::KeyType key);
		bool KeyPressed(const key::KeyType key);
		bool KeyDown(const std::string& key);
		bool KeyPressed(const std::string& key);
		bool MouseDown(mouse::Button button);
		bool MousePressed(mouse::Button button);
		void SetMouse(const int xposition, const int yposition);
		void SetTitle(const std::string& title);
		void Print(const std::string& text, const int xposition, const int yposition);
		int GetFontSize() const;
		void SetFontSize(const int size);
		void SetFont(const std::string&);
		void SetFontByName(const std::string&);
		bool IsMultisampleSupported() const;
		void SetIcon(const std::string&);
		void UpdateKeyStates();
		double GetMouseWheel() const;
#ifndef __APPLE__
	#ifdef __linux
		boost::shared_ptr<Display> pDisplay_;
		static void ReleaseXData(void*);
	#else
		static void ReleaseDC(HWND, HDC);
		static void ReleaseRC(HGLRC);
	#endif
#endif
	private:
		int GetKeyCode(jngl::key::KeyType key);
		std::string GetFontFileByName(const std::string& fontname);
		bool fullscreen_, running_, isMouseVisible_, isMultisampleSupported_, anyKeyPressed_;
		boost::array<bool, 3> mouseDown_;
		boost::array<bool, 3> mousePressed_;
		std::map<unsigned int, bool> keyDown_;
		std::map<unsigned int, bool> keyPressed_;
		std::map<std::string, bool> characterDown_;
		std::map<std::string, bool> characterPressed_;
		std::stack<bool*> needToBeSetFalse_;
		int mousex_, mousey_, fontSize_, width_, height_;
		double mouseWheel_;
		std::string fontName_;
		const static unsigned int PNG_BYTES_TO_CHECK = 4;

		// <fontSize, <fontName, Font> >
		boost::ptr_map<int, boost::ptr_map<std::string, Font> > fonts_;
#ifndef __APPLE__
#ifdef WIZ
		NativeWindowType nativeWindow_;
		EGLDisplay display_;
		EGLContext context_;
		EGLSurface surface_;
		std::map<unsigned int, double> keyTimeout_;
#else
	#ifdef __linux
		::Window window_;
		GLXContext context_;
		int screen_;
		XF86VidModeModeInfo oldMode_;
	#else
		boost::shared_ptr<boost::remove_pointer<HGLRC>::type> pRenderingContext_;
		boost::shared_ptr<boost::remove_pointer<HWND>::type> pWindowHandle_;
		boost::shared_ptr<boost::remove_pointer<HDC>::type> pDeviceContext_;
		int arbMultisampleFormat_;

		bool InitMultisample(HINSTANCE, PIXELFORMATDESCRIPTOR);
		void Init(const std::string& title, bool multisample);
		void DistinguishLeftRight();
	#endif
#endif
#else
		SDL_Window* sdlWindow;
		SDL_GLContext context;
#endif
	};
}
