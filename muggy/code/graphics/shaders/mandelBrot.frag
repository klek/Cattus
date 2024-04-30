#version 450
//#extension GL_ARB_separate_shader_objects : enable

#define M_RE_START      -2.8f
#define M_RE_END        1.0f
#define M_IM_START      -1.5f
#define M_IM_END        1.5f
#define MAX_ITERATION   1000

layout(location = 0) in vec4 vPosition;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform Push {
    float width;
    float height;
    float frame;
} push;


vec3 mapColor( float t )
{
    vec3 ambient = vec3( 0.09f, 0.12f, 0.16f );
    return vec3( 3.0f * t, 5.0f * t, 10.0f * t ) + ambient;
}

vec2 complexSquare( vec2 z )
{
    return vec2( z.x * z.x - z.y * z.y, 2.0f * z.x * z.y );
}

vec3 drawMandelBrot( vec2 uv )
{
    // Calculate this pixels position in the xy-plane spanning from
    // M_RE_START to M_RE_END
    const float xSpan = ( M_RE_END - M_RE_START ) / push.width;
    const float ySpan = ( M_IM_END - M_IM_START ) / push.height;
    const float xStart = M_RE_START / push.width;
    const float yStart = M_IM_START / push.width;

//    const vec2 c = vec2( M_RE_START + uv.x * ( M_RE_END - M_RE_START ),
//                         M_IM_START + uv.y * ( M_IM_END - M_IM_START ) );
//    const vec2 c = vec2( M_RE_START + uv.x * ( M_RE_END - M_RE_START ),
//                         M_IM_START + uv.y * ( M_IM_END - M_IM_START ) );
    const vec2 c = vec2( -0.5f + uv.x * ( 0.5f + 0.5f ), uv.y );
    vec2 z = vec2( 0.0f, 0.0f );
    for ( int i = 0; i < MAX_ITERATION; i++ )
    {
        z = complexSquare( z ) + c;
        const float d = dot( z, z );
        if ( d > 4.0f )
        {
            const float t = i + 1 - log( log2( d ) );
            return mapColor( t / MAX_ITERATION );
        }
    }

    return vec3( 1.0f, 1.0f, 1.0f );
}

void main()
{
    // TODO(klek): We need to get width and height into the shader such
    //             that we can normalize the numbers
    const vec2 norm = vec2( 1.0f / push.width, 1.0f / push.height );
    const vec2 uv = vec2(vPosition.x, vPosition.y );
    outColor = vec4( drawMandelBrot( uv ), 1.0f );
}

/*
const int N = 300;
const int COLOR_COUNT = 4;
const vec4 COLORS[COLOR_COUNT] = vec4[](
    vec4(1.0, 1.0, 1.0, 1.0),
    vec4(0.070588235, 0.137254902, 0.588235294, 1.0),
    vec4(0.97254902, 0.854901961, 0.321568627, 1.0),
    vec4(0.341176471, 0.054901961, 0.141176471, 1.0)
);
const int STRETCH = 5;


vec2 multiplyComplex(vec2 a, vec2 b) {
    float real = (a.x * b.x) - (a.y * b.y);
    float imag = (a.x * b.y) + (a.y * b.x);
    return vec2(real, imag);
}

void main() {
    vec2 z = vec2( vPosition.x, vPosition.y );
    int n = 0;

    bool withinMandelbrot = true;
    for(; n < N; n++) {
        z = multiplyComplex(z, z) + vec2( vPosition.x, vPosition.y );
        if(length(z) > 2) {
            withinMandelbrot = false;
            break;
        }
    }

    if(withinMandelbrot) {
        outColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else {
        float m = float(n % STRETCH) / STRETCH;
        outColor = mix(
            COLORS[n / STRETCH % COLOR_COUNT],
            COLORS[(n / STRETCH + 1) % COLOR_COUNT],
            m);
    }
}
*/