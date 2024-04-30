#version 450

#define J_RE_START      -0.5f
#define J_RE_END        0.5f
#define J_IM_START      -1.0f
#define J_IM_END        1.0f
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

vec3 drawJuliaSet( vec2 uv )
{
    // 
    const float f = push.frame * 0.0002f;
    const vec2 w = vec2( cos( f ), sin( f ) );
    const vec2 c = ( 2 * w - complexSquare( w ) ) * 0.26f; 
    vec2 z = vec2( uv.x, uv.y );
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
    const vec2 uv = vec2( vPosition.x, vPosition.y );
    outColor = vec4( drawJuliaSet( uv ), 1.0f );
}