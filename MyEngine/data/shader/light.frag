#version 330 core 
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D Texture;
uniform vec4 lightColor;
void main()
{ 
    
    vec4 sampleColor=texture(Texture, TexCoords);
    if(sampleColor.rgb==vec3(0,0,0))
    {
       if(lightColor.rgb==vec3(0,0,0))
       {
        FragColor =vec4(1,1,1,1) ;
       }
       else
       {
             FragColor=lightColor;
       }
    }
    else
    {
       FragColor=sampleColor;
    }
    
   FragColor =vec4(1,1,1,1) ;
}