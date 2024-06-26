struct PointLight {
    vec3 Position;
    vec3 colorAmbient;
    float Linear;
    float Quadratic;
    float Radius;
    float LightSmoothness;
};

struct SpotLight {
    vec3 Position;
    vec3 Direction;
    vec3 colorAmbient;
    float Linear;
    float Quadratic;
    float Radius;
    float LightSmoothness;
    float innerCutoff;
    float outerCutoff;
};

struct AreaLight {
    vec3 color;
    float intensity;
    vec3 points[4];
    bool twoSided;
};



