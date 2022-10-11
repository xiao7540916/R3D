#version 460
layout(early_fragment_tests) in;
layout(binding = 0, r32ui)uniform uimage2D head_pointer_image;
layout(binding = 1, rgba32ui)uniform writeonly uimageBuffer list_buffer;
layout(binding = 0, offset = 0)uniform atomic_uint index_count;
layout(location = 0)out vec4 FragColor;
void main() {
    uint index;//计数器值
    uint old_head;
    uvec4 item;//打包的数据
    vec4 frag_color;//片元颜色

    frag_color = vec4(0.0, 0.8, 0.0, 0.5);
    index = atomicCounterIncrement(index_count);
    //将计数器值写入链表头图像中此片元对应的位置
    old_head = imageAtomicExchange(head_pointer_image, ivec2(gl_FragCoord.xy), index);

    item.x = old_head;
    item.y = packUnorm4x8(frag_color);
    item.z = floatBitsToUint(gl_FragCoord.z);
    imageStore(list_buffer, int(index), item);
}