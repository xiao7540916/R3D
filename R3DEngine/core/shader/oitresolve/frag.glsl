#version 460

layout(binding = 0, r32ui)uniform uimage2D head_pointer_image;
layout(binding = 1, rgba32ui)uniform uimageBuffer list_buffer;
const int MAX_FRAGMENTS = 15;//不透明最大层数
uvec4 fragment_list[MAX_FRAGMENTS];
layout(location = 0)out vec4 FragColor;

int build_local_fragment_list(){
    int fragment_count = 0;
    uint current_index = imageLoad(head_pointer_image, ivec2(gl_FragCoord).xy).x;
    while (current_index != 0 && fragment_count < MAX_FRAGMENTS)
    {
        uvec4 fragment = imageLoad(list_buffer, int(current_index));
        fragment_list[fragment_count] = fragment;
        current_index = fragment.x;
        fragment_count++;
    }
    return fragment_count;
}
void sort_fragment_list(int in_fragcount){
    uint i, j;

    if (in_fragcount > 1)
    {

        for (i = 0; i < in_fragcount - 1; i++)
        {
            for (j = i + 1; j < in_fragcount; j++)
            {
                uvec4 fragment1 = fragment_list[i];
                uvec4 fragment2 = fragment_list[j];

                float depth1 = uintBitsToFloat(fragment1.z);
                float depth2 = uintBitsToFloat(fragment2.z);

                if (depth1 < depth2)
                {
                    fragment_list[i] = fragment2;
                    fragment_list[j] = fragment1;
                }
            }
        }

    }
}
vec4 calculate_final_color(int in_frag_count){
    //排列顺序，按照由远到近
    vec4 finalcol = vec4(0);
    for (int i = 0; i < in_frag_count; i++)
    {
        vec4 modulator = unpackUnorm4x8(fragment_list[i].y);
        vec4 opacityinfo = unpackUnorm4x8(fragment_list[i].w);
        finalcol = mix(finalcol, modulator, modulator.a);
    }
    return finalcol;
}
void main() {
    int frag_count = 0;
    frag_count = build_local_fragment_list();
    sort_fragment_list(frag_count);
    FragColor = calculate_final_color(frag_count);
}
