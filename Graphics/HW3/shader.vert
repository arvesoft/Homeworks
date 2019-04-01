#version 410

layout(location = 0) in vec3 position;

// Data from CPU
uniform mat4 MVP; // ModelViewProjection Matrix
uniform mat4 MV; // ModelView idMVPMatrix
uniform vec4 cameraPosition;
uniform float heightFactor;

// Texture-related data
uniform sampler2D rgbTexture;
// Int have errors ??
uniform float widthTexture;
uniform float heightTexture;

// Output to Fragment Shader
out vec2 textureCoordinate; // For texture-color
out vec3 vertexNormal; // For Lighting computation
out vec3 ToLightVector; // Vector from Vertex to Light;
out vec3 ToCameraVector; // Vector from Vertex to Camera;

vec3 getHeight(vec3 vector, float widthTexture_f, float heightTexture_f){

    vec2 textureCoord;

    textureCoord.x = float(widthTexture_f - vector.x) / widthTexture_f;
    textureCoord.y = float(heightTexture_f - vector.z) / heightTexture_f;

    vec4 textureColor = texture(rgbTexture, textureCoord);

    vector.y = heightFactor * (0.2126 * textureColor.r + 0.7152 * textureColor.g + 0.0722 * textureColor.b);

  return vector;
}


void main(){

  float widthTexture_f = float(widthTexture);
  float heightTexture_f = float(heightTexture);

  int forCalculate[18] = int[](1, 0, 2, 0, 3, 1, 2, 4, 5, 3, 4, 5, 0, 0, 0, 0, 0, 0);

  vec3 pos_vec;
  vec3 normal_vec = vec3(0.0f, 0.0f, 0.0f);

  textureCoordinate.x = float(widthTexture_f - position.x) / widthTexture_f;
  textureCoordinate.y = float(heightTexture_f - position.z) / heightTexture_f;

  vec4 textureColor = texture(rgbTexture, textureCoordinate);

  pos_vec.x = position.x;
  pos_vec.y = heightFactor * (0.2126 * textureColor.x + 0.7152 * textureColor.y + 0.0722 * textureColor.z);
  pos_vec.z = position.z;

  vec3 vec_array[6] = vec3[6](
      vec3(pos_vec.x, 0, pos_vec.z - 1),      // Ust
      vec3(pos_vec.x + 1, 0, pos_vec.z - 1),  // Sag alt
      vec3(pos_vec.x - 1, 0, pos_vec.z),      // Sol
      vec3(pos_vec.x + 1, 0, pos_vec.z),      // Sag
      vec3(pos_vec.x - 1, 0, pos_vec.z + 1),  // Sol ust
      vec3(pos_vec.x, 0, pos_vec.z + 1)       // Alt
  );

  ToLightVector = normalize( vec3(widthTexture_f / 2.0, widthTexture_f + heightTexture_f, heightTexture_f / 2.0) - pos_vec);
  ToCameraVector = normalize(cameraPosition.xyz - pos_vec);

  for(int i = 0; i < 6; i++){

      // Check are they inside of jpg, if they are calculate height
    if(!(vec_array[i].x < 0 || vec_array[i].x > widthTexture + 1 ||
      vec_array[i].z < 0 || vec_array[i].z > heightTexture + 1)){

      forCalculate[12 + i] = 1;
      vec_array[i] = getHeight(vec_array[i], widthTexture_f, heightTexture_f);
    }
}

  int count = 0;
  for(int i = 0; i < 6; i++){

      if(forCalculate[12 + forCalculate[2 * i]] == 1 && forCalculate[12 + forCalculate[2 * i + 1]] == 1){

          normal_vec +=  cross(vec_array[forCalculate[2 * i]] - pos_vec,
                                         vec_array[forCalculate[2 * i + 1]] - pos_vec) ;
          count++;

      }
  }

  // Average of normals
  if( count != 0) vertexNormal = normal_vec / count;

  gl_Position = MVP * vec4(pos_vec, 1.0f);

}
