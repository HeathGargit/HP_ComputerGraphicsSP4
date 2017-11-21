#ifndef SHADERINATOR
#define SHADERINATOR

class Shaderinator
{
public:
	//constructors
	Shaderinator(char* inputString, unsigned int shaderType);
	~Shaderinator();
	
	//accessor
	unsigned int getShader();

private:
	//funcs
	bool LoadShader(char* inputString, unsigned int shaderType);

	//member variables
	const char* m_RawShader;
	unsigned int m_Shader;
};

#endif