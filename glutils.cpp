static void DrawAxis()
{
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(32, 0, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 32, 0);

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 32);
	glEnd();
}

// draw a textured quad -1 .. 1
static void DrawTexturedQuad()
{
	glBegin(GL_TRIANGLE_STRIP);

	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(-1.0f, -1.0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex2f(1.0f, -1.0f);

	glTexCoord2f(0.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);

	glTexCoord2f(1.0f, 1.0f);
	glVertex2f(1.0f, 1.0f);

	glEnd();
}

static void MakeTexture(int imagew, int imageh, void *pixels)
{
	glGenTextures(1, texobj);
	glBindTexture(GL_TEXTURE_2D, texobj[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imagew, imageh, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

// draw a 3d wireframe bounding box
static void DrawBounds(float min[3], float max[3])
{
	glBegin(GL_LINE_LOOP);
	glVertex3f(min[0], min[1], min[2]);
	glVertex3f(max[0], min[1], min[2]);
	glVertex3f(max[0], max[1], min[2]);
	glVertex3f(min[0], max[1], min[2]);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(min[0], min[1], max[2]);
	glVertex3f(max[0], min[1], max[2]);
	glVertex3f(max[0], max[1], max[2]);
	glVertex3f(min[0], max[1], max[2]);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(min[0], min[1], min[2]);
	glVertex3f(min[0], min[1], max[2]);
	glVertex3f(max[0], min[1], min[2]);
	glVertex3f(max[0], min[1], max[2]);
	glVertex3f(max[0], max[1], min[2]);
	glVertex3f(max[0], max[1], max[2]);
	glVertex3f(min[0], max[1], min[2]);
	glVertex3f(min[0], max[1], max[2]);
	glEnd();
}

static void DrawVector(float origin[3], float dir[3])
{
	float s = 2.0f;

	glBegin(GL_LINES);
	{
		glVertex3f(origin[0], origin[1], origin[2]);
		glVertex3f(s * dir[0] + origin[0], s * dir[1] + origin[1], s * dir[2] + origin[2]);
	}
	glEnd();
}

static void DrawMatrix(float matrix[4][4])
{
	float vectors[4][3] =
	{
		{ matrix[0][0], matrix[1][0], matrix[2][0] },
		{ matrix[0][1], matrix[1][1], matrix[2][1] },
		{ matrix[0][2], matrix[1][2], matrix[2][2] },
		{ matrix[0][3], matrix[1][3], matrix[2][3] },
	};

	glColor3f(1, 0, 0);
	DrawVector(vectors[3], vectors[0]);
	glColor3f(0, 1, 0);
	DrawVector(vectors[3], vectors[1]);
	glColor3f(0, 0, 1);
	DrawVector(vectors[3], vectors[2]);
}

// clip is usually the modelviewproj matrix
void TransformModelToWindow(float vs[2], float clip[4][4], float renderw, float renderh, float vm[3])
{
	float vc[4];

	// transform to clip space
	vc[0] = (vm[0] * clip[0][0]) + (vm[1] * clip[0][1]) + (vm[2] * clip[0][2]) + clip[0][3];
	vc[1] = (vm[0] * clip[1][0]) + (vm[1] * clip[1][1]) + (vm[2] * clip[1][2]) + clip[1][3];
	vc[2] = (vm[0] * clip[2][0]) + (vm[1] * clip[2][1]) + (vm[2] * clip[2][2]) + clip[2][3];
	vc[3] = (vm[0] * clip[3][0]) + (vm[1] * clip[3][1]) + (vm[2] * clip[3][2]) + clip[3][3];

	// transform to normalized coordinates
	vs[0] = vc[0] / vc[3];
	vs[1] = vc[1] / vc[3];

	// transform to window coordinates
	vs[0] = 0.5f * (1.0f + vs[0]) * renderw;
	vs[1] = 0.5f * (1.0f + vs[1]) * renderh;
}

static GLint GetCompileStatus(const GLuint obj)
{
        GLint param;
        glGetShaderiv(obj, GL_COMPILE_STATUS, &param);

        return param;
}

static GLint GetLinkStatus(const GLuint obj)
{
        GLint param;
        glGetProgramiv(obj, GL_LINK_STATUS, &param);

        return param;
}

static char* GetError(const GLuint obj)
{
        static char buffer[1024];

	if (glIsShader(obj))
	{
		glGetShaderInfoLog(obj, 1024, NULL, buffer);
		return buffer;
	}

	if (glIsProgram(obj))
	{
		glGetProgramInfoLog(obj, 1024, NULL, buffer);
		return buffer;
	}

	return NULL;
}


static GLuint CompileGLSLShader(const GLenum shadertype, const char *src)
{
        GLuint  shader;
        const char *strings[1] = { src };

        shader = glCreateShader(shadertype);
        glShaderSource(shader, 1, strings, NULL);
        glCompileShader(shader);

        if (GetCompileStatus(shader) == GL_FALSE)
        {
                printf("Compile Error:\n%s\n", GetError(shader));
                glDeleteShader(shader);

                return 0;
        }

        return shader;
}

static GLuint LinkProgram(const GLuint shader)
{
	GLuint program = glCreateProgram();
	glAttachShader(program, shader);
	glLinkProgram(program);

	if (GetLinkStatus(program) == GL_FALSE)
	{
                printf("Link Error:\n%s\n", GetError(program));
                glDeleteProgram(program);
	}

	return program;
}

