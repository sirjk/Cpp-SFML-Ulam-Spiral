#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Tilemap : public Drawable, public Transformable
{
	VertexArray m_vertices;
	Texture m_tileset;

	virtual void draw(RenderTarget& target, RenderStates states) const {

		//apply the transform
		states.transform *= getTransform();

		//apply the tileset texture
		states.texture = &m_tileset;

		//draw the vertex array
		target.draw(m_vertices, states);

	}

public:

	bool load(const std::string& tileset, Vector2u tileSize, int** tiles, unsigned int width, unsigned int height) {

		if (!m_tileset.loadFromFile(tileset))
			return false;

		m_vertices.setPrimitiveType(Quads);
		m_vertices.resize(width * height * 4);

		for (unsigned int i = 0; i < width; ++i)			//width=316, height=316
			for (unsigned int j = 0; j < height; ++j) {

				//int tileNumber = tiles[i + j * width];
				int tileNumber = tiles[j][i];

				int tu = tileNumber % (m_tileset.getSize().x / tileSize.x);
				int tv = tileNumber / (m_tileset.getSize().x / tileSize.x);

				Vertex* quad = &m_vertices[(i + j * width) * 4];

				quad[0].position = Vector2f(i * tileSize.x, j * tileSize.y);
				quad[1].position = Vector2f((i + 1) * tileSize.x, j * tileSize.y);
				quad[2].position = Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);
				quad[3].position = Vector2f(i * tileSize.x, (j + 1) * tileSize.y);

				quad[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
				quad[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
				quad[2].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
				quad[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
			}
		return true;
	}
};

