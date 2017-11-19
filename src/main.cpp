#include <iostream>

#include <SFML/Window.hpp>

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <SFML/Graphics/Image.hpp>
#include <SFML/System/Clock.hpp>

#include "ShaderProgram.hpp"
#include "GLHelper.hpp"
#include "Sampler2D.hpp"


static void initGLEW()
{
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	if (GLEW_OK != err)
		std::cerr << "Error while initializing GLEW: " << glewGetErrorString(err) << std::endl;

	std::cout << "Using GLEW version: " << glewGetString(GLEW_VERSION) << std::endl;
}

int main()
{
	/* Window creation, OpenGL initialization */
	sf::ContextSettings openGL3DContext(24, 0, 0, //depth, stencil, antialiasing
		3, 3, //openGL 3.3 requested
		sf::ContextSettings::Core);
	sf::Window window;
	window.create(sf::VideoMode(256, 256), "Flow", sf::Style::Titlebar | sf::Style::Close, openGL3DContext);
	window.setVerticalSyncEnabled(true);
	initGLEW();

	std::cout << "Using OpenGL version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Using OpenGL renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Using GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl << std::endl;

	sf::Image img;
	img.loadFromFile("rc/big.png");
	img.flipVertically();

	glm::uvec2 bufferSize(img.getSize().x, img.getSize().y);
	std::vector<float> density;
	for (unsigned int iY = 0u; iY < img.getSize().y; ++iY) {
		for (unsigned int iX = 0u; iX < img.getSize().x; ++iX) {
			density.emplace_back(img.getPixel(iX, iY).r);
		}
	}

	std::vector<fvec2> pt;
	{
		uvec2 densitySize(bufferSize.x, bufferSize.y);
		Sampler2D sampler(densitySize, density);

		sf::Clock clock;
		unsigned int nbSamples = 10000;
		for (unsigned int i = 0u; i < nbSamples; ++i) {
			pt.emplace_back(sampler.sample());
		}
		std::cout << "Computed " << nbSamples << " samples in " << clock.getElapsedTime().asSeconds() << " seconds";
		std::cout << " (" << static_cast<float>(nbSamples) / clock.getElapsedTime().asSeconds() << " samples/sec)." << std::endl;
	}

	GLuint texId = 0u;
	GLCHECK(glGenTextures(1, &texId));
	GLCHECK(glBindTexture(GL_TEXTURE_2D, texId));
	GLCHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getSize().x, img.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr()));
	GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCHECK(glBindTexture(GL_TEXTURE_2D, 0));

	/* VAO creation */
	GLuint _emptyVAO = 0u;
	GLCHECK(glGenVertexArrays(1, &_emptyVAO));
	GLCHECK(glBindVertexArray(_emptyVAO));

	/* VBO creation */
	GLuint _ptVBO = 0u;
	GLCHECK(glGenBuffers(1, &_ptVBO));
	GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, _ptVBO));
	GLCHECK(glBufferData(GL_ARRAY_BUFFER, pt.size() * sizeof(glm::vec2), pt.data(), GL_STATIC_DRAW));
	const GLuint ptALoc = 0u;
	GLCHECK(glEnableVertexAttribArray(ptALoc));
	GLCHECK(glVertexAttribPointer(ptALoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0));

	GLCHECK(glBindVertexArray(0u));

	ShaderProgram pointsShader, texShader;
	pointsShader.loadFromFile("shaders/points.vert", "shaders/points.frag");
	texShader.loadFromFile("shaders/tex.vert", "shaders/tex.frag");

	window.setActive(true);
	glViewport(0, 0, window.getSize().x, window.getSize().y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	{
		//ShaderProgram::bind(texShader);

		//TextureBinder binder;
		//binder.bindTexture(texShader, "tex", texId);

		//GLCHECK(glBindVertexArray(_emptyVAO));
		//GLCHECK(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
		//GLCHECK(glBindVertexArray(0));

		//ShaderProgram::unbind();
	}
	{
		ShaderProgram::bind(pointsShader);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		GLCHECK(glPointSize(1.f));
		GLCHECK(glBindVertexArray(_emptyVAO));
		GLCHECK(glDrawArrays(GL_POINTS, 0, pt.size()));
		GLCHECK(glBindVertexArray(0));

		ShaderProgram::unbind();
	}

	window.display();

	{
		char c;
		std::cin >> c;
	}

	return EXIT_SUCCESS;
}