#include "CommonShaders.h"
#include "GraphicsContext.h"


renderers* renderers::singleton = nullptr;


GradientShader2::GradientShader2(GraphicsContext* gc)
{
	_shaderprogram = new ShaderProgram2<glm::vec2, glm::vec4>(
		"position", "color",
		VERTEX_SHADER
		({
			attribute vec3 position;
			attribute vec4 color;
			uniform mat4 transform;
			uniform float point_size;
			varying vec4 v_color;

			void main()
			{
				vec4 p = transform * vec4(position.x, position.y, position.z, 1);

				gl_Position = p;
				gl_PointSize = point_size;

				v_color = color;
			}
		}),
		FRAGMENT_SHADER
		({
			varying vec4 v_color;

			void main()
			{
				gl_FragColor = v_color;
			}
		})
	);
	_shaderprogram->_blend_sfactor = GL_SRC_ALPHA;
	_shaderprogram->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}



PlainShader2::PlainShader2(GraphicsContext* gc)
{
	_shaderprogram = new ShaderProgram1<glm::vec2>(
		"position",
		VERTEX_SHADER
		({
			attribute vec2 position;
			uniform mat4 transform;
			uniform float point_size;

			void main()
			{
				vec4 p = transform * vec4(position.x, position.y, 0, 1);

				gl_Position = p;
				gl_PointSize = point_size;
			}
		}),
		FRAGMENT_SHADER
		({
			uniform vec4 color;

			void main()
			{
				gl_FragColor = color;
			}
		})
	);
	_shaderprogram->_blend_sfactor = GL_SRC_ALPHA;
	_shaderprogram->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}


PlainShader3::PlainShader3(GraphicsContext* gc)
{
	_shaderprogram = new ShaderProgram1<glm::vec3>(
		"position",
		VERTEX_SHADER
		({
			attribute vec3 position;
			uniform mat4 transform;
			uniform float point_size;

			void main()
			{
				vec4 p = transform * vec4(position.x, position.y, position.z, 1);

				gl_Position = p;
				gl_PointSize = point_size;
			}
		}),
		FRAGMENT_SHADER
		({
			uniform vec4 color;

			void main()
			{
				gl_FragColor = color;
			}
		})
	);
	_shaderprogram->_blend_sfactor = GL_SRC_ALPHA;
	_shaderprogram->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}


TextureShader2::TextureShader2(GraphicsContext* gc)
{
	_shaderprogram = new ShaderProgram2<glm::vec2, glm::vec2>(
		"position", "texcoord",
		VERTEX_SHADER
		({
			uniform mat4 transform;
			attribute vec2 position;
			attribute vec2 texcoord;
			varying vec2 _texcoord;

			void main()
			{
				vec4 p = transform * vec4(position.x, position.y, 0, 1);

				_texcoord = texcoord;

				gl_Position = p;
				gl_PointSize = 1.0;
			}
		}),
		FRAGMENT_SHADER
		({
			uniform sampler2D texture;
			varying vec2 _texcoord;

			void main()
			{
				gl_FragColor = texture2D(texture, _texcoord);
			}
		})
	);
	_shaderprogram->_blend_sfactor = GL_ONE;
	_shaderprogram->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}


TextureShader3::TextureShader3(GraphicsContext* gc)
{
	_shaderprogram = new ShaderProgram2<glm::vec3, glm::vec2>(
		"position", "texcoord",
		VERTEX_SHADER
		({
			uniform mat4 transform;
			attribute vec3 position;
			attribute vec2 texcoord;
			varying vec2 _texcoord;

			void main()
			{
				vec4 p = transform * vec4(position.x, position.y, position.z, 1);

				_texcoord = texcoord;

				gl_Position = p;
				gl_PointSize = 1.0;
			}
		}),
		FRAGMENT_SHADER
		({
			uniform sampler2D texture;
			varying vec2 _texcoord;

			void main()
			{
				gl_FragColor = texture2D(texture, _texcoord);
			}
		})
	);
	_shaderprogram->_blend_sfactor = GL_ONE;
	_shaderprogram->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}


OpaqueTextureShader2::OpaqueTextureShader2(GraphicsContext* gc)
{
	_shaderprogram = new ShaderProgram2<glm::vec2, glm::vec2>(
		"position", "texcoord",
		VERTEX_SHADER
		({
			uniform mat4 transform;
			attribute vec2 position;
			attribute vec2 texcoord;
			varying vec2 _texcoord;

			void main()
			{
				vec4 p = transform * vec4(position.x, position.y, 0, 1);

				_texcoord = texcoord;

				gl_Position = p;
				gl_PointSize = 1.0;
			}
		}),
		FRAGMENT_SHADER
		({
			uniform sampler2D texture;
			varying vec2 _texcoord;

			void main()
			{
				gl_FragColor = texture2D(texture, _texcoord);
			}
		})
	);
	_shaderprogram->_blend_sfactor = GL_ONE;
	_shaderprogram->_blend_dfactor = GL_ZERO;
}


AlphaTextureShader2::AlphaTextureShader2(GraphicsContext* gc)
{
	_shaderprogram = new ShaderProgram2<glm::vec2, glm::vec2>(
		"position", "texcoord",
		VERTEX_SHADER
		({
			uniform mat4 transform;
			attribute vec2 position;
			attribute vec2 texcoord;
			varying vec2 _texcoord;

			void main()
			{
				vec4 p = transform * vec4(position.x, position.y, 0, 1);

				_texcoord = texcoord;

				gl_Position = p;
				gl_PointSize = 1.0;
			}
		}),
		FRAGMENT_SHADER
		({
			uniform sampler2D texture;
			uniform float alpha;
			varying vec2 _texcoord;

			void main()
			{
				vec4 c = texture2D(texture, _texcoord) * alpha;
				gl_FragColor = c;
			}
		})
	);
	_shaderprogram->_blend_sfactor = GL_ONE;
	_shaderprogram->_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;
}




renderers::renderers()
{
	_gradient_renderer = new GradientShader2(nullptr);
	_plain_renderer = new PlainShader2(nullptr);
	_plain_renderer3 = new PlainShader3(nullptr);
	_texture_renderer = new TextureShader2(nullptr);
	_texture_renderer3 = new TextureShader3(nullptr);
	_opaque_texture_renderer = new OpaqueTextureShader2(nullptr);
	_alpha_texture_renderer = new AlphaTextureShader2(nullptr);
}
