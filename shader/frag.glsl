varying vec3 N;
varying vec3 v;
uniform sampler2D tex;
uniform bool hasTex;
void main()
{
  vec3 L = normalize(gl_LightSource[0].position.xyz - v);   
  vec4 diff = gl_FrontLightProduct[0].diffuse * max(dot(N,L), 0.0)
              *gl_FrontMaterial.diffuse;
  vec3 E = normalize(-v);
  vec3 refl = normalize(-reflect(L,N));
  vec4 Ispec = gl_FrontLightProduct[0].specular 
                * pow(max(dot(refl,E),0.0),gl_FrontMaterial.shininess)
                * gl_FrontMaterial.specular;
  Ispec = clamp(Ispec, 0.0, 1.0);
  vec4 texCol;
  
  texCol = hasTex ? texture2D(tex, gl_TexCoord[0].st)
    :vec4(1.0,1.0,1.0,1.0);
  
  diff = clamp(diff, 0.0, 1.0);

  vec4 Iamb = gl_FrontLightProduct[0].ambient*gl_FrontMaterial.ambient;
  gl_FragColor = texCol*(diff+Iamb+Ispec);
}
