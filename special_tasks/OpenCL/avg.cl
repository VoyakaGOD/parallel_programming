__kernel void sum_array(
    __global    const float    *input,
    __global    float          *output,
    __local     float          *local_sum
)
{
    int local_id = get_local_id(0);
    int global_id = get_global_id(0);
    int group_size = get_local_size(0);

    local_sum[local_id] = input[global_id];
    barrier(CLK_LOCAL_MEM_FENCE);

    for(int stride = group_size/2; stride > 0; stride /= 2)
    {
        if(local_id < stride)
            local_sum[local_id] += local_sum[local_id + stride];
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    if(local_id == 0)
        output[get_group_id(0)] = local_sum[0];
}
