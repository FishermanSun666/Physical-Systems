/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include <string>
#include <vector>

#include "Vector2.h"
#include "Vector4.h"

namespace PhysicalProject {
	namespace Maths {
		class Vector2;
		class Vector3;
		class Vector4;
	}
	namespace Rendering {
		class TextureBase;

		class SimpleFont	{
		public:

			SimpleFont(const std::string& fontName, const std::string& texName);
			~SimpleFont();

			struct InterleavedTextVertex {
				PhysicalProject::Maths::Vector2 pos;
				PhysicalProject::Maths::Vector2 texCoord;
				PhysicalProject::Maths::Vector4 colour;
			};

			int GetVertexCountForString(const std::string& text);
			void BuildVerticesForString(const std::string& text, const Maths::Vector2& startPos, const Maths::Vector4& colour, float size, std::vector<Maths::Vector3>&positions, std::vector<Maths::Vector2>&texCoords, std::vector<Maths::Vector4>&colours);
			void BuildInterleavedVerticesForString(const std::string& text, const Maths::Vector2& startPos, const Maths::Vector4& colour, float size, std::vector<InterleavedTextVertex>&vertices);

			const TextureBase* GetTexture() const {
				return texture;
			}



		protected:
			//matches stbtt_bakedchar
			struct FontChar {
				unsigned short x0;
				unsigned short y0;
				unsigned short x1;
				unsigned short y1;
				float xOff;
				float yOff;
				float xAdvance;
			};

			FontChar*		allCharData;
			TextureBase*	texture;

			int startChar;
			int numChars;

			float texWidth;
			float texHeight;
			float texWidthRecip;
			float texHeightRecip;
		};
	}
}

