
#include "ShaderNode_hlb.h"


ShaderNode_hlb::ShaderNode_hlb()
:m_rippleDistance(0.0f),
m_rippleRange(0.02f)
{
}

ShaderNode_hlb::~ShaderNode_hlb()
{
}


ShaderNode_hlb* ShaderNode_hlb::createWithVertexFile(const std::string &name_vert, const std::string& name_frag)
{
    auto node = new (std::nothrow) ShaderNode_hlb();
    node->initWithVertex(name_vert, name_frag);
    node->autorelease();

    return node;
}


bool ShaderNode_hlb::initWithVertex(const std::string &vert, const std::string &frag)
{
    //auto glprogram = GLProgram::createWithByteArrays(vert.c_str(), frag.c_str());   
    auto glprogram = new GLProgram();
    glprogram->initWithFilenames(vert.c_str(), frag.c_str());
        
    //��attribute  ����
    glprogram->bindAttribLocation("a_position", 0);  
    glprogram->bindAttribLocation("a_texCoord", 1);  
    
    glprogram->link(); //��Ϊ�������ԣ�������Ҫlink һ�£������޷�ʶ������
    
    //��ȡattribute  ������ʶ
    m_attributePosition = glGetAttribLocation(glprogram->getProgram(), "a_position");  
    m_attributeColor = glGetAttribLocation(glprogram->getProgram(), "a_color");  
    m_attributeTexCoord = glGetAttribLocation(glprogram->getProgram(), "a_texCoord");  


    //��ȡuniform  ������ʶ
    m_uniformTex0 = glGetUniformLocation(glprogram->getProgram(), "tex0");  
    m_uniformDist = glGetUniformLocation(glprogram->getProgram(), "u_rippleDistance");  
    m_uniformRange = glGetUniformLocation(glprogram->getProgram(), "u_rippleRange");  

     /*����uniform ����()*/
    glprogram->updateUniforms(); //�ϴ�ϵͳ�ڲ�Ĭ�ϵ�uniform ����
    
        
    //ʹ����ɫ������
    this->setGLProgram(glprogram);
    glprogram->release();
    CHECK_GL_ERROR_DEBUG();

   //ͼƬ��Ϣ
    m_Texture = Director::getInstance()->getTextureCache()->addImage("hlb_image.png");


    m_time = 0;
    setContentSize(Size(m_Texture->getPixelsWide(), m_Texture->getPixelsHigh()));
    setColor(Color4F(1.0, 1.0, 1.0, 0.5));
    scheduleUpdate();

    
    return true;
}


void ShaderNode_hlb::update(float dt)
{
    //m_time += dt;
    
    float rippleSpeed = 0.25f;
    float maxRippleDistance = 1;
    m_rippleDistance += rippleSpeed * dt;
    m_rippleRange = (1 - m_rippleDistance / maxRippleDistance) * 0.02f;
 
    if (m_rippleDistance > maxRippleDistance) 
    {
        //unscheduleUpdate();
        m_rippleDistance = 0;        
    }    
}

void ShaderNode_hlb::setColor(const Color4F& color)
{
    m_color[0] = color.r;
    m_color[1] = color.g;
    m_color[2] = color.b;
    m_color[3] = color.a;
}

void ShaderNode_hlb::setContentSize(const Size &newSize)
{
    Node::setContentSize(newSize);
    m_resolution = Vec2(newSize.width, newSize.height);
}

void ShaderNode_hlb::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
    auto w = m_resolution.x;
    auto h = m_resolution.y;


    getGLProgram()->use();
    getGLProgram()->setUniformsForBuiltins(transform);


     /*��������λ*/
    GL::bindTexture2D( m_Texture->getName()); 

    

    /*��ȡ��Ļ���ݵ�����*/
    //glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,   0, 0, w,  h,  0);


    /*����attribute ����ֵ*/
    glEnableVertexAttribArray(m_attributePosition);  //����������������
    glDisableVertexAttribArray(m_attributeColor);  //��ɫ���ݲ��࣬����Ҫ�������鴫��
    GLfloat vertices[12] = {  
        0, 0,
        w, 0, 
        w, h, 
        0, 0, 
        0, h, 
        w, h,
    };  
    glVertexAttribPointer(m_attributePosition, 2, GL_FLOAT, GL_FALSE, 0, vertices);  
    glVertexAttrib4fv(m_attributeColor, m_color);  


    //��������
    auto _maxS = m_Texture->getMaxS();
    auto _maxT = m_Texture->getMaxT();
    GLfloat coordinates[] = {
        0,          _maxT,
        _maxS,  _maxT,
        _maxS,  0,
        0,          _maxT,
        0,          0,
         _maxS, 0
    };
    glEnableVertexAttribArray(m_attributeTexCoord);
    glVertexAttribPointer(m_attributeTexCoord, 2, GL_FLOAT, GL_FALSE, 0, coordinates);


    getGLProgram()->setUniformLocationWith1i(m_uniformTex0, 0); 
    getGLProgram()->setUniformLocationWith1f(m_uniformDist,  m_rippleDistance);
    getGLProgram()->setUniformLocationWith1f(m_uniformRange,  m_rippleRange);



    /*��ʼ����*/
    glDrawArrays(GL_TRIANGLES, 0, 6);  

    CHECK_GL_ERROR_DEBUG();
}

